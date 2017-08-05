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
void print_shape1()
{
    for (int n = 21, i = -n / 2; i < n / 2 + 1; i++)
        printf("%*.*s\n", n - abs(i), 1 + (n / 2 - abs(i)) * 2, "***********");
}


#define FILENAME "attendance"
#ifdef DEBUG
//#define FILENAME "randomWeekG/randomWeek"
#endif

#define LINE_MAX 50
#define WEEK_MAX 52
#define WEEK_NUM 1 
const int LUNCH_BREAK[3] = {0,30, 0};
const int WEEK_WORK_REQ[3] = {40, 0, 0};
const int WORK_DAYS_PER_WEEK = 5;

#define NORMAL_SH   0 //in 7:00 out 15:30
#define SHORT_SH    1 //in 8:30 out 14:30
//
// modes:
//      0 - Last week   
//      1 - specefic week 
//

enum time{HR, MIN, SEC, DAY};
void adjTime(int* _N)
{
    int N[3] = {0};
    N[SEC] = _N[SEC];
    N[MIN] = _N[MIN];
    N[HR] = _N[HR];
#ifdef DEBUG
            printf("DEBUG: adjTime(1) IN- %d:%d:%d OUT- %d:%d:%d\n", _N[HR], _N[MIN], _N[SEC], N[HR], N[MIN], N[SEC]);
#endif

    while( N[SEC] > 59)
    {
        N[SEC] -= 60; 
        N[MIN]++;
    }
    while( N[SEC] < -59)
    {
        N[SEC] += 60; 
        N[MIN]--;
    }
    while( N[MIN] > 59)
    {
        N[MIN] -= 60; 
        N[HR]++;
    }
    while( N[MIN] < -59)
    {
        N[MIN] += 60; 
        N[HR]--;
    }

#ifdef DEBUG
            printf("DEBUG: adjTime(2) IN- %d:%d:%d OUT- %d:%d:%d\n", _N[HR], _N[MIN], _N[SEC], N[HR], N[MIN], N[SEC]);
#endif
/*************************************************/
/*************************************************/

// if HR is - but MIN is + (HR++, MIN-60) || if MIN is - but SEC is + (MIN++, SEC-60)
// if MIN is - but HR is + (HR--, MIN+60) || if SEC is - but MIN is + (MIN--, SEC+60)


    while( (N[HR]>=0) && (N[MIN]<0) )
    {   
        N[MIN] += 60;
        N[HR]--;
    }
    while( (N[MIN]>=0) && (N[SEC]<0) )
    {   
        N[SEC] += 60;
        N[MIN]--;
    }

#ifdef DEBUG
            printf("DEBUG: adjTime(1) IN- %d:%d:%d OUT- %d:%d:%d\n", _N[HR], _N[MIN], _N[SEC], N[HR], N[MIN], N[SEC]);
#endif
//account for when HR < 0 but MIN > 0 and SEC > 0
    if(N[HR] < 0){
        if(N[SEC] >= 0)
            N[SEC] *= -1;
        if(N[MIN] >= 0)
            N[MIN] *= -1;
    }
#ifdef DEBUG
            printf("DEBUG: adjTime(4) IN- %d:%d:%d OUT- %d:%d:%d\n", _N[HR], _N[MIN], _N[SEC], N[HR], N[MIN], N[SEC]);
#endif

    _N[SEC] = N[SEC];
    _N[MIN] = N[MIN];
    _N[HR] = N[HR];
}

void checkOverflow(int *T){
        if(T[SEC] > 59)
        {
            T[SEC] -= 60;
            T[MIN]++;
        }
        else if(T[SEC] < 0)
        {
            T[SEC] += 60;
            T[MIN]--;
        }
        if(T[MIN] > 59)
        {
            T[MIN] -= 60;
            T[HR]++;
        }
        else if(T[MIN] < 0)
        {
            T[MIN] += 60;
            T[HR]--;
        }
}
enum days{Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};
int convert_from_sec(int* n, long long s)
{
    long long temp = s;
    n[HR] = (temp/3600);
    temp = (temp%3600);
    n[MIN] = temp/60; 
    n[SEC] = temp%60;
return 0;
}
int convert_to_sec(int* n, long long* s)
{
    *s = n[SEC] + 60*n[MIN] + 3600*n[HR]; 
return 0; 
}

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
            *dayX = dayCnt - (dayCnt%7);    //Week's Start
            *dayY = dayCnt-1;               //TODAY
            printf("\nINIT : default (current week %d -> %d), for a certian period enter ./prog X to Y\n", *dayX, *dayY);
            break;
        case 2: // one argument
            sscanf(argv[1],"%d", &numWeek);
            if(numWeek > weekCnt)
            {
                printf("ERROR: week number > available\n");
                exit(0);
            }
            *dayX = (numWeek*7) - 7;    //Week's Start
            *dayY = *dayX+6;            //Week's End

            printf("\nINIT : WEEK %d - from day %d until %d\n", numWeek, *dayX, *dayY);
            break;
        case 3:
            if(!strcmp(argv[1],"d")){
                sscanf(argv[2],"%d", dayX);
                *dayY = dayCnt - 1;
                printf("\nINIT : computing the period from Day %d until TODAY(%d)\n", *dayX, *dayY);
                break;
            }
        default: // argc > 2
            sscanf(argv[1],"%d", dayX);
            sscanf(argv[2],"%d", dayY);
            printf("\n init : computing the period from Day %d until %d\n", *dayX, *dayY);
            break;
    }
}

//TODO:
void _print_readFile(int* inTime, int* outTime, int* week, int weekCnt, int dayCnt){

    for(int i=0; i<dayCnt; i++)
    {
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
        week[i] = malloc(sizeof(int*) * 4); //one year
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


void calcWorkedSec(long long* _workedTime, int dayX, int dayY, int dayCnt, long long* _inSec, long long* _outSec, long long _nowSec)
{
    long long workedTime = 0;
    workedTime = *_workedTime;
    int today = dayCnt-1;
    int wday = today%7;
    printf("wday:%d\n", wday);
    long long oneDay = 0;

#ifdef DEBUG
    int oneDay_v[3] = {0};
// print the period's log
    for(int i=dayX; i<=dayY; i++)
        printf("%03d IN- %lldd OUT- %lld\n", i, _inSec[i], _outSec[i]);
#endif
    for(int i=dayX; i<=dayY; i++)
    {
        if( (i==dayY) && (dayY==today) && (_outSec[i]==0)) //if dayY is Today
        {
            workedTime += _nowSec - _inSec[i]; // -59 to 59
            oneDay = _nowSec - _inSec[i]; // -59 to 59

#ifdef DEBUG
    convert_from_sec(oneDay_v, oneDay);
    printf("DEBUG: TODAY[%d] worked %02d:%02d:%02d\n", i, oneDay_v[HR], oneDay_v[MIN], oneDay_v[SEC]);
    printf("DEBUG: nowTime %lld \n", _nowSec);
#endif
        }
        else
        {
            workedTime += _outSec[i] - _inSec[i]; // -59 to 59
            oneDay = _outSec[i] - _inSec[i]; // -59 to 59
        }

#ifdef DEBUG
    int worked_temp[3] = {0};
    convert_from_sec(oneDay_v, oneDay);
    convert_from_sec(worked_temp, workedTime);
    printf("DEBUG: day[%d] worked %02d:%02d:%02d\n", i, oneDay_v[HR], oneDay_v[MIN], oneDay_v[SEC]);
    printf("DEBUG: day[%d] worked %02d:%02d:%02d so far\n", i, worked_temp[HR], worked_temp[MIN], worked_temp[SEC]);
#endif
       } 

    *_workedTime = workedTime;
}

void calcNetWeekTime(int* _workedTime, int* _netWeekTime, int* totalWeekTime, int dayY)
{
    int N[3]   = {0};  
    N[HR]      = _netWeekTime[HR];
    N[MIN]     = _netWeekTime[MIN];
    N[SEC]     = _netWeekTime[SEC];

    N[SEC] = totalWeekTime[SEC] - _workedTime[SEC]; // -59 to 59
    N[MIN] = totalWeekTime[MIN] - _workedTime[MIN]; // -59 to 59
    N[HR] = totalWeekTime[HR] - _workedTime[HR];
    adjTime(N);

#ifdef DEBUG
    printf("DEBUG; netTime(before min) \"%d:%d:%d\" \n", N[HR], N[MIN], N[SEC]);
#endif

    int remDays = 6 - (dayY%7);
#ifdef DEBUG
    printf("DEBUG; remDays %d\n", remDays);
#endif
    // make sure net remaining week time is above daily work minimum(8:30 to 14:30)
    int minRemWeekTime = (remDays - 2) * 6;
    if( N[HR] < minRemWeekTime)
        N[HR] += minRemWeekTime;

#ifdef DEBUG
    printf("DEBUG; netTime(After min) \"%d:%d:%d\" \n", N[HR], N[MIN], N[SEC]);
#endif
    _netWeekTime[HR] = N[HR];
    _netWeekTime[MIN] = N[MIN];
    _netWeekTime[SEC] = N[SEC];
}

void calcRemDayTime(int* netWTime, int dayY, int* nowTime)
{
    int _maxRemDTime[3] = {0};  // remaining day time w/ 8:30-14:30 next days. (Short shift)
    int _norRemDTime[3] = {0};  // remaining day time w/ 7:00-15:30 next days. (Normal shift)
    int _minRemWeekTime[3] = {0};
    int _norRemWeekTime[3] = {0};
    int _remDays = 6 - (dayY%7);
    int today = dayY%7;
    if(_remDays > 2)
    {
        _minRemWeekTime[HR] = (_remDays - 2) * 6;
        _norRemWeekTime[HR] = (_remDays - 2) * 8;
        _norRemWeekTime[MIN] = (_remDays - 2) * 30;
        while(_norRemWeekTime[MIN] > 59)
        {
            _norRemWeekTime[MIN] -= 60;
            _norRemWeekTime[HR]++;
        }
    }

    _maxRemDTime[SEC] = netWTime[SEC] - _minRemWeekTime[SEC]; // >=0
    _maxRemDTime[MIN] = netWTime[MIN] - _minRemWeekTime[MIN]; // >=0
    _maxRemDTime[HR] = netWTime[HR] - _minRemWeekTime[HR];    // >=0
    checkOverflow(_maxRemDTime);
    _norRemDTime[SEC] = netWTime[SEC] - _norRemWeekTime[SEC]; // >=0
    _norRemDTime[MIN] = netWTime[MIN] - _norRemWeekTime[MIN]; // >=0
    _norRemDTime[HR] = netWTime[HR] - _norRemWeekTime[HR];    // >=0
    adjTime(_norRemDTime);
    

    printf("Maximum shift : %02d:%02d:%02d ", _maxRemDTime[HR], _maxRemDTime[MIN], _maxRemDTime[SEC]);
    int _shLeaveTime[3] = {3};
    _shLeaveTime[SEC] = nowTime[SEC] + _maxRemDTime[SEC]; // >=0
    _shLeaveTime[MIN] = nowTime[MIN] + _maxRemDTime[MIN]; // >=0
    _shLeaveTime[HR] = nowTime[HR] + _maxRemDTime[HR];    // >=0
    checkOverflow(_shLeaveTime);
    printf(" @ %02d:%02d:%02d\n", _shLeaveTime[HR], _shLeaveTime[MIN], _shLeaveTime[SEC]);
    printf("[NORMAL] shift : %02d:%02d:%02d", _norRemDTime[HR], _norRemDTime[MIN], _norRemDTime[SEC]);
    int _norLeaveTime[3] = {3};
    _norLeaveTime[SEC] = nowTime[SEC] + _norRemDTime[SEC]; // >=0
    _norLeaveTime[MIN] = nowTime[MIN] + _norRemDTime[MIN]; // >=0
    _norLeaveTime[HR] = nowTime[HR] + _norRemDTime[HR];    // >=0
    adjTime(_norLeaveTime);
    printf(" @ %02d:%02d:%02d\n", _norLeaveTime[HR], _norLeaveTime[MIN], _norLeaveTime[SEC]);

    print_shape1();
    printf("\tSUMMARY\n");
    if(today<4) // if berofr thursday
         printf("to leave Thursday at 14:30, work today until %02d:%02d:%02d\n", _shLeaveTime[HR],
                                                                                _shLeaveTime[MIN], 
                                                                                _shLeaveTime[SEC]); 

    else
        printf("Remaining | %02d:%02d:%02d\n", netWTime[HR], netWTime[MIN], netWTime[SEC]);


}

// FIXME :: need more efficiency
void offThursday(int* nowTime, int* netWTime, int dayY)
{
    int _bufTime[3] = {0};
    _bufTime[SEC] = netWTime[SEC]; // >=0
    _bufTime[MIN] = netWTime[MIN] - (30 - nowTime[MIN]); // >=0
    _bufTime[HR] = netWTime[HR] - (14 - nowTime[HR]);    // >=0
    int today = dayY%7;

#ifdef DEBUG
    printf("DEBUG: buf(before adj) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif
    adjTime(_bufTime);
#ifdef DEBUG
    printf("DEBUG: buf(after adj) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif
// 0 S, 1 M, 2 T, 3 W, 4 R, 5 F, 6 U
    int _remDays = 6 - (dayY%7);
    _bufTime[HR] -= (_remDays-2) * 6; 
#ifdef DEBUG
    printf("DEBUG: buf(before adj - nextDays inc) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif
    adjTime(_bufTime);
#ifdef DEBUG
    printf("DEBUG: buf(after adj - nextDays inc) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif

    int _shLeaveTime[3] = {0};
    _shLeaveTime[SEC] = _bufTime[SEC]; // >=0
    _shLeaveTime[MIN] = _bufTime[MIN] + 30;
    _shLeaveTime[HR] = _bufTime[HR] + 14;

    // -1:53:00
    // 14:30:00
    //
    // 14:23:00
    adjTime(_shLeaveTime);
#ifdef DEBUG
    printf("Short Shift --> offThursday @ %02d:%02d:%02d     ---- buf %02d:%02d:%02d\n", _shLeaveTime[HR], _shLeaveTime[MIN], _shLeaveTime[SEC],
                                                                                         _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);  
#endif
    // buf - (1:00 + (_remDays - 3)*2:30:00 )
    _bufTime[HR] -= 1 + (_remDays-3) * 2; // one hour (inst. 14:30 -> 15:30) + remDays ex/Thursday (8:30 hrs) shift.
    _bufTime[MIN] -= (_remDays-3) * 30; // one hour (inst. 14:30 -> 15:30) + remDays ex/Thursday (8:30 hrs) shift.
#ifdef DEBUG
    printf("DEBUG: buf(before adj - nextDays inc) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif
    adjTime(_bufTime);
#ifdef DEBUG
    printf("DEBUG: buf(after adj - nextDays inc) -- %02d:%02d:%02d\n", _bufTime[HR], _bufTime[MIN], _bufTime[SEC]);
#endif

    int _norLeaveTime[3] = {0};
    _norLeaveTime[SEC] = _bufTime[SEC]; // >=0
    _norLeaveTime[MIN] = _bufTime[MIN] + 30;
    _norLeaveTime[HR] = _bufTime[HR] + 14;

    adjTime(_norLeaveTime);
#ifdef DEUBG
    printf("Normal Shift --> offThursday @ %02d:%02d:%02d     ---- buf %02d:%02d:%02d\n", _norLeaveTime[HR], _norLeaveTime[MIN], _norLeaveTime[SEC],
                                                                                         _bufTime[HR], _bufTime[MIN], _bufTime[SEC]); 
#endif
    
    if(today<4) // if berofr thursday
        printf("if you work today until 14:30, then you leave Thursday at %02d:%02d:%02d\n", _shLeaveTime[HR], _shLeaveTime[MIN], _shLeaveTime[SEC]); 
}

int calcTotalWeekSec(long long* _totalWeekTime)
{
    long long temp = 0;
    temp = ( WEEK_WORK_REQ[SEC] + ( LUNCH_BREAK[SEC] * WORK_DAYS_PER_WEEK) ) + 
           ( 60*(WEEK_WORK_REQ[MIN] + ( LUNCH_BREAK[MIN] * WORK_DAYS_PER_WEEK)) ) +
           ( 3600*( WEEK_WORK_REQ[HR] + ( LUNCH_BREAK[HR] * WORK_DAYS_PER_WEEK)) ) ;

    printf("tmep %lld\nl", temp);
 
    *_totalWeekTime = temp;
    return 0;
}
int avgSec(long long _data, int numDays, long long* _avg)
{
    long long temp = 0;
    temp = _data/numDays;

    *_avg = temp;
    return 0;
}

int calcTotalSec(long long* _result, long long _avg, int _period)
{
    long long temp = 0;
    int workDays = (_period - ( ((_period/5)-1) *2) );
    temp = _avg * workDays;

#ifdef DEBUG
    printf("DEBUG -- %lld  || period: %d days\n", temp, workDays);
#endif

    *_result = temp;

    return 0;
}

int calcWeekRemSec(long long* _leftWeek, long long _totalWeekTime, long long _myTotalWeekTime)
{
    long long leftWeek = 0;
    leftWeek = _totalWeekTime - _myTotalWeekTime;
    *_leftWeek = leftWeek;

    return 0; 
}

int main(int argc, char** argv)
{
    int dayY = 0;
    int dayX = 0;
    int weekCnt = 0;
    int dayCnt  = 0;
    int workedTime[3] = {0};
    long long worked_sec = 0; 

    int _netWeekTime[3] = {0};
    long long net_week_sec = 0; 

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
    printf ("in seconds %lld\n", now_sec);
    printf ("back to %02d:%02d:%02d \n", nowTime[HR], nowTime[MIN], nowTime[SEC]);
//--------------------------------------------
    calcTotalWeekSec(&total_week_sec);
    printf ("total_week seconds %lld\n", total_week_sec);
    convert_from_sec(totalWeekTime, total_week_sec);
    printf(" -- Total work hours per week : %02d:%02d:%02d\n", totalWeekTime[HR], totalWeekTime[MIN], totalWeekTime[SEC]); 
    avgSec(total_week_sec, WORK_DAYS_PER_WEEK, &daily_avg_sec);
    printf(" -- Supposed daily average: %lld sec\n", daily_avg_sec);
    convert_from_sec(dailyAvg, daily_avg_sec);
    printf(" -- Supposed daily average: %d:%d:%d\n", dailyAvg[HR], dailyAvg[MIN], dailyAvg[SEC]);

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
#ifdef DEBUG 
    printf("weekCnt=%d\tdayCnt=%d\n", weekCnt, dayCnt);
#endif
    for(int i=0; i<dayCnt; i++)
    { 
        convert_to_sec(inTime[i], &inSec[i]);
        convert_to_sec(outTime[i], &outSec[i]);
        //printf(" -- day[%d]: IN %lld sec ----  OUT %lld\n", i, inSec[i], outSec[i]);
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
/*

    calcNetWeekTime(workedTime, _netWeekTime, totalWeekTime, dayY);
    printf("\n");
    calcRemDayTime(_netWeekTime, dayY, nowTime);
    offThursday(nowTime, _netWeekTime, dayY);

*/

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
