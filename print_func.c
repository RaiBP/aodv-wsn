#include "print_func.h"





/**
 * add entry to discovery table
 */
void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info)
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
 * broadcast the request
 */
void sendreq(struct REQ_PACKAGE* req)
{
    static char packet[REQ_LEN];

    rreq2packet(req, packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, REQ_LEN);
    broadcast_send(&req_conn);

    printf("Broadcasting Request to %d with ID:%d and Source:%d\n",req->dest, req->id, req->src);
}

/**
 * send the REPLY message
 */
void sendrep(struct REP_PACKAGE* rep, int next)
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
            rep->src, next, rep->id, rep->dest, rep->src, rep->hops);
}


/**
 * print the Discovery Table
 */
void printDiscoveryTable()
{
    int i = 0;
    int flag = 0;

    printf("Discovery Table");
    for(i=0; i<MAX_TABLE_SIZE;i++)
    {
        if(discoveryTable[i].valid!= 0)
        {
            printf("\n    {ID:%d; Src:%d; Dest:%d; Snd:%d;}",
                    discoveryTable[i].id,
                    discoveryTable[i].src,
                    discoveryTable[i].dest,
                    discoveryTable[i].snd);
            flag++;
        }
    }
    if(flag==0)
        printf(" is empty \n");
    else
        printf("\n");
}


/**
 * print the Routing Table
 */
void printRoutingTable()
{
    int i;
    char flag = 0;

    printf("Routing Table");

    if(routingTable.valid!= 0)
    {
    	printf("\n   {Dest:%d; Next:%d; Hops:%d; Age:%d; RSSI:%d}",
                    routingTable.dest, routingTable.next,
                    routingTable.hops, routingTable.age,
					routingTable.rssi);
        flag ++;
    }
    if(flag==0)
        printf(" is empty\n");
    else
        printf("\n");
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


/**
 * print the Waiting table
 */
void printWaitingTable()
{
    int i, flag = 0;

    printf("Data Waiting Table");
    for(i=0; i<MAX_WAIT_DATA;i++)
    {
        if(waitingTable[i].valid!= 0)
        {
            printf("\n    {Dest:%d; Age:%d;}",
                    waitingTable[i].data_pkg.dest,
                    waitingTable[i].age);
            flag++;
        }
    }
    if(flag==0)
        printf(" is empty \n");
    else
        printf("\n");
}


char updateTables(struct REP_PACKAGE * rep, linkaddr_t from)
{

    int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

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
void clearDiscovery(struct REP_PACKAGE* rep)
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
char isDuplicateReq(struct REQ_PACKAGE* req)
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
