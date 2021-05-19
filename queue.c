#include <stdlib.h>
#include "queue.h"
#include <stddef.h>
#include <stdio.h>
#include "structs.h"

void queue_add(process_queue_node** head, process_s* p) {
    process_queue_node* node = malloc(sizeof(process_queue_node));

    node->proc = p;
    node->next = NULL;
    node->prev = NULL;

    if(*head == NULL) {
        *head = node;
    } else if(is_less(node->proc, (*head)->proc)){
        process_queue_node* former_head = *head;
        *head = node;

        node->next = former_head;
        former_head->prev = node;
    } else {
        process_queue_node* insert_before = *head;
        int broke = 0;
        int cont = 1;
        while(cont) {
            if(insert_before->next != NULL) {
                insert_before = insert_before->next;
                cont = !is_less(node->proc, insert_before->proc);
            } else {
                process_queue_node* last_node = insert_before; 
                broke = 1;

                last_node->next = node;
                node->prev = last_node;
                break;
            }
        }
        if(!broke) {
            process_queue_node* prev = insert_before->prev;

            prev->next = node;
            node->prev = prev;

            node->next = insert_before;
            insert_before->prev = node;
        }
    }
}

void queue_remove(process_queue_node** head, int id) {
    if(*head == NULL) {
        return;
    }
    process_queue_node* to_be_removed = *head;
    while(to_be_removed != NULL) {
        if(to_be_removed->proc->id != id) {
            to_be_removed = to_be_removed->next;
        } else {
            break;
        }
    }
    if(to_be_removed == NULL) {
        return;
    } else {
        process_queue_node* prev = to_be_removed->prev;
        process_queue_node* next = to_be_removed->next;

        if(prev != NULL) {
            prev->next = next;
        }
        if(next != NULL) {
            next->prev = prev;
        }

        if(to_be_removed == *head) {
            *head = next;
        }

        free(to_be_removed->proc);
        free(to_be_removed);
    }
}

void queue_print(process_queue_node** head){
    if(*head == NULL) {
        return;
    }
    process_queue_node* current = *head;
    while(current != NULL) {
            printf("id %d ts: %d data: %d \n", current->proc->id, current->proc->ts, current->proc->data);
            current = current->next;
    }
}
int queue_before_me(process_queue_node** head, int id){
    int sum = 0;
    if(*head == NULL) {
        return sum ;
    }
    process_queue_node* current = *head;
    while(current != NULL) {
        if(current->proc->id != id) {
            sum+=current->proc->data ;
            current = current->next;
        } else {
            return sum;
        }
    }
}
void queue_clear(process_queue_node** head){
     if(*head == NULL)
        return;
    process_queue_node* to_be_removed = *head;
    while(to_be_removed != NULL) {
        process_queue_node* temp = to_be_removed->next;
        free(to_be_removed->proc);
        free(to_be_removed);
        to_be_removed = temp;
    }
}
