#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAXCHAR 100
#define size(A) (sizeof(A)/sizeof(int))
#define NUM_VIDEO 5
#define MAX_NUM_OF_SEGMENTS 294
#define MAX_PATH_LEN 256
#define NUM_QP 6
#define INIT_QP 1
static unsigned int QUANTIZIATION_PARAMS[] = {22, 28, 34, 38, 42, 48};
static char *VIDEO_NAME[10] = {"lam", "sony", "star", "terminate", "tokyo"};

static struct SEGMENT {
    double *ls_rate;
    unsigned int *ls_qp;
}seg = {
    .ls_rate = NULL,
    .ls_qp = NULL
};

static struct VIDEO {
    struct SEGMENT *ls_seg;
    unsigned int num_seg;
    char *vd_name;
}video = {
    .ls_seg = NULL,
    .num_seg = 0,
    .vd_name = NULL
};

static struct VIDEO *ls_vd = NULL;

/* Ham khoi tao list video */

struct VIDEO *init_lsvd(unsigned int init_qp) {
    struct VIDEO *ls_vd = NULL;
    
    ls_vd = (struct VIDEO *) malloc (sizeof(struct VIDEO) * NUM_VIDEO);
    for (int i = 0; i < NUM_VIDEO; i++) {
        ls_vd[i].ls_seg = (struct SEGMENT *) malloc(sizeof(struct SEGMENT) * MAX_NUM_OF_SEGMENTS);
        for (int j = 0; j < MAX_NUM_OF_SEGMENTS; j++) {
            ls_vd[i].ls_seg[j].ls_rate = (double *) malloc(sizeof(double) * NUM_QP);
            if (!init_qp) {
                ls_vd[i].ls_seg[j].ls_qp = (unsigned int *) malloc(sizeof(unsigned int *) * NUM_QP);
            }
        }
    }
    perror("Malloc done \n");
    return ls_vd;
}

double str2num(char *);
struct VIDEO *load_data(char *data_dir,
               unsigned int num_video, 
               unsigned int num_qp,
               char **video_name, 
               unsigned int *qp_params) 
{
    struct VIDEO *ls_vd = NULL;
    unsigned int r, k=0;
    FILE *fd = NULL;
    char *file_name = NULL;
    char str[MAXCHAR];

    ls_vd =(struct VIDEO *) malloc(num_video * sizeof(struct VIDEO));
    for (int i = 0; i < num_video; i++) {
        ls_vd[i].ls_seg = (struct SEGMENT *)malloc(sizeof(struct SEGMENT) * MAX_NUM_OF_SEGMENTS);
        for (int j = 0; j < MAX_NUM_OF_SEGMENTS; j++) {
            ls_vd[i].ls_seg[j].ls_rate = (double *)malloc(sizeof(double) * num_qp);
            ls_vd[i].ls_seg[j].ls_qp = (unsigned int *)malloc(sizeof(unsigned int) * num_qp);
        }
    }
    printf("LOADING DATA ...\n");
    for (int i = 0; i < num_video; i++) {
        /* Get data for one video */
        for (int j = 0; j < num_qp; j++) {
            file_name = malloc(MAX_PATH_LEN * sizeof(char));
            r = snprintf(file_name, MAX_PATH_LEN, 
                                    "%s%s_bitrate/%sqp%d.txt", 
                                    data_dir, video_name[i], video_name[i], qp_params[j]);
            
            //printf("Get data from file directory: %s\n", file_name);
            fd = fopen(file_name, "r");
            if (fd == NULL) {
                perror("Could not open file name");
                exit(EXIT_FAILURE);
            }
            ls_vd[i].vd_name = video_name[i];
            /* Get data for segment */
            while (fgets(str, MAXCHAR, fd)) {
                ls_vd[i].ls_seg[k].ls_qp[j] = qp_params[j];
                ls_vd[i].ls_seg[k].ls_rate[j] = str2num(str);
                //if (k < 5) printf("%f\t", ls_vd[i].ls_seg[k].ls_rate[j]);
                k += 1;
            }
            k = 0;
            free(file_name);
            fclose(fd);
        }
    }
    perror("LOADING DATA");
    return ls_vd;
}

void print_data(const char *filename, struct VIDEO *ls_vd, unsigned int num_vd, unsigned int num_seg, unsigned int num_qp) {
    FILE *fd = fopen(filename, "w");

    if (fd == NULL) {
        perror("Could not open file name");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_vd; i++) {
        fprintf(fd, "%d,%s", i,ls_vd[i].vd_name);
        for (int j = 0; j < num_seg; j++) {
            for (int q = 0; q < num_qp; q++) {
                fprintf(fd, ",%f", ls_vd[i].ls_seg[j].ls_rate[q]);
            }
            fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
    }
    fclose(fd);
    printf("PRINT DATA DONE!\n");
}

double str2num(char *str) {
    int i;
    double val = 0.0 , power;

    for (i = 0; (str[i] >= '0' && str[i] <= '9'); i++) {
        val = val * 10.0 + (str[i] - '0'); 
    }
    if (str[i] == '.') {
        i++;
    }
    for (power = 1.0 ; (str[i] >= '0', str[i] <= '9'); i++) {
        val = val * 10.0 + (str[i] - '0');
        power *= 10;
    }
    //printf("------------%f", val/power);
    return val/power;
}

struct VIDEO *pick_adapt_R(double e, struct VIDEO *ls_vd, unsigned int num_vd, unsigned int num_qp) {
    struct VIDEO *ls_vd_adapt_R = NULL;
    double Bmax = 0.0;
    unsigned int qp_max = 0;

    ls_vd_adapt_R = (struct VIDEO *) malloc(num_vd * sizeof(struct VIDEO));
    for (int i = 0; i < num_vd; i++) {
        ls_vd_adapt_R[i].ls_seg = (struct SEGMENT *) malloc(sizeof(struct SEGMENT) * MAX_NUM_OF_SEGMENTS);
        for (int j = 0; j < MAX_NUM_OF_SEGMENTS; j++) {
            ls_vd_adapt_R[i].ls_seg[j].ls_rate = (double *)malloc(sizeof(double));
            ls_vd_adapt_R[i].ls_seg[j].ls_qp = (unsigned int *)malloc(sizeof(unsigned int));
        }
    }

    for (int i = 0; i < num_vd; i++) {
        /* each video */    
        ls_vd_adapt_R[i].vd_name = ls_vd[i].vd_name;
        for (int j = 0; j < MAX_NUM_OF_SEGMENTS; j++) {
            /*consider one segment*/
            if (ls_vd[i].ls_seg[j].ls_rate[num_qp-1] >= e) {
                Bmax = ls_vd[i].ls_seg[j].ls_rate[num_qp-1];
                qp_max = ls_vd[i].ls_seg[j].ls_qp[num_qp-1];
            }
            else {
                for (int q = 0; q < num_qp; q++) {
                    if (ls_vd[i].ls_seg[j].ls_rate[q] < e) {
                        if (Bmax < ls_vd[i].ls_seg[j].ls_rate[q]) {
                            Bmax = ls_vd[i].ls_seg[j].ls_rate[q];
                            qp_max = ls_vd[i].ls_seg[j].ls_qp[q];
                        } else {
                            qp_max = qp_max;
                            Bmax = Bmax;
                        }
                    }
                }
            }
            ls_vd_adapt_R[i].ls_seg[j].ls_qp[0] = qp_max;
            ls_vd_adapt_R[i].ls_seg[j].ls_rate[0] = Bmax;
            Bmax = 0.0;
            qp_max = 0;
            /*printf("Bmax: %f\t QP: %d\tVideo: %s\t Segment: %d\n", ls_vd_adapt_R[i].ls_seg[j].ls_rate[0],
                                                                   ls_vd_adapt_R[i].ls_seg[j].ls_qp[0], 
                                                                   ls_vd[i].vd_name, j+1);*/
            
        }
    }
    return ls_vd_adapt_R;
}

double cal_d0(struct SEGMENT *segs, double rate_r, unsigned int num_seg) {
    double acc_data = 0.0, x_max =0.0, d0 = 0.0;

    for (int j = 0; j < num_seg; j++) {
        acc_data += segs[j].ls_rate[0]; 
            
        x_max = ((acc_data - (j+1) * rate_r) < x_max) ? x_max : (acc_data - (j+1) * rate_r);
        printf("acc_data: %f \t R: %f \t xmax: %f\n", acc_data, acc_data - (j+1)*rate_r, x_max);
    }
    d0 = x_max/rate_r;
    printf("video[0]: %f\n", d0);
    return d0;
}
/* Ham xay dung du lieu duong du lieu tich luy*/

void data_tich_luy(struct VIDEO *ls_vd, unsigned int num_vd, 
    unsigned int num_qp, unsigned int num_seg) 
{
    double sum = 0.0;
    struct VIDEO *tl_vd = NULL;
    FILE *fd = NULL;
    fd = fopen("./data_tich_luy.csv", "w");
    tl_vd = init_lsvd(!INIT_QP);

    for (int i = 0; i < num_vd; i++) {
        tl_vd[i].vd_name = ls_vd[i].vd_name;
        //printf("--------------%s\n", tl_vd[i].vd_name);
        for (int q = 0; q < num_qp; q++) {
            for (int j = 0; j < num_seg; j++) {
                sum += ls_vd[i].ls_seg[j].ls_rate[q];
                //if (j < 5) printf("SUM = %f\n", sum);
                tl_vd[i].ls_seg[j].ls_rate[q] = sum;
            }
            sum = 0.0;
        }
    }
    for (int i = 0; i < num_vd; i++) {
        fprintf(fd, "%s", tl_vd[i].vd_name);
        for (int q = 0; q < num_qp; q++) {
            for (int j = 0; j < num_seg; j++) {
                fprintf(fd, ",%f", tl_vd[i].ls_seg[j].ls_rate[q]);
            }
            fprintf(fd, "\n");
        }
    }
    printf("Du lieu tich luy: DONE\n");
    fclose(fd);
}

/* Ham xay dung du lieu duong bao*/
struct VIDEO *envelop_dt(struct VIDEO *ls_vd,unsigned int num_vd, unsigned int num_qp, unsigned int num_seg)
{
    struct VIDEO *ev_vd = NULL;
    struct VIDEO *dt_tmp = NULL;
    dt_tmp = init_lsvd(!INIT_QP);
    struct SEGMENT *ls_seg = NULL;
    
    for (int i = 0; i < num_vd; i++) {
        for (int j = 0; j < num_seg; j++) {
            for (int q = 0; q < num_qp; q++) {
                dt_tmp[i].ls_seg[j].ls_rate[q] = 0.0;
            }
        }
    }
    //printf("===============\n");
    ev_vd = init_lsvd(!INIT_QP);
    unsigned int index = num_seg;
    double R_max = 0.0;
    unsigned int d = 0;
    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO[%d]\n", i);
        ev_vd[i].vd_name = ls_vd[i].vd_name;    
        for (int q = 0; q < num_qp; q++) {
            //printf("QP = %d\n", q);
            index = num_seg;
            for (int j = 0; j < num_seg; j++) {
                /* Create data to find envelop */
                d = 0;
                while ((d < index) && (index > 0)) {
                    /* D[k] = D[k] + R[j+k]
                       j = 0, d = 0: D[0] = D[0] + R[0]  (D[0] = R[0])
                       j = 0, d = 1: D[1] = D[1] + R[1]  (D[1] = R[1])
                       j = 0, ....
                       j = 1, d = 0: D[0] = D[0] + R[1]  (D[0] = R[0] + R[1])
                       j = 1, d = 1: D[1] = D[0] + R[2]  (D[1] = R[1] + R[2])

                       j = 1, d = 292:                    D[292] = R[292] + R[293]
                     */
                    dt_tmp[i].ls_seg[d].ls_rate[q] += ls_vd[i].ls_seg[j+d].ls_rate[q];
                    d++;
                }
                /* search for envelop */
                for (int k = 0; k < index; k++) {
                    R_max = (R_max > dt_tmp[i].ls_seg[k].ls_rate[q]) ? R_max : dt_tmp[i].ls_seg[k].ls_rate[q];
                }
                index -= 1;
                //printf("WHERE?\n");
                ev_vd[i].ls_seg[j].ls_rate[q] = R_max;
                R_max = 0.0;
            }
        }
    }
    printf("========ENVELOP DATA DONE!=======\n");
    return ev_vd;
}

int main(int arg, char **argv)
{
    double e = 150;
    char *file_dir = (char *) malloc(sizeof(char) * 128);
    struct VIDEO *ls_vd_adapt_R = NULL;
    struct VIDEO *envelop_vd = NULL;
    
    file_dir = "./VBR_video_bitrate/";
    
    //printf("file directory: %s\n", file_dir);

    /*Load data*/
    ls_vd = load_data(file_dir, NUM_VIDEO, NUM_QP, VIDEO_NAME, QUANTIZIATION_PARAMS);

    //data_tich_luy(ls_vd, NUM_VIDEO, NUM_QP, MAX_NUM_OF_SEGMENTS);

    //envelop_vd = envelop_dt(ls_vd, NUM_VIDEO, NUM_QP, MAX_NUM_OF_SEGMENTS);
    int a= 1;
    struct VIDEO *envelop_adapt_R = NULL;

    ls_vd_adapt_R = pick_adapt_R(e, ls_vd, NUM_VIDEO, NUM_QP);
    envelop_adapt_R = envelop_dt(ls_vd_adapt_R, NUM_VIDEO, a, MAX_NUM_OF_SEGMENTS);

    //print_data("data_adapt_R_envelop.csv", envelop_adapt_R, NUM_VIDEO, MAX_NUM_OF_SEGMENTS, a);
    data_tich_luy(ls_vd_adapt_R, NUM_VIDEO, a, MAX_NUM_OF_SEGMENTS);
    
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
    }
    
    //print_data("data.csv", ls_vd_adapt_R, NUM_VIDEO, MAX_NUM_OF_SEGMENTS, a);
    //print_data("envelop_data.csv", envelop_vd, NUM_VIDEO, MAX_NUM_OF_SEGMENTS, NUM_QP);

    //double d0 = cal_d0(ls_vd_adapt_R[0].ls_seg, e, MAX_NUM_OF_SEGMENTS);
    printf("Epxilone = %f\n", e);
    return 0;
}