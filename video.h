#ifndef _VIDEO_H
#define _VIDEO_H


#define MAXCHAR 100
#define NUM_VIDEO 5
#define MAX_NUM_OF_SEGMENTS 294
#define MAX_PATH_LEN 256
#define NUM_QP 6
#define INIT_QP 1
#define T (2)                   //thoi gian cua mot segment
#ifndef _PARAM_
static unsigned int QUANTIZIATION_PARAMS[] = {22, 28, 34, 38, 42, 48};
static char *VIDEO_NAME[10] = {"lam", "sony", "star", "terminate", "tokyo"};
#endif

struct QP_VIDEO {
    double *ls_rate;
    double qp;
};

#define QPVD struct QP_VIDEO

struct LIST_QP_VIDEO {
    QPVD *ls_qpvd;
    char *vd_name;
};

static unsigned int num_qp = NUM_QP;
#ifndef _NUM_VD_
static unsigned int num_vd = NUM_VIDEO;
#endif
static unsigned int num_seg = MAX_NUM_OF_SEGMENTS;

#define VIDEOS struct LIST_QP_VIDEO


//static struct VIDEO *ls_vd = NULL;

/* Ham khoi tao list video */
QPVD *init_qpvd();

/* Ham khoi tao video co nhieu qp */
VIDEOS *init_lst_vd();
/* Find max bitrate of video segments in set of QP */
double find_max_rate(double *ls_rate);
double find_min_rate(double *ls_rate);
#endif
