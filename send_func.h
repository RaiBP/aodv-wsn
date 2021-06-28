#include "struct.h"
#include "net/rime/rime.h"

#ifndef SEND_FUNC_H
#define SEND_FUNC_H

void senddata(struct DATA_PACKAGE *data, int next, struct unicast_conn data_conn);
void sendreq(struct REQ_PACKAGE* req, struct broadcast_conn req_conn);
void sendrep(struct REP_PACKAGE* rep, int next, struct unicast_conn rep_conn);

#endif
