#include <stdio.h>
#include <stdlib.h>
#define _PARAM_
#include "video.h"
#include "standio.h"
#include "proc_video.h"
#include "heap.h"
#define num_rate (20)
//#define _ONE_VIDEO_
//#define _DEBUG_
#define _MAIN_
//#define _FIND_BW_

const char *file_data = "./VBR_video_bitrate/";

int main(int arg, char **argv)
{
    VIDEOS *ls_vd = NULL;
    
    ls_vd = load_data(file_data);

#ifdef _ONE_VIDEO_
    QPVD *ls_adapt = NULL;
    double e = 0.0, u = 0.0, u_max =0.0;
    double Rmax = 0, Rmin = 0, Rstep, step, RATE;
    double max_rate = 0.0, min_rate = 0.0;
    double *env = (double *) malloc(sizeof(double) * num_seg);
    double arr[100][2];
    /*Load data*/
    for (int i = 0; i < 2; i++) {
        printf("===video[%d]: %s===\n", i, ls_vd[i].vd_name);
        Rmax = max_rate = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        Rmin = min_rate = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        //printf("Rmin = %f\tRmax = %f\n", Rmin, Rmax);
        Rstep = (Rmax - Rmin)/10;
        e = min_rate;
        RATE = Rmin;
        step = (max_rate - min_rate)/250;
        for (int j = 0; j < 10; j++){
            e = min_rate;
            //printf("max_rate %f\n", max_rate);
            //printf("RATE =========== %f", RATE);
            while (e <= Rmax) {
                ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);
                env = env_data(ls_adapt->ls_rate); 
                //printf("----------e = %f\n",e);
                u = calculate_U(env,ls_adapt->qp, RATE);
                u_max = (u_max > u) ? u_max : u;
                e += step;
            }
            arr[j][0] = RATE;
            arr[j][1] = u_max;
            RATE += Rstep;
            u_max = 0;
        }
        for (int k = 0; k < 10; k++) {
        printf("R = %f\tU = %f\n", arr[k][0], arr[k][1]);
        }
    }
    
#endif

#ifdef _DEBUG_
    double *acc = NULL;
    double *env = NULL;
    QPVD *adapt_data = NULL;
    double u;
    printf("DEBUG MODE\n");
    //acc = data_tich_luy(ls_vd[0].ls_qpvd[0].ls_rate);

    //adapt_data = pick_adapt_R(500,ls_vd[0].ls_qpvd);
    //print_list_rate("./data_print/adapt_data500.csv", adapt_data->ls_rate);
    //printf("QP adapt = %d", adapt_data->qp);

    //acc = acc_data(adapt_data->ls_rate);
    acc = acc_data(ls_vd[0].ls_qpvd[0].ls_rate);

    print_list_rate("./data_print/acc_data.csv", acc);

    env = env_data(ls_vd[0].ls_qpvd[0].ls_rate);

    //printf("Max rate: %f\n", find_rate_max(adapt_data->ls_rate));
    //d0 = cal_d0(env_data, RATE);
    print_list_rate("./data_print/env_data.csv", env);

    //u = calculate_U(env, adapt_data->qp, 500);
    u = calculate_U(env, 22, 2000);
    printf("U = %f\n", u);
#endif

#ifdef _MAIN_
    QPVD *ls_adapt = NULL;
    double e = 0.0, u = 0.0, u_max =0.0;
    double Rmax = 0, Rmin = 0, Rstep, step, RATE;
    double max_rate = 0.0, min_rate = 0.0;
    double *env = (double *) malloc(sizeof(double) * num_seg);
    node *tmp = (node *) malloc(sizeof(node)); 
    

    double arr[num_vd][num_rate][2];
    int select[num_vd];
    /*Load data*/
    for (int i = 0; i < num_vd; i++) {
        Rmin = find_min_rate(ls_vd[i].ls_qpvd[num_qp - 1].ls_rate);
        Rmax = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        
        Rstep = (Rmax - Rmin)/num_rate;
        e = min_rate;
        RATE = Rmin;
        max_rate = Rmax;
        min_rate = Rmin;
        step = (max_rate - min_rate)/100;
        for (int j = 0; j < num_rate; j++){
            e = min_rate;
            while (e <= Rmax) {
                ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);
                env = env_data(ls_adapt->ls_rate);
                u = calculate_U(env,ls_adapt->qp, RATE);
                u_max = (u_max > u) ? u_max : u;
                e += step;
            }
            arr[i][j][0] = RATE;
            arr[i][j][1] = u_max;
            RATE += Rstep;
            u_max = 0;
        }
    }
    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO: %s\n",ls_vd[i].vd_name);
        for (int k = 0; k < num_rate; k++) {
            printf("R = %f\tU = %f\n", arr[i][k][0], arr[i][k][1]);
        }
    }
    double sumR = 0, BW = 3000, old_rate;
    heap *h = NULL;

    for (BW = 3000; BW <= 15000;) {
    //printf("tmp->rate = %f\n", tmp->rate);
        h = h_init(num_vd);
        sumR = 0;
        for (int i = 0; i < num_vd; i++) {
            tmp->rate = arr[i][0][0];
            tmp->U = arr[i][0][1];
            tmp->alpha = (arr[i][1][1] - arr[i][0][1])/(arr[i][1][0] - arr[i][0][0]);
            tmp->vd_id = i;
            tmp->rt_id = 0;
            hpush(h, tmp);
            sumR += tmp->rate;
        }
        if (BW == 4000) print_heap(h);
        printf("SumR = %f\n", sumR);
        while ((h->size > 0)) {
            tmp = hpop_peak(h);
            old_rate = tmp->rate;
            tmp->rate = arr[tmp->vd_id][tmp->rt_id][0];
            sumR = sumR + tmp->rate - old_rate;
            if (sumR <= BW) {
                select[tmp->vd_id] = tmp->rt_id;
                tmp->U = arr[tmp->vd_id][tmp->rt_id][1];
                tmp->rt_id += 1;
                tmp->alpha = (arr[tmp->vd_id][tmp->rt_id + 1][1] - tmp->U)/
                             (arr[tmp->vd_id][tmp->rt_id + 1][0] - tmp->rate);
                hpush(h, tmp);
                if (BW == 4000) print_heap(h);
            } else break;
        }
        sumR = 0;
        printf("=============BW: %f===============\n", BW);
        for (int i = 0; i < num_vd; i++) {
            printf("VIDEO: %s\t, rate: %f\t, U: %f\n", ls_vd[i].vd_name,
                                                       arr[i][select[i]][0],
                                                       arr[i][select[i]][1]);
            sumR += arr[i][select[i]][0];
        }
        printf("SUM RATE: %f\n", sumR);
        BW += 1000;
        h_free(h);
    }
    
    
    
#endif

#ifdef _FIND_BW_
    double MINR[num_vd], MAXR[num_vd];
    
    for (int i = 0; i < num_vd; i++) {
        MINR[i] = find_min_rate(ls_vd[i].ls_qpvd[num_qp - 1].ls_rate);
        MAXR[i] = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        printf("%s video:\t\t MIN RATE %f\t MAX RATE: %f\n", ls_vd[i].vd_name, MINR[i], MAXR[i]);
    }
#endif
    return 0;
}
