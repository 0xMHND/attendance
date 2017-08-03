//
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
enum days{Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};

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

// FIXME: BUG| this doesn't fix 3:00:-xx
void adjTime(int* _N)
{
    int N[3] = {0};
    N[SEC] = _N[SEC];
    N[MIN] = _N[MIN];
    N[HR] = _N[HR];

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

/*************************************************/
/*************************************************/

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

//account for when HR < 0 but MIN > 0 and SEC > 0
    if(N[HR] < 0){
        if(N[SEC] >= 0)
            N[SEC] *= -1;
        if(N[MIN] >= 0)
            N[MIN] *= -1;
    }

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

void calcWorkedTime(int* _workedTime, int dayX, int dayY, int dayCnt, int** inTime, int** outTime, int* nowTime)
{
    int workedTime[3]   = {0};
    workedTime[HR]      = _workedTime[HR];
    workedTime[MIN]     = _workedTime[MIN];
    workedTime[SEC]     = _workedTime[SEC];
    int today = dayCnt-1;
    int oneDay[3] = {0};

#ifdef DEBUG
// print the period's log
    for(int i=dayX; i<=dayY; i++)
        printf("%03d IN- %02d:%02d:%02d OUT- %02d:%02d:%02d\n", i, inTime[i][HR], 
                                                                 inTime[i][MIN],
                                                                 inTime[i][SEC], 
                                                                 outTime[i][HR], 
                                                                 outTime[i][MIN], 
                                                                 outTime[i][SEC]);
#endif
    for(int i=dayX; i<=dayY; i++)
    {
        if( (i==dayY) && (dayY==today) && (outTime[i][HR]==0) && (outTime[i][MIN]==0) && (outTime[i][SEC]==0)) //if dayY is Today
        {
            workedTime[SEC] += nowTime[SEC] - inTime[i][SEC]; // -59 to 59
            oneDay[SEC] = nowTime[SEC] - inTime[i][SEC]; // -59 to 59
            workedTime[MIN] += nowTime[MIN] - inTime[i][MIN]; // -59 to 59
            oneDay[MIN] = nowTime[MIN] - inTime[i][MIN]; // -59 to 59
            workedTime[HR] += nowTime[HR] - inTime[i][HR];
            oneDay[HR] = nowTime[HR] - inTime[i][HR];
        }
        else
        {
            workedTime[SEC] += outTime[i][SEC] - inTime[i][SEC]; // -59 to 59
            oneDay[SEC] = outTime[i][SEC] - inTime[i][SEC]; // -59 to 59
            workedTime[MIN] += outTime[i][MIN] - inTime[i][MIN]; // -59 to 59
            oneDay[MIN] = outTime[i][MIN] - inTime[i][MIN]; // -59 to 59
            workedTime[HR] += outTime[i][HR] - inTime[i][HR];
            oneDay[HR] = outTime[i][HR] - inTime[i][HR];
        }
        adjTime(workedTime);
        //checkOverflow(oneDay);
        adjTime(oneDay);

#ifdef DEBUG
    printf("DEBUG: day[%d] worked %02d:%02d:%02d\n", i, oneDay[HR], oneDay[MIN], oneDay[SEC]);
    printf("DEBUG: day[%d] worked %02d:%02d:%02d so far\n", i, workedTime[HR], workedTime[MIN], workedTime[SEC]);
#endif
       } 

    _workedTime[HR]      = workedTime[HR];
    _workedTime[MIN]     = workedTime[MIN];
    _workedTime[SEC]     = workedTime[SEC];
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
    
    printf("if you work today until 14:30, then you leave Thursday at %02d:%02d:%02d\n", _shLeaveTime[HR], _shLeaveTime[MIN], _shLeaveTime[SEC]); 
}

int calcTotalWeekTime(int* _totalWeekTime)
{
    int temp[3] = {0};
    temp[SEC] = WEEK_WORK_REQ[SEC] + ( LUNCH_BREAK[SEC] * WORK_DAYS_PER_WEEK);
    temp[MIN] = WEEK_WORK_REQ[MIN] + ( LUNCH_BREAK[MIN] * WORK_DAYS_PER_WEEK);
    temp[HR] = WEEK_WORK_REQ[HR] + ( LUNCH_BREAK[HR] * WORK_DAYS_PER_WEEK);

    adjTime(temp);

    _totalWeekTime[SEC] = temp[SEC]; 
    _totalWeekTime[MIN] = temp[MIN]; 
    _totalWeekTime[HR] = temp[HR];
    return 0;
}
int avg(int* _data, int numDays, int* _avg)
{
    int temp[3] = {0};
    temp[SEC] = (_data[SEC] + ( (_data[MIN]%numDays) * 60) )/ numDays;
    temp[MIN] = (_data[MIN] + ( (_data[HR]%numDays) * 60) )/ numDays;
    temp[HR] = (_data[HR]/numDays);

    adjTime(temp);

    _avg[SEC] = temp[SEC]; 
    _avg[MIN] = temp[MIN]; 
    _avg[HR] = temp[HR];
    return 0;
}

int calcTotal(int* _result, int* _avg, int _period)
{
    int temp[3] = {0};
    int workDays = (_period - ( ((_period/5)-1) *2) );
    temp[SEC] = _avg[SEC] * workDays;
    temp[MIN] = _avg[MIN] * workDays;
    temp[HR] = _avg[HR] * workDays;

#ifdef DEBUG
    printf("DEBUG -- %d:%d:%d  || period: %d days\n", temp[HR], temp[MIN], temp[SEC], workDays);
#endif

    adjTime(temp);

    _result[SEC] = temp[SEC]; 
    _result[MIN] = temp[MIN]; 
    _result[HR] = temp[HR];

    return 0;
}
int main(int argc, char** argv)
{
    int dayY = 0;
    int dayX = 0;
    int weekCnt = 0;
    int dayCnt  = 0;
    int workedTime[3] = {0};
    int _netWeekTime[3] = {0};
    int nowTime[4] = {0};

    int totalWeekTime[3] = {0}; // 40 Hr/week + 5(00:30:00)'lunch break'
    int dailyAvg[3] = {0};
    int totalTime[3] = {0}; // 40 Hr/week + 5(00:30:00)'lunch break'
    int myTotalTime[3] = {0};

    calcTotalWeekTime(totalWeekTime);
    printf(" -- Total work hours per week : %02d:%02d:%02d\n", totalWeekTime[HR], totalWeekTime[MIN], totalWeekTime[SEC]); 
    avg(totalWeekTime, WORK_DAYS_PER_WEEK, dailyAvg);
    printf(" -- Supposed daily average: %d:%d:%d\n", dailyAvg[HR], dailyAvg[MIN], dailyAvg[SEC]);
    
// *******    READ_FILe    ******* //
    int** week; //month, day, year
    week = malloc(sizeof(int*) * WEEK_MAX); //one year

    int** inTime = malloc(sizeof(int*)*7*WEEK_MAX); 
    int** outTime = malloc(sizeof(int*)*7*WEEK_MAX); 
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

    calcTotal(totalTime, dailyAvg, dayCnt-1);
    printf(" -- Supposed total worked time until today: %02d:%02d:%02d\n", totalTime[HR], totalTime[MIN], totalTime[SEC]);

    calcWorkedTime( myTotalTime, 0, dayCnt-2, dayCnt, inTime, outTime, nowTime);
    printf(" -- my total worked time until today: %02d:%02d:%02d\n", myTotalTime[HR], myTotalTime[MIN], myTotalTime[SEC]);
// *******    ARGV    ******* //
    getArgv(argc, argv, &dayX, &dayY, dayCnt, weekCnt);

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
    
    //
    // FINISHED READING DATA
    // NOW COMPUTE 
    //

    calcWorkedTime( workedTime, dayX, dayY, dayCnt, inTime, outTime, nowTime);
#ifdef DEBUG
    printf("DEBUG: tot_worked %02d:%02d:%02d\n", workedTime[HR], workedTime[MIN], workedTime[SEC]);
#endif

    calcNetWeekTime(workedTime, _netWeekTime, totalWeekTime, dayY);
    printf("\n");
    calcRemDayTime(_netWeekTime, dayY, nowTime);

    offThursday(nowTime, _netWeekTime, dayY);


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
