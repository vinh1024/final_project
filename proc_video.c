#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "heap.h"
#include "proc_video.h"
#include "standio.h"

#define printd(str) printf("%s\n", str);
#define d0max (0.5)
#define uthrs (0.09)
double bw_use, best_sumbw, best_sumU;
double sum_u;
static int index[20];
//static int index[20] = {0};
struct point data[num_vd][num_rate];
int sl[num_vd];
double BW;

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

double *calculate_U1(double *ls_env, double qp_avg, double R)
{
    double *d0nU = (double *) malloc(sizeof(double) * 2);
    double Ud = 0.0, Uq = 0.0, d0 = 0.0;
    double dmax = 0.0, delta = 0.0;
    for (int i = 0; i <= num_seg; i++) {
        delta = ls_env[i] - R * 2 * i; 
        if (dmax < delta)
            dmax = delta;
    }
    d0 = dmax/R;
    Uq = -0.172 * qp_avg + 9.249;
    Ud = -0.862 * log((d0 + 6.718)) + 5;
    if (d0 >= 0 && d0 < 0.5) {
        //printf("QP = %f\t\td0 = %f\t U = %f\n", qp_avg, d0, (0.8*Uq + 0.2*Ud));
        d0nU[0] = d0;
        d0nU[1] = 0.8 * Uq + 0.2 * Ud;
        return d0nU;
    } else {
        d0nU[0] = 0;
        d0nU[1] = 0;
        return d0nU;
    }
}

double calculate_U(double *ls_env, double qp_avg, double R)
{
    double Ud = 0.0, Uq = 0.0, d0 = 0.0;
    double dmax = 0.0, delta = 0.0;
    for (int i = 0; i <= num_seg; i++) {
        delta = ls_env[i] - R * 2 * i; 
        if (dmax < delta)
            dmax = delta;
    }
    d0 = dmax/R;
    if (d0 >= 0 && d0 < 1) {
        Uq = -0.172 * qp_avg + 9.249;
        Ud = -0.862 * log((d0 + 6.718)) + 5;
        //printf("QP = %f\t\td0 = %f\t U = %f\n", qp_avg, d0, (0.8*Uq + 0.2*Ud));
        return 0.8 * Uq + 0.2 * Ud;
    }
    return 0;
}


double find_min_rate(double *ls_vd)
{
    double min_r, R = 0.0;
    double *ls_env_minqp = env_data(ls_vd);
    for (int i = 1; i <= num_seg; i++) {
        R = ls_env_minqp[i]/(i * T + d0max);
        min_r = (R < min_r) ? min_r : R;
    }
    return min_r;
}

double find_max_rate(double *ls_vd)
{
    double max_r, R = 0.0;
    double *ls_env_maxqp = env_data(ls_vd);
    for (int i = 1; i <= num_seg; i++) {
        R = ls_env_maxqp[i]/(i * T);
        max_r = (max_r < R) ? R : max_r;
    }
    return max_r;
}

void full_search(struct point d[5][20], int n, double BW, 
                            int *select) 
{
    double sumU = 0, bw_use = 0;
    double best_sumbw = 0, best_sumU = 0;

    for (int i0 = 0; i0 < n; i0++) {
        bw_use += d[0][i0].rate;
        for (int i1 = 0; i1 < n; i1++) {
            bw_use += d[1][i1].rate;
            for (int i2 = 0; i2 < n; i2++) {
                bw_use += d[2][i2].rate;
                for (int i3 = 0; i3 < n; i3++) {
                    bw_use += d[3][i3].rate;
                    for (int i4 = 0; i4 < n; i4++) {
                        bw_use += d[4][i4].rate;
                        if (bw_use < BW) {
                            sumU =  d[0][i0].utility + d[1][i1].utility + d[2][i2].utility
                                  + d[3][i3].utility + d[4][i4].utility;
                            if (sumU > best_sumU || (sumU == best_sumU && bw_use < best_sumbw)) {
                                best_sumbw = bw_use;
                                best_sumU = sumU;
                                select[0] = i0;
                                select[1] = i1;
                                select[2] = i2;
                                select[3] = i3;
                                select[4] = i4;
                            }
                        }
                        bw_use -= d[4][i4].rate;
                    }
                    bw_use -= d[3][i3].rate;
                }
                bw_use -= d[2][i2].rate;
            }
           bw_use -= d[1][i1].rate;
        }
        bw_use -= d[0][i0].rate;
    }
}

void fast_heap(struct point d[5][20], double BW, int *select) 
{
    heap *h = h_init(num_vd);
    double old_rate, bw_use = 0;
    node *tmp = (node *) malloc(sizeof(node));

    for (int i = 0; i < num_vd; i++) {
        tmp->rate = d[i][0].rate;
        tmp->U = d[i][0].utility;
        tmp->alpha = (d[i][1].utility - d[i][0].utility)/
                        (d[i][1].rate - d[i][0].rate);
        tmp->vd_id = i;
        tmp->rt_id = 0;
        hpush(h, tmp);
        bw_use += tmp->rate;
    }
    while ((h->size > 0)) {
        tmp = hpop_peak(h);
        old_rate = tmp->rate;
        tmp->rate = d[tmp->vd_id][tmp->rt_id].rate;
        bw_use = bw_use + tmp->rate - old_rate;
        if (bw_use <= BW) {
            select[tmp->vd_id] = tmp->rt_id;
            tmp->U = d[tmp->vd_id][tmp->rt_id].utility;
            tmp->rt_id += 1;
            tmp->alpha = (d[tmp->vd_id][tmp->rt_id + 1].utility - tmp->U)/
                         (d[tmp->vd_id][tmp->rt_id + 1].rate - tmp->rate);
            hpush(h, tmp);
        } else break;
    }
    free(tmp);
    h_free(h);
}

void search(int i)
{
    if (bw_use > BW)
        return;
    else {
        if (i == num_vd) {
            if ((sum_u > best_sumU) || ((sum_u == best_sumU) && bw_use < best_sumbw)) {
                best_sumbw = bw_use;
                best_sumU = sum_u;
                for (int j = 0; j < 20; j++) {
                    sl[j] = index[j];
                }
            }
        } else {
            for (int j = 0; j < 20; j++) {
                index[i] = j;
                bw_use += data[i][j].rate;
                sum_u += data[i][j].utility;
                search(i+1);
                bw_use -= data[i][j].rate;
                sum_u -= data[i][j].utility;
            }
        }
    }
}

void accurate_algorithm(int *select)
{
    sum_u = 0;
    best_sumbw = 0;
    best_sumU = 0;
    bw_use = 0;
    
    for (int i = 0; i < 20; i++) {
        index[i] = 0;
        sl[i] = -1;
    }
    search(0);
    for (int i = 0; i < 20; i++) {
        select[i] = sl[i];
    }
}

void lagrange_algorithm(struct point d[5][20], double BW, int *select)
{
    int first_ind[num_vd];
    int mid_ind[num_vd];
    int last_ind[num_vd];
    int temp = 0;
    double use_bw;
    double minSlop = 0, maxSlop = 1, /*M_PI/2,*/ nextSlop;
    bool exit;
    bool p = false;

    for (int i = 0; i < num_vd; i++) {
        last_ind[i] = 20;
        mid_ind[i] = 0;
        first_ind[i] = 0;
    }
    while (true) {
        if (p == true) break;
        use_bw = 0;
        nextSlop = (minSlop + maxSlop)/2;
        for (int i = 0; i < num_vd; i++) {
            double uLagrange = -1;

            if (first_ind[i] == last_ind[i]) {
                use_bw += d[i][mid_ind[i]].rate;
                continue;
            }
            for (int j = 0; j < 20; j++) {
                if (d[i][j].utility - (nextSlop) * (d[i][j].rate) > uLagrange) {
                    uLagrange = d[i][j].utility - (nextSlop) * (d[i][j].rate);
                    if (p == false) 
                        /*printf("i: %d, j: %d\tuL = u - h.R \t %f = %f - %f * %f\n"
                        , i, j, uLagrange, d[i][j].utility, nextSlop, d[i][j].rate);
                        if (uLagrange > d[i][j+1].utility - nextSlop*d[i][j+1].rate)
                            printf("========:i: %d, j: %d\tuL = u - h.R \t %f = %f - %f * %f\n"
                            , i, j, (d[i][j+1].utility - nextSlop*d[i][j+1].rate), 
                            d[i][j+1].utility, nextSlop, d[i][j+1].rate);*/
                    mid_ind[i] = j;
                }
            }
            //printf("\n\n");
            use_bw += d[i][mid_ind[i]].rate;
        }
        /*printf("---------------------------\n");
        printf("last_ind: [%d], [%d], [%d], [%d], [%d]\n", last_ind[0],
            last_ind[1],
            last_ind[2],
            last_ind[3],
            last_ind[4]);
        printf("mid_ind : [%d], [%d], [%d], [%d], [%d]\n", mid_ind[0],
            mid_ind[1],
            mid_ind[2],
            mid_ind[3],
            mid_ind[4]);
        printf("firt_ind: [%d], [%d], [%d], [%d], [%d]\n", first_ind[0],
            first_ind[1],
            first_ind[2],       
            first_ind[3],
            first_ind[4]);
        printf("BW: %f\n", use_bw);*/

        if (use_bw == BW) {
            for (int i = 0; i < num_vd; i++) {
                select[i] = mid_ind[i];
            }
            break;
        } else if (use_bw < BW) {
            for (int i = 0; i < num_vd; i++) {
                first_ind[i] = mid_ind[i];
            }
            //printf("use_bw < BW-----maxSlop (%f) = nextSlop (%f), minSlop: %f\n", maxSlop, nextSlop, minSlop);
            maxSlop = nextSlop;

        } else if (use_bw > BW) {
            for (int i = 0; i < num_vd; i++) {
                last_ind[i] = mid_ind[i];
            }
            //printf("use_bw < BW====minSlop (%f) = nextSlop (%f), maxSlop: %f\n", minSlop, nextSlop, maxSlop);
            minSlop = nextSlop;
        }
        temp = 0;
        for (int i = 0; i < num_vd; i++) {
            temp += last_ind[i] - first_ind[i];
        }
        exit = true;
        for (int i = 0; i < num_vd; i++) {
            if (first_ind[i] != mid_ind[i] && last_ind[i] != mid_ind[i]) {
                exit = false;
                break;
            }
        }
        if (exit && (temp < num_vd) && use_bw < BW) {
            for(int i = 0; i < num_vd; i++) {
                select[i] = first_ind[i];
            }
            printf("BW use: %f\n", use_bw);
            break;
        }
    }
}