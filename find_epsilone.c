#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXCHAR 100
#define size(A) (sizeof(A)/sizeof(int))
#define NUM_VIDEO 5
#define MAX_NUM_OF_SEGMENTS 294
#define MAX_PATH_LEN 256
#define NUM_QP 6

static unsigned int QUANTIZIATION_PARAMS[] = {22, 28, 34, 38, 42, 48};
static char *VIDEO_NAME[10] = {"lam", "sony", "star", "terminate", "tokyo"};


static struct SEGMENT {
    double rate;
    unsigned int qp;
}seg = {
    .rate = 0.0,
    .qp = 0
};

static struct VIDEO {
    struct SEGMENT *list_seg;
    unsigned int num_seg;
    char *vd_name;
}video = {
    .list_seg = NULL,
    .num_seg = 0,
    .vd_name = NULL
};

static struct VIDEO *list_vd = NULL;
double str2num(char *);
struct VIDEO *load_data(char *data_dir,
               unsigned int num_video, 
               unsigned int num_qp,
               char **video_name, 
               unsigned int *qp_params) 
{
    struct VIDEO *list_vd = NULL;
    list_vd = malloc(num_video * num_qp * sizeof(struct VIDEO));
    unsigned int r, k=0;
    FILE *fd = NULL;
    char *file_name = NULL;
    char str[MAXCHAR];
    struct SEGMENT *ls_seg = NULL;
    printf("LOADING DATA ...\n");
    for (int i = 0; i < num_video; i++) {
        /* Get data for one video */
        for (int j = 0; j < num_qp; j++) {
            k = 0;
            file_name = malloc(MAX_PATH_LEN * sizeof(char *));
            r = snprintf(file_name, MAX_PATH_LEN, 
                                    "%s%s_bitrate/%sqp%d.txt", 
                                    data_dir, video_name[i], video_name[i], qp_params[j]);
            printf("Get data from file directory: %s\n", file_name);

            fd = fopen(file_name, "r");
            if (fd == NULL) {
                printf("Could not open %s\n", file_name);
            }
            //printf("open success!\n");
            list_vd[i * num_qp + j].vd_name = video_name[i];
            ls_seg = malloc(MAX_NUM_OF_SEGMENTS * sizeof(struct SEGMENT));
            /* Get data for segment */
            while (fgets(str, MAXCHAR, fd)) {
                //printf("Get data for segments\n%f\n", qp_params[j]);
                ls_seg[k].qp = qp_params[j];
                ls_seg[k].rate = str2num(str);
                //if (k < 5) printf("%f\t", ls_seg[k].rate);
                k += 1;
            }
            list_vd[i * num_qp + j].num_seg = k;
            list_vd[i * num_qp + j].list_seg = ls_seg;
            
            ls_seg = NULL;// tro ve NULL de free Neu khong tro ve null ma free thi bi mat du lieu o list_seg[0] va 1
            free(ls_seg);
            free(file_name);
            fclose(fd);
            for (int n = 0; n < 5; n++) {
                printf("------------------\n");
                printf("%f\t%d\t", list_vd[i * num_qp + j].list_seg[n].rate, 
                                    list_vd[i * num_qp + j].list_seg[n].qp);
            }
            printf("\n");
        }
    }
    printf("LOADING DATA: Done!\n");
    return list_vd;
}


void print_data(struct VIDEO *list_vd, unsigned int num_vd, unsigned int num_qp) {

    printf("NUM OF DATA: %d\n", num_vd*num_qp);
    for (int i = 0; i < num_vd; i++) {
        for (int q = 0; q < num_qp; q++) {
            for (int j = 0; j < 5; j++) {
                if (list_vd[i * num_qp + q].list_seg[j].qp == 48) {
                    printf("%s\t%f\t%d\n", list_vd[i * num_qp + q].vd_name, 
                                           list_vd[i * num_qp + q].list_seg[j].rate, 
                                           list_vd[i * num_qp + q].list_seg[j].qp);
                }
            }
        }
        printf("\n");
    }
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

    return val/power;
}

struct VIDEO *pick_adapt_R(double e, struct VIDEO *list_vd, unsigned int num_vd, unsigned int num_qp) {
    struct VIDEO *list_vd_adapt_R = NULL;
    struct SEGMENT *list_seg_adapt_R = NULL;
    struct SEGMENT seg_max_B = {.rate = 0.0, .qp = 0};

    list_vd_adapt_R = malloc(num_vd * num_qp * sizeof(struct VIDEO));

    unsigned int num_data = num_qp * num_vd;

    for (int i = 0; i < num_vd; i++) {
        /* each video */
        //list_vd_adapt_R[i].vd_name = list_vd[i].vd_name;
        //list_vd_adapt_R[i].num_seg = list_vd[i].num_seg;
        printf("-----------------\n");
        //list_seg_adapt_R = malloc(list_vd[0].num_seg * sizeof(struct SEGMENT));

        for (int j = 0; j < list_vd[0].num_seg; j++) {
            /*consider one segment*/
            for (int q = 0; q < num_qp; q++) {
                // print rate cua moi phan doan
                /*if (i < 2 && j < 3) {
                    printf("\n%s", list_vd[i * num_qp + q].vd_name);
                    printf("\t%f\t", list_vd[i * num_qp + q].list_seg[j].rate);
                    printf("%d\t", list_vd[i * num_qp + q].list_seg[j].qp);
                }*/

                if (list_vd[i * num_qp + q].list_seg[0].rate > e) {
                    seg_max_B = list_vd[i * num_vd + q].list_seg[0];
                    printf("+++++++%f", seg_max_B.rate);
                }
                else {
                    seg_max_B = (seg_max_B.rate < list_vd[i * num_qp + q].list_seg[j].rate) 
                                    ? list_vd[i * num_qp + q].list_seg[j] : seg_max_B;
                }
            }
            //if (i < 2 && j < 3) printf("\n");
        }
        printf("]]]]]]]]]]]]]]]]]]\n");
        //list_vd_adapt_R[i * num_vd + k].list_seg = list_seg_adapt_R;
        //free(list_seg_adapt_R);
    }
    return list_vd_adapt_R;
}


int main(int arg, char **argv)
{
    double e = 100.0;
    char *file_dir = NULL;
    struct VIDEO *list_vd_adapt_R = NULL;
    list_vd_adapt_R = malloc(sizeof(struct VIDEO)*NUM_VIDEO);
    

    file_dir = argv[1];

    printf("file directory: %s\n", file_dir);
    list_vd = malloc(NUM_VIDEO * sizeof(video));

    /*Load data*/
    list_vd = load_data(file_dir, NUM_VIDEO, NUM_QP, VIDEO_NAME, QUANTIZIATION_PARAMS);

    printf("DATA: \n");
    print_data(list_vd, NUM_VIDEO, NUM_QP);

    list_vd_adapt_R = pick_adapt_R(e, list_vd, NUM_VIDEO, NUM_QP);
    return 0;
}