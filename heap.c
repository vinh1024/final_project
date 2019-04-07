#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

#define CL(i) (2*i + 1)
#define CR(i) (2*i + 2)
#define P(i) (i/2)
heap *h_init(unsigned int num_nd) 
{
    heap *h = (heap *) malloc(sizeof(heap));
    h->size = 0;
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

void hpush(heap *hm, node *nd)
{
    unsigned int index = hm->size;
    hm->ls_node[index].U = nd->U;
    printf("index: %d, size: %d\n", index, hm->size);
    
    while ((index > 0) && (hm->ls_node[index].U > hm->ls_node[P(index)].U)) {
        swap (&(hm->ls_node[index]), &(hm->ls_node[P(index)]));
        index = P(index);
    }
    hm->size += 1;
}

node *hpop_peak(heap *hm)
{
    node *nd = (node *) malloc(sizeof(node));
    //node *lagre = (node *) malloc(sizeof(node));
    unsigned int index = 0, large = 0;
    nd->U = hm->ls_node[index].U;
    hm->ls_node[index].U = hm->ls_node[hm->size - 1].U;
    hm->size -= 1;
    print_heap(hm);

    while (index < hm->size) {
        large = (hm->ls_node[CL(index)].U > hm->ls_node[CR(index)].U) ? 
                                            CL(index) : CR(index);
        if ((hm->ls_node[index].U < hm->ls_node[large].U)) 
            swap(&(hm->ls_node[index]), &(hm->ls_node[large]));
        //printf("large: %d\n", large);
        print_heap(hm);
        index = large;
    }
    return nd;
}

void print_node(node nd)
{
    //printf("[%f, %f]  ", nd.U, nd.rate);
    printf("[%f]  ", nd.U);

}

void print_heap(heap *h)
{
    printf("=======HEAP TREE========\n");
    unsigned int k = 2, i = 0;
    while (i < h->size) {
        while ((i + 1) < k) {
            if (i  < h->size) printf("[%f]\t", h->ls_node[i].U);
            ++i;
        }
        printf("\n");
        k *= 2;
    }
}

void h_free(heap *h)
{
    free(h->ls_node);
    free(h);
}

#define _DEBUG_HEAP_
#ifdef _DEBUG_HEAP_

int main()
{
    printf("================DEBUG HEAP=================\n");
    double a[10] = {45, 24, 85, 100, 12, 42, 78, 8, 1, 58};
    node *arr = (node *) malloc(sizeof(node) * 10);
    for (int i = 0; i < 10; i++) {
        arr[i].U = a[i];
        printf("%d\t", i);
    }
    printf("\n");

    heap *h = h_init(10);

    for (int i = 0; i < 10; i++)
    {
        hpush(h, &(arr[i]));
    }   

    printf("===============PRINT HEAP=================\n");
    printf("size of heap: %d\n", h->size);
    print_heap(h);

    hpop_peak(h);
    printf("size of heap: %d\n", h->size);

    print_heap(h);
    h_free(h);
    return 0;
}

#endif