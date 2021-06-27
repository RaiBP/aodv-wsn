#include "contiki.h"

#ifndef PROJECT_CONF_H
#define PROJECT_CONF_H


/*INF SETTING*/
#define INF_HOPS 50                 // Infinite of hops
#define INF_RSSI 999                 // Infinite of rssi

/*MAXTIME STTING*/
#define ROUTE_DISCOVERY_TIME 5   		// maximum time to obtain route to a destination
//#define ROUTE_EXPIRATION_TIME 90   	// maximum time a route entry is considered valid
#define DATA_DELTA_TIME 20   			// Sending data per 20 seconds
#define MAX_QUEUEING_TIME 10    		// Maximum time for a data package to remain in the queue before being discarded

/*SIZES OF DATA*/
#define DATA_PAYLOAD_LEN 37     // length of payload in data packages
#define TEMPERATURE_PAYLOAD_LEN 6    // length of temperature payload
#define LUX_PAYLOAD_LEN 5      // length of lux value payload

/*NODES*/
#define MAX_NODES 7     // total number of nodes
#define MAX_WAIT_DATA 10    // Maximum data packages waiting to be sent
#define DISCO_SIZE MAX_NODES*MAX_NODES

/*CHANNELS*/
#define RREQ_CHANNEL 15
#define RREP_CHANNEL 16
#define DATA_CHANNEL 17
#define ACK_CHANNEL 18

/*DESTINATION*/
#define DESTINATION 0x01

/*OUTPUT*/
#define DATA_PAY "Temperature: %d.\nLuminance: %d."

#endif  // PROJECT_CONF_H

