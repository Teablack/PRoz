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
                    debug("Dostałem REQUEST OD %d", pakiet.src);
                    queue_add(&desk_queue,create_process_s(pakiet.src,pakiet.ts, pakiet.data));
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->ts = queue_my_ts(&desk_queue,rank);
                    pkt->src = rank;
                    pkt->data = ln;
                    debug("Wysyłam ACK do %d", pakiet.src);
                    sendPacket(pkt, rank, ACK_DESK);
                    debug("Skończyłem wysyłać ACK do %d", pakiet.src);
                }
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
