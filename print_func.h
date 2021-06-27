#include "contiki.h"
#include "core/net/linkaddr.h"
#include "random.h"
#include "net/rime/rime.h"
#include "project_conf.h"
#include "struct.h"


struct DISCOVERY_TABLE discoveryTable[MAX_TABLE_SIZE];
struct ROUTING_TABLE routingTable;
struct WAITING_TABLE waitingTable[MAX_WAIT_DATA];





struct broadcast_conn req_conn;
struct unicast_conn rep_conn;





/*send functions*/
void sendreq(struct REQ_PACKAGE* req);

/*table functions*/
void addEntryToDiscoveryTable(struct DISCOVERY_TABLE* req_info);
char updateTables(struct REP_PACKAGE * rep, linkaddr_t from);
void clearDiscovery(struct REP_PACKAGE* rep);


char isDuplicateReq(struct REQ_PACKAGE* req);

/*print functions*/
void printDiscoveryTable();
void printRoutingTable();
