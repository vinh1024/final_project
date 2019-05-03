#ifndef _PROC_VIDEO_H
#define _PROC_VIDEO_H

#define _PARAM_
#include "video.h"
#include "standio.h"

/* Thich ung chat luong video*/
QPVD *pick_adapt_R(double e, QPVD *ls_qpvd);

/* Tao cac diem du lieu tich luy */
double *acc_data(double *ls_rate);

void print_acc_data(const char *file_name, VIDEOS *ls_vd);

/* Tao duong bao du lieu */
double *env_data(double *ls_dt);

double *calculate_U1(double *ls_env,double qp_avg, double R);
double calculate_U(double *ls_env, double qp_avg, double R);

/* Tim bang thong thap nhat cap cho video co muc chat luong
   thap nhat co the choi voi do tre lon nhat*/
double find_min_rate(double *ls_env_minqp);

/* Tim bang thong cao nhat cap cho video co muc chat luong cao nhat
   co the choi voi do tre bang 0 */
double find_max_rate(double *ls_env_maxqp);

void full_search(struct point d[5][20], int n, double BW, int *select);

void fast_heap(struct point d[5][20], double BW, int *select);
#endif
