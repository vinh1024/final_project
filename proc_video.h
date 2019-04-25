#ifndef _PROC_VIDEO_H
#define _PROC_VIDEO_H

#define _PARAM_
#include "video.h"
#include "standio.h"

QPVD *pick_adapt_R(double e, QPVD *ls_qpvd);

double *acc_data(double *ls_rate);

void print_acc_data(const char *file_name, VIDEOS *ls_vd);

double *env_data(double *ls_dt);

//double cal_d0(double *ls_ev_data, double rate_r);

double calculate_U(double *ls_env,double qp_avg, double R);

#endif
