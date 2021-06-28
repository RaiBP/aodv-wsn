#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/adc-zoul.h"

#include <stdio.h>  // For printf

#include "project_conf.h"
#include "print_func.h"
#include "send_func.h"
#include "struct.h"

// Connections
static struct unicast_conn rep_conn;
static struct unicast_conn data_conn;
static struct broadcast_conn req_conn;

/*table functions*/
static void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info);
static char updateRoutingTable(struct REP_PACKAGE *rep, const linkaddr_t *from);
static void clearDiscovery(struct REP_PACKAGE* rep);
char addToWaitingTable(struct DATA_PACKAGE *data);

static char isDuplicateReq(struct REQ_PACKAGE* req);

/*callback functions*/
static void reply_callback(struct unicast_conn *c, const linkaddr_t *from);
static void request_callback(struct broadcast_conn *c, const linkaddr_t *from);
static void data_callback(struct unicast_conn *c, const linkaddr_t *from);

/*callbacks*/
static const struct unicast_callbacks data_cbk = {data_callback};
static const struct unicast_callbacks rep_cbk = {reply_callback};
static const struct broadcast_callbacks req_cbk = {request_callback};

/*getdata functions*/
static int getTemperatureValue();
static int getLuxValue();

static int getNextHop();



/*---------------------processes---------------*/
PROCESS(initial_process, "Initialize the routing table and open connections");
PROCESS(data_process, "Send data.");
PROCESS(request_process, "Handle REQ_PACKAGE messages");
PROCESS(aging_process, "Decide the expiration of tables");


AUTOSTART_PROCESSES(&initial_process, &data_process, &request_process, &aging_process);

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
	unicast_open(&rep_conn, RREP_CHANNEL, &rep_cbk);
	broadcast_open(&req_conn, RREQ_CHANNEL, &req_cbk);

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

	     	process_post(&request_process, PROCESS_EVENT_CONTINUE, &discovery);

	        }

	    }

	PROCESS_END();
}


/**
 * Perform the outgoing Request route
 */
PROCESS_THREAD(request_process, ev, data)
{
    static struct REQ_PACKAGE req;

    PROCESS_BEGIN();

    while(1)
    {
        leds_off(LEDS_YELLOW);

        PROCESS_WAIT_EVENT_UNTIL(ev != sensors_event);

        leds_on(LEDS_YELLOW);

        req.id = ((struct DISCOVERY_TABLE*)data)->id;
        req.src = ((struct DISCOVERY_TABLE*)data)->src;
        req.dest = ((struct DISCOVERY_TABLE*)data)->dest;

        addEntryToDiscoveryTable((struct DISCOVERY_TABLE*)data);    //create entry in routing discovery table

        sendreq(&req, req_conn);    //broadcasts the REQUEST
    }

    PROCESS_END();
}

/**
 * check the aging table periodically
 */
PROCESS_THREAD(aging_process, ev, data)
{
    static struct etimer et;
    static int flag;
    static int i;
    static int next;

    PROCESS_BEGIN();

    leds_off(LEDS_RED);

    while(1)
    {
        etimer_set(&et, CLOCK_CONF_SECOND);

        PROCESS_WAIT_EVENT_UNTIL(ev != sensors_event);

        leds_off(LEDS_RED);

        // Clean routingTable
        flag = 0;

            if(routingTable.age > 0 && routingTable.valid ==1)
            {
                routingTable.age --;
                // if age has run out (route too old)
                if(routingTable.age == 0)
                {
                    routingTable.valid = 0;
                    routingTable.next.u8[0]= 0xFF;
                    routingTable.next.u8[1]= 0xFF;
                    routingTable.hops = INF_HOPS;
                    routingTable.rssi = INF_RSSI;
                    printf("route to %d has expired!\n",i+1);
                    leds_on(LEDS_RED);
                    flag++;
                }
            }

        if (flag != 0)  // if the routing table has changed
            printRoutingTable();

        // Clean discovery table
        flag = 0;
        for(i=0; i<MAX_TABLE_SIZE; i++)
        {
            if(discoveryTable[i].age > 0 && discoveryTable[i].valid ==1)
            {
                // if age has run out (route request too old)
                if(discoveryTable[i].age == 0)
                {
                    discoveryTable[i].valid = 0;
                    printf("REQUEST from %d to %d (ID:%d) has expired!\n",
                            discoveryTable[i].src.u8[1], discoveryTable[i].dest.u8[1], discoveryTable[i].id);
                    flag++;
                }
                discoveryTable[i].age --;
            }
        }
        if (flag != 0)  // if the discovery table has changed
            printDiscoveryTable();

        // Refresh waiting table
        flag = 0;
        for(i=0; i<MAX_WAIT_DATA; i++)
        {
            if(waitingTable[i].valid != 0)
            {
                next = getNextHop();
                if (next != 0)  // if the request in the waiting table find a route
                {
                    senddata(&waitingTable[i].data_pkg, next, data_conn);
                    waitingTable[i].valid = 0;
                    flag++;
                }
                else
                {
                    waitingTable[i].age--;
                    if (waitingTable[i].age < 0)
                    {
                        waitingTable[i].valid = 0;
                        flag++;
                    }
                }
            }
        }
        if (flag != 0)  // if the waiting table has changed
            printWaitingTable();
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

/**
 * called when receive a packet on REP_CHANNEL
 */
static void reply_callback(struct unicast_conn *c, const linkaddr_t *from)
{
    char *packet;
    packetbuf_copyto(packet);
    static struct REP_PACKAGE rep;
    static int i;

    // if REPLY package received
    if(packet2rep(packet, &rep)!=0)
    {
        printf("REPLY received from %d [ID:%d, Dest:%d, Src:%d, Hops:%d, RSSI: %d]\n",
                        from->u8[1], rep.id, rep.dest.u8[1], rep.src.u8[1], rep.hops, rep.rssi);

        // check if reply table updates
        if(updateRoutingTable(&rep, from))
        {
            // if the source is not me forward reply to all nodes waiting
            if(rep.src.u8[1] != linkaddr_node_addr.u8[1])
            {
                rep.hops = rep.hops + 1;
                for(i=0; i<MAX_TABLE_SIZE; i++){
                    if(discoveryTable[i].valid != 0
                        && discoveryTable[i].id == rep.id){
                            sendrep(&rep, discoveryTable[i].snd.u8[1], rep_conn);
                    }
                }
            }
            clearDiscovery(&rep);
        }
    }
}


/**
 * called when receive a packet on REQ_CHANNEL
 */
static void request_callback(struct broadcast_conn *c, const linkaddr_t *from)
{


	static struct DISCOVERY_TABLE req_info;
    static struct REQ_PACKAGE req;
    static struct REP_PACKAGE rep;
    char *packet;
    packetbuf_copyto(packet);

    // case REQUEST package received
    if(packet2req(packet, &req) != 0)
    {
        printf("ROUTE_REQUEST received from %d [ID:%d, Dest:%d, Src:%d]\n",
                        from->u8[1], req.id, req.dest.u8[1], req.src.u8[1]);

        // case destination is me
        if(req.dest.u8[1] == linkaddr_node_addr.u8[1])
        {
            rep.id = req.id;
            rep.src = req.src;
            rep.dest = req.dest;
            rep.hops = 0;
            rep.rssi = 0;

            //sends a REPLY to the Request sender
            sendrep(&rep, from->u8[1], rep_conn);

        }

        // case the destination is NOT me AND the Request is new
        else if(isDuplicateReq(&req)==0)
        {
            req_info.id = req.id;
            req_info.src = req.src;
            req_info.dest = req.dest;
            req_info.snd.u8[1] = from->u8[1];

            //wakes up the request process
            process_post(&request_process, PROCESS_EVENT_CONTINUE, &req_info);
        }
        // case the Request is duplicated
        else
        {
            printf("Duplicated REQUEST: Discarded!\n");
        }
    }

}

/*---------------------table functions---------------*/

/**
 * add entry to discovery table
 */
static void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info)
{
    int i;
    for(i=0; i<MAX_TABLE_SIZE; i++){
        if(discoveryTable[i].valid == 0){
            discoveryTable[i].id = req_info->id;
            discoveryTable[i].src = req_info->src;
            discoveryTable[i].dest = req_info->dest;
            discoveryTable[i].snd = req_info->snd;
            discoveryTable[i].valid = 1;
            discoveryTable[i].age = MAX_DISCOVERY_TIME;
            break;
        }
    }
    printDiscoveryTable();
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
            printWaitingTable();
            return 1;
        }

    }
    return 0;
}

/**
 * 	If better route is found, update routing table and return 1
 * 	Otherwise return 0
 */
static char updateRoutingTable(struct REP_PACKAGE *rep, const linkaddr_t *from)
{

    uint16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

    rep->rssi = (rep->rssi * rep->hops + rssi) / (rep->hops + 1);

    //if the ROUTE_REPLY received shows a better path
    if(rep->rssi > routingTable.rssi)
    {
        //UPDATES the routing discovery table!
        routingTable.dest = rep->dest;
        routingTable.hops = rep->hops;
        routingTable.next.u8[1] = from->u8[1];
        routingTable.age = MAX_ROUTE_TIME;
        routingTable.valid = 1;
        routingTable.rssi = rep->rssi;
        printRoutingTable();
        return 1;
    }
    return 0;
}

/**
 * clear discovery table
 */
static void clearDiscovery(struct REP_PACKAGE *rep)
{
    int i;
    for(i=0; i<MAX_TABLE_SIZE; i++){
        if(discoveryTable[i].valid != 0
            && discoveryTable[i].id == rep->id
           // && discoveryTable[i].src == rep->src        // enable for ditinguish on reply id
            && discoveryTable[i].dest.u8[1] == rep->dest.u8[1])
                discoveryTable[i].valid = 0;
    }
}

/**
 * check if the received Request was already in the discovery Table
 */
static char isDuplicateReq(struct REQ_PACKAGE *req)
{
    for(int i=0; i<MAX_TABLE_SIZE; i++){
        if(discoveryTable[i].valid != 0
            && discoveryTable[i].id == req->id
            && discoveryTable[i].src.u8[1] == req->src.u8[1]
            && discoveryTable[i].dest.u8[1] == req->dest.u8[1])
                return 1;
    }
    return 0;
}


/*---------------------get data functions---------------*/

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





