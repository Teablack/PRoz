#include "main.h"
#include "watek_glowny.h"
#include "structs.h"
#include "queue.h"

void mainLoop()
{
    while(TRUE){
        if(stan==INIT){
            debug("Wysyłam Request z przydziałem %d biurek do wszystkich", ln);
            packet_t *pkt = malloc(sizeof(packet_t));
            pkt->ts = lclock;
            pkt->src = rank;
            pkt->data = ln;
            queue_clear(&desk_queue);
            queue_add(&desk_queue,create_process_s(lclock, rank, ln));
            for(int i=0;i<size;i++)
                sendPacket(pkt, i, REQUEST_FOR_DESK);
            debug("Skonczylem wysylac");
            free_B=B;
            changeClock(1);
            debug("Zmieniam stan na WAITING_TO_DISCUSS");
            changeState(WAITING_TO_DISCUSS);
        }
        else if(stan==WAITING_TO_DISCUSS){
            //EDIT algorytmu - rozbic na dwa stany
            while(queue_size(&desk_queue)<size){
                //tu chyba zawieszam się - muszę zasypiac ? czekam aż drugi wątek mnie obudzi??
                //dalej w wątku komunikacyjnym:
                //?odbieranie po jednej REQUEST_FOR_DESK lub ACK_DESK - tu inna reakcja co do free_B ?
            }
           //musisz rozbic by nie reag na requesty - przerzucic changeState w komunikacyjny 
            while(free_B<ln) {//pozniej zadekl wylosuje ? a moze przesylac tam?
                //zawieszam sie
            }
            changeClock(1);
            changeState(DISCUSSION);
        }
        else if(stan==DISCUSSION){
            sleep(10000);
            //RELEASE_DESKS do wszystkich
            
            //REQUEST_FOR_ROOM do wsz
            //wstawiam do kolejki room_queue
            changeClock(1);
            changeState(WAITING_FOR_ROOM);
        }
        else if(stan==WAITING_FOR_ROOM){
            //tak samo jak w WAITING_TO_DISCUSS
            changeClock(1);
            changeState(THE_BIG_LIE);
        }
        else if(stan==THE_BIG_LIE){
            changeClock(1);
            changeState(WAITING_FOR_STARTING_FIELD);
        }
        else if(stan==WAITING_FOR_STARTING_FIELD){
            changeClock(1);
            changeState(BIG_BOOM);
        }
        else if(stan==BIG_BOOM){
            changeClock(1);
            changeState(WAITING_FOR_ONE_DESK);
        }
        else if(stan==WAITING_FOR_ONE_DESK){
            changeClock(1);
            changeState(EXPLANATION);
        }
        else if(stan==EXPLANATION){
            //RELEASE_DESK do wsz
            changeClock(1);
            changeState(INIT);
        }
        
    }
    // srandom(rank);
    // while (stan != InFinish) {
    //     int perc = random()%100; 

    //     if (perc<STATE_CHANGE_PROB) {
    //         if (stan==INIT) {
    //             debug("Zmieniam stan na wysyłanie");
    //             changeState( InSend );
    //             packet_t *pkt = malloc(sizeof(packet_t));
    //             pkt->data = perc;
    //             changeTallow( -perc);
    //             sleep( SEC_IN_STATE); // to nam zasymuluje, że wiadomość trochę leci w kanale
    //                                   // bez tego algorytm formalnie błędny za każdym razem dawałby poprawny wynik
    //             sendPacket( pkt, (rank+1)%size,TALLOWTRANSPORT);
    //             changeState( INIT );
    //             debug("Skończyłem wysyłać");
    //         } 
    //     }
    //     sleep(SEC_IN_STATE);
    //}   
}
