//FIXME: currently only 1 year
//
//TODO: 
//      - add a "VERBOSE" mode + the DEBUG mode
//      - add a help option (short desc. + options).
//
//FIXME: search for 'malloc' and then free it
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ansiiColor.h"
#include "calc.h"

void print_shape1()
{
    for (int n = 21, i = -n / 2; i < n / 2 + 1; i++)
        printf("%*.*s\n", n - abs(i), 1 + (n / 2 - abs(i)) * 2, "***********");
}


#define FILENAME "attendance"
#ifdef DEBUG
//#define FILENAME "randomWeekG/randomWeek"
#endif


enum days{Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};

int mode = 0;
// 4 cases:
//      Curr. Week:      no arg.
//      week Z:          1 arg.
//      from X to Y:     >2 arg. argv[1] != 'd'
//      from X to Today: = 3 arg. argv[1] =='d'
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

int main(int argc, char** argv)
{


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
    int norLeaveThur[3] = {0};
    long long nor_leave_thur = 0;
    int shLeaveThur[3] = {0};
    long long sh_leave_thur = 0;
    int nowLeaveThur[3] = {0};
    long long nor_now_leave_thur = 0;
    long long sh_now_leave_thur = 0;

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
//    printf(" -- Total work hours per week : %02d:%02d:%02d\n", totalWeekTime[HR], totalWeekTime[MIN], totalWeekTime[SEC]);
    avgSec(total_week_sec, WORK_DAYS_PER_WEEK, &daily_avg_sec);
    convert_from_sec(dailyAvg, daily_avg_sec);
//    printf(" -- Supposed daily average: %d:%d:%d\n", dailyAvg[HR], dailyAvg[MIN], dailyAvg[SEC]);

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
    printf("weekCnt=%d\tdayCnt=%d\n", weekCnt, dayCnt);

    for(int i=0; i<dayCnt; i++)
    { 
        convert_to_sec(inTime[i], &inSec[i]);
        convert_to_sec(outTime[i], &outSec[i]);
    }

    calcTotalSec(&total_sec, daily_avg_sec, dayCnt-1);
    convert_from_sec(totalTime, total_sec);
    printf(" -- Supposed total worked time until today: %02d:%02d:%02d\n", totalTime[HR], totalTime[MIN], totalTime[SEC]);

    calcWorkedSec(&my_total_sec , 0, dayCnt-1, dayCnt, inSec, outSec, now_sec);
    convert_from_sec(myTotalTime, my_total_sec);
    printf(" -- my total worked time until today: %02d:%02d:%02d\n", myTotalTime[HR], myTotalTime[MIN], myTotalTime[SEC]);

// *******    ARGV    ******* //
    getArgv(argc, argv, &dayX, &dayY, dayCnt, weekCnt);

    calcWorkedSec( &my_week_sec, dayX, dayY, dayCnt, inSec, outSec, now_sec);
    convert_from_sec(myWeekTime, my_week_sec);
    printf(" -- my week work time until today: %02d:%02d:%02d\n", myWeekTime[HR], myWeekTime[MIN], myWeekTime[SEC]);

    calcWeekRemSec(&left_week_sec, total_week_sec, my_week_sec);
    convert_from_sec(leftWeek, left_week_sec);
    printf(" -- Left in the week: %02d:%02d:%02d\n", leftWeek[HR], leftWeek[MIN], leftWeek[SEC]);
    if(mode == 0){
        int today = dayY;
        calcMinWeekRemSec(&min_left_week_sec, total_week_sec, my_week_sec, dayCnt);
        convert_from_sec(minLeftWeek, min_left_week_sec);
        printf(" -- Minimum Left in the week: %02d:%02d:%02d\n", minLeftWeek[HR], minLeftWeek[MIN], minLeftWeek[SEC]);

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

        
        long long inNor = (7*(60*60)); // in at 7:00
        long long inSh = (8*(60*60)) + (30*60); //in at 8:30 
        calcOffThursday(&nor_leave_thur, &sh_leave_thur, &nor_now_leave_thur, &sh_now_leave_thur,  min_left_week_sec, now_sec, dayCnt);

        convert_from_sec(norLeaveThur, nor_leave_thur+inNor);
        printf("out today @ 15:30 (restNor)-> leave Thur @ %02d:%02d:%02d\n", norLeaveThur[HR], norLeaveThur[MIN], norLeaveThur[SEC]);
        convert_from_sec(shLeaveThur, sh_leave_thur+inSh);
        printf("out today @ 14:30 (restSh) -> leave Thur @ %02d:%02d:%02d\n", shLeaveThur[HR], shLeaveThur[MIN], shLeaveThur[SEC]);

        convert_from_sec(nowLeaveThur, nor_now_leave_thur+inNor);
        printf("out NOW(restNor, inNor) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
        convert_from_sec(nowLeaveThur, nor_now_leave_thur+inSh);
        printf("out NOW(restNor, inSh) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
        convert_from_sec(nowLeaveThur, sh_now_leave_thur+inNor);
        printf("out NOW(restSh, inNor) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
        convert_from_sec(nowLeaveThur, sh_now_leave_thur+inSh);
        printf("out NOW(restSh, inSh) -> leave Thur @ %02d:%02d:%02d\n", nowLeaveThur[HR], nowLeaveThur[MIN], nowLeaveThur[SEC]);
    }

    for(int i=0; i<(7*WEEK_MAX); i++)
    {
        free(inTime[i]);
        free(outTime[i]);
    }
    for(int i=0; i<WEEK_MAX; i++) //TODO: extend more than a year
    {
        free(week[i]);
    }
    free(week);
    return 0 ;
}
