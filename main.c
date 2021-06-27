#include "contiki.h"
#include "random.h"

#include "net/rime/rime.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/adc-zoul.h"

#include "project_conf.h"
#include "print_func.h"
#include "struct.h"
#include <stdio.h>		// For printf

/*callback functions*/
static void reply_callback(struct unicast_conn *, const linkaddr_t *);
static void request_callback(struct broadcast_conn *, const linkaddr_t *);


PROCESS(request_process, "Handle REQ_PACKAGE messages");
PROCESS(aging_process, "Decide the expiration of tables");


AUTOSTART_PROCESSES(&request_process, &aging_process);



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
    static int dest;

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
                //dest = waitingTable[i].data_pkg.dest;
                next = getNext(waitingTable[i].data_pkg.dest);
                if (next != 0)  // if the request in the waiting table find a route
                {
                    senddata(&waitingTable[i].data_pkg, next);
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


/*CALLBACKS FUNCTIONS*/

/**
 * called when receive a packet on REP_CHANNEL
 */
static void reply_callback(struct unicast_conn *c, const linkaddr_t *from)
{
    static char *packet;
    packetbuf_copyto(packet);
    static struct REP_PACKAGE rep;
    static int i;

    // if REPLY package received
    if(packet2rep(packet, &rep)!=0)
    {
        printf("REPLY received from %d [ID:%d, Dest:%d, Src:%d, Hops:%d, RSSI: %d]\n",
                        from->u8[1], rep.id, rep.dest, rep.src, rep.hops, rep.rssi);

        // check if reply table updates
        if(updateTables(&rep, *from))
        {
            // if the source is not me forward reply to all nodes waiting
            if(rep.src.u8[1] != linkaddr_node_addr.u8[1])
            {
                rep.hops = rep.hops + 1;
                for(i=0; i<MAX_TABLE_SIZE; i++){
                    if(discoveryTable[i].valid != 0
                        && discoveryTable[i].id == rep.id){
                            sendrep(&rep, discoveryTable[i].snd.u8[1]);
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
    static char *packet;
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
    }

}








