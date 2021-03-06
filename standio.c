#include <stdio.h>
#include <stdlib.h>
#include "standio.h"


VIDEOS *load_data(const char *data_dir) 
{
    VIDEOS *ls_vd = NULL;
    unsigned int k = 0;
    FILE *fd = NULL;
    char *file_name = NULL;
    char str[MAXCHAR];

    ls_vd =(VIDEOS *) malloc(num_vd * sizeof(VIDEOS));

    for (int i = 0; i < num_vd; i++) {
        ls_vd[i].ls_qpvd = (QPVD *) malloc(sizeof(QPVD) * num_qp);
        for (int j = 0; j < num_qp; j++) {
            ls_vd[i].ls_qpvd[j].ls_rate = (double *) malloc(sizeof(double) * num_seg);
        }
    }
    printf("LOADING DATA ...\n");
    for (int i = 0; i < num_vd; i++) {
        /* Get data for one video */
        for (int j = 0; j < num_qp; j++) {
            file_name = malloc(MAX_PATH_LEN * sizeof(char));
            snprintf(file_name, MAX_PATH_LEN, 
                                "%s%s_bitrate/%sqp%d.txt", 
                                data_dir, VIDEO_NAME[i], VIDEO_NAME[i], QUANTIZIATION_PARAMS[j]);
            //printf("Get data from file directory: %s\n", file_name);
            fd = fopen(file_name, "r");
            if (fd == NULL) {
                perror("Could not open file name");
                exit(EXIT_FAILURE);
            }
            ls_vd[i].vd_name = VIDEO_NAME[i];
            ls_vd[i].ls_qpvd[j].qp = (double)QUANTIZIATION_PARAMS[j];
            /* Get data for segment */
            while (fgets(str, MAXCHAR, fd)) {
                ls_vd[i].ls_qpvd[j].ls_rate[k] = str2num(str);
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

void print_data(const char *filename, VIDEOS *ls_vd) 
{
    FILE *fd = fopen(filename, "w");

    if (fd == NULL) {
        perror("Could not open file name");
        exit(EXIT_FAILURE);
    }
    //printf("numvd: %d, numseg: %d, numqp: %d\n", num_vd, num_seg, num_qp);
    for (int i = 0; i < num_vd; i++) {
        fprintf(fd, "%s\n", ls_vd[i].vd_name);
        for (int j = 0; j < num_qp; j++) {
            fprintf(fd, "QP%f", ls_vd[i].ls_qpvd[j].qp);
            for (int k = 0; k < num_seg; k++) {
                fprintf(fd, ",%f", ls_vd[i].ls_qpvd[j].ls_rate[k]);
            }
            fprintf(fd, "\n");
        }
        fprintf(fd, "\n");
    }
    fclose(fd);
    printf("PRINT DATA DONE!\n");
}

double str2num(char *str) 
{
    int i;
    double val = 0.0 , power;

    for (i = 0; (str[i] >= '0' && str[i] <= '9'); i++) {
        val = val * 10.0 + (str[i] - '0'); 
    }
    if (str[i] == '.') {
        i++;
    }
    for (power = 1.0 ; (str[i] >= '0' && str[i] <= '9'); i++) {
        val = val * 10.0 + (str[i] - '0');
        power *= 10;
    }
    return val/power;
}

void print_list_rate(const char *file_name, double *ls_rate) 
{
    FILE *fd = NULL;
    fd = fopen(file_name, "w");
    for (int i = 0; i < num_seg; i++) {
        printf("%f\t", ls_rate[i]);
        fprintf(fd, ",%f", ls_rate[i]);
        if (((i+1)%5) == 0) printf("\n");
    }
    printf("\n");
    fclose(fd);
}

void print_result(VIDEOS *ls_vd, struct point d[5][20], FILE *fd, int *select) {
    double bw_use = 0;
    double sum_u = 0;

    for (int i = 0; i < num_vd; i++) {
        printf("VIDEO: %10s,\trate: %f,\tU: %f\n", ls_vd[i].vd_name,
                                                   d[i][select[i]].rate,
                                                   d[i][select[i]].utility);

        fprintf(fd, ",%s, %f, %f,\n", ls_vd[i].vd_name,
                                      d[i][select[i]].rate,
                                      d[i][select[i]].utility);
        fprintf(fd, ",");
        bw_use += d[i][select[i]].rate;
        sum_u += d[i][select[i]].utility;
    }

    printf("\n\tU_avg  : %f\n", sum_u/num_vd);
    printf("\tSUM_BW : %f\n", bw_use);
    fprintf(fd, ",U_avg, %f\n"
                ",,SUM_BW, %f\n", sum_u/num_vd, bw_use);
}