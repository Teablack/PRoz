#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct proc_queue_n process_queue_node;
typedef struct proc process_s;

struct proc
{
    int id;
    int ts;
    int data;
};

struct proc_queue_n
{
    process_s *proc;
    process_queue_node *prev;
    process_queue_node *next;
};

int is_less(process_s *, process_s *);
process_s *create_process_s(int, int, int);

#endif
