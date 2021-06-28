#include "print_func.h"

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


