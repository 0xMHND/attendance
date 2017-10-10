#ifndef __FUNC_H
#define __FUNC_H
#endif

#include <stdint.h>
#include "plot.h"

#define FILE_ATTENDANCE "../attendance"
#define FILE_HOME "../home"

#define DAYS_SIZE 52*7 //one year
#define WEEK_MAX 52 //one year
#define LINE_MAX 512
#define COLUMNS 4 //homeOUt, workIN, workOUT

typedef enum {OFF, FULL, HALF, ABS, LATE, SPECIAL} DAY_t;

typedef struct {
    char day;
    DAY_t type;
    uint32_t workIn;
    uint32_t workOut;
    uint32_t homeIN;
    uint32_t homeOUT;
    char note[256]; //any note about the day
} DATA_t;

typedef struct {
    uint8_t week_num;
    uint8_t week_month;
    uint8_t week_day;
    uint16_t week_year;

    DATA_t days[7];
} WEEK_t;


void plot_stats(WEEK_t * w, uint16_t index, time_t time_now);
double avg_out(WEEK_t *w, uint16_t index);
double avg_in(WEEK_t *w, uint16_t index);
int return_day_number(char day);
void readAttendance(WEEK_t *w, uint16_t* week_index); 
void readHome(WEEK_t *w); 
void printStats(WEEK_t *stats, uint16_t week_index);

