#include "main.h"
#include "watek_glowny.h"
#include "structs.h"
#include "queue.h"

void mainLoop()
{
    packet_t *pkt = malloc(sizeof(packet_t));
    while(TRUE){
        if(stan==INIT){
            debug("Wysyłam Request z przydziałem %d biurek do wszystkich", ln);
            
            pkt->data = ln;
            pkt->qts = lclock;

            desk_queue_clear();
            desk_queue_add(rank, lclock, ln);
        
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, REQUEST_FOR_DESK);
            }

            debug("Skonczylem wysylac REQUEST_FOR_DESK");
           
            debug("Zmieniam stan na WAITING_TO_DISCUSS");
            changeState(WAITING_TO_DISCUSS);

        }
        else if(stan==WAITING_TO_DISCUSS){   

            pthread_mutex_lock(&mainMut); 
            while(desk_queue_size()<size){
               debug(".....zasypiam...");
               pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);

            debug(".....budze sie...");
            
            pthread_mutex_lock(&mainMut); 
           
            while(desk_queue_free()<ln) {
                debug(".....zasypiam2...");
                pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie2...");
            
            debug("Wchodze do sekcji krytycznej (stan DISCUSSION)");
            changeState(DISCUSSION);
        }
        else if(stan==DISCUSSION){

            sleep(5);
            pkt->data = ln;
            pkt->ts = lclock;
            debug("Wysyłam RELEASE DESK");
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, RELEASE_DESK);
            }
            debug("Skonczylem wysylac RELEASE DESK");
            debug("Wysyłam REQUEST_FOR_ROOM");
            room_queue_clear();
            room_queue_add(rank, lclock, 1);
            
            pkt->data = 1;
            pkt->qts = lclock;
            
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, REQUEST_FOR_ROOM);
            }
            debug("Skonczylem wysylac REQUEST_FOR_ROOM");
            debug("Zmieniam stan na WAITING_FOR_ROOM");
            changeState(WAITING_FOR_ROOM);
        }
        else if(stan==WAITING_FOR_ROOM){
            sleep(4);

            pthread_mutex_lock(&mainMut);
            while(room_queue_size()<size){        //po requescie/ack od kazdego //sprawdzac timestamp RELEASE na wszelki > nasz REQUEST
               debug(".....zasypiam...");
               pthread_cond_wait(&cond, &mainMut);
            }
            //debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE ROOM");
            //room_queue_print();
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie...");
            pthread_mutex_lock(&mainMut);

            while(room_queue_free()<1) {
                debug(".....zasypiam2...");
                pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie2...");

            debug("Wchodze do sekcji krytycznej (stan THE_BIG_LIE)");
            changeState(THE_BIG_LIE);
        }
        else if(stan==THE_BIG_LIE){
            sleep(4);
            pkt->data = 0;
        
            debug("Wysyłam RELEASE_ROOM");
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, RELEASE_ROOM);
            }
            debug("Skonczylem wysylac RELEASE_ROOM");

            field_queue_clear();
            field_queue_add(rank, lclock, 1);
            debug("Wysyłam REQUEST_FOR_STARTING_FIELD");
            pkt->data = 1;
            pkt->qts = lclock;
            
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, REQUEST_FOR_STARTING_FIELD);
            }
            debug("Skonczylem wysylac REQUEST_FOR_STARTING_FIELD");
            debug("Zmieniam stan na WAITING_FOR_STARTING_FIELD");

            changeState(WAITING_FOR_STARTING_FIELD);
        }
        else if(stan==WAITING_FOR_STARTING_FIELD){
            sleep(5);
            pthread_mutex_lock(&mainMut);

            while(field_queue_size()<size){        
               debug(".....zasypiam...");
               pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie...");
            pthread_mutex_lock(&mainMut);
            //debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE FIELD");
            //field_queue_print();
            
            while(field_queue_free()<1) {
                debug(".....zasypiam2...");
                pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie2...");
            debug("Wchodze do sekcji krytycznej (stan BIG_BOOM)");
            changeState(BIG_BOOM);
        }
        else if(stan==BIG_BOOM){
            sleep(1);
            
            pkt->data = 0;
        
            debug("Wysyłam RELEASE_STARTING_FIELD");
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, RELEASE_STARTING_FIELD);
            }
            debug("Skonczylem wysylac RELEASE_STARTING_FIELD");

            desk_queue_clear();
            desk_queue_add(rank, lclock, 1);

            debug("Wysyłam REQUEST_FOR_DESK");
            pkt->data = 1;
            pkt->qts = lclock;
            
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, REQUEST_FOR_DESK);
            }
            debug("Skonczylem wysylac REQUEST_FOR_DESK");
            debug("Zmieniam stan na WAITING_FOR_ONE_DESK");

            changeState(WAITING_FOR_ONE_DESK);
        }
        else if(stan==WAITING_FOR_ONE_DESK){
            pthread_mutex_lock(&mainMut);
            while(desk_queue_size()<size){ 
                debug(".....zasypiam...");
                pthread_cond_wait(&cond, &mainMut);       
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie...");
            pthread_mutex_lock(&mainMut);
            debug("PO PIERWSZYCH REQUESTACH/ACK/RELEASE ONE DESK");
            //desk_queue_print();
            
            while(desk_queue_free()<1) {
                debug(".....zasypiam2...");
                pthread_cond_wait(&cond, &mainMut);
            }
            pthread_mutex_unlock(&mainMut);
            debug(".....budze sie2...");
            debug("Wchodze do sekcji krytycznej (stan EXPLANATION)");
            changeState(EXPLANATION);
        }
        else if(stan==EXPLANATION){
            sleep(3);
            pkt->data = 0;
        
            debug("Wysyłam RELEASE ONE DESK");
            for(int i=0;i<size;i++){
                if (i!=rank)
                    sendPacket(pkt, i, RELEASE_DESK);
            }
            debug("Skonczylem wysylac RELEASE ONE DESK");
            debug("Zmieniam stan na INIT");
            changeState(INIT);
        }
        
    }
}
