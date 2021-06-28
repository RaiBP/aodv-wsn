
#include "contiki.h"
#include "core/net/linkaddr.h"
#include "net/rime/rime.h"
#include "project_conf.h"
#include "struct.h"

#ifndef PRINT_FUNC_H
#define PRINT_FUNC_H

/*tables define*/
static struct DISCOVERY_TABLE discoveryTable[MAX_TABLE_SIZE];
static struct ROUTING_TABLE routingTable;
static struct WAITING_TABLE waitingTable[MAX_WAIT_DATA];


/*print functions*/
void printDiscoveryTable();
void printRoutingTable();
void printWaitingTable(struct WAITING_TABLE *waitingTable);


#endif

