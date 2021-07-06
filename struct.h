#include "contiki.h"
#include "core/net/linkaddr.h"

#include "project_conf.h"

#ifndef STRUCT_H
#define STRUCT_H

#define DATA "DATA;ID:%2d;SRC:%1d;DEST:%1d;PAYLOAD:%s;ROUTE:%1d,%1d,%1d,%1d,%1d,%1d"
#define ACK "ACK;ID:%2d;SRC:%1d"
#define REQ "REQUEST;ID:%2d;SRC:%1d;DEST:%1d"
#define REP "REPLY;ID:%2d;SRC:%1d;DEST:%1d;HOP:%1d;RSSI:%3d"

#define DATA_H "DATA"
#define REQ_H "REQEST"
#define REP_H "REPLY"
#define ACK_H "ACK"

#define DATA_H_LEN sizeof(DATA_H)
#define REQ_H_LEN sizeof(REQ_H)
#define REP_H_LEN sizeof(REP_H)
#define ACK_H_LEN sizeof(ACK_H)

#define DATA_LEN sizeof(DATA)-1 - 15 + DATA_PAYLOAD_LEN
#define ACK_LEN sizeof(ACK)-1 - 3
#define REQ_LEN sizeof(REQ)-1 - 5
#define REP_LEN sizeof(REP)-1 - 7

/**
 * data package
 */
typedef struct{
	char head[DATA_H_LEN];
	uint8_t id;
	linkaddr_t src;
	linkaddr_t dest;
	char message[DATA_PAYLOAD_LEN];
	linkaddr_t route[MAX_NODES];
	uint8_t hops;
}DATA_PACKAGE;

/**
 * acknowledgment package
 */
typedef struct{
	char head[ACK_H_LEN];
	uint8_t id;
	linkaddr_t src;
	linkaddr_t dest;
}ACK_PACKAGE;

/**
 * request package
 */
typedef struct{
	char head[REQ_H_LEN];
	uint8_t id;
	linkaddr_t src;
	linkaddr_t dest;
}REQ_PACKAGE;

/**
 * reply package
 */
typedef struct{
	char head[REP_H_LEN];
	uint8_t id;
	linkaddr_t src;
	linkaddr_t dest;
	uint8_t hops;
	int16_t rssi;

}REP_PACKAGE;


/**
 * routing table
 */
struct ROUTING_TABLE{
	linkaddr_t dest;
	linkaddr_t next;
    uint8_t hops;
    uint8_t valid;
    uint8_t age;
    int16_t rssi;
};

/**
 * discovery table
 */
struct DISCOVERY_TABLE{
	uint8_t id;
    linkaddr_t src;
    linkaddr_t dest;
    linkaddr_t snd;
    uint8_t valid;
    uint8_t age;
};

/**
 * waiting table
 */
struct WAITING_TABLE{
    DATA_PACKAGE data_pkg;
    uint8_t age;
    uint8_t valid;
};

/**
 * waiting ack table
 */
struct WAITING_ACK_TABLE{
	DATA_PACKAGE data_pkg;
	uint8_t age;
	uint8_t valid;
};

///*----------struct to packet------*/
//void data2packet(DATA_PACKAGE* data, char* packet);
//void ack2packet(ACK_PACKAGE* ack, char* packet);
//void req2packet(struct REQ_PACKAGE* req, char* packet);
//void rep2packet(struct REP_PACKAGE* rep, char* packet);
//
///*----------packet to struct------*/
//int packet2data(char* package, DATA_PACKAGE* data);
//int packet2ack(char* package, ACK_PACKAGE* ack);
//int packet2req(char* package, struct REQ_PACKAGE* req);
//int packet2rep(char* package, struct REP_PACKAGE* rep);


#endif
