#ifndef TUNNELS_QUEUE_H
#define TUNNELS_QUEUE_H

#include "structs.h"

void queue_add(process_queue_node**, process_s*);
void queue_remove(process_queue_node**, int);
void queue_print(process_queue_node**);
int queue_before_me(process_queue_node**, int);
void queue_clear(process_queue_node**);
int queue_size(process_queue_node**);
int queue_my_ts(process_queue_node**,int);
#endif 
