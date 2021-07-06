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
#define DATA_H_LEN sizeof(DATA_H)

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
	char route[MAX_NODES];
	uint8_t hops;
}DATA_PACKAGE;

/**
 * acknowledgment package
 */
struct ACK_PACKAGE{
	int id;
	linkaddr_t src;
	linkaddr_t dest;
};

/**
 * request package
 */
struct REQ_PACKAGE{
	int id;
	linkaddr_t src;
	linkaddr_t dest;
};

/**
 * reply package
 */
struct REP_PACKAGE{
	int id;
	linkaddr_t src;
	linkaddr_t dest;
	int hops;
	int16_t rssi;

};


/**
 * routing table
 */
struct ROUTING_TABLE{
	linkaddr_t dest;
	linkaddr_t next;
    int hops;
    int valid;
    int age;
    int16_t rssi;
};

/**
 * discovery table
 */
struct DISCOVERY_TABLE{
    int id;
    linkaddr_t src;
    linkaddr_t dest;
    linkaddr_t snd;
    int valid;
    int age;
};

/**
 * waiting table
 */
struct WAITING_TABLE{
    DATA_PACKAGE data_pkg;
    int age;
    int valid;
};

/**
 * waiting ack table
 */
struct WAITING_ACK_TABLE{
	DATA_PACKAGE data_pkg;
	int age;
	int valid;
};

/*----------struct to packet------*/
void data2packet(DATA_PACKAGE* data, char* packet);
void ack2packet(struct ACK_PACKAGE* ack, char* packet);
void req2packet(struct REQ_PACKAGE* req, char* packet);
void rep2packet(struct REP_PACKAGE* rep, char* packet);

/*----------packet to struct------*/
int packet2data(char* package, DATA_PACKAGE* data);
int packet2ack(char* package, struct ACK_PACKAGE* ack);
int packet2req(char* package, struct REQ_PACKAGE* req);
int packet2rep(char* package, struct REP_PACKAGE* rep);


#endif
