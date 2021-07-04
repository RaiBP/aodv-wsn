#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/adc-zoul.h"
#include "dev/cc2538-rf.h"

#include <stdio.h>  // For printf
#include <stdlib.h>
#include <math.h>
#include <random.h>

#include "project_conf.h"
#include "print_func.h"
#include "struct.h"

// Connections
static struct unicast_conn rep_conn;
static struct unicast_conn data_conn;
static struct broadcast_conn req_conn;
static struct unicast_conn ack_conn;

/*table functions*/
static void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info);
static char updateRoutingTable(struct REP_PACKAGE *rep, const linkaddr_t *from);
static void clearDiscovery(struct REP_PACKAGE* rep);
void addToWaitingTable(struct DATA_PACKAGE *data);
void addToWaitingAckTable(struct DATA_PACKAGE *data);

static char isDuplicateReq(struct REQ_PACKAGE* req);

/*callback functions*/
static void reply_callback(struct unicast_conn *c, const linkaddr_t *from);
static void request_callback(struct broadcast_conn *c, const linkaddr_t *from);
static void data_callback(struct unicast_conn *c, const linkaddr_t *from);
static void ack_callback(struct unicast_conn *c, const linkaddr_t *from);

/*callbacks*/
static const struct unicast_callbacks data_cbk = {data_callback};
static const struct unicast_callbacks rep_cbk = {reply_callback};
static const struct broadcast_callbacks req_cbk = {request_callback};
static const struct unicast_callbacks ack_cbk = {ack_callback};

/*transmit functions*/
static void senddata(struct DATA_PACKAGE *data, int next);
static void sendreq(struct REQ_PACKAGE* req);
static void sendrep(struct REP_PACKAGE* rep, int next);
static void sendack(struct ACK_PACKAGE *ack, int pre);


/*getdata functions*/
static int getTemperatureValue();
static int getLuxValue();

static int getNextHop();


static struct WAITING_TABLE waitingTable[MAX_WAIT_DATA];
static struct DISCOVERY_TABLE discoveryTable[MAX_TABLE_SIZE];
static struct ROUTING_TABLE routingTable;

/*---------------------processes---------------*/
PROCESS(initial_process, "Initialize the routing table and open connections");
PROCESS(data_process, "Send data.");
PROCESS(request_process, "Handle REQ_PACKAGE messages");
PROCESS(aging_process, "Decide the expiration of tables");


AUTOSTART_PROCESSES(&initial_process, &data_process, &request_process, &aging_process);

PROCESS_THREAD(initial_process, ev, data){

	PROCESS_EXITHANDLER({
		unicast_close(&rep_conn);
		unicast_close(&data_conn);
	 	broadcast_close(&req_conn);
	 	unicast_close(&ack_conn);
	});

	PROCESS_BEGIN();

	printf("--------Process initialized--------\n");

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
	unicast_open(&ack_conn, ACK_CHANNEL, &ack_cbk);

	PROCESS_END();
}

PROCESS_THREAD(data_process, ev, data){

	static struct etimer et;
    static struct DATA_PACKAGE data_pkg;
	static int id = 1;

    static struct DISCOVERY_TABLE discovery;

    static int next;

    static int initial_delay;

	PROCESS_BEGIN();

    initial_delay = random_rand() % DATA_DELTA_TIME;
    etimer_set(&et, (CLOCK_CONF_SECOND) * initial_delay );

	while(1){

		leds_off(LEDS_GREEN);

		PROCESS_WAIT_EVENT_UNTIL(ev != sensors_event);

		leds_on(LEDS_GREEN);

	  	// Send own data
		if(ev == PROCESS_EVENT_TIMER && linkaddr_node_addr.u8[1]!=0x01){
			printf("Generate new data.\n");
			// Set message
			data_pkg.dest.u8[0] = (DESTINATION >> 8) & 0xFF;
			data_pkg.dest.u8[1] = DESTINATION & 0xFF;
			data_pkg.id = id;
			data_pkg.src.u8[0] = linkaddr_node_addr.u8[0];
			data_pkg.src.u8[1] = linkaddr_node_addr.u8[1];
			sprintf(data_pkg.message, DATA_PAY, getTemperatureValue(), getLuxValue());
			data_pkg.hops = 0;
			data_pkg.route[6] = "000000";
			data_pkg.route[0] = linkaddr_node_addr.u8[1];
			//printf("route is %d\n", data_pkg.route[0]);

			id = (id<99) ? id+1 : 1;

			// Send data every 20 seconds
			etimer_set(&et, CLOCK_CONF_SECOND * DATA_DELTA_TIME);
		}
		// Forward data
		else{
			printf("Forwarding data...\n");
			data_pkg.dest.u8[0] = ((struct DATA_PACKAGE*)data)->dest.u8[0];
			data_pkg.dest.u8[1] = ((struct DATA_PACKAGE*)data)->dest.u8[1];
			data_pkg.id = ((struct DATA_PACKAGE*)data)->id;
			data_pkg.src.u8[0] = ((struct DATA_PACKAGE*)data)->src.u8[0];
			data_pkg.src.u8[1] = ((struct DATA_PACKAGE*)data)->src.u8[1];
			strcpy(data_pkg.message,((struct DATA_PACKAGE*)data)->message);
			data_pkg.hops = ((struct DATA_PACKAGE*)data)->hops + 1;
			int h = data_pkg.hops;
			printf("hops in this forwarding data is %d\n",h);
			printf("data package route now is:%d,%d,%d,%d,%d,%d\n",((struct DATA_PACKAGE*)data)->route[0],((struct DATA_PACKAGE*)data)->route[1],
					((struct DATA_PACKAGE*)data)->route[2],((struct DATA_PACKAGE*)data)->route[3],((struct DATA_PACKAGE*)data)->route[4],
					((struct DATA_PACKAGE*)data)->route[5]);

			data_pkg.route[0] = ((struct DATA_PACKAGE*)data)->route[0];
			data_pkg.route[1] = ((struct DATA_PACKAGE*)data)->route[1];
			data_pkg.route[2] = ((struct DATA_PACKAGE*)data)->route[2];
			data_pkg.route[3] = ((struct DATA_PACKAGE*)data)->route[3];
			data_pkg.route[4] = ((struct DATA_PACKAGE*)data)->route[4];
			data_pkg.route[5] = ((struct DATA_PACKAGE*)data)->route[5];
			if(data_pkg.route[h-1] != linkaddr_node_addr.u8[1]){
				data_pkg.route[h] = linkaddr_node_addr.u8[1];
			}


			printf("route is updated to %d,%d,%d,%d,%d,%d\n", data_pkg.route[0], data_pkg.route[1], data_pkg.route[2], data_pkg.route[3], data_pkg.route[4], data_pkg.route[5]);
		}


		if(linkaddr_node_addr.u8[1]!=0x01){
			next = getNextHop();

			// If the route exists
			if(next!=0){

				printf("Route exists, so sending message\n");
				printf("Sending message: %s\n", data_pkg.message);
				printf("The data package id is:%d, src is :%d\n",data_pkg.id,data_pkg.src.u8[1]);
				senddata(&data_pkg, next);
				// add to waiting table wait for ack
				addToWaitingAckTable(&data_pkg);
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

        sendreq(&req);    //broadcasts the REQUEST
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
        etimer_set(&et, CLOCK_CONF_SECOND * TIME_FACTOR);

        PROCESS_WAIT_EVENT_UNTIL(ev != sensors_event);

        leds_off(LEDS_RED);

        // Clean routingTable
        flag = 0;

            if(routingTable.age > 0 && routingTable.valid ==1)
            {
                routingTable.age--;
                // if age has run out (route too old)
                if(routingTable.age == 0)
                {
                    routingTable.valid = 0;
                    routingTable.next.u8[0]= 0xFF;
                    routingTable.next.u8[1]= 0xFF;
                    routingTable.hops = INF_HOPS;
                    routingTable.rssi = INF_RSSI;
                    printf("route to %d has expired!\n",routingTable.dest.u8[1]);
                    leds_on(LEDS_RED);
                    flag++;
                }
            }

        if (flag != 0)  // if the routing table has changed
            printRoutingTable(routingTable);

        // Clean discovery table
        flag = 0;
        for(i=0; i<MAX_TABLE_SIZE; i++)
        {
            if(discoveryTable[i].age > 0 && discoveryTable[i].valid ==1)
            {
            	discoveryTable[i].age --;
                // if age has run out (route request too old)
                if(discoveryTable[i].age == 0)
                {
                    discoveryTable[i].valid = 0;
                    printf("REQUEST from %d to %d (ID:%d) has expired!\n",
                            discoveryTable[i].src.u8[1], discoveryTable[i].dest.u8[1], discoveryTable[i].id);
                    flag++;
                }

            }
        }
        if (flag != 0)  // if the discovery table has changed
            printDiscoveryTable(discoveryTable);

        // Refresh waiting table
        flag = 0;
        for(i=0; i<MAX_WAIT_DATA; i++)
        {
            if(waitingTable[i].valid != 0)
            {
                next = getNextHop();
                if (next != 0)  // if the request in the waiting table find a route
                {
                    senddata(&waitingTable[i].data_pkg, next);
                    waitingTable[i].valid = 0;
                    addToWaitingAckTable(&waitingTable[i].data_pkg);
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
            printWaitingTable(waitingTable);
    }
    PROCESS_END();
}


/*---------------------callback functions---------------*/
static void data_callback(struct unicast_conn *c, const linkaddr_t *from){
    static struct DATA_PACKAGE data;
    static struct ACK_PACKAGE ack;
//    char data_packet[DATA_LEN];
    static char data_packet[DATA_LEN];

    printf("\n--------Data received--------\n");

    //strncpy(data_packet, (char *)packetbuf_dataptr(), DATA_LEN);
    packetbuf_copyto(&data_packet);

    printf("Received data: %s\n", data_packet);

    if(packet2data(data_packet, &data) != 0){
    	packetbuf_clear();
        // if the destination is itself
    	packetbuf_clear();
        if(data.dest.u8[1] == linkaddr_node_addr.u8[1]){
            printf("DATA RECEIVED of src %d:\n{%s}\n with Route:{%d,%d,%d,%d,%d,%d}\n", data.src.u8[1], data.message,
            		data.route[0], data.route[1], data.route[2], data.route[3], data.route[4], data.route[5]);
        }
        // otherwise
        else{
            process_post(&data_process, PROCESS_EVENT_CONTINUE, &data);
        }

        // send ACK
        ack.id = data.id;
        ack.src = data.src;
        sendack(&ack, from->u8[1]);

    }
    // case not data package
    else{
        printf("Incorrect data package: %s\n", data_packet);
    }
    packetbuf_clear();
}

static void ack_callback(struct unicast_conn *c, const linkaddr_t *from){
    static struct ACK_PACKAGE ack;
//    char packet[ACK_LEN];
    static char ack_packet[ACK_LEN];

    printf("\n--------Ack received--------\n");

//    strncpy(packet, (char *)packetbuf_dataptr(), ACK_LEN);
    packetbuf_copyto(&ack_packet);

    printf("Received ack: %s\n", ack_packet);

    if(packet2ack(ack_packet, &ack) != 0){
    	packetbuf_clear();
    	for(int i = 0; i < MAX_WAIT_DATA; i++){
    		if(waitingTable[i].data_pkg.id == ack.id
    				&& waitingTable[i].data_pkg.src.u8[1] == ack.src.u8[1]){

    			waitingTable[i].valid = 0;
    			printWaitingTable(waitingTable);
    		}
    	}
    }
    // case not ack package
    else{
        printf("Incorrect ack package: %s\n", ack_packet);
    }
    packetbuf_clear();
}

/**
 * called when receive a packet on REP_CHANNEL
 */
static void reply_callback(struct unicast_conn *c, const linkaddr_t *from)
{
//	char packet[REP_LEN];
//    void *packet;
	static char rep_packet[] = "REPLY;ID:00;SRC:0;DEST:0;HOP:0;RSSI:000";
    static struct REP_PACKAGE rep;
    static int i;

    printf("\n--------Reply received--------\n");

    //strncpy(packet, (char *)packetbuf_dataptr(), REP_LEN);
    packetbuf_copyto(&rep_packet);

    printf("Received reply: %s\n", rep_packet);

    // if REPLY package received
    if(packet2rep(rep_packet, &rep)!=0)
    {
    	//packetbuf_clear();
        printf("REPLY received from %d [ID:%d, Dest:%d, Src:%d, Hops:%d, RSSI: %d]\n",
                        from->u8[1], rep.id, rep.dest.u8[1], rep.src.u8[1], rep.hops, rep.rssi);

        // check if reply table updates
        if(updateRoutingTable(&rep, from))
        {

            // if the source is not me forward reply to all nodes waiting
            if(rep.src.u8[1] != linkaddr_node_addr.u8[1])
            {
            	printf("the source is not me, so i am sending reply to:%d\n",rep.src.u8[1]);
                rep.hops = rep.hops + 1;
                for(i=0; i<MAX_TABLE_SIZE; i++){
                    if(discoveryTable[i].valid != 0
                        && discoveryTable[i].id == rep.id){
                            sendrep(&rep, discoveryTable[i].snd.u8[1]);
                            printf("sending reply to %d\n",discoveryTable[i].snd.u8[1]);
                    }
                }
            }
            clearDiscovery(&rep);
        }
    }
    packetbuf_clear();
}


/**
 * called when receive a packet on REQ_CHANNEL
 */
static void request_callback(struct broadcast_conn *c, const linkaddr_t *from)
{
	static struct DISCOVERY_TABLE req_info;
    static struct REQ_PACKAGE req;
    static struct REP_PACKAGE rep;
    static char req_packet[] = "REQUEST;ID:00;SRC:0;DEST:0";
    //static char packet[REQ_LEN];

    printf("\n--------Request received--------\n");
    packetbuf_copyto(&req_packet);
    //strncpy(packet, (char *)packetbuf_dataptr(), REQ_LEN);
    printf("Received request: %s\n", req_packet);

    // case REQUEST package received
    if(packet2req(req_packet, &req) != 0)
    {
    	packetbuf_clear();
        printf("ROUTE_REQUEST received from %d [ID:%d, Dest:%d, Src:%d]\n",
                        from->u8[1], req.id, req.dest.u8[1], req.src.u8[1]);

        // case destination is me
        if(req.dest.u8[1] == linkaddr_node_addr.u8[1])
        {
            rep.id = req.id;
            rep.src = req.src;
            rep.dest = req.dest;
            rep.hops = 0;
            rep.rssi = INF_RSSI;

            //sends a REPLY to the Request sender
            sendrep(&rep, from->u8[1]);

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
        packetbuf_clear();
    }

    packetbuf_clear();
}

/*---------------------table functions---------------*/
/**
 * add entry to discovery table
 */
static void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info)
{
    for(int i=0; i<MAX_TABLE_SIZE; i++){
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
    printDiscoveryTable(discoveryTable);
}

/**
 * Add the data to the waiting table
 */
void addToWaitingTable(struct DATA_PACKAGE *data){
	for(int i=0; i<MAX_WAIT_DATA; i++) {
	        if(waitingTable[i].data_pkg.src.u8[1] == data->src.u8[1]
	        		&& waitingTable[i].data_pkg.id == data->id
					&& waitingTable[i].valid == 1) {
	            return;
	        }
	    }
    for(int i=0; i<MAX_WAIT_DATA; i++) {
        if(waitingTable[i].valid == 0) {
        	printf("Add data into (route) waiting table.\n");
            waitingTable[i].data_pkg = *data;
            waitingTable[i].age = MAX_QUEUEING_TIME;
            waitingTable[i].valid = 1;
            printWaitingTable(waitingTable);
            return;
        }
    }
    printf("There is no more space in waiting table.\n");
}

/**
 * Add the data to the waiting table waiting for acknowledge
 */
void addToWaitingAckTable(struct DATA_PACKAGE *data){
	for(int i=0; i<MAX_WAIT_DATA; i++) {
		if(waitingTable[i].data_pkg.src.u8[1] == data->src.u8[1]
				&& waitingTable[i].data_pkg.id == data->id
				&& waitingTable[i].valid == 1) {
			return;
		}
	}
    for(int i=0; i<MAX_WAIT_DATA; i++) {
        if(waitingTable[i].valid == 0) {
        	printf("Add data into ack waiting table.\n");
            waitingTable[i].data_pkg = *data;
            waitingTable[i].age = MAX_ACK_WAIT_TIME;
            waitingTable[i].valid = 1;
            //printWaitingTable(waitingTable);
            return;
        }
    }
    printf("There is no more space in waiting table.\n");
    return;
}

/**
 * 	If better route is found, update routing table and return 1
 * 	Otherwise return 0
 */
static char updateRoutingTable(struct REP_PACKAGE *rep, const linkaddr_t *from)
{

    int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
    printf("rssi:%d\n",rssi);
    rssi = abs(rssi);

    rep->rssi = (rep->rssi * rep->hops + rssi) / (rep->hops + 1);

    //if the ROUTE_REPLY received shows a better path
    if(rep->rssi < routingTable.rssi)
    {
        //UPDATES the routing discovery table!
        routingTable.dest = rep->dest;
        routingTable.hops = rep->hops;
        routingTable.next.u8[1] = from->u8[1];
        routingTable.age = MAX_ROUTE_TIME;
        routingTable.valid = 1;
        routingTable.rssi = rep->rssi;
        printRoutingTable(routingTable);
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
            && discoveryTable[i].src.u8[1] == rep->src.u8[1]        // enable for ditinguish on reply id
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

	/* Configure the ADC ports */
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 | ZOUL_SENSORS_ADC3);

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

/*---------------------transmit functions---------------*/
/**
 *  Send data
 */
static void senddata(struct DATA_PACKAGE *data, int next){
    static char packet[DATA_LEN];

    static linkaddr_t next_addr;
    next_addr.u8[1]=next;
    next_addr.u8[0]=0x00;

    printf("\n--------Data sending--------\n");

    data2packet(data, packet);
    printf("Send data packet: %s\n", packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, DATA_LEN);
    unicast_send(&data_conn, &next_addr);
    //packetbuf_clear();

    printf("Sending DATA {%s} to %d via %d \n",
            data->message, data->dest.u8[1], next);
}

/**
 * broadcast the request
 */
static void sendreq(struct REQ_PACKAGE* req)
{
    static char packet[] = "REQUEST;ID:00;SRC:0;DEST:0";

    printf("\n--------Request sending--------\n");

    req2packet(req, packet);
    printf("Send request packet: %s\n", packet);
    //printf("size of req is % d\n", sizeof(req));
    //printf("size of struct REQ_PACKAGE is %d\n", sizeof(struct REQ_PACKAGE));
    packetbuf_clear();
    packetbuf_copyfrom(packet, REQ_LEN);
    broadcast_send(&req_conn);
    //packetbuf_clear();

    printf("Broadcasting Request to %d with ID:%d and Source:%d\n",req->dest.u8[1], req->id, req->src.u8[1]);

}

/**
 * send the REPLY message
 */
static void sendrep(struct REP_PACKAGE* rep, int next)
{
    static char packet[REP_LEN];

    static linkaddr_t to_rimeaddr;
    to_rimeaddr.u8[1]=next;
    to_rimeaddr.u8[0]=0;

    printf("\n--------Reply sending--------\n");

    rep2packet(rep, packet);
    printf("Send reply packet: %s\n", packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, REP_LEN);
    unicast_send(&rep_conn, &to_rimeaddr);
    //packetbuf_clear();
    printf("to_rimeaddr is now:%d\n", to_rimeaddr.u8[1]);

    printf("Sending ROUTE_REPLY toward %d via %d [ID:%d, Dest:%d, Src:%d, Hops:%d]\n",
            rep->src.u8[1], next, rep->id, rep->dest.u8[1], rep->src.u8[1], rep->hops);
}

static void sendack(struct ACK_PACKAGE *ack, int pre){
	static char packet[ACK_LEN];

	static linkaddr_t pre_addr;
	pre_addr.u8[1]=pre;
	pre_addr.u8[0]=0;

	printf("\n--------Ack sending--------\n");

	ack2packet(ack, packet);
	printf("Send ack packet: %s\n", packet);
	packetbuf_clear();
	packetbuf_copyfrom(packet, ACK_LEN);
	unicast_send(&ack_conn, &pre_addr);
	//packetbuf_clear();

	printf("Sending ACK to %d\n", pre);
}




