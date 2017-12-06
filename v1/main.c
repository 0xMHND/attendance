//FIXME: currently only 1 year
//
//TODO: 
//      - add a "VERBOSE" mode + the DEBUG mode
//      - add a help option (short desc. + options).
//      - test cases in randomG
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../lib/ansiiColor.h"
#include "calc.h"
#include "plot.h"

#define FILENAME "../attendance"
//#define FILENAME "randomWeekG/randomWeeks"
#define THURSDAY 4
#define _7AM_10MIN_INTERVALS (7*60/10)
#define _5PM_10MIN_INTERVALS (17*60/10)
#define ONE_DAY_IN_SEC 24*60*60

void getArgv(int argc, char** argv, int* dayX, int* dayY, int dayCnt, int weekCnt);
void readFile(int** inTime, int** outTime, int** week, int* out_weekCnt, int* out_dayCnt);
void print_shape1()
{
    for (int n = 21, i = -n / 2; i < n / 2 + 1; i++)
        printf("%*.*s\n", n - abs(i), 1 + (n / 2 - abs(i)) * 2, "***********");
}

int mode = 0;

void drawToday( uint8_t period, uint8_t start, uint8_t end, uint8_t target)
{

//u2588 -> block
//u2591 -> dots
printf("]");
    for(uint8_t i=start; i<period; i++)
    {
        if( (i>start) && (i<end) ) //between my start until my end(now)
            printf(ANSI_COLOR_REVERSE"#"ANSI_COLOR_RESET);
            //printf(ANSI_COLOR_REVERSE"\u2588"ANSI_COLOR_RESET);
        else // other blocks
            printf(ANSI_COLOR_LIGHTCYAN"-"ANSI_COLOR_RESET);
            //printf(ANSI_COLOR_LIGHTCYAN"\u2591"ANSI_COLOR_RESET);
        if( i==target ) //target is supposed 8hr normal day.
            printf(ANSI_COLOR_RED"#"ANSI_COLOR_RESET);
            //printf(ANSI_COLOR_RED"\u2591"ANSI_COLOR_RESET);
        if( i==(_5PM_10MIN_INTERVALS) ) //at 5pm
            printf(ANSI_COLOR_YELLOWBLUE"#"ANSI_COLOR_RESET);
            //printf(ANSI_COLOR_YELLOWBLUE"\u2591"ANSI_COLOR_RESET);
    }
printf("[\n");
} 
int main(int argc, char** argv)
{
    int today = 0;
    int dayY = 0;
    int dayX = 0;
    int weekCnt = 0;
    int dayCnt  = 0;

    int nowTime[4] = {0};
    long long now_sec = 0; 

    int totalWeekTime[3] = {0}; // 40 Hr/week + 5(00:30:00)'lunch break'
    long long total_week_sec = 0; 
    int dailyAvg[3] = {0};
    long long daily_avg_sec = 0;
    int totalTime[3] = {0}; // 40 Hr/week + 5(00:30:00)'lunch break'
    long long total_sec = 0;
    int myTotalTime[3] = {0};
    long long my_total_sec = 0;
    int myWeekTime[3] = {0};
    long long my_week_sec = 0;
    int leftWeek[3] = {0};
    long long left_week_sec = 0;
    int minLeftWeek[3] = {0};
    long long min_left_week_sec = 0;
    int shLeaveToday[3] = {0};
    long long sh_leave_today = 0;
    int norLeaveToday[3] = {0};
    long long nor_leave_today = 0;
    long long nor_leave_thur = 0;
    long long sh_leave_thur = 0;
    int nowLeaveThur[3] = {0};
    long long nor_now_leave_thur = 0;
    long long sh_now_leave_thur = 0;
    int shThurRestNor[3] = {0};
    long long sh_thur_rest_nor = 0;

// *******    TIME    ******* //
    time_t rawtime;
    time( &rawtime );
    struct tm * timeinfo;
    timeinfo = localtime ( &rawtime );

    nowTime[HR] = timeinfo->tm_hour;
    nowTime[MIN] = timeinfo->tm_min;
    nowTime[SEC] = timeinfo->tm_sec;
    nowTime[DAY] = timeinfo->tm_wday; // days since sunday(0) -> 6
    printf ("Today is %s", asctime(timeinfo));
    convert_to_sec(nowTime, &now_sec);
    convert_from_sec(nowTime, now_sec);

//--------------------------------------------
    calcTotalWeekSec(&total_week_sec);
    convert_from_sec(totalWeekTime, total_week_sec);
    avgSec(total_week_sec, WORK_DAYS_PER_WEEK, &daily_avg_sec);
    convert_from_sec(dailyAvg, daily_avg_sec);

// *******    READ_FILe    ******* //
    int** week; //month, day, year
    week = malloc(sizeof(int*) * WEEK_MAX); //one year

    int** inTime = malloc(sizeof(int*)*7*WEEK_MAX); 
    long long* inSec = malloc(sizeof(long long)*7*WEEK_MAX); 
    int** outTime = malloc(sizeof(int*)*7*WEEK_MAX); 
    long long* outSec = malloc(sizeof(long long)*7*WEEK_MAX); 
    for(int i=0; i<(7*WEEK_MAX); i++)
    {
        inTime[i] = malloc(sizeof(int)*3);
        outTime[i] = malloc(sizeof(int)*3);
        for(int j=0; j<3; j++)
        {
            inTime[i][j] = 0;
            outTime[i][j] = 0;
        }
    }

    readFile( inTime, outTime, week, &weekCnt, &dayCnt);
    today = dayCnt%7;

    for(int i=0; i<dayCnt; i++)
    { 
        convert_to_sec(inTime[i], &inSec[i]);
        convert_to_sec(outTime[i], &outSec[i]);
    }

    calcTotalSec(&total_sec, daily_avg_sec, dayCnt-1);
    convert_from_sec(totalTime, total_sec);

    calcWorkedSec(&my_total_sec , 0, dayCnt-1, dayCnt, inSec, outSec, now_sec);
    convert_from_sec(myTotalTime, my_total_sec);

// *******    ARGV    ******* //
    getArgv(argc, argv, &dayX, &dayY, dayCnt, weekCnt);

    calcWorkedSec( &my_week_sec, dayX, dayY, dayCnt, inSec, outSec, now_sec);
    convert_from_sec(myWeekTime, my_week_sec);

    calcWeekRemSec(&left_week_sec, total_week_sec, my_week_sec);
    convert_from_sec(leftWeek, left_week_sec);

    if(mode == 0){
        calcMinWeekRemSec(&min_left_week_sec, total_week_sec, my_week_sec, dayCnt);
        convert_from_sec(minLeftWeek, min_left_week_sec);
        printf(" -- Minimum Left in the week: %02d:%02d:%02d\n", minLeftWeek[HR], minLeftWeek[MIN], minLeftWeek[SEC]);
        int avgLeft[3] = {0};
        long long avg_left=0;
        avgSec(min_left_week_sec, 4-(dayY%7), &avg_left);
        convert_from_sec(avgLeft, avg_left);
        printf(" -- %d avg left: %02d:%02d:%02d\n", 4-(dayY%7), avgLeft[HR], avgLeft[MIN], avgLeft[SEC]);

        calcLeaveToday( &nor_leave_today, &sh_leave_today, min_left_week_sec, dayCnt);
        printf("NORAMAL days:\n\t");
        convert_from_sec(norLeaveToday, nor_leave_today);
        printf(" -- %02d:%02d:%02d", norLeaveToday[HR], norLeaveToday[MIN], norLeaveToday[SEC]);
        convert_from_sec(norLeaveToday, nor_leave_today+now_sec);
        printf(" @ %02d:%02d:%02d\n", norLeaveToday[HR], norLeaveToday[MIN], norLeaveToday[SEC]);
        printf("SHORT days:\n\t");
        convert_from_sec(shLeaveToday, sh_leave_today);
        printf(" -- %02d:%02d:%02d", shLeaveToday[HR], shLeaveToday[MIN], shLeaveToday[SEC]);
        convert_from_sec(shLeaveToday, sh_leave_today+now_sec);
        printf(" @ %02d:%02d:%02d\n", shLeaveToday[HR], shLeaveToday[MIN], shLeaveToday[SEC]);

        uint8_t period = ONE_DAY_IN_SEC/(60*10); //one day of 10 minutes.
        uint8_t start = (inSec[dayY] / (60*10) );// - _7AM_10MIN_INTERVALS ;   //in time in 10 minutes intervals - 7am in 10 minutes intervals
        uint8_t end = (now_sec/(60*10));// - _7AM_10MIN_INTERVALS;             //now time in 10 minutes intervals - 7am in 10 minutes intervals
        uint8_t target = ((nor_leave_today+now_sec)/(60*10));// - _7AM_10MIN_INTERVALS; //today's normal leave time in 10 minutes intervals - 7am in 10 minutes intervals
        drawToday(period, start, end, target); 

        if(today < THURSDAY)
        {
            long long inNor = (7*(60*60)); // in at 7:00
            long long inSh = (8*(60*60)) + (30*60); //in at 8:30 
            calcOffThursday(&nor_leave_thur, &sh_leave_thur, &nor_now_leave_thur, &sh_now_leave_thur, &sh_thur_rest_nor, min_left_week_sec, now_sec, dayCnt);

            convert_from_sec(shThurRestNor, sh_thur_rest_nor+now_sec);
            printf("To leave @ Thur 14:30 -> leave today @ %02d:%02d:%02d (restNor)\n", shThurRestNor[HR], shThurRestNor[MIN], shThurRestNor[SEC]);
            /*
            convert_from_sec(norLeaveThur, nor_leave_thur+inNor);
            printf("out today @ 15:30 (restNor)-> leave Thur @ %02d:%02d:%02d\n", norLeaveThur[HR], norLeaveThur[MIN], norLeaveThur[SEC]);
            convert_from_sec(shLeaveThur, sh_leave_thur+inSh);
            printf("out today @ 14:30 (restSh) -> leave Thur @ %02d:%02d:%02d\n", shLeaveThur[HR], shLeaveThur[MIN], shLeaveThur[SEC]);
            */

            if(now_sec>(14*3600+30*60))
            {
                convert_from_sec(nowLeaveThur, nor_now_leave_thur+inNor);
                printf("out NOW(restNor, inNor) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
                convert_from_sec(nowLeaveThur, nor_now_leave_thur+inSh);
                printf("out NOW(restNor, inSh) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
                convert_from_sec(nowLeaveThur, sh_now_leave_thur+inNor);
                printf("out NOW(restSh, inNor) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
                convert_from_sec(nowLeaveThur, sh_now_leave_thur+inSh);
                printf("out NOW(restSh, inSh) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
            }
        }
    }

    print_shape1();
    printf("       SUMMARY\n");
    if(mode == 0)
        printf("     %02d:%02d:%02d left \n", minLeftWeek[HR], minLeftWeek[MIN], minLeftWeek[SEC]);
    else
        printf("     %02d:%02d:%02d left \n", leftWeek[HR], leftWeek[MIN], leftWeek[SEC]);
    printf("     %02d:%02d:%02d worked \n", myWeekTime[HR], myWeekTime[MIN], myWeekTime[SEC]);

#ifdef VERBOSE
    printf("weekCnt=%d\tdayCnt=%d\n", weekCnt, dayCnt);
    printf(" -- Total work hours per week : %02d:%02d:%02d\n", totalWeekTime[HR], totalWeekTime[MIN], totalWeekTime[SEC]);
    printf(" -- Supposed daily average: %d:%d:%d\n", dailyAvg[HR], dailyAvg[MIN], dailyAvg[SEC]);
    printf(" -- Supposed total worked time until today: %02d:%02d:%02d\n", totalTime[HR], totalTime[MIN], totalTime[SEC]);
    printf(" -- my total worked time until today: %02d:%02d:%02d\n", myTotalTime[HR], myTotalTime[MIN], myTotalTime[SEC]);
    printf(" -- my week work time until today: %02d:%02d:%02d\n", myWeekTime[HR], myWeekTime[MIN], myWeekTime[SEC]);
    printf(" -- Left in the week: %02d:%02d:%02d\n", leftWeek[HR], leftWeek[MIN], leftWeek[SEC]);
#endif


    uint64_t xvals[dayCnt];
    uint64_t** yvals = malloc( sizeof(uint64_t*) * 2);
    yvals[0] = malloc( sizeof(uint64_t) * dayCnt);
    yvals[1] = malloc( sizeof(uint64_t) * dayCnt);
    for(int i=0; i<dayCnt; i++)
    {
        yvals[0][i] = inSec[i];
        if( (i==dayY)&&(outSec[i]==0) )
            yvals[1][i] = now_sec;
        else
            yvals[1][i] = outSec[i];
        xvals[i] = i+1;
    }
    plot_data(xvals, yvals, dayCnt, 2);

    for(int i=0; i<(7*WEEK_MAX); i++)
    {
        free(inTime[i]);
        free(outTime[i]);
    }
    free(inTime);
    free(outTime);
    for(int i=0; i<WEEK_MAX; i++) //TODO: extend more than a year
    {
        free(week[i]);
    }
    free(week);

    free(yvals[0]);
    free(yvals[1]);
    free(yvals);

    return 0 ;
}

void getArgv(int argc, char** argv, int* dayX, int* dayY, int dayCnt, int weekCnt)
{
    int numWeek = 1;
#ifdef DEBUG
    for(int i=0; i<argc; i++)
        printf("argv[%d] : %s\n", i, argv[i]);
#endif
    switch(argc)
    {
        case 1: //NO Arguments
            mode = 0;
            *dayX = dayCnt - (dayCnt%7);    //Week's Start
            *dayY = dayCnt-1;               //TODAY
            printf("\nINIT : default (current week %d -> %d), for a certian period enter ./prog X to Y\n", *dayX, *dayY);
            break;
        case 2: // one argument
            mode = 1;
            sscanf(argv[1],"%d", &numWeek);
            if( (numWeek >= weekCnt) || (numWeek <= 0)) 
            {
                printf("ERROR: week number should be btw/ 1 -- weekCnt\n");
                exit(0);
            }
            *dayX = (numWeek*7) - 7;    //Week's Start
            *dayY = *dayX+6;            //Week's End

            printf("\nINIT : WEEK %d - from day %d until %d\n", numWeek, *dayX, *dayY);
            break;
        case 3:
            mode = 2;
            if(!strcmp(argv[1],"d")){
                sscanf(argv[2],"%d", dayX);
                *dayY = dayCnt - 1;
                printf("\nINIT : computing the period from Day %d until TODAY(%d)\n", *dayX, *dayY);
                break;
            }
        default: // argc > 2
            mode = 3;
            sscanf(argv[1],"%d", dayX);
            sscanf(argv[2],"%d", dayY);
            printf("\n init : computing the period from Day %d until %d\n", *dayX, *dayY);
            break;
    }
}

// 4 cases:
//      Curr. Week:      no arg.
//      week Z:          1 arg.
//      from X to Y:     >2 arg. argv[1] != 'd'
//      from X to Today: = 3 arg. argv[1] =='d'
void readFile(int** inTime, int** outTime, int** week, int* out_weekCnt, int* out_dayCnt)
{
    int weekCnt = *out_weekCnt;
    int dayCnt = *out_dayCnt;

    FILE* fp = fopen(FILENAME, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    char day = 'X';
    char buf[LINE_MAX];
    for(int i=0; i<WEEK_MAX; i++)
    {
        week[i] = malloc(sizeof(int*) * 4);
        for(int j=0;j<4;j++)
        {
            week[i][j]=0;
        }
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if( (buf[0]=='W') && (buf[1]=='E') ) //if first word is WEEK
        {
            if(week[51][0] == WEEK_MAX) //we've hit 52 weeks.
                week = realloc(week, sizeof(int*) * WEEK_MAX); //one year
            sscanf(buf, "WEEK%d %d-%d-%d", &week[weekCnt][0], &week[weekCnt][1], 
                                           &week[weekCnt][2], &week[weekCnt][3]);  
            weekCnt++;
        }
        else{
            sscanf(buf, "%c %d:%d:%d %d:%d:%d", &day, &inTime[dayCnt][HR], &inTime[dayCnt][MIN], 
                                                      &inTime[dayCnt][SEC], &outTime[dayCnt][HR], 
                                                      &outTime[dayCnt][MIN], &outTime[dayCnt][SEC]);
#ifdef DEBUG1
            printf("%c IN- %d:%d:%d OUT- %d:%d:%d\n", day, inTime[dayCnt][HR], 
                                                           inTime[dayCnt][MIN],
                                                           inTime[dayCnt][SEC], 
                                                           outTime[dayCnt][HR],
                                                           outTime[dayCnt][MIN], 
                                                           outTime[dayCnt][SEC]);
#endif
            dayCnt++;
        }
    }
    fclose(fp);

    *out_weekCnt = weekCnt;
    *out_dayCnt = dayCnt;
}

