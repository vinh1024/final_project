#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define _PARAM_
#include "video.h"
#include "standio.h"
#include "proc_video.h"
#include "heap.h"

//#define ONE_VIDEO
//#define DEBUG
#define MAIN
//#define FIND_BW
static FILE* fd = NULL;
#ifdef MAIN 
#define ALGORITHM_NAME(algorithm) \
    printf("______________%s_______________\n", algorithm);
#endif

const char *file_data = "./VBR_video_bitrate/";
struct point data[num_vd][num_rate];
int main(int arg, char **argv)
{
    VIDEOS *ls_vd = NULL;
    ls_vd = load_data(file_data);

#ifdef ONE_VIDEO
    
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
                d0u = calculate_U1(env,ls_vd[i].ls_qpvd[q].qp , RATE);
                if (u_max < d0u[1]) {
                    u_max = d0u[1];
                    d0 = d0u[0];
                    qp = ls_vd[i].ls_qpvd[q].qp;
                }
            }
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
    

    for (int i = 0; i < num_vd; i++) {
        Rmin = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        Rmax = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);

        Rstep = (Rmax - Rmin)/num_rate;
        RATE = Rmin;
        for (int j = 0; j < num_rate; j++){
            ls_adapt = pick_adapt_R(RATE, ls_vd[i].ls_qpvd);
            
            data[i][j].rate = RATE;
            u_max =  0.8 * (-0.172 * ls_adapt->qp + 9.249) + 0.2 * (-0.862 * log((6.718)) + 5);
            data[i][j].utility = (u_max > 5) ? round(u_max) : u_max;
            data[i][j].d0 = 0;
            data[i][j].qp = round(ls_adapt->qp);
            RATE += Rstep;
            d0 = 0;
            u_max = 0;
        }
    }
    for (int i = 0; i < num_vd; i++) {
        for (int j = 0; j < num_rate; j++) {
            fprintf(fd, "%s", ls_vd[i].vd_name);
            fprintf(fd, ",%f, %f, %f, %f\n", data[i][j].rate, data[i][j].utility,
                                            data[i][j].d0, data[i][j].qp);
        }
    }
    free(ls_adapt->ls_rate);
    free(ls_vd->ls_qpvd);
    fclose(fd);

#endif

#ifdef DEBUG
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

#ifdef MAIN
    QPVD *ls_adapt = NULL;
    double e = 0.0, u = 0.0, u_max =0.0;
    double Rmax = 0, Rmin = 0, Rstep, step, RATE;
    double max_rate = 0.0, min_rate = 0.0;
    double *env = (double *) malloc(sizeof(double) * num_seg);
    

    int *select = (int *) malloc(sizeof(int) * num_vd);

    //FILE *fd = NULL;
    char final_file[1024] = "final_result.csv";
    
    
    
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
    free(env);
    free(ls_adapt->ls_rate);

    double BW = 3000;
    double start, end;
    //double sumbw =0;
    //fd = fopen("accurate_alogrithm.txt", "w");
    fd = fopen(final_file, "w");
    int *result = (int *) malloc(sizeof(int) * num_vd);
    
    for (BW = 4000; BW <= 17000;) {
        fprintf(fd, "BandWidth, Algorithm, Video name, Rate, Utility,\n");
        //fprintf(fd, ",BandWidth, Algorithm, Video name, Rate, Utility,\n");
        printf("===============\tBW = %f\t=================\n", BW);

        printf("_____________________Fast heap______________________\n");
        fprintf(fd, "%f, Fast heap", BW);
        start = clock();
        fast_heap(data, BW, select);
        end = clock();
        print_result(ls_vd, data, fd, select);
        fprintf(fd, ",,TIME, %f\n\n", (end - start)/CLOCKS_PER_SEC);
        printf("\n\n");

        printf("_____________________Full search____________________\n");
        fprintf(fd, ", Full search");
        start = clock();
        full_search(data, num_rate, BW, select);
        end = clock();
        print_result(ls_vd, data, fd, select);
        fprintf(fd, ",,TIME, %f\n\n", (end - start)/CLOCKS_PER_SEC);
        printf("\n\n");


        printf("_____________________Lagrange Algorithm_____________\n");
        fprintf(fd, ", Lagrange Algorithm");
        start = clock();
        lagrange_algorithm(data, BW, select);
        end = clock();
        print_result(ls_vd, data, fd, select);
        fprintf(fd, ",,TIME, %f\n\n", (end - start)/CLOCKS_PER_SEC);
        printf("\n\n");
        
        BW += 1000;
    }

    fclose(fd);
    FILE *fd1 = fopen("algorithm.csv", "w");
    for (int BW = 3000; BW <= 17000;) {
        printf("_____________________Accurate Algorithm_____________\n");
        fprintf(fd1, ", Accurate Algorithm");
        
        start = clock();
        accurate_algorithm(result);
        end = clock();
        print_result(ls_vd, data, fd1, result);
        fprintf(fd1, ",,TIME, %f\n\n", (end - start)/CLOCKS_PER_SEC);
        printf("\n\n");
        BW += 1000;
    }
    //fclose(fd1);


#endif
    return 0;
}
