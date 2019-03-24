#ifndef _STANDIO_
#define _STANDIO_

#include "video.h"

double str2num(char *str);

VIDEOS *load_data(const char *data_dir);


void print_data(const char *filename, VIDEOS *ls_vd, 
                unsigned int num_vd, unsigned int num_seg, unsigned int num_qp);

#endif

