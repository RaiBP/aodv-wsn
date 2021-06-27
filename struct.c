#include "struct.h"


/*----------struct to packet------*/
void data2packet(struct DATA_PACKAGE* data, char* packet){
    sprintf(packet, DATA, data->id, data->src, data->dest, data->message);
}
void ack2packet(struct ACK_PACKAGE* ack, char* packet){
	sprintf(packet, ACK, ack->id, ack->src);
}
void req2packet(struct REQ_PACKAGE* req, char* packet){
	sprintf(packet, REQ, req->id, req->src, req->dest);
}
void rep2packet(struct REP_PACKAGE* rep, char* packet){
	sprintf(packet, REP, rep->id, rep->src, rep->dest, rep->hops, rep->rssi);
}

/*----------packet to struct------*/
int packet2data(char* package, struct DATA_PACKAGE* data){
	static char id[2];
	static char src[1];
	static char dest[1];
	if(strncmp(package, "DATA", 4) == 0){
		// id
		id[0] = package[8];
		id[1] = package[9];
		data->id = atoi(id);
		//src
		src[0] = package[15];
		data->src = atoi(src);
		// dest
		dest[0] = package[22];
		data->dest = atoi(dest);
		//message
		strncpy(data->message, package+32, DATA_PAYLOAD_LEN);

	    return 1;
	}
	return 0;
}

int packet2ack(char* package, struct ACK_PACKAGE* ack){
	static char id[2];
	static char src[1];
	if(strncmp(package, "ACK", 3) == 0){
		// id
		id[0] = package[7];
		id[1] = package[8];
		ack->id = atoi(id);
		//src
		src[0] = package[14];
		ack->src = atoi(src);

		return 1;
	}
	return 0;

}

int packet2req(char* package, struct REQ_PACKAGE* req){
	static char id[2];
	static char src[1];
	static char dest[1];
	if(strncmp(package, "REQUEST", 7) == 0){
		// id
		id[0] = package[11];
		id[1] = package[12];
		req->id = atoi(id);
		//src
		src[0] = package[18];
		req->src = atoi(src);
		//dest
		dest[0] = package[25];
		req->dest = atoi(dest);

		return 1;
	}
	return 0;

}

int packet2rep(char* package, struct REP_PACKAGE* rep){
	static char id[2];
	static char src[1];
	static char dest[1];
	static char hop[1];
	static char rssi[2];
	if(strncmp(package, "REPLY", 5) == 0){
		// id
		id[0] = package[9];
		id[1] = package[10];
		rep->id = atoi(id);
		//src
		src[0] = package[16];
		rep->src = atoi(src);
		//dest
		dest[0] = package[23];
		rep->dest = atoi(dest);

		return 1;
	}
	return 0;

}
