#include "video.h"
#include <stdio.h>
#include <stdlib.h>

QPVD *init_qpvd() 
{
    QPVD *qpvd;
    qpvd->ls_rate = (double *) malloc(sizeof(double) * MAX_NUM_OF_SEGMENTS);
    qpvd->qp = 0;
    
    perror("Malloc");
    return qpvd;
}

VIDEOS *init_lst_vd()
{
    VIDEOS *vd_lsqp;
    vd_lsqp->vd_name = (char *)malloc(sizeof(char) * 128);
    vd_lsqp->ls_qpvd = (QPVD *)malloc(sizeof(QPVD) * NUM_QP);
    return vd_lsqp;
}

double find_rate_max(QPVD *vdqp)
{
    double seg_max_rate = 0.0;
    for (int i = 0; i < MAX_NUM_OF_SEGMENTS; i++) {
        seg_max_rate = (seg_max_rate > vdqp->ls_rate[i]) ? seg_max_rate : vdqp->ls_rate[i];
    }
    return seg_max_rate;
}