//#include "struct.h"
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//
///*----------struct to packet------*/
//void data2packet(DATA_PACKAGE* data, char* packet){
//    sprintf(packet, DATA, data->id, data->src.u8[1], data->dest.u8[1], data->message,
//    		data->route[0], data->route[1], data->route[2], data->route[3], data->route[4], data->route[5]);
//}
//void ack2packet(ACK_PACKAGE* ack, char* packet){
//	sprintf(packet, ACK, ack->id, ack->src.u8[1]);
//}
//void req2packet(struct REQ_PACKAGE* req, char* packet){
//	sprintf(packet, REQ, req->id, req->src.u8[1], req->dest.u8[1]);
//}
//void rep2packet(struct REP_PACKAGE* rep, char* packet){
//	sprintf(packet, REP, rep->id, rep->src.u8[1], rep->dest.u8[1], rep->hops, rep->rssi);
//}
//
///*----------packet to struct------*/
//int packet2data(char* package, DATA_PACKAGE* data){
//	static char id[] = "00";
//	static char src[] = "0";
//	static char dest[] = "0";
//	printf("--------packet2data--------\n");
//	if(strncmp(package, "DATA", 4) == 0){
//		printf("Is data!\n");
//		printf("data package: %s\n",package);
//		// id
//		id[0] = package[8];
//		id[1] = package[9];
//		data->id = atoi(id);
//		//src
//		src[0] = package[15];
//		data->src.u8[1] = atoi(src);
//		// dest
//		dest[0] = package[22];
//		data->dest.u8[1] = atoi(dest);
//		// route
//		data->route[0] = package[71] - 48;
////		printf("packet to data route[0] is: %d, package[71] is : %d\n",data->route[0], package[71]);
//		data->route[1] = package[73] - 48;
//		data->route[2] = package[75] - 48;
//		data->route[3] = package[77] - 48;
//		data->route[4] = package[79] - 48;
//		data->route[5] = package[81] - 48;
//		//message
//		strncpy(data->message, package+32, DATA_PAYLOAD_LEN - 5);
//
//	    return 1;
//	}
//	printf("Not data!\n");
//	return 0;
//}
//
//int packet2ack(char* package, ACK_PACKAGE* ack){
//	static char id[] = "00";
//	static char src[] = "0";
//	printf("--------packet2ack--------\n");
//	if(strncmp(package, "ACK", 3) == 0){
//		printf("Is ack!\n");
//		// id
//		id[0] = package[7];
//		id[1] = package[8];
//		ack->id = atoi(id);
//		//src
//		src[0] = package[14];
//		ack->src.u8[1] = atoi(src);
//		return 1;
//	}
//	printf("Not ack!\n");
//	return 0;
//
//}
//
//int packet2req(char* package, struct REQ_PACKAGE* req){
//	static char id[] = "00";
//	static char src[] ="0";
//	static char dest[] = "0";
//	printf("--------packet2req--------\n");
//	if(strncmp(package, "REQUEST", 7) == 0){
//		printf("Is request!\n");
//		// id
//		id[0] = package[11];
//		id[1] = package[12];
//		req->id = atoi(id);
//		//src
//		src[0] = package[18];
//		req->src.u8[1] = atoi(src);
//		//dest
//		dest[0] = package[25];
//		req->dest.u8[1] = atoi(dest);
//		return 1;
//	}
//	printf("Not request!\n");
//	return 0;
//
//}
//
//int packet2rep(char* package, struct REP_PACKAGE* rep){
//	static char id[] = "00";
//	static char src[] = "0";
//	static char dest[] = "0";
//	static char hop[] = "0";
//	static char rssi[] = "000";
//	printf("--------packet2rep--------\n");
//	if(strncmp(package, "REPLY", 5) == 0){
//		printf("Is reply!\n");
//		// id
//		id[0] = package[9];
//		id[1] = package[10];
//		rep->id = atoi(id);
//		//src
//		src[0] = package[16];
//		//int source = atoi(src);
//		rep->src.u8[0] = 0x00;
//		rep->src.u8[1] = atoi(src);
//		//dest
//		dest[0] = package[23];
//		rep->dest.u8[1] = atoi(dest);
//		//hop
//		hop[0] = package[29];
//		rep->hops = atoi(hop);
//		//rssi
//		rssi[0] = package[36];
//		rssi[1] = package[37];
//		rssi[2] = package[38];
//		rep->rssi = atoi(rssi);
//
//		return 1;
//	}
//	printf("Not reply!\n");
//	return 0;
//
//}
