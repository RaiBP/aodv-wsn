#include "send_func.h"

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
