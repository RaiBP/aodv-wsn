
#include "contiki.h"
#include "core/net/linkaddr.h"
#include "net/rime/rime.h"
#include "project_conf.h"
#include "struct.h"

#ifndef PRINT_FUNC_H
#define PRINT_FUNC_H


/*print functions*/
void printDiscoveryTable(struct DISCOVERY_TABLE *discoveryTable);
void printRoutingTable(struct ROUTING_TABLE routingTable);
void printWaitingTable(struct WAITING_TABLE *waitingTable);
void printWaitingackTable(struct WAITING_ACK_TABLE *waitingackTable);

#endif

