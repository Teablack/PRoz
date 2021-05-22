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
            pkt->qts = lclock;

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
            while(desk_queue_size()<size){        //po requescie/ack od kazdego //sprawdzac timestamp RELEASE na wszelki > nasz REQUEST
               //czekam aż drugi wątek mnie obudzi?
            }
            debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE");
            desk_queue_print();
            while(desk_queue_free()<ln) {
                //czekam aż drugi wątek mnie obudzi?
            }
            //debug("Wchodze do sekcji krytycznej (stan DISCUSSION)");
            
            changeState(DISCUSSION);
        }
        else if(stan==DISCUSSION){

            packet_t *pkt1 = malloc(sizeof(packet_t));
            pkt1->data = ln;
        
            debug("Wysyłam RELEASE", ln);
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt1, i, RELEASE_DESK);
            }
            debug("Skonczylem wysylac");

            room_queue_clear();
            room_queue_add(rank, lclock, 1);
            
            packet_t *pkt2 = malloc(sizeof(packet_t));
            pkt2->data = 1;
            pkt2->qts = lclock;
            
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt2, i, REQUEST_FOR_ROOM);
            }
            debug("Skonczylem wysylac");
            debug("Zmieniam stan na WAITING_FOR_ROOM");
        
            changeState(WAITING_FOR_ROOM);
        }
        else if(stan==WAITING_FOR_ROOM){

            while(room_queue_size()<size){        //po requescie/ack od kazdego //sprawdzac timestamp RELEASE na wszelki > nasz REQUEST
               //czekam aż drugi wątek mnie obudzi?
            }
            debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE");
            room_queue_print();
            
            while(room_queue_free()<1) {
                //czekam aż drugi wątek mnie obudzi?
            }
            debug("Wchodze do sekcji krytycznej (stan THE_BIG_LIE)");
            sleep(500000);
            //changeState(THE_BIG_LIE);
        }
        else if(stan==THE_BIG_LIE){
            
            changeState(WAITING_FOR_STARTING_FIELD);
        }
        else if(stan==WAITING_FOR_STARTING_FIELD){
            
            changeState(BIG_BOOM);
        }
        else if(stan==BIG_BOOM){
            
            changeState(WAITING_FOR_ONE_DESK);
        }
        else if(stan==WAITING_FOR_ONE_DESK){
            
            changeState(EXPLANATION);
        }
        else if(stan==EXPLANATION){
            //RELEASE_DESK do wsz
            
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
