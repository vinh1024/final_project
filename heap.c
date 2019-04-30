#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

#define CL(i) (2*i + 1)
#define CR(i) (2*i + 2)
#define P(i)  ((i-1)/2)
heap *h_init(unsigned int num_nd) 
{
    heap *h = (heap *) malloc(sizeof(heap));
    h->size = 0;
    h->ls_node = (node *) malloc(sizeof(node) * num_nd);
    return h;
}

void equal(node *dst, node *nd) {
    dst->alpha = nd->alpha;
    dst->rate = nd->rate;
    dst->U = nd->U;
    dst->vd_id = nd->vd_id;
    dst->rt_id = nd->rt_id;
}

bool h_empty(heap *h)
{
    if (h->size) return true;
    return false;
}

void swap(node *n1, node *n2)
{
    node tmp;
    equal(&tmp, n1);
    equal(n1, n2);
    equal(n2, &tmp);
}

void hpush(heap *hm, node *nd)
{
    unsigned int index;
    hm->size += 1;
    index = hm->size - 1;
    equal(&(hm->ls_node[index]), nd);
    //printf("PUSH NODE: alpha: %f, size: %d\n", hm->ls_node[index].alpha, hm->size);
    
    while ((index > 0) && (hm->ls_node[index].alpha > hm->ls_node[P(index)].alpha)) {
        swap (&(hm->ls_node[index]), &(hm->ls_node[P(index)]));
        index = P(index);
    }
}

node *hpop_peak(heap *hm)
{
    node *nd = (node *) malloc(sizeof(node));
    unsigned int index = 0, large = 0;
    //int tmp;
    equal(nd, &(hm->ls_node[index]));
    hm->size -= 1;
    //printf("sizeeeeeeeeeeeeeeeeee: %d",hm->size);
    equal(&(hm->ls_node[index]), &(hm->ls_node[hm->size]));

    while (index < (hm->size-1) && hm->size > 0 && index >= 0) {
        if (CL(index) >= hm->size){
            large = CL(index);
            //printf("...............%d, size= %d\n",index, hm->size);
            //printf("00000000000000\n");
            //return nd;
        } else if (CR(index >= hm->size)) {
            large = CR(index);
            if (CL(index) == (hm->size -1)) {
                large = CL(index);
                if (hm->ls_node[index].alpha < hm->ls_node[large].alpha) 
                    swap(&(hm->ls_node[index]), &(hm->ls_node[large]));
            }
        } else {
            large = (hm->ls_node[CL(index)].alpha > hm->ls_node[CR(index)].alpha) ? 
                                                CL(index) : CR(index);
            
            if (hm->ls_node[index].alpha < hm->ls_node[large].alpha) 
                swap(&(hm->ls_node[index]), &(hm->ls_node[large]));
            //print_heap(hm);
        }
        index = large;
    }
    //printf("iiiiiiiiiiiiiiiii: %d\n", index);
    return nd;
}

void print_node(node *nd)
{
    //printf("[%f, %f]  ", nd.U, nd.rate);
    printf("NODE: video_id: %d, \t"
                 "rt_id: %d, \t"
                 "U = %f, \t"
                 "R = %f,\t"
                 "alpha = %f\n", nd->vd_id, nd->rt_id, nd->U, nd->rate, nd->alpha);
}

void print_heap(heap *h)
{
    printf("=======HEAP TREE========\n");
    unsigned int k = 2, i = 0;
    while (i < h->size) {
        while ((i + 1) < k) {
            if (i  < h->size) 
                printf("[ID:%d][A:%f][R:%f][U:%f]\t\t", h->ls_node[i].vd_id,
                                               h->ls_node[i].alpha, 
                                               h->ls_node[i].rate,
                                               h->ls_node[i].U);
            ++i;
        }
        printf("\n\n");
        k *= 2;
    }
}

void h_free(heap *h)
{
    free(h->ls_node);
    free(h);
}

//#define _DEBUG_HEAP_
#ifdef _DEBUG_HEAP_

int main()
{
    printf("================DEBUG HEAP=================\n");
    double a[15] = {45, 32, 90, 104, 200, 13, 24, 85, 100, 12, 42, 78, 8, 1, 58};
    node *arr = (node *) malloc(sizeof(node) * 10);
    for (int i = 0; i < 10; i++) {
        arr[i].U = a[i];
        printf("%d\t", i);
    }
    printf("\n");

    heap *h = h_init(15);

    for (int i = 0; i < 15; i++)
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