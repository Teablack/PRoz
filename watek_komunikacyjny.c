#include "main.h"
#include "watek_komunikacyjny.h"
#include "structs.h"
#include "queue.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    packet_t pakiet;
    state_t stan_cp;
    debug("..ja zyje...");
    while(TRUE){

        pthread_mutex_lock(&stateMut);

        stan_cp = stan;

        pthread_mutex_unlock(&stateMut);
        
        while(
            (stan_cp == WAITING_TO_DISCUSS) 
            || (stan_cp == WAITING_FOR_ROOM) 
            || (stan_cp == WAITING_FOR_STARTING_FIELD) 
            || (stan_cp == WAITING_FOR_ONE_DESK) 
        ){
            debug("..czekam na recv...");
            MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  //tu typy wiadomosci
            setClock(pakiet.ts+1);
            packet_t *pkt = malloc(sizeof(packet_t));
            switch(status.MPI_TAG){

                case REQUEST_FOR_DESK: 

                    if(stan_cp == WAITING_TO_DISCUSS){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        desk_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        
                        pkt->qts = desk_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = ln;
                        
                        debug("Wysyłam ACK do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_DESK);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if((stan_cp == WAITING_FOR_ROOM) 
                    ||(stan_cp == WAITING_FOR_STARTING_FIELD)
                    ){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        debug("Wysyłam ACK do %d", pakiet.src);

                        //packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stan_cpie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_DESK);
                        debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                    }
                    else if(stan_cp == WAITING_FOR_ONE_DESK){
                            //pthread_mutex_unlock(&stateMut);
                       // if(pakiet.ts > desk_queue_my_ts()){                     //poprawka - przyjmuje wiad wyslane po moim requescie
                            debug("Dostałem REQUEST OD %d", pakiet.src);
                            desk_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                            
                            //packet_t *pkt = malloc(sizeof(packet_t));
                            pkt->qts = desk_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                            pkt->data = 1;
                            
                            debug("Wysyłam ACK do %d", pakiet.src);
                            sendPacket(pkt, pakiet.src, ACK_DESK);
                            debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                        //}
                    }
                break;
                case ACK_DESK: 
                    if((stan_cp == WAITING_TO_DISCUSS)||(stan_cp == WAITING_FOR_ONE_DESK)){
                        //pthread_mutex_unlock(&stateMut);
                        if(pakiet.ts > desk_queue_my_ts()){ 
                            debug("Dostałem ACK OD %d", pakiet.src);
                            debug(" Dostałem juz: %d odpowiedzi ",desk_queue_size());
                            desk_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                        }
                    }
                break;
                case RELEASE_DESK: 
                    if((stan_cp == WAITING_TO_DISCUSS)||(stan_cp == WAITING_FOR_ONE_DESK)){
                        //pthread_mutex_unlock(&stateMut);
                        if(pakiet.qts > desk_queue_my_ts()){ 
                            debug("Dostałem RELEASE OD %d", pakiet.src);
                            desk_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                        }
                    }
                break;
                case REQUEST_FOR_ROOM: 
                    
                    if((stan_cp == WAITING_TO_DISCUSS)
                    || (stan_cp == WAITING_FOR_STARTING_FIELD)
                    || (stan_cp == WAITING_FOR_ONE_DESK)
                    ){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST FOR ROOM OD %d  ", pakiet.src);
                        //packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stan_cpie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_ROOM);
                        debug("Skończyłem wysyłać ACK ROOM do %d", pakiet.src);
                    }
                    else if(stan_cp == WAITING_FOR_ROOM){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST FOR ROOM OD %d", pakiet.src);
                        room_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        //packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = room_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = 1;
                        
                        debug("Wysyłam ACK ROOM do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_ROOM);
                        debug("Skończyłem wysyłać ACK ROOM do %d", pakiet.src);
                    }
                break;
                case ACK_ROOM: 
                    if(stan_cp == WAITING_FOR_ROOM){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem ACK ROOM OD %d", pakiet.src);
                        debug(" Dostałem juz: %d odpowiedzi ",room_queue_size());
                        room_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                    }
                break;
                case RELEASE_ROOM: 
                    if(stan_cp == WAITING_FOR_ROOM){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem RELEASE ROOM OD %d", pakiet.src);
                        room_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    
                    }
                break;
                case REQUEST_FOR_STARTING_FIELD: 
                    if(stan_cp == WAITING_FOR_STARTING_FIELD){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST OD %d", pakiet.src);
                        field_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                        
                        //packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->qts = field_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                        pkt->data = 1;
                        
                        debug("Wysyłam ACK FIELD do %d", pakiet.src);
                        sendPacket(pkt, pakiet.src, ACK_STARTING_FIELD);
                        debug("Skończyłem wysyłać ACK FIELD do %d", pakiet.src);
                    }
                    else if((stan_cp == WAITING_FOR_ROOM)
                    || (stan_cp == WAITING_FOR_ONE_DESK)
                    || (stan_cp == WAITING_TO_DISCUSS)
                    ){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem REQUEST FOR START FIELD OD %d ", pakiet.src);
                        //packet_t *pkt = malloc(sizeof(packet_t));
                        pkt->data = 0;      //nie potrzebuje biurek w tym stan_cpie
                        pkt->qts = lclock;
                        sendPacket(pkt, pakiet.src, ACK_STARTING_FIELD);
                        debug("Skończyłem wysyłać ACK FIELD do %d", pakiet.src);
                    }
                break;
                case ACK_STARTING_FIELD: 
                    if(stan_cp == WAITING_FOR_STARTING_FIELD){
                        //pthread_mutex_unlock(&stateMut);
                        debug("Dostałem ACK FIELD %d", pakiet.src);
                        debug(" Dostałem juz: %d odpowiedzi ",field_queue_size());
                        field_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                        
                    }
                break;
                case RELEASE_STARTING_FIELD: 
                    if(stan_cp == WAITING_FOR_STARTING_FIELD){
                        //pthread_mutex_unlock(&stateMut);
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
