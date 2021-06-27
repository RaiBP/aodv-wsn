#include "print_func.h"


void printWaitingTable(struct WAITING_TABLE *waitingTable){

    int flag = 0;

    printf("Waiting Table");
    for(int i = 0; i < MAX_WAIT_DATA; i++){
        if(waitingTable[i].valid != 0)
        {
            printf("\n\t{Src:%d; Dest:%d; Age:%d;}",
            		waitingTable[i].data_pkg.src.u8[1],
            		waitingTable[i].data_pkg.dest.u8[1],
            		waitingTable[i].age);
            flag++;
        }
    }

    if(flag==0){
        printf(" is empty \n");
    }
    else{
        printf("\n");
    }

}
