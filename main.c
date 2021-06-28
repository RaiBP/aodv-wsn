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



/*connections*/
static struct broadcast_conn req_conn;
static struct unicast_conn rep_conn;

/*send functions*/
static void sendreq(struct REQ_PACKAGE* req);
static void sendrep(struct REP_PACKAGE* rep, int next);




/*table functions*/
static void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info);
static char updateTables(struct REP_PACKAGE * rep, linkaddr_t from);
static void clearDiscovery(struct REP_PACKAGE* rep);


static char isDuplicateReq(struct REQ_PACKAGE* req);

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
    //static int dest;

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
                    //senddata(&waitingTable[i].data_pkg, next);
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
                        from->u8[1], rep.id, rep.dest.u8[1], rep.src.u8[1], rep.hops, rep.rssi);

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



/*sendfunctions*/

/**
 * broadcast the request
 */
static void sendreq(struct REQ_PACKAGE* req)
{
    static char packet[REQ_LEN];

    req2packet(req, packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, REQ_LEN);
    broadcast_send(&req_conn);

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

    rep2packet(rep, packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, REP_LEN);
    unicast_send(&rep_conn, &to_rimeaddr);

    printf("Sending ROUTE_REPLY toward %d via %d [ID:%d, Dest:%d, Src:%d, Hops:%d]\n",
            rep->src.u8[1], next, rep->id, rep->dest.u8[1], rep->src.u8[1], rep->hops);
}



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
 * get the next hop for this destination request
 */
int getNext(int dest)
{
    if (routingTable.valid != 0)
        return routingTable.next.u8[1];
    return 0;
}


static char updateTables(struct REP_PACKAGE * rep, linkaddr_t from)
{

    uint16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

    rep->rssi = (rep->rssi * rep->hops + rssi) / (rep->hops + 1);

    //if the ROUTE_REPLY received shows a better path
    if(rep->rssi > routingTable.rssi)
    {
        //UPDATES the routing discovery table!
        routingTable.dest = rep->dest;
        routingTable.hops = rep->hops;
        routingTable.next.u8[1] = from.u8[1];
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
static void clearDiscovery(struct REP_PACKAGE* rep)
{
    int i;
    for(i=0; i<MAX_TABLE_SIZE; i++){
        if(discoveryTable[i].valid != 0
            && discoveryTable[i].id == rep->id
           // && discoveryTable[i].src == rep->src        // enable for ditinguish on reply id
            && discoveryTable[i].dest.u8[1] == rep->dest.u8[1])
                discoveryTable[i].valid = 0;
                return;
    }
    return;
}


//
/**
 * check if the received Request was already in the discovery Table
 */
static char isDuplicateReq(struct REQ_PACKAGE* req)
{
    int i;
    for(i=0; i<MAX_TABLE_SIZE; i++){
        if(discoveryTable[i].valid != 0
            && discoveryTable[i].id == req->id
            && discoveryTable[i].src.u8[1] == req->src.u8[1]
            && discoveryTable[i].dest.u8[1] == req->dest.u8[1])
                return 1;
    }
    return 0;
}


