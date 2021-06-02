#include <stdlib.h>
#include "structs.h"

int is_less(process_s *p1, process_s *p2)
{
    return (p1->ts < p2->ts) || ((p1->ts == p2->ts) && (p1->id < p2->id));
}

process_s *create_process_s(int id, int ts, int data)
{
    process_s *p = malloc(sizeof(process_s));
    p->id = id;
    p->ts = ts;
    p->data = data;
    return p;
}
