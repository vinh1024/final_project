#ifndef _PROC_VIDEO_H
#define _PROC_VIDEO_H

#define _PARAM_
#include "video.h"
#include "standio.h"

QPVD *pick_adapt_R(double e, QPVD *ls_qpvd);

double *data_tich_luy(double *ls_rate);

void print_data_tich_luy(const char *file_name, VIDEOS *ls_vd);

double *envelop_vd(double *ls_dt_tl);

double cal_d0(double *ls_ev_data, double rate_r);

double cal_U(QPVD *adapt_vd, double rate);

#endif
