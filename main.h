#ifndef GLOBALH
#define GLOBALH

#define _GNU_SOURCE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "structs.h"
/* odkomentować, jeżeli się chce DEBUGI */
//#define DEBUG 
/* boolean */

#define TRUE 1
#define FALSE 0

/* stany procesu */
typedef enum {INIT, WAITING_TO_DISCUSS, WAITING_FOR_ROOM, DISCUSSION, THE_BIG_LIE, WAITING_FOR_STARTING_FIELD, BIG_BOOM, WAITING_FOR_ONE_DESK, EXPLANATION} state_t; 
extern state_t stan;
extern int rank;
extern int size;
extern int ln; //licznosc zespołu 

/*Zasoby */
extern int B;
extern int K;
extern int F;

/* to może przeniesiemy do global... */
typedef struct {
    int ts;       /* timestamp (zegar lamporta */
    int src;      /* pole nie przesyłane, ale ustawiane w main_loop */
    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
    int qts;   /* uzywany tylko w ACK [ jednak nie tylko ]*/
} packet_t;
extern MPI_Datatype MPI_PAKIET_T;

/* kolejki*/
extern process_queue_node* desk_queue;
extern process_queue_node* room_queue;
extern process_queue_node* field_queue;

extern pthread_mutex_t stateMut;
extern pthread_cond_t cond;
extern pthread_mutex_t mainMut;

/* Typy wiadomości */
#define REQUEST_FOR_DESK 1
#define REQUEST_FOR_ROOM 2
#define REQUEST_FOR_STARTING_FIELD 3
#define RELEASE_DESK 4
#define RELEASE_ROOM 5
#define RELEASE_STARTING_FIELD 6
#define ACK_DESK 7
#define ACK_ROOM 8
#define ACK_STARTING_FIELD 9


/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
					   "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape. 
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów
                                            
*/
extern int lclock;

#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%d]:[%d] " FORMAT "%c[%d;%dm\n", 27, (1+(rank/7))%2, 31+(6+rank)%7, rank, lclock, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

//#define debug(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

#define P_WHITE printf("%c[%d;%dm",27,1,37);
#define P_BLACK printf("%c[%d;%dm",27,1,30);
#define P_RED printf("%c[%d;%dm",27,1,31);
#define P_GREEN printf("%c[%d;%dm",27,1,33);
#define P_BLUE printf("%c[%d;%dm",27,1,34);
#define P_MAGENTA printf("%c[%d;%dm",27,1,35);
#define P_CYAN printf("%c[%d;%d;%dm",27,1,36);
#define P_SET(X) printf("%c[%d;%dm",27,1,31+(6+X)%7);
#define P_CLR printf("%c[%d;%dm",27,0,37);

/* printf ale z kolorkami i automatycznym wyświetlaniem RANK. Patrz debug wyżej po szczegóły, jak działa ustawianie kolorków */
#define println(FORMAT, ...) printf("%c[%d;%dm [%d]:[%d] " FORMAT "%c[%d;%dm\n", 27, (1+(rank/7))%2, 31+(6+rank)%7, rank, lclock, ##__VA_ARGS__, 27,0,37);

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);
void changeState(state_t);
int changeClock(int);
int setClock(int);

void desk_queue_add(int, int, int);
void desk_queue_replace(int, int, int);
void desk_queue_remove(int);
int desk_queue_free();
int desk_queue_my_ts();
void desk_queue_clear();
void desk_queue_print();
int desk_queue_size();

void room_queue_add(int, int, int);
void room_queue_replace(int, int, int);
void room_queue_remove(int);
int room_queue_free();
int room_queue_my_ts();
void room_queue_clear();
void room_queue_print();
int room_queue_size();

void field_queue_add(int, int, int);
void field_queue_replace(int, int, int);
void field_queue_remove(int);
int field_queue_free();
int field_queue_my_ts();
void field_queue_clear();
void field_queue_print();
int field_queue_size();
#endif
