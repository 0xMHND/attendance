#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "func.h"


void print_sec(uint32_t sec){
    printf("%02d:%02d:%02d", sec/3600, (sec%3600)/60, (sec%3600)%60);
}


int main(int argc, char** argv){

    WEEK_t * stats = malloc( sizeof(WEEK_t) * WEEK_MAX);
    uint16_t week_index = 0;

    //Get current time
    time_t rawtime; time( &rawtime ); struct tm * timeinfo; timeinfo = localtime ( &rawtime );
    printf ("Today is %s", asctime(timeinfo));
    time_t time_now = timeinfo->tm_sec + timeinfo->tm_min*60 + timeinfo->tm_hour*3600;


    //Read Stats from Attendance file
    readAttendance(stats, &week_index);

    //Set today's work out to current time 
    if(stats[week_index-1].days[timeinfo->tm_wday].workOut == 0)
        stats[week_index-1].days[timeinfo->tm_wday].workOut = time_now;

    //Read Stats from Home file.
    readHome(stats);

    // print
    printStats(stats, week_index);
    printf ("avg To work : ");
    print_sec(avg_in(stats, week_index));
    printf("\n");
    printf ("avg To home : ");
    print_sec(avg_out(stats, week_index));
    printf("\n");

    //Plot
    plot_stats(stats, week_index, time_now);

    return 0;
}

