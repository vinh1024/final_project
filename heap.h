#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct NODE {
    double rate;
    double U;
    double alpha;
    int vd_id;
    int rt_id;
}node;

typedef struct HEAP {
    struct NODE *ls_node;
    unsigned int size;
}heap;


heap *h_init(unsigned int num_nd);

void equal(node *n1, node *n2);

bool h_empty(heap *h);

void swap(node *n1, node *n2);

void hpush(heap *hm, node *nd);

node *hpop_peak(heap *hm);

//Display

void print_node(node *nd);
void print_heap(heap *h);

void h_free(heap *h);

#endif