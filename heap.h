#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct NODE {
    double rate;
    double U;
}node;

typedef struct HEAP {
    struct NODE *ls_node;
    unsigned int size;
}heap;


heap *heap_init(unsigned int num_nd);

bool h_empty(heap *h);

void swap(node *n1, node *n2);

// processing heap

void heapify(heap *h, int i);

void build_heap_max(heap *hm, node *ls_nd, unsigned int size);

void hpush(heap *hm, node *nd);

node *hpop_peak(heap *hm);

//Display

void print_node(node nd);

void print_heap(heap *h);

#endif