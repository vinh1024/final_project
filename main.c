#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    
    /* ADAPTIVE MODE */
    QPVD *ls_adapt = NULL;
    double e = 0, u_max = 0, d0 = 0, qp = 0, *d0u = NULL;
    double Rmax = 0, Rmin = 0, Rstep, step, RATE;
    double max_rate = 0, min_rate = 0;
    double *env = (double *) malloc(sizeof(double) * num_seg);
    struct point data[num_vd][num_rate];

    char file_one_video[1024] = "./data_final/adapt_1vd_result1.csv";
    FILE *fd = fopen(file_one_video, "w");
    fprintf(fd, "VIDEO, Rate, Utility, d0, QP\n");
   
    for (int i = 0; i < num_vd; i++) {

        Rmin = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        Rmax = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        
        //printf("Rmin = %f\tRmax = %f\n", Rmin, Rmax);
        Rstep = (Rmax - Rmin)/num_rate;
        min_rate = Rmin;
        max_rate = Rmax;
        e = min_rate;
        RATE = Rmin;
        step = (max_rate - min_rate)/250;
        for (int j = 0; j < num_rate; j++){
            e = min_rate;
            while (e <= Rmax) {
                ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);
                env = env_data(ls_adapt->ls_rate); 
                //printf("----------e = %f\n",e);
                d0u = calculate_U1(env,ls_adapt->qp, RATE);
                if (u_max < d0u[1]) {
                    u_max = d0u[1];
                    d0 = d0u[0];
                    qp = ls_adapt->qp; 
                }
                e += step;
            }
            data[i][j].rate = RATE;
            data[i][j].utility = (u_max > 5) ? round(u_max) : u_max;
            data[i][j].d0 = d0;
            data[i][j].qp = round(qp);
            RATE += Rstep;
            d0 = 0;
            u_max = 0;
        }
    }
    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO: %s\n", ls_vd[i].vd_name);
        fprintf(fd, "%s", ls_vd[i].vd_name);
        for (int k = 0; k < num_rate; k++) {
            printf("R = %f\tU = %f \t d0 = %f\t qp = %f\n", data[i][k].rate, data[i][k].utility, 
                                                            data[i][k].d0, data[i][k].qp);
            fprintf(fd, ",%f, %f, %f, %f\n", data[i][k].rate, data[i][k].utility,
                                            data[i][k].d0, data[i][k].qp);
            //fprintf(fd, ",");
        }
        fprintf(fd, "\n");
    }

    /* NO ADAPTIVE MODE */
    /* Voi moi R tinh cho d0 cho 7 muc chat luong tinh U cua chon ra U lon nhat
       de so sanh voi U-adapt chon duoc ung voi toc do bit R */
    d0 =0; u_max = 0; qp = 0;
    
    for (int i = 0; i < num_vd; i++) {
        Rmin = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        Rmax = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        Rstep = (Rmax - Rmin)/num_rate;
        min_rate = Rmin;
        max_rate = Rmax;
        RATE = Rmin;
        
        for (int j = 0; j < num_rate; j++){
            for (int q = 0; q < num_qp; q++) {
                env = env_data(ls_vd[i].ls_qpvd[q].ls_rate); 
                //printf("----------e = %f\n",e);
                d0u = calculate_U1(env,ls_vd[i].ls_qpvd[q].qp , RATE);
                //printf("d0: %f, u: %f\n", d0u[0], d0u[1]);
                if (u_max < d0u[1]) {
                    u_max = d0u[1];
                    d0 = d0u[0];
                    qp = ls_vd[i].ls_qpvd[q].qp;
                }
            }
            //printf("==========\n");
            data[i][j].rate = RATE;
            data[i][j].utility = u_max;
            data[i][j].d0 = d0;
            data[i][j].qp = qp;
            RATE += Rstep;
            u_max = 0;
            d0 = 0;
            qp = 0;
        }

    }
    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO: %s\n", ls_vd[i].vd_name);
        fprintf(fd, "%s", ls_vd[i].vd_name);
        for (int j = 0; j < num_rate; j++) {
            printf("Rate: %f,\tU: %f,\td0: %f,\tQP: %f\n", data[i][j].rate,
                                                           data[i][j].utility,
                                                           data[i][j].d0,
                                                           data[i][j].qp);
            fprintf(fd, ",%f, %f, %f, %f\n", data[i][j].rate, data[i][j].utility,
                                            data[i][j].d0, data[i][j].qp);
        }
        fprintf(fd, "\n");
    }
    fclose(fd);

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
    struct point data[num_vd][num_rate];
    int select[num_vd];

    FILE *fd = NULL;
    char final_file[1024] = "final_result.csv";
    fd = fopen(final_file, "w");
    fprintf(fd, "BandWidth, Video name, Rate, Utility,\n");
    
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
            data[i][j].rate = RATE;
            data[i][j].utility = u_max;
            RATE += Rstep;
            u_max = 0;
        }
    }
    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO: %s\n",ls_vd[i].vd_name);
        for (int k = 0; k < num_rate; k++) {
            printf("R = %f\tU = %f\n", data[i][k].rate, data[i][k].utility);
        }
    }
    double sumR = 0, BW = 3000, old_rate;
    heap *h = NULL;

    for (BW = 4000; BW <= 15000;) {
        h = h_init(num_vd);
        sumR = 0;
        for (int i = 0; i < num_vd; i++) {
            tmp->rate = data[i][0].rate;
            tmp->U = data[i][0].utility;
            tmp->alpha = (data[i][1].utility - data[i][0].utility)/(data[i][1].rate - data[i][0].rate);
            tmp->vd_id = i;
            tmp->rt_id = 0;
            hpush(h, tmp);
            sumR += tmp->rate;
        }
        //if (BW == 4000) print_heap(h);
        printf("SumR = %f\n", sumR);
        while ((h->size > 0)) {
            tmp = hpop_peak(h);
            old_rate = tmp->rate;
            tmp->rate = data[tmp->vd_id][tmp->rt_id].rate;
            sumR = sumR + tmp->rate - old_rate;
            if (sumR <= BW) {
                select[tmp->vd_id] = tmp->rt_id;
                tmp->U = data[tmp->vd_id][tmp->rt_id].utility;
                tmp->rt_id += 1;
                tmp->alpha = (data[tmp->vd_id][tmp->rt_id + 1].utility - tmp->U)/
                             (data[tmp->vd_id][tmp->rt_id + 1].rate - tmp->rate);
                hpush(h, tmp);
            } else break;
        }
        sumR = 0;
        //printf("=============BW: %f===============\n", BW);
        fprintf(fd, "%f, ", BW);
        for (int i = 0; i < num_vd; i++) {
            printf("VIDEO: %s\t, rate: %f\t, U: %f\n", ls_vd[i].vd_name,
                                                       data[i][select[i]].rate,
                                                       data[i][select[i]].utility);
            fprintf(fd, "%s, %f, %f,\n", ls_vd[i].vd_name,
                                          data[i][select[i]].rate,
                                          data[i][select[i]].utility);
            fprintf(fd, " ,");
            sumR += data[i][select[i]].rate;
        }
        printf("SUM RATE: %f\n", sumR);
        fprintf(fd, "SUM RATE, %f\n\n", sumR);
        printf("=====================Full search ===================\n");
        full_search(data, num_rate, BW);
        printf("====================================================\n");
        BW += 1000;
        h_free(h);
    }


    /* Full search */

    

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
