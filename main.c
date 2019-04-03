#include <stdio.h>
#include "video.h"
#include "standio.h"
#include "proc_video.h"

//#define _DEBUG_
const char *file_data = "./VBR_video_bitrate/";

int main(int arg, char **argv)
{
    VIDEOS *ls_vd = NULL;
    QPVD *ls_adapt = NULL;

    double e = 0.0, u = 0.0, u_max =0.0;
    double RATE = 2000, step;
    double max_rate = 0.0, min_rate = 0.0;
    double *ls_envelop_dt = (double *) malloc(sizeof(double) * num_seg);
    /*Load data*/
    ls_vd = load_data(file_data);
    //print_data("data1.csv", ls_vd, num_vd, num_seg, num_qp);
    
#ifndef _DEBUG_
    for (int i = 0; i < 1; i++) {
        
        printf("===video[%d]: %s\t RATE: %f===\n", i, ls_vd[i].vd_name, RATE);
        max_rate = find_max_rate(ls_vd[i].ls_qpvd[0].ls_rate);
        min_rate = find_min_rate(ls_vd[i].ls_qpvd[num_qp-1].ls_rate);
        e = min_rate;
        step = (max_rate - min_rate)/250;
        //printf("max_rate %f\n", max_rate);
        
        while (e < max_rate) {
            ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);      
            u = cal_U(ls_adapt, RATE);
            u_max = (u_max > u) ? u_max : u;
            
            e += step;
        }
        if (u_max > 0) printf("U_max = %f\n", u_max);
        u_max = 0.0;
        e = 0.0;
        //printf("Max rate: %f" , find_rate_max(ls_vd[i].ls_qpvd[0].ls_rate));
    }
    //printf("Epxilone = %f\n", e);
#endif

#ifdef _DEBUG_
    double *ls_data_tich_luy = NULL;
    double *env_data = NULL;
    QPVD *adapt_data = NULL;
    
    printf("DEBUG MODE\n");
    //ls_data_tich_luy = data_tich_luy(ls_vd[0].ls_qpvd[0].ls_rate);

    adapt_data = pick_adapt_R(500,ls_vd[0].ls_qpvd);
    print_list_rate("./data_print/adapt_data500.csv", adapt_data->ls_rate);
    //printf("QP adapt = %d", adapt_data->qp);

    ls_data_tich_luy = data_tich_luy(adapt_data->ls_rate);

    print_list_rate("./data_print/acc_data500.csv", ls_data_tich_luy);

    env_data = envelop_vd(adapt_data->ls_rate);

    printf("Max rate: %f\n", find_rate_max(adapt_data->ls_rate));
    //d0 = cal_d0(env_data, RATE);
    print_list_rate("./data_print/envelop_data500.csv", env_data);
#endif

    return 0;
}