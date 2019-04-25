#include <stdio.h>
#define _PARAM_
#include "video.h"
#include "standio.h"
#include "proc_video.h"

//#define _DEBUG_

const char *file_data = "./VBR_video_bitrate/";

int main(int arg, char **argv)
{
    VIDEOS *ls_vd = NULL;
    
    ls_vd = load_data(file_data);

#ifndef _DEBUG_
    QPVD *ls_adapt = NULL;
    double e = 0.0, u = 0.0;// u_max =0.0;
    double Rmax = 0, Rmin = 0, Rstep, step, RATE;
    double max_rate = 0.0, min_rate = 0.0;
    double *env = (double *) malloc(sizeof(double) * num_seg);
    /*Load data*/
    for (int i = 0; i < 2; i++) {
        printf("===video[%d]: %s===\n", i, ls_vd[i].vd_name);
        Rmax = max_rate = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        Rmin = min_rate = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        printf("Rmin = %f\tRmax = %f\n", Rmin, Rmax);
        Rstep = (Rmax - Rmin)/5;
        e = min_rate;
        RATE = Rmin;
        step = (max_rate - min_rate)/250;
        for (int j = 0; j < 5; j++){
            e = min_rate;
            //printf("max_rate %f\n", max_rate);
            printf("RATE =========== %f", RATE);
            while (e <= Rmax) {
                ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);
                env = env_data(ls_adapt->ls_rate); 
                printf("----------e = %f\n",e);
                u = calculate_U(env,ls_adapt->qp, RATE);
                //u_max = (u_max > u) ? u_max : u;
                e += step;
            }
            RATE += Rstep;
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

    return 0;
}
