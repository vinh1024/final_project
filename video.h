#ifndef _VIDEO_H
#define _VIDEO_H


#define MAXCHAR 100
#define NUM_VIDEO 5
#define MAX_NUM_OF_SEGMENTS 294
#define MAX_PATH_LEN 256
#define NUM_QP 6
#define INIT_QP 1


static unsigned int QUANTIZIATION_PARAMS[] = {22, 28, 34, 38, 42, 48};
static char *VIDEO_NAME[10] = {"lam", "sony", "star", "terminate", "tokyo"};


struct QP_VIDEO {
    double *ls_rate;
    unsigned int qp;
};

struct LIST_QP_VIDEO {
    struct QP_VIDEO *ls_qpvd;
    char *vd_name;
};

static unsigned int num_qp = NUM_QP;
static unsigned int num_vd = NUM_VIDEO;
static unsigned int num_seg = MAX_NUM_OF_SEGMENTS;

#define VIDEOS struct LIST_QP_VIDEO
#define QPVD struct QP_VIDEO

//static struct VIDEO *ls_vd = NULL;

/* Ham khoi tao list video */
QPVD *init_qpvd();

/* Ham khoi tao video co nhieu qp */
VIDEOS *init_lst_vd();
/* Find max bitrate of video segments in set of QP */
double find_rate_max(QPVD *qpvd);

#endif