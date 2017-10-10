#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define LOG_PATH "./"
#define BUF_SIZE 256


int plot_data(uint64_t* xvals, uint64_t** yvals, int size, int yValCnt);
