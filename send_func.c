#include "send_func.h"
#include <stdio.h>

/**
 *  Send data
 */
void senddata(struct DATA_PACKAGE *data, int next, struct unicast_conn data_conn){
    static char packet[DATA_LEN];

    static linkaddr_t next_addr;
    next_addr.u8[1]=next;
    next_addr.u8[0]=0;

    data2packet(data, packet);
    packetbuf_clear();
    packetbuf_copyfrom(packet, DATA_LEN);
    unicast_send(&data_conn, &next_addr);

    printf("Sending DATA {%s} to %d via %d \n",
            data->message, data->dest.u8[1], next);
}

/**
 * broadcast the request
 */
void sendreq(struct REQ_PACKAGE* req, struct broadcast_conn req_conn)
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
void sendrep(struct REP_PACKAGE* rep, int next, struct unicast_conn rep_conn)
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
