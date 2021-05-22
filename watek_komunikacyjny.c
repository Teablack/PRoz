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
	    debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  //tu typy wiadomosci
        setClock(pakiet.ts+1);
        switch(status.MPI_TAG){

            case REQUEST_FOR_DESK: 

                if(stan == WAITING_TO_DISCUSS){
                    //sleep(100);
                    //debug("Dostałem REQUEST OD %d", pakiet.src);
                    desk_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                    
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->qts = desk_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                    pkt->data = ln;
                    
                    //debug("Wysyłam ACK do %d", pakiet.src);
                    sendPacket(pkt, pakiet.src, ACK_DESK);
                    //debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                }
                else if(stan == WAITING_FOR_ROOM){
                    //sleep(100);
                    //debug("Dostałem REQUEST OD %d", pakiet.src);
                    //debug("Wysyłam ACK do %d", pakiet.src);
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->data = 0;      //nie potrzebuje biurek w tym stanie
                    pkt->qts = lclock;
                    sendPacket(pkt, pakiet.src, ACK_DESK);
                    //debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                }
            break;
            case ACK_DESK: 
                //sleep(100);
                if(stan == WAITING_TO_DISCUSS){
                    //debug("Dostałem ACK OD %d", pakiet.src);
                    desk_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                    //desk_queue_print();
                }
            break;
            case RELEASE_DESK: 
                //sleep(100);
                if(stan == WAITING_TO_DISCUSS){
                    //debug("Dostałem RELEASE OD %d", pakiet.src);
                    desk_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    //desk_queue_print();
                }
            break;
            case REQUEST_FOR_ROOM: 
                //sleep(100);
                if(stan == WAITING_TO_DISCUSS){
                    debug("Dostałem REQUEST FOR ROOM OD %d w czasie %d ze zn cz %d ", pakiet.src, pakiet.ts, pakiet.qts);
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->data = 0;      //nie potrzebuje biurek w tym stanie
                    pkt->qts = lclock;
                    sendPacket(pkt, pakiet.src, ACK_ROOM);
                    //debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                }
                else if(stan == WAITING_FOR_ROOM){
                    //sleep(100);
                    debug("Dostałem REQUEST FOR ROOM OD %d w czasie %d ze zn cz %d", pakiet.src, pakiet.ts, pakiet.qts);
                    room_queue_replace(pakiet.src, pakiet.qts, pakiet.data);
                    room_queue_print();
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->qts = room_queue_my_ts();  //uwaga ! wysyłam swoj znacznik czasowy w kolejce a nie ts
                    pkt->data = 1;
                    
                    debug("Wysyłam ACK ROOM do %d", pakiet.src);
                    sendPacket(pkt, pakiet.src, ACK_ROOM);
                    //debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                }
            break;
            case ACK_ROOM: 
                if(stan == WAITING_FOR_ROOM){
                    debug("Dostałem ACK ROOM OD %d", pakiet.src);
                    room_queue_replace(pakiet.src,pakiet.qts, pakiet.data);
                    room_queue_print();
                }
            break;
            case RELEASE_ROOM: 
                if(stan == WAITING_FOR_ROOM){
                    debug("Dostałem RELEASE ROOM OD %d", pakiet.src);
                    room_queue_replace(pakiet.src,pakiet.ts, 0);    //zero oznacza zwolnienie zasobow
                    room_queue_print();
                }
            break;
            case REQUEST_FOR_STARTING_FIELD: 
    
            break;
            case ACK_STARTING_FIELD: 
    
            break;
            case RELEASE_STARTING_FIELD: 
    
            break;
            default:
            break;
        }
        // switch(status.MPI_TAG){
        //         case FINISH: 
        //                 changeState(InFinish);
        //         break;
        //         case TALLOWTRANSPORT: 
        //                 changeTallow( pakiet.data);
        //                 debug("Dostałem wiadomość od %d z danymi %d",pakiet.src, pakiet.data);
        //         break;
        //         case GIVEMESTATE: 
        //                 pakiet.data = tallow;
        //                 sendPacket(&pakiet, ROOT, STATE);
        //                 debug("Wysyłam mój stan do monitora: %d funtów łoju na składzie!", tallow);
        //         break;
        //         case STATE:
        //                 numberReceived++;
        //                 globalState += pakiet.data;
        //                 if (numberReceived > size-1) {
        //                         debug("W magazynach mamy %d funtów łoju.", globalState);
        //                 } 
        //         break;
        //         case INMONITOR: 
        //                 changeState( InMonitor );
        //                 debug("Od tej chwili czekam na polecenia od monitora");
        //         break;
        //         case INRUN: 
        //                 changeState( INIT );
        //                 debug("Od tej chwili decyzję podejmuję autonomicznie i losowo");
        //         break;
        //         default:
        //         break;
        // }
    }
}
