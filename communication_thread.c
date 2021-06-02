#include "main.h"
#include "communication_thread.h"

void *startComThread(void *ptr)
{
    MPI_Status status;
    packet_t packet;
    state_t state_cp;
    while (TRUE)
    {
        state_cp = state;
        if (
            (state_cp == WAITING_TO_DISCUSS) || (state_cp == WAITING_FOR_ROOM) || (state_cp == WAITING_FOR_STARTING_FIELD) || (state_cp == WAITING_FOR_ONE_DESK))
        {
            MPI_Recv(&packet, 1,MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            setClock(packet.ts+1);
            packet_t *pkt = malloc(sizeof(packet_t));

            switch(status.MPI_TAG)
            {
            case REQUEST_FOR_DESK:

                if (state_cp == WAITING_TO_DISCUSS)
                {
                    desk_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                    pkt->qts = desk_queue_my_ts();
                    pkt->data = ln;
                    sendPacket(pkt, packet.src, ACK_DESK);
                }
                else if ((state_cp == WAITING_FOR_ROOM) || (state_cp == WAITING_FOR_STARTING_FIELD))
                {
                    pkt->data = 0;
                    pkt->qts = lclock;
                    sendPacket(pkt, packet.src, ACK_DESK);
                }
                else if (state_cp == WAITING_FOR_ONE_DESK)
                {
                    desk_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                    pkt->qts = desk_queue_my_ts();
                    pkt->data = 1;
                    sendPacket(pkt, packet.src, ACK_DESK);
                }
                break;
            case ACK_DESK:
                if ((state_cp == WAITING_TO_DISCUSS) || (state_cp == WAITING_FOR_ONE_DESK))
                {
                    if (packet.ts > desk_queue_my_ts())
                    {
                        desk_queue_replace(packet.src, packet.qts, packet.data);
                        pthread_cond_signal(&cond);
                    }
                }
                break;
            case RELEASE_DESK:
                if ((state_cp == WAITING_TO_DISCUSS) || (state_cp == WAITING_FOR_ONE_DESK))
                {
                    if (packet.ts > desk_queue_my_ts())
                    {
                        desk_queue_replace(packet.src, packet.ts, 0);
                        pthread_cond_signal(&cond);
                    }
                }
                break;
            case REQUEST_FOR_ROOM:

                if ((state_cp == WAITING_TO_DISCUSS) || (state_cp == WAITING_FOR_STARTING_FIELD) || (state_cp == WAITING_FOR_ONE_DESK))
                {
                    pkt->data = 0;
                    pkt->qts = lclock;
                    sendPacket(pkt, packet.src, ACK_ROOM);
                }
                else if (state_cp == WAITING_FOR_ROOM)
                {
                    room_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                    pkt->qts = room_queue_my_ts();
                    pkt->data = 1;
                    sendPacket(pkt, packet.src, ACK_ROOM);
                }
                break;
            case ACK_ROOM:
                if (state_cp == WAITING_FOR_ROOM)
                {
                    room_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                }
                break;
            case RELEASE_ROOM:
                if (state_cp == WAITING_FOR_ROOM)
                {
                    room_queue_replace(packet.src, packet.ts, 0);
                    pthread_cond_signal(&cond);
                }
                break;
            case REQUEST_FOR_STARTING_FIELD:
                if (state_cp == WAITING_FOR_STARTING_FIELD)
                {
                    field_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                    pkt->qts = field_queue_my_ts();
                    pkt->data = 1;
                    sendPacket(pkt, packet.src, ACK_STARTING_FIELD);
                }
                else if ((state_cp == WAITING_FOR_ROOM) || (state_cp == WAITING_FOR_ONE_DESK) || (state_cp == WAITING_TO_DISCUSS))
                {
                    pkt->data = 0;
                    pkt->qts = lclock;
                    sendPacket(pkt, packet.src, ACK_STARTING_FIELD);
                }
                break;
            case ACK_STARTING_FIELD:
                if (state_cp == WAITING_FOR_STARTING_FIELD)
                {
                    field_queue_replace(packet.src, packet.qts, packet.data);
                    pthread_cond_signal(&cond);
                }
                break;
            case RELEASE_STARTING_FIELD:
                if (state_cp == WAITING_FOR_STARTING_FIELD)
                {
                    field_queue_replace(packet.src, packet.ts, 0);
                    pthread_cond_signal(&cond);
                }
                break;
            default:
                break;
            }
        }
    }
}
