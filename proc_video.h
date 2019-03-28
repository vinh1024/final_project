#ifndef _PROC_VIDEO_H
#define _PROC_VIDEO_H

#include "video.h"
#include "standio.h"


QPVD *pick_adapt_R(double e, QPVD *qpvd);

void data_tich_luy(VIDEOS *ls_vd);

QPVD *envelop_vd(QPVD *qpvd);

double cal_d0(QPVD *qpvd, double rate_r);


#endif