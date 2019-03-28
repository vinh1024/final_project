#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "proc_video.h"
#include "standio.h"

QPVD *pick_adapt_R(double e, QPVD *ls_qpvd)
{
    QPVD *vd_adapt = (QPVD *) malloc(sizeof(QPVD));
    vd_adapt->ls_rate = (double *) malloc(sizeof(double) * num_seg);
    vd_adapt->qp = 0;

    double r_adapt = 0.0;
    unsigned int qp_adapt = 0, sum_qp_adapt = 0;
    for (int i = 0; i < num_seg; i++) {
        for (int j = 0; j < num_qp; j++) {
            if (ls_qpvd[num_qp - 1].ls_rate[i] > e) {
                r_adapt = ls_qpvd[num_qp - 1].ls_rate[i];
                qp_adapt = ls_qpvd[num_qp - 1].qp;
            } else {
                if (r_adapt < ls_qpvd[j].ls_rate[i]) {
                    r_adapt = ls_qpvd[j].ls_rate[i];
                    qp_adapt = ls_qpvd[j].qp;
                } else {
                    r_adapt = r_adapt;
                    qp_adapt = qp_adapt;
                }
            }
        }
        vd_adapt->ls_rate[i] = r_adapt;
        sum_qp_adapt += qp_adapt;
        r_adapt = 0.0;
    }
    vd_adapt->qp = qp_adapt/num_seg;
    return vd_adapt;
}

double *data_tich_luy(double *ls_rate) 
{
    double sum = 0.0;
    double *ls_dt_tl = (double *) malloc(sizeof(double) * num_seg);

    for (int i = 0; i < num_seg; i++) {
        sum += ls_rate[i]; 
        ls_dt_tl[i] = sum;
    }
    return ls_dt_tl;
}

void print_data_tich_luy(const char *file_name, VIDEOS *ls_vd)
{
    double sum = 0.0;
    VIDEOS *ls_vd_tl = (VIDEOS *) malloc(sizeof(VIDEOS) * num_vd);
    FILE *fd = NULL;
    fd = fopen(file_name, "w");
    if (fd) {
        perror("Could not open file to write");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_vd; i++) {
        ls_vd_tl[i].ls_qpvd = (QPVD *) malloc(sizeof(QPVD) * num_qp);
        for (int j = 0; j < num_qp; j++) {
            ls_vd_tl[i].ls_qpvd[j].ls_rate = (double *) malloc(sizeof(double) * num_seg);
        }
    }
    
    for (int i = 0; i < num_vd; i++) {
        fprintf(fd, "%s", ls_vd_tl[i].vd_name);
        for (int j = 0; j < num_qp; j++) {
            for (int k = 0; k < num_seg; k++) {
                fprintf(fd, ",%f", ls_vd_tl[i].ls_qpvd[j].ls_rate[k]);
            }
            fprintf(fd, "\n");
        }
    }
    fclose(fd);
    printf("Ghi du lieu tich luy vao file %s\n", file_name);
}

double *envelop_vd(double *ls_dt_tl)
{
    double *env_vd = NULL, *dt_tmp = NULL;
    unsigned int k = 0;
    double R_max = 0.0;
    unsigned int index = num_seg;
    dt_tmp = (double *) malloc(sizeof(double) * num_seg);
    env_vd = (double *) malloc(sizeof(double) * num_seg);
    for (int i = 0; i < num_seg; i++) {
        dt_tmp[i] = 0.0;
    }
    for (int i = 0; i < num_seg; i++) {
        k = 0;
        while ((k < index) && (index > 0)) {
            /* D[k] = D[k] + R[i+k]
            i = 0, k = 0: D[0] = D[0] + R[0]  (D[0] = R[0])
            i = 0, k = 1: D[1] = D[1] + R[1]  (D[1] = R[1])
            i = 0, ....
            i = 1, k = 0: D[0] = D[0] + R[1]  (D[0] = R[0] + R[1])
            i = 1, k = 1: D[1] = D[0] + R[2]  (D[1] = R[1] + R[2])

            i = 1, k = 292:                    D[292] = R[292] + R[293]
            */
            //printf("%d\t %d\n", i, k);
            dt_tmp[k] += ls_dt_tl[i + k];
            k++;
            //  printf("%d\n", k);
        }
        for (int j = 0; j < index; j++) {
            R_max = (R_max > dt_tmp[j]) ? R_max : dt_tmp[j];
        }
        index -= 1;
        env_vd[i] = R_max;
        R_max = 0.0;
    }
    printf("Envelop data done!\n");
    return env_vd;
}

double cal_d0(double *ls_ev_data, double rate_r)
{
    double delta_max = 0.0, delta = 0.0;
    for (int i = 0; i < num_seg; i++) {
        delta = rate_r * i - ls_ev_data[i];
        delta_max = (delta_max > delta) ? delta_max : delta;
    }
    printf("------------delta max = %f\n", delta_max);
    if ((delta_max >= 0) && (delta_max < 0.5 * rate_r)) {
        printf("delta max = %f\n", delta_max);
        return delta_max/rate_r;
    }

    return -1;
}

double cal_U(QPVD *adapt_vd, double rate)
{
    double Ud = 0.0, Uq = 0.0, d0 = 0.0;
    //QPVD *env_vd = NULL;
    double *ls_data_tl = NULL, *env_vd = NULL;

    printf("Caculate U\n");
    //ls_data_tl = data_tich_luy(adapt_vd->ls_rate);  //Done

    printf("---------------------------------------\n");
    env_vd = envelop_vd(adapt_vd);

    d0 = cal_d0(env_vd, rate);

    if (d0 < 0)
        return -1;
    printf("===========QP_avg = %d\td0 = %f\n", adapt_vd->qp, d0);
    Uq = -0.172 * adapt_vd->qp + 9.249;
    Ud = -0.862 * log((d0 + 6.718)) + 5;

    return Uq + Ud;
}

