#ifndef _VIDEO_H
#define _VIDEO_H


#define MAXCHAR 100
#define MAX_PATH_LEN 256
#define INIT_QP 1
#define T (2)                   //thoi gian cua mot segment
#define num_qp (6)
#define num_vd (5)
#define num_seg (294)
#define num_rate (20)
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



#define VIDEOS struct LIST_QP_VIDEO

struct point {
	double rate;
	double qp;
	double utility;
	double d0;
};
//static struct VIDEO *ls_vd = NULL;

/* Ham khoi tao list video */
QPVD *init_qpvd();

/* Ham khoi tao video co nhieu qp */
VIDEOS *init_lst_vd();
/* Find max bitrate of video segments in set of QP */

#endif
