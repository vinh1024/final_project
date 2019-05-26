#ifndef _STANDIO_
#define _STANDIO_

#include "video.h"

double str2num(char *str);

VIDEOS *load_data(const char *data_dir);


void print_data(const char *filename, VIDEOS *ls_vd);

void print_list_rate(const char *file_name, double *ls_rate);

void print_result(VIDEOS *ls_vd, struct point d[5][20], FILE *fd, int *select);

#endif

