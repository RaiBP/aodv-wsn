#include "struct.h"
#include "net/rime/rime.h"

#ifndef SEND_FUNC_H
#define SEND_FUNC_H

void senddata(struct DATA_PACKAGE *data, int next, struct unicast_conn data_conn);

#endif
