#include "print_func.h"
#include <stdio.h>


/**
 * print the Discovery Table
 */
void printDiscoveryTable(struct DISCOVERY_TABLE *discoveryTable)
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
                    discoveryTable[i].src.u8[1],
                    discoveryTable[i].dest.u8[1],
                    discoveryTable[i].snd.u8[1]);
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
void printRoutingTable(struct ROUTING_TABLE routingTable)
{
    char flag = 0;

    printf("Routing Table");

    if(routingTable.valid!= 0)
    {
    	printf("\n   {Dest:%d; Next:%d; Hops:%d; Age:%d; RSSI:%d}",
                    routingTable.dest.u8[1], routingTable.next.u8[1],
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
 * print the Waiting table
 */
void printWaitingTable(struct WAITING_TABLE *waitingTable){

    int flag = 0;

    printf("Waiting Table");
    for(int i = 0; i < MAX_WAIT_DATA; i++){
        if(waitingTable[i].valid != 0)
        {
            printf("\n\t{Src:%d; Dest:%d; Age:%d;}",
            		waitingTable[i].data_pkg.src.u8[1],
            		waitingTable[i].data_pkg.dest.u8[1],
            		waitingTable[i].age);
            flag++;
        }
    }

    if(flag==0){
        printf(" is empty \n");
    }
    else{
        printf("\n");
    }

}

