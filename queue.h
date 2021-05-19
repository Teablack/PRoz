#ifndef TUNNELS_QUEUE_H
#define TUNNELS_QUEUE_H

#include "structs.h"

void queue_add(process_queue_node**, process_s*);
void queue_remove(process_queue_node**, int);
int queue_is_head(process_queue_node*, int);
int queue_exists(process_queue_node*, int);
int queue_get_position(process_queue_node*, int);
void queue_print(process_queue_node**);

#endif 
