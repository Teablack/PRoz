#include "main.h"
#include "watek_komunikacyjny.h"
#include "watek_glowny.h"
#include "structs.h"
#include "queue.h"
#include <pthread.h>

/* sem_init sem_destroy sem_post sem_wait */
//#include <semaphore.h>
/* flagi dla open */
//#include <fcntl.h>

int lclock;
state_t stan=INIT;;
volatile char end = FALSE;
int size,rank, B, K, ln, F; /* nie trzeba zerować, bo zmienna globalna statyczna */
MPI_Datatype MPI_PAKIET_T;
pthread_t threadKom, threadMon;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t callowMut = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t desk_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t room_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t starting_field_mut = PTHREAD_MUTEX_INITIALIZER;

process_queue_node* desk_queue = NULL;
process_queue_node* room_queue = NULL;
process_queue_node* starting_field_queue = NULL;

void check_thread_support(int provided)
{

    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

/* srprawdza, czy są wątki, tworzy typ MPI_PAKIET_T
*/
void inicjuj(int *argc, char ***argv)
{
    int provided;
    MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    const int nitems=3; /* bo packet_t ma trzy pola */
    int       blocklengths[3] = {1,1,1};
    MPI_Datatype typy[3] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[3]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    MPI_Type_create_struct(nitems, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(rank);

    pthread_create( &threadKom, NULL, startKomWatek , 0);
    // if (rank==0) {
	//     pthread_create( &threadMon, NULL, startMonitor, 0);
    // }
    // debug("jestem");
}

/* usunięcie zamkków, czeka, aż zakończy się drugi wątek, zwalnia przydzielony typ MPI_PAKIET_T
   wywoływane w funkcji main przed końcem
*/
void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    if (rank==0) pthread_join(threadMon,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

/* opis patrz main.h */
void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;  
    if (pkt==0) {
        pkt = malloc(sizeof(packet_t)); 
        freepkt=1;
    }
    pkt->src = rank;
    pkt->ts = changeClock(1);
    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    if(freepkt) free(pkt); 
}

//lepiej zmienic nazwe - to zmienia zegar przy odebraniu wiadomosci 
int setClock(int newClock){
    pthread_mutex_lock( &callowMut );
    lclock = (lclock+1 > newClock)? (lclock+1):newClock;
    pthread_mutex_unlock( &callowMut );
    return lclock;
}

//zwykly clock+1
int changeClock(int newClock){
    pthread_mutex_lock( &callowMut );
    lclock+=newClock;
    pthread_mutex_unlock( &callowMut );
    return lclock;
}

void changeState(state_t newState)
{
    changeClock(1);
    pthread_mutex_lock( &stateMut );
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}

int main(int argc, char **argv)
{
    /* Tworzenie wątków, inicjalizacja itp */
    inicjuj(&argc,&argv); // tworzy wątek komunikacyjny w "watek_komunikacyjny.c"

    B = 10;
    K = 4;
    F = 1;
    srandom(rank);
    ln = random()%5+2;
    
    mainLoop();          // w pliku "watek_glowny.c"
    
    finalizuj();
    return 0;
}

void desk_queue_add(int id, int time, int data){
    pthread_mutex_lock(&desk_mut);
    queue_add(&desk_queue,create_process_s(id, time, data));
    pthread_mutex_unlock(&desk_mut);
}

void desk_queue_replace(int id, int time, int data){
    pthread_mutex_lock(&desk_mut);
    queue_remove(&desk_queue,id);
    queue_add(&desk_queue,create_process_s(id, time, data));
    pthread_mutex_unlock(&desk_mut);
}
void desk_queue_remove(int id){
    pthread_mutex_lock(&desk_mut);
    queue_remove(&desk_queue,id);
    pthread_mutex_unlock(&desk_mut);
}

int desk_queue_free(){
    pthread_mutex_lock(&desk_mut);
    int n = B - queue_before_me(&desk_queue, rank);
    pthread_mutex_unlock(&desk_mut);
    return n;
}

int desk_queue_my_ts(){
    pthread_mutex_lock(&desk_mut);
    int n = queue_my_ts(&desk_queue,rank);
    pthread_mutex_unlock(&desk_mut);
    return n;
}

void desk_queue_clear(){
    pthread_mutex_lock(&desk_mut);
    queue_clear(&desk_queue);
    pthread_mutex_unlock(&desk_mut);
}
void desk_queue_print(){
    pthread_mutex_lock(&desk_mut);
    queue_print(&desk_queue);
    pthread_mutex_unlock(&desk_mut);
}

int desk_queue_size(){
    int n = 0;
    pthread_mutex_lock(&desk_mut);
    n = queue_size(&desk_queue);
    pthread_mutex_unlock(&desk_mut);
    return n;
}

//ROOM

void room_queue_add(int id, int time, int data){
    pthread_mutex_lock(&room_mut);
    queue_add(&room_queue,create_process_s(id, time, data));
    pthread_mutex_unlock(&room_mut);
}

void room_queue_replace(int id, int time, int data){
    pthread_mutex_lock(&room_mut);
    queue_remove(&room_queue,id);
    queue_add(&room_queue,create_process_s(id, time, data));
    pthread_mutex_unlock(&room_mut);
}
void room_queue_remove(int id){
    pthread_mutex_lock(&room_mut);
    queue_remove(&room_queue,id);
    pthread_mutex_unlock(&room_mut);
}

int room_queue_free(){
    pthread_mutex_lock(&room_mut);
    int n = B - queue_before_me(&room_queue, rank);
    pthread_mutex_unlock(&room_mut);
    return n;
}

int room_queue_my_ts(){
    pthread_mutex_lock(&room_mut);
    int n = queue_my_ts(&room_queue,rank);
    pthread_mutex_unlock(&room_mut);
    return n;
}

void room_queue_clear(){
    pthread_mutex_lock(&room_mut);
    queue_clear(&room_queue);
    pthread_mutex_unlock(&room_mut);
}
void room_queue_print(){
    pthread_mutex_lock(&room_mut);
    queue_print(&room_queue);
    pthread_mutex_unlock(&room_mut);
}

int room_queue_size(){
    pthread_mutex_lock(&room_mut);
    int n = 0;
    n = queue_size(&room_queue);
    pthread_mutex_unlock(&room_mut);
    return n;
}