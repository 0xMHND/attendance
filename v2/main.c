#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "func.h"

void print_sec(uint32_t sec){
    printf("%02d:%02d:%02d", sec/3600, (sec%3600)/60, (sec%3600)%60);
}
double avg_out(WEEK_t *w, uint16_t index){
    double sum = 0.0;
    uint16_t days_cnt = 0;
    for(int i=0; i<index; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(w[i].days[j].homeIN > 0)
            {
                sum += w[i].days[j].homeIN - w[i].days[j].workOut;
                days_cnt++;
            }
        }

    }
    sum = sum/days_cnt;
    return sum;
}
float avg_in(WEEK_t *w, uint16_t index){
    double sum = 0.0;
    uint16_t days_cnt = 0;
    for(int i=0; i<index; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(w[i].days[j].homeOUT > 0)
            {
                sum += w[i].days[j].workIn - w[i].days[j].homeOUT;
                days_cnt++;
            }
        }

    }
    sum = sum/days_cnt;
    return sum;
}

int main(int argc, char** argv){

    WEEK_t * stats = malloc( sizeof(WEEK_t) * WEEK_MAX);
    uint16_t week_index = 0;

    readAttendance(stats, &week_index);
    readHome(stats);

//    printStats(stats, week_index);

    printf ("avg To work : ");
    print_sec(avg_in(stats, week_index));
    printf("\n");
    printf ("avg To home : ");
    print_sec(avg_out(stats, week_index));
    printf("\n");
    return 0;
}


