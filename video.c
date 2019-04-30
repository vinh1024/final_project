#define _PARAM_
#define _NUM_VD_
#include "video.h"
#include <stdio.h>
#include <stdlib.h>


//#define T (2) // time of segments

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



