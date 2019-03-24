#include "video.h"
#include "standio.h"

#include <stdio.h>

const char *file_data = "./VBR_video_bitrate/";


static VIDEOS *ls_vd = NULL;



int main(int arg, char **argv)
{
    double e = 150;
    //char *file_dir = (char *) malloc(sizeof(char) * 128);
    //struct VIDEO *ls_vd_adapt_R = NULL;
    //struct VIDEO *envelop_vd = NULL;
    

    /*Load data*/
    ls_vd = load_data(file_data);
    //print_data("data1.csv", ls_vd, num_vd, num_seg, num_qp);
    printf("done!\n");

    //data_tich_luy(ls_vd, NUM_VIDEO, NUM_QP, MAX_NUM_OF_SEGMENTS);

    //envelop_vd = envelop_dt(ls_vd, NUM_VIDEO, NUM_QP, MAX_NUM_OF_SEGMENTS);
    //int a= 1;
    //struct VIDEO *envelop_adapt_R = NULL;

    //ls_vd_adapt_R = pick_adapt_R(e, ls_vd, NUM_VIDEO, NUM_QP);
    //envelop_adapt_R = envelop_dt(ls_vd_adapt_R, NUM_VIDEO, a, MAX_NUM_OF_SEGMENTS);

    //print_data("data_adapt_R_envelop.csv", envelop_adapt_R, NUM_VIDEO, MAX_NUM_OF_SEGMENTS, a);
    //data_tich_luy(ls_vd_adapt_R, NUM_VIDEO, a, MAX_NUM_OF_SEGMENTS);
    /*
    unsigned int sum_q = 0;
    double avg_q = 0.0;
    for (int i = 0; i < NUM_VIDEO; i++) {
        for (int j = 0; j < MAX_NUM_OF_SEGMENTS; j++) {
            //if(j < 5) printf("%d\t",ls_vd_adapt_R[i].ls_seg[j].ls_qp[0]);
            sum_q += ls_vd_adapt_R[i].ls_seg[j].ls_qp[0]; 
        }
        avg_q = ((double)sum_q)/MAX_NUM_OF_SEGMENTS;
        printf("video[%d]: %f\n", i+1, avg_q);
        sum_q = 0;
        avg_q = 0.0;
    }*/
    
    
    //print_data("envelop_data.csv", envelop_vd, NUM_VIDEO, MAX_NUM_OF_SEGMENTS, NUM_QP);

    //double d0 = cal_d0(ls_vd_adapt_R[0].ls_seg, e, MAX_NUM_OF_SEGMENTS);
    printf("Epxilone = %f\n", e);
    return 0;
}