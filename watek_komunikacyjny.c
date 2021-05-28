#include "main.h"
#include "watek_komunikacyjny.h"
#include "structs.h"
#include "queue.h"
/*
#define REQUEST_FOR_DESK 1
#define REQUEST_FOR_ROOM 2
#define REQUEST_FOR_STARTING_FIELD 3
#define RELEASE_DESK 4
#define RELEASE_ROOM 5
#define RELEASE_STARTING_FIELD 6
#define ACK_DESK 7
#define ACK_ROOM 8
#define ACK_STARTING_FIELD 9
*/

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message= FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while(TRUE){
        while(
            (stan !=INIT) 
            &&(stan !=DISCUSSION) 
            &&(stan !=THE_BIG_LIE) 
            &&(stan !=BIG_BOOM) 
            &&(stan !=EXPLANATION)
        ){
            debug("czekam na recv");
            MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  //tu typy wiadomosci
            setClock(pakiet.ts+1);
            switch(status.MPI_TAG){

                case REQUEST_FOR_DESK: 

                    if(stan == WAITING_TO_DISCUSS){
                    
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        desk_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = desk_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = ln;
                        
                        debug("Wysyłam ACK do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_DESK);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if((stan == WAITING_FOR_ROOM) 
                    ||(stan == WAITING_FOR_STARTING_FIELD)
                    ){
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        debug("Wysyłam ACK do %d", pakiet.src);
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stanie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_DESK);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if(stan == WAITING_FOR_ONE_DESK){
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        desk_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = desk_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = 1;
                        
                        debug("Wysyłam ACK do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_DESK);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                break;
                case ACK_DESK: 
                    if((stan == WAITING_TO_DISCUSS)||(stan == WAITING_FOR_ONE_DESK)){
                        debug("Dostałem ACK OD %d", pakiet.src);
                        desk_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                    }
                break;
                case RELEASE_DESK: 
                    if((stan == WAITING_TO_DISCUSS)||(stan == WAITING_FOR_ONE_DESK)){
                        debug("Dostałem RELEASE OD %d", pakiet.src);
                        desk_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    }
                break;
                case REQUEST_FOR_ROOM: 
                    
                    if((stan == WAITING_TO_DISCUSS)
                    || (stan == WAITING_FOR_STARTING_FIELD)
                    || (stan == WAITING_FOR_ONE_DESK)
                    ){
                        debug("Dostałem REQUEST FOR ROOM OD %d  ", pakiet.src);
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stanie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_ROOM);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if(stan == WAITING_FOR_ROOM){
                        
                        debug("Dostałem REQUEST FOR ROOM OD %d", pakiet.src);
                        room_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = room_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = 1;
                        
                        debug("Wysyłam ACK ROOM do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_ROOM);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                break;
                case ACK_ROOM: 
                    if(stan == WAITING_FOR_ROOM){
                        debug("Dostałem ACK ROOM OD %d", pakiet.src);
                        room_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                        
                    }
                break;
                case RELEASE_ROOM: 
                    if(stan == WAITING_FOR_ROOM){
                        debug("Dostałem RELEASE ROOM OD %d", pakiet.src);
                        room_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    
                    }
                break;
                case REQUEST_FOR_STARTING_FIELD: 
                    if(stan == WAITING_FOR_STARTING_FIELD){
                    
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        field_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = field_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = 1;
                        
                        debug("Wysyłam ACK do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_STARTING_FIELD);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if((stan == WAITING_FOR_ROOM)
                    || (stan == WAITING_FOR_ONE_DESK)
                    || (stan == WAITING_TO_DISCUSS)
                    ){
                        debug("Dostałem REQUEST FOR START FIELD OD %d ", pakiet.src);
                        packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stanie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_STARTING_FIELD);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                break;
                case ACK_STARTING_FIELD: 
                    if(stan == WAITING_FOR_STARTING_FIELD){
                        debug("Dostałem ACK FIELD %d", pakiet.src);
                        field_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                        
                    }
                break;
                case RELEASE_STARTING_FIELD: 
                    if(stan == WAITING_FOR_STARTING_FIELD){
                        debug("Dostałem RELEASE FIELD OD %d", pakiet.src);
                        field_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    
                    }
                break;
                default:
                break;
            }
        }
    }
}
