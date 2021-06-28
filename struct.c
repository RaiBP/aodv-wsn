#include "struct.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*----------struct to packet------*/
void data2packet(struct DATA_PACKAGE* data, char* packet){
    sprintf(packet, DATA, data->id, data->src.u8[1], data->dest.u8[1], data->message);
}
void ack2packet(struct ACK_PACKAGE* ack, char* packet){
	sprintf(packet, ACK, ack->id, ack->src.u8[1]);
}
void req2packet(struct REQ_PACKAGE* req, char* packet){
	sprintf(packet, REQ, req->id, req->src.u8[1], req->dest.u8[1]);
}
void rep2packet(struct REP_PACKAGE* rep, char* packet){
	sprintf(packet, REP, rep->id, rep->src.u8[1], rep->dest.u8[1], rep->hops, rep->rssi);
}

/*----------packet to struct------*/
int packet2data(char* package, struct DATA_PACKAGE* data){
	static char id[2];
	static char src[1];
	static char dest[1];
	printf("--------packet2data--------\n");
	if(strncmp(package, "DATA", 4) == 0){
		printf("Is data!\n");
		// id
		id[0] = package[8];
		id[1] = package[9];
		data->id = atoi(id);
		//src
		src[0] = package[15];
		data->src.u8[1] = atoi(src);
		// dest
		dest[0] = package[22];
		data->dest.u8[1] = atoi(dest);
		//message
		strncpy(data->message, package+32, DATA_PAYLOAD_LEN);

	    return 1;
	}
	printf("Not data!\n");
	return 0;
}

int packet2ack(char* package, struct ACK_PACKAGE* ack){
	static char id[2];
	static char src[1];
	printf("--------packet2ack--------\n");
	if(strncmp(package, "ACK", 3) == 0){
		printf("Is ack!\n");
		// id
		id[0] = package[7];
		id[1] = package[8];
		ack->id = atoi(id);
		//src
		src[0] = package[14];
		ack->src.u8[1] = atoi(src);

		return 1;
	}
	printf("Not ack!\n");
	return 0;

}

int packet2req(char* package, struct REQ_PACKAGE* req){
	static char id[2];
	static char src[1];
	static char dest[1];
	printf("--------packet2req--------\n");
	if(strncmp(package, "REQUEST", 7) == 0){
		printf("Is request!\n");
		// id
		id[0] = package[11];
		id[1] = package[12];
		req->id = atoi(id);
		//src
		src[0] = package[18];
		req->src.u8[1] = atoi(src);
		//dest
		dest[0] = package[25];
		req->dest.u8[1] = atoi(dest);

		return 1;
	}
	printf("Not request!\n");
	return 0;

}

int packet2rep(char* package, struct REP_PACKAGE* rep){
	static char id[2];
	static char src[1];
	static char dest[1];
	static char hop[1];
	static char rssi[3];
	printf("--------packet2rep--------\n");
	if(strncmp(package, "REPLY", 5) == 0){
		printf("Is reply!\n");
		printf("package is %s\n", package);
		// id
		id[0] = package[9];
		id[1] = package[10];
		rep->id = atoi(id);
		printf("9.item of package is %s, 10.item of package is:%s\n", package[9], package[10]);
		printf("id is %d\n", rep->id);
		//src
		src[0] = package[16];
		int source = atoi(src);
		rep->src.u8[0] = 0xFF;
		rep->src.u8[1] = atoi(src);
		printf("src[0] %s\n", src[0]);
		printf("source is %d\n", source);
		printf("16.item of package is %s\n", package[16]);
		printf("scr.u8[1] is %d\n", rep->src.u8[1]);
		//dest
		dest[0] = package[23];
		rep->dest.u8[1] = atoi(dest);
		printf("25.item of package is %s\n", package[25]);
		printf("dest.u8[1] is %d\n", rep->dest.u8[1]);
		//hop
		hop[0] = package[29];
		rep->hops = atoi(hop);
		//rssi
		rssi[0] = package[36];
		rssi[1] = package[37];
		rssi[2] = package[38];
		rep->rssi = atoi(rssi);

		return 1;
	}
	printf("Not reply!\n");
	return 0;

}
