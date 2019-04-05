#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

heap *heap_init(unsigned int num_nd) 
{
    heap *h = (heap *) malloc(sizeof(heap));
    h->size = num_nd;
    h->ls_node = (node *) malloc(sizeof(node) * num_nd);
    return h;
}

bool h_empty(heap *h)
{
    if (h->size) return false;
    return true;
}

void swap(node *n1, node *n2)
{
    node tmp;
    tmp.rate = n1->rate;
    tmp.U = n1->U;
    n1->rate = n2->rate;
    n1->U = n2->U;
    n2->rate = tmp.rate;
    n2->U = tmp.U;
}

void heapify(heap *h, int i)
{
    
}

void print_node(node nd)
{
    printf("[%f, %f]  ", nd.U, nd.rate);
}

void print_heap(heap *h)
{
    unsigned int k = 0, i = 0;
    while (i < h->size) {
        while (i < 2^k) {
            print_node(h->ls_node[i]);
            ++i;
        }
        printf("\n");
        ++k;
    }
}