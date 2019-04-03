#include "video.h"
#include <stdio.h>
#include <stdlib.h>

QPVD *init_qpvd() 
{
    QPVD *qpvd = (QPVD *) malloc(sizeof(QPVD));
    qpvd->ls_rate = (double *) malloc(sizeof(double) * num_seg);
    qpvd->qp = 0;
    
    perror("Malloc");
    return qpvd;
}

VIDEOS *init_lst_vd()
{
    VIDEOS *vd_lsqp = (VIDEOS *) malloc(sizeof(VIDEOS));
    vd_lsqp->vd_name = (char *)malloc(sizeof(char) * 128);
    vd_lsqp->ls_qpvd = (QPVD *)malloc(sizeof(QPVD) * num_qp);
    return vd_lsqp;
}

double find_max_rate(double *ls_rate)
{
    double seg_max_rate = 0.0;
    //printf("Finding maximum rate of segment\n");
    for (int i = 0; i < num_seg; i++) {
        
        seg_max_rate = (seg_max_rate > ls_rate[i]) ? seg_max_rate : ls_rate[i];
        //printf("%d\t %f\t %f\n",i, ls_rate[i], seg_max_rate);
    }
    //printf("Find maximum rate done!\n");
    return seg_max_rate;
}
double find_min_rate(double *ls_rate)
{
    double seg_min_rate = 0.0;
    for (int i = 0; i < num_seg; i++) {
        seg_min_rate = (seg_min_rate < ls_rate[i]) ? seg_min_rate : ls_rate[i];
    }
    return seg_min_rate;
}