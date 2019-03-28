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

    double e = 300, u = 0.0;
    double RATE = 3000;
    double max_rate = 0.0;
    double *ls_envelop_dt = (double *) malloc(sizeof(double) * num_seg);
    /*Load data*/
    ls_vd = load_data(file_data);
    //print_data("data1.csv", ls_vd, num_vd, num_seg, num_qp);
    
#ifndef _DEBUG_
    for (int i = 0; i < num_vd; i++) {
        printf("video[%d]\n", i);
        max_rate = find_rate_max(ls_vd[i].ls_qpvd[0].ls_rate);
        //printf("Max rate: %f" , find_rate_max(ls_vd[i].ls_qpvd[0].ls_rate));

        ls_adapt = pick_adapt_R(e, ls_vd[i].ls_qpvd);

        //ls_envelop_dt = envelop_vd(ls_adapt);

        
        
        u = cal_U(ls_adapt, RATE);
        printf("Video name: %s\n"
               "Max rate segment: %f\t"
               "U = %f\n",
               ls_vd[i].vd_name, max_rate, u);
    }
    //printf("Epxilone = %f\n", e);
#endif

#ifdef _DEBUG_
    double *ls_data_tich_luy = NULL;
    double *env_data = NULL;
    const char *ev_data_file = "./data_print/Envelop_data.csv";
    const char *acc_data_file = "./data_print/Accumulate_data.csv";
    printf("DEBUG MODE\n");
    ls_data_tich_luy = data_tich_luy(ls_vd[0].ls_qpvd[0].ls_rate);
    print_list_rate(ev_data_file, ls_data_tich_luy);
    printf("---------------------------------\n");
    env_data = envelop_vd(ls_vd[0].ls_qpvd[0].ls_rate);
    print_list_rate(acc_data_file, env_data);
#endif

    return 0;
}