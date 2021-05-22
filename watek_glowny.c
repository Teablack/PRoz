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
            pkt->data = ln;

            desk_queue_clear();
            desk_queue_add(rank, lclock, ln);

            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, REQUEST_FOR_DESK);
            }

            debug("Skonczylem wysylac");
           
            debug("Zmieniam stan na WAITING_TO_DISCUSS");
            changeState(WAITING_TO_DISCUSS);
        }
        else if(stan==WAITING_TO_DISCUSS){   
            while(queue_size(&desk_queue)<size){        //po requescie/ack od kazdego //sprawdzac timestamp RELEASE na wszelki > nasz REQUEST
               //czekam aż drugi wątek mnie obudzi?
            }
            debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE");
            desk_queue_print();
            while(desk_queue_free()<ln) {
                //czekam aż drugi wątek mnie obudzi?
            }
            debug("Wchodze do sekcji krytycznej (stan DISCUSSION)");
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
