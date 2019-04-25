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
            } else if (ls_qpvd[j].ls_rate[i] < e)
            {
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
    vd_adapt->qp = ((double)sum_qp_adapt)/num_seg;
    //print_list_rate("./data_print/test_adapt.csv", vd_adapt->ls_rate);
    return vd_adapt;
}

double *acc_data(double *ls_rate) 
{
    double sum = 0.0;
    double *ls_dt_tl = (double *) malloc(sizeof(double) * (num_seg + 1));
    ls_dt_tl[0] = sum;
    for (int i = 1; i <= num_seg; i++) {
        sum += ls_rate[i-1] * T; 
        ls_dt_tl[i] = sum;
    }
    return ls_dt_tl;
}

void print_acc_data(const char *file_name, VIDEOS *ls_vd)
{
    //double sum = 0.0;
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

double *env_data(double *ls_dt)
{
    double *ls_env = (double *) malloc(sizeof(double) * (num_seg +1));
    unsigned int k = 0;
    double R_max = 0.0;
    unsigned int index = num_seg +1;
    double dt_tmp[num_seg];
   
    //print_list_rate("./data_print/data_adapt.csv", ls_dt);
    for (int i = 0; i <= num_seg; i++) {
        dt_tmp[i] = 0.0;
    }
    for (int i = 1; i <= num_seg; i++) {
        k = 0;
        while ((k < index) && (index > 1)) {
            
            //printf("%d\t %d\n", i, k);
            dt_tmp[k] = dt_tmp[k] + ls_dt[i + k] * 2;
            k++;
            //  printf("%d\n", k);
        }
        for (int j = 0; j < index; j++) {
            R_max = (R_max > dt_tmp[j]) ? R_max : dt_tmp[j];
            //printf("R max = %f\n", R_max);
        }
        index -= 1;
        ls_env[i] = R_max;
        R_max = 0.0;
    }
    return ls_env;
}
/* D[k] = D[k] + R[i+k]
i = 0, k = 0: D[0] = D[0] + R[0]  (D[0] = R[0])
i = 0, k = 1: D[1] = D[1] + R[1]  (D[1] = R[1])
i = 0, ....
i = 1, k = 0: D[0] = D[0] + R[1]  (D[0] = R[0] + R[1])
i = 1, k = 1: D[1] = D[0] + R[2]  (D[1] = R[1] + R[2])

i = 1, k = 292:                    D[292] = R[292] + R[293]
*/

double calculate_U(double *ls_env, double qp_avg, double R)
{
    double Ud = 0.0, Uq = 0.0, d0 = 0.0;
    double dmax = 0.0, delta = 0.0;
    for (int i = 0; i <= num_seg; i++) {
        delta = ls_env[i] - R * 2 * i; 
        if (dmax < delta)
            dmax = delta;
        //if (i < 2) printf("delta: %f, dmax: %f\n", delta, dmax);
    }
    //printf("d0======: %f\t", d0);
    d0 = dmax/R;    
    //printf("R = %f\tdmax = %f\t", R, dmax);
    
    //printf("------------d0 = %f\n", d0);
    if (d0 >= 0 && d0 < 1) {
        Uq = -0.172 * qp_avg + 9.249;
        Ud = -0.862 * log((d0 + 6.718)) + 5;
        printf("QP = %f\t\td0 = %f\t U = %f\n", qp_avg, d0, (0.8*Uq + 0.2*Ud));
        return 0.8 * Uq + 0.2 * Ud;
    } else 
        printf("d0 = %f\n",d0);
    return 0;
}

