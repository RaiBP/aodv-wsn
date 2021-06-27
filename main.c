#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include "dev/zoul-sensors.h"
#include "dev/adc-zoul.h"
#include <stdio.h>

#include "project_conf.h"
#include "print_func.h"
#include "struct.h"
#include "print_func.h"
#include "send_func.h"

static void data_callback(struct unicast_conn *c, const linkaddr_t *from);

static int getTemperatureValue();
static int getLuxValue();

static int getNextHop();

char addToWaitingTable(struct DATA_PACKAGE *data);


// Tables
static struct ROUTING_TABLE routingTable;
struct WAITING_TABLE waitingTable[MAX_WAIT_DATA];

// Connections
static struct unicast_conn data_conn;

// Callback functions
static const struct unicast_callbacks data_cbk = {data_callback};


/*---------------------processes---------------*/
PROCESS(initial_process, "Initialize the routing table and open connections");
PROCESS(data_process, "Send data.");


AUTOSTART_PROCESSES(&initial_process, &data_process);

PROCESS_THREAD(initial_process, ev, data){

	PROCESS_BEGIN();

	printf("-----Process initialized-----");

    // initialize routing table
	routingTable.dest.u8[0] = (DESTINATION >> 8) & 0xFF;
	routingTable.dest.u8[1] = DESTINATION & 0xFF;
	routingTable.valid = 0;
	routingTable.hops = INF_HOPS;
	routingTable.rssi = INF_RSSI;

	// open connections
	unicast_open(&data_conn, DATA_CHANNEL, &data_cbk);
//	unicast_open(&rep_conn, RREP_CHANNEL, &rep_cbk);
//	broadcast_open(&req_conn, RREQ_CHANNEL, &req_cbk);

	PROCESS_END();
}

PROCESS_THREAD(data_process, ev, data){

	static struct etimer et;
    static struct DATA_PACKAGE data_pkg;
	static int id = 1;

    static struct DISCOVERY_TABLE discovery;

    static int next;


	PROCESS_BEGIN();

	etimer_set(&et, CLOCK_CONF_SECOND);

	while(1){

		leds_off(LEDS_GREEN);

		PROCESS_WAIT_EVENT();

		leds_on(LEDS_GREEN);

	  	// Send own data
		if(ev == PROCESS_EVENT_TIMER){
			// Set message
			data_pkg.dest.u8[0] = (DESTINATION >> 8) & 0xFF;
			data_pkg.dest.u8[1] = DESTINATION & 0xFF;
			data_pkg.id = id;
			data_pkg.src.u8[0] = linkaddr_node_addr.u8[0];
			data_pkg.src.u8[1] = linkaddr_node_addr.u8[1];
			sprintf(data_pkg.message, DATA_PAY, getTemperatureValue(), getLuxValue());

			id = (id<99) ? id+1 : 1;

			// Send data every 20 seconds
			etimer_set(&et, CLOCK_CONF_SECOND * DATA_DELTA_TIME);
		}
		// Forward data
		else{
			data_pkg.dest.u8[0] = ((struct DATA_PACKAGE*)data)->dest.u8[0];
			data_pkg.dest.u8[1] = ((struct DATA_PACKAGE*)data)->dest.u8[1];
			data_pkg.id = ((struct DATA_PACKAGE*)data)->id;
			data_pkg.src.u8[0] = ((struct DATA_PACKAGE*)data)->src.u8[0];
			data_pkg.src.u8[1] = ((struct DATA_PACKAGE*)data)->src.u8[1];
			strcpy(data_pkg.message,((struct DATA_PACKAGE*)data)->message);
		}

		next = getNextHop();

		// If the route exists
		if(next!=0){
	    	printf("Sending message: %s\n", data_pkg.message);
	    	senddata(&data_pkg, next, data_conn);
		}
		// Otherwise send request broadcast
		else{
	      	printf("There is no existing route to %d.\n", data_pkg.dest.u8[1]);

	      	addToWaitingTable(&data_pkg);

	       	discovery.id = data_pkg.id;
	       	discovery.src.u8[0] = linkaddr_node_addr.u8[0];
	       	discovery.src.u8[1] = linkaddr_node_addr.u8[1];
	       	discovery.dest.u8[0] = data_pkg.dest.u8[0];
	       	discovery.dest.u8[1] = data_pkg.dest.u8[1];
	       	discovery.snd.u8[0] = linkaddr_node_addr.u8[0];
	       	discovery.snd.u8[1] = linkaddr_node_addr.u8[1];

//	     	process_post(&request_process, PROCESS_EVENT_CONTINUE, &discovery);

	        }

	    }

	PROCESS_END();
}

/*---------------------callback functions---------------*/
static void data_callback(struct unicast_conn *c, const linkaddr_t *from){
    static struct DATA_PACKAGE data;
    char *dataptr;

    packetbuf_copyto(dataptr);

    if(packet2data(dataptr, &data) != 0){
        // if the destination is itself
        if(data.dest.u8[1] == linkaddr_node_addr.u8[1]){
            printf("DATA RECEIVED of src %d:\n{%s}\n", data.src.u8[1], data.message);
        }
        // otherwise
        else{
            process_post(&data_process, PROCESS_EVENT_CONTINUE, &data);
        }
    }
    // case not data package
    else{
        printf("Incorrect data package: %s\n", dataptr);
    }
}



/*--------------------- ---------------*/
/**
 * Get the Temperature value from the sensor.
 */
static int getTemperatureValue(){
	return cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
}

/**
 * Get the lux value from the sensor.
 */
static int getLuxValue(){
	float m =1.6009;
	float b = 41.269;
	static uint16_t adc3_value;
	adc3_value = adc_zoul.value(ZOUL_SENSORS_ADC3) >> 4;
	uint8_t adc_input = adc3_value;

	//Read voltage from the phidget interface
	int Vref = 5;

	//Convert the voltage in lux with the provided formula
	int voltage = (adc_input * Vref * 200)/4096;
	int lux = m * voltage + b;

	//Return the value of the light with maximum value equal to 1000
	if (lux>1000){
		lux = 1000;
	}

	return lux;
}

/**
 *	Return the next hop to the destination
 */
static int getNextHop(){
    if (routingTable.valid != 0){
        return routingTable.next.u8[1];
    }

    return 0;
}

/**
 * Add the data to the waiting table
 */
char addToWaitingTable(struct DATA_PACKAGE *data){
    for(int i=0; i<MAX_WAIT_DATA; i++) {
        if(waitingTable[i].valid == 0) {
            waitingTable[i].data_pkg = *data;
            waitingTable[i].age = MAX_QUEUEING_TIME;
            waitingTable[i].valid = 1;
            printWaitingTable(waitingTable);
            return 1;
        }
    }
    return 0;
}


