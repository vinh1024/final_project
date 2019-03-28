#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "proc_video.h"

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
        for (int j = 0; j < num_qp; j++) {
            for (int k = 0; k < num_seg; k++) {
                sum += ls_vd[i].ls_qpvd[j].ls_rate[k]/1000; // (MByte)
                ls_vd_tl[i].ls_qpvd[j].ls_rate[k] = sum;
            }
            sum = 0.0;
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

QPVD *envelop_vd(QPVD *qpvd)
{
    QPVD *env_vd = NULL, *dt_tmp = NULL;
    unsigned int k = 0;
    double R_max = 0.0;
    unsigned int index = num_seg;
    dt_tmp = (QPVD *) malloc(sizeof(QPVD));
    dt_tmp->ls_rate = (double *) malloc(sizeof(double) * num_seg);
    env_vd = (QPVD *) malloc(sizeof(QPVD));
    env_vd->ls_rate = (double *) malloc(sizeof(double) * num_seg);
    for (int i = 0; i < num_seg; i++) {
        k = 0;
        while ((k < index) && (index > 0)) {
            /* D[k] = D[k] + R[j+k]
            j = 0, d = 0: D[0] = D[0] + R[0]  (D[0] = R[0])
            j = 0, d = 1: D[1] = D[1] + R[1]  (D[1] = R[1])
            j = 0, ....
            j = 1, d = 0: D[0] = D[0] + R[1]  (D[0] = R[0] + R[1])
            j = 1, d = 1: D[1] = D[0] + R[2]  (D[1] = R[1] + R[2])

            j = 1, d = 292:                    D[292] = R[292] + R[293]
            */
            dt_tmp->ls_rate[k] += qpvd->ls_rate[i + k];
            k++;
        }
        for (int j = 0; j < index; j++) {
            R_max = (R_max > dt_tmp->ls_rate[j]) ? R_max : dt_tmp->ls_rate[j];
        }
        index -= 1;
        env_vd->ls_rate[i] = R_max;
        R_max = 0.0;
    }
    return env_vd;
}

double cal_d0(QPVD *env_vd, double rate_r)
{
    double delta_max = 0.0, delta = 0.0;
    for (int i = 0; i < num_seg; i++) {
        delta = rate_r - env_vd->ls_rate[i];
        delta_max = (delta_max > delta) ? delta_max : delta;
    }

    if ((delta_max >= 0) && (delta_max < 0.5 * rate_r))
        return delta_max/rate_r;

    return -1;
}

double cal_U(QPVD *adapt_vd, double rate)
{
    double Ud = 0.0, Uq = 0.0, d0 = 0.0;
    QPVD *env_vd = NULL;

    env_vd = envelop_vd(adapt_vd);
    d0 = cal_d0(env_vd, rate);
    if (d0 < 0)
        return -1;
    Uq = -0.172 * adapt_vd->qp + 9.249;
    Ud = -0.862 * log(d0 + 6.718) + 5;
    
    return Uq + Ud;
}

