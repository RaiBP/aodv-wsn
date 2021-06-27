#include "contiki.h"
#include "core/net/linkaddr.h"

#include "project_conf.h"

#ifndef STRUCT_H
#define STRUCT_H

#define DATA "DATA;ID:%2d;SRC:%1d;DEST:%1d;PAYLOAD:%s"
#define ACK "ACK;ID:%2d;SRC:%1d"
#define REQ "REQUEST;ID:%2d;SRC:%1d;DEST:%1d"
#define REP "REPLY;ID:%2d;SRC:%1d;DEST:%1d;HOP:%1d;RSSI:%3d"

#define DATA_LEN sizeof(DATA)-1 - 7 + DATA_PAYLOAD_LEN
#define ACK_LEN sizeof(ACK)-1 - 3
#define REQ_LEN sizeof(REQ)-1 - 5
#define REP_LEN sizeof(REP)-1 - 7

/**
 * data package
 */
struct DATA_PACKAGE{
	int id;
	linkaddr_t src;
	linkaddr_t dest;
	char message[DATA_LEN];
};

/**
 * acknowledgment package
 */
struct ACK_PACKAGE{
	int id;
	linkaddr_t src;
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
    struct DATA_PACKAGE data_pkg;
    int age;
    int valid;
};

/*----------struct to packet------*/
void data2packet(struct DATA_PACKAGE* data, char* packet);
void ack2packet(struct ACK_PACKAGE* ack, char* packet);
void req2packet(struct REQ_PACKAGE* req, char* packet);
void rep2packet(struct REP_PACKAGE* rep, char* packet);

/*----------packet to struct------*/
int packet2data(char* package, struct DATA_PACKAGE* data);
int packet2ack(char* package, struct ACK_PACKAGE* ack);
int packet2req(char* package, struct REQ_PACKAGE* req);
int packet2rep(char* package, struct REP_PACKAGE* rep);


#endif
