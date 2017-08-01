//
//FIXME: currently only 1 year
//
//FIXME: search for 'malloc' and then free it
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ansiiColor.h"


#define FILENAME "attendance"
#define LINE_MAX 50
#define WEEK_MAX 52
#define WEEK_NUM 1 

const int _totalWTime[3] = {42,30,0}; // 40 Hr/week + 5(00:30:00)'lunch break'
#define NORMAL_SH   0 //in 7:00 out 15:30
#define SHORT_SH    1 //in 8:30 out 14:30
//
// modes:
//      0 - Last week   
//      1 - specefic week 
//

enum time{HR, MIN, SEC, DAY};
enum days{Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};

int* timeWorked(int** inTime, int** outTime, int numDays);
int* computeOneDay(int day, int* inTime, int* outTime, int* oneDay);
int* remTime(int* totalTime);
void offThursday(int* remTime, int* nowTime);
void backOnSchedule(int* rem, int* nowTime);

void curWeek(int weekCnt, int** inTime, int** outTime, struct tm* timeinfo);
void oneWeek(int weekCnt, int** inTime, int** outTime, struct tm* timeinfo);

// 4 cases:
//      Curr. Week:      no arg.
//      week Z:          1 arg.
//      from X to Y:     >2 arg. argv[1] != 'd'
//      from X to Today: = 3 arg. argv[1] =='d'
void getArgv(int argc, char** argv, int* dayX, int* dayY, int dayCnt)
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
            *dayX = 7 - (numWeek*7);    //Week's Start
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
#ifdef DEBUG
//        printf("scanned %s\n", buf);
#endif
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
#ifdef DEBUG
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

    for(int i=dayX; i<=dayY; i++)
    {
        if( (i==dayY) && (dayY==today) ) //if dayY is Today
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
        checkOverflow(workedTime);
        checkOverflow(oneDay);

#ifdef DEBUG
    printf("DEBUG: day[%d] worked %02d:%02d:%02d\n", i, oneDay[HR], oneDay[MIN], oneDay[SEC]);
#endif
       } 

    _workedTime[HR]      = workedTime[HR];
    _workedTime[MIN]     = workedTime[MIN];
    _workedTime[SEC]     = workedTime[SEC];
}

void calcNetWeekTime(int* _workedTime, int* _netWeekTime, int* totalWeekTime)
{
    int N[3]   = {0};  
    N[HR]      = _netWeekTime[HR];
    N[MIN]     = _netWeekTime[MIN];
    N[SEC]     = _netWeekTime[SEC];

    N[SEC] = totalWeekTime[SEC] - _workedTime[SEC]; // -59 to 59
    N[MIN] = totalWeekTime[MIN] - _workedTime[MIN]; // -59 to 59
    N[HR] = totalWeekTime[HR] - _workedTime[HR];
    
    if( N[HR]>0 )
        checkOverflow(N);
    else if( (N[MIN]>0) && (N[SEC]<0) )
    {   
        N[SEC] += 60;
        N[MIN]--;
    }
    else if( N[SEC] < (-59) )
    {
        N[SEC] += 60;
        N[MIN]--;
    }
    else if( N[MIN] < (-59) )
    {
        N[MIN] += 60;
        N[HR]--;
    }

#ifdef DEBUG
    printf("DEBUG; netTime \"%d:%d:%d\" \n", N[HR], N[MIN], N[SEC]);
#endif
    _netWeekTime[HR] = N[HR];
    _netWeekTime[MIN] = N[MIN];
    _netWeekTime[SEC] = N[SEC];
}

int main(int argc, char** argv)
{
// *******XXX READ_FILe XXX******* //
    int weekCnt = 0;
    int dayCnt  = 0;
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
    printf("weekCnt - %d\tdayCnt - %d\n", weekCnt, dayCnt);
#endif

// *******XXX ARGV XXX******* //
    int dayY = 0;
    int dayX = 0;
    getArgv(argc, argv, &dayX, &dayY, dayCnt);

// *******XXX TIME XXX******* //
    time_t rawtime;
    time( &rawtime );
    struct tm * timeinfo;
    timeinfo = localtime ( &rawtime );

    int nowTime[4] = {0};
    nowTime[HR] = timeinfo->tm_hour;
    nowTime[MIN] = timeinfo->tm_min;
    nowTime[SEC] = timeinfo->tm_sec;
    nowTime[DAY] = timeinfo->tm_wday; // days since sunday(0) -> 6
    
    printf ("Today is %s", asctime(timeinfo));
    

    //
    //
    // FINISHED READING DATA
    // NOW COMPUTE 
    //
    //
    int _workedTime[3] = {0};
    calcWorkedTime( _workedTime, dayX, dayY, dayCnt, inTime, outTime, nowTime);
#ifdef DEBUG
    printf("DEBUG: tot_worked %02d:%02d:%02d\n", _workedTime[HR], _workedTime[MIN], _workedTime[SEC]);
#endif

    int _netWeekTime[3] = {0};
    calcNetWeekTime(_workedTime, _netWeekTime, _totalWTime);
/*
    curWeek( weekCnt,  inTime,  outTime, timeinfo);
    oneWeek( WEEK_NUM,  inTime,  outTime, timeinfo);

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
*/
    printf("\n");
    return 0 ;
}

void curWeek(int weekCnt, int** inTime, int** outTime, struct tm* timeinfo)
{
    printf("\nCURRENT : @@ " ANSI_COLOR_LIGHTCYAN"WEEK %d" ANSI_COLOR_RESET " @@\n",weekCnt);
    int weekStart = (weekCnt*7) - 7;
    int** curInTime = malloc(sizeof(int*)*7);
    int** curOutTime = malloc(sizeof(int*)*7);
    for(int i=0; i<7; i++)
    {
        curInTime[i] = malloc(sizeof(int)*3);
        curOutTime[i] = malloc(sizeof(int)*3);
        for(int j=0; j<3; j++)
        {
            curInTime[i][j] = inTime[weekStart+i][j];
            curOutTime[i][j] = outTime[weekStart+i][j];
        }
    }
    int* t_worked;
    t_worked = timeWorked(curInTime, curOutTime, timeinfo->tm_wday);

    int* remainingTime;
    remainingTime = remTime(t_worked);

    int nowTime[4] = {0};
    nowTime[HR] = timeinfo->tm_hour;
    nowTime[MIN] = timeinfo->tm_min;
    nowTime[SEC] = timeinfo->tm_sec;
    nowTime[DAY] = timeinfo->tm_wday; // days since sunday(0) -> 6

    backOnSchedule( remainingTime, nowTime);

    free(t_worked);
    free(remainingTime);
    for(int i=0; i<7; i++)
    {
        free(curInTime[i]);
        free(curOutTime[i]);
    }
}
void oneWeek(int weekCnt, int** inTime, int** outTime, struct tm* timeinfo)
{
    printf("\n\t@@ WEEK %d @@\n",weekCnt);
    int weekStart = (weekCnt*7) - 7;
    int** curInTime = malloc(sizeof(int*)*7);
    int** curOutTime = malloc(sizeof(int*)*7);
    for(int i=0; i<7; i++)
    {
        curInTime[i] = malloc(sizeof(int)*3);
        curOutTime[i] = malloc(sizeof(int)*3);
        for(int j=0; j<3; j++)
        {
            curInTime[i][j] = inTime[weekStart+i][j];
            curOutTime[i][j] = outTime[weekStart+i][j];
        }
    }
    int* t_worked;
    t_worked = timeWorked(curInTime, curOutTime, 6);

    int* remainingTime;
    remainingTime = remTime(t_worked);

    int nowTime[4] = {0};
    nowTime[HR] = timeinfo->tm_hour;
    nowTime[MIN] = timeinfo->tm_min;
    nowTime[SEC] = timeinfo->tm_sec;
    nowTime[DAY] = timeinfo->tm_wday; // days since sunday(0) -> 6

    printf("worked %02d:%02d:%02d  out of 42:30:00\n", t_worked[HR], t_worked[MIN], t_worked[SEC]);
    printf("Rem.Time %02d:%02d:%02d\n", remainingTime[HR], remainingTime[MIN], remainingTime[SEC]);
//    offThursday(remainingTime, nowTime);

    free(t_worked);
    free(remainingTime);
    for(int i=0; i<7; i++)
    {
        free(curInTime[i]);
        free(curOutTime[i]);
    }
}

/*
 WEEKLY REMAINING HOURS  

 % get input hours %  
    read file "attendanceWeek"
 % calculate % 
    for(5)
        sum += computeOneDay;
 % print results % 
*/
int* computeOneDay(int day, int* inTime, int* outTime, int* oneDay){

    //outTime - inTime - 30min(lunch)
    oneDay[SEC] = outTime[SEC] - inTime[SEC];
    oneDay[MIN] = outTime[MIN] - inTime[MIN] - 30;
    if(day > 4 )
        oneDay[MIN]+=30;
    oneDay[HR] = outTime[HR] - inTime[HR];
    while(oneDay[SEC] < 0)
    {
        oneDay[SEC] += 60; 
        oneDay[MIN]--;
    }
    while(oneDay[MIN] < 0)
    {
        oneDay[MIN] += 60; 
        oneDay[HR]--;
    }
#ifdef DEBUG
    switch(day){
        case 0:
            printf("Sunday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 1:
            printf("Monday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 2:
            printf("Tuesday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 3:
            printf("Wednesday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 4:
            printf("Thursday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 5:
            printf("Friday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        case 6:
            printf("Saturday || in(%d:%d:%d) out(%d:%d:%d) =  %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC], oneDay[HR], oneDay[MIN], oneDay[SEC]);
            break;
        default:
            printf("error: reading day\n");
            break;
    }
#endif
    return oneDay;
}       

int* timeWorked(int** inTime, int** outTime, int numDays)
{
    int* worked = malloc(sizeof(int)*3);
    int* temp = malloc(sizeof(int)*3);

    for(int i=0; i<3; i++)
        worked[i] = 0;


    for(int day=0; day <= numDays; day++)
    {
        temp = computeOneDay(day,inTime[day], outTime[day], temp);
        worked[HR]+=temp[HR];
        worked[SEC]+=temp[SEC];
        if(worked[SEC]>59)
        {
            worked[SEC] -= 60;
            worked[MIN]++;
        }
        worked[MIN]+=temp[MIN];
        if(worked[MIN]>59)
        {
            worked[MIN] -= 60;
            worked[HR]++;
        }
#ifdef debug
        printf("worked %02d:%02d:%02d\n", worked[HR], worked[MIN], worked[SEC]);
#endif
    }
    free(temp);
#ifdef debug
    printf("TOTAL WORKED %02d:%02d:%02d\n", worked[HR], worked[MIN], worked[SEC]);
#endif
    return worked;
}

int* remTime(int* t_worked){
    // remaining  = (40HRs + 2.5lunch) - t_worked
    int totalSupposed[3] = {42, 30, 0}; //42:30:00 
    int* remain = malloc(sizeof(int)*3);

    remain[SEC] = totalSupposed[SEC] - t_worked[SEC];
    remain[MIN] = totalSupposed[MIN] - t_worked[MIN];
    remain[HR]  = totalSupposed[HR] - t_worked[HR];
    if(remain[SEC] < 0)
    {
        remain[SEC] += 60; 
        remain[MIN]--;
    }
    if(remain[MIN] < 0)
    {
        remain[MIN] += 60; 
        remain[HR]--;
    }
#ifdef DEBUG
    printf("DEBUGGING: total RemTime ++ %02d:%02d:%02d ++\n", remain[HR], remain[MIN], remain[SEC]);
#endif

    return remain;
}

void offThursday(int* rem, int* nowTime){

    // AssuMINg 8:30 inTime on Thursday,
    // projecting 8HRs per remaining days and today leave at 15:30
    // Time to leave is 15:30 + remaining - 8:30*remDays - (15:30-now).
    
    int remDays = 4 - nowTime[DAY]; //remaining days in the week ex/today
    int todayRemTime[3] = {0};
    int buffTime[3] = {0};
    int leaveTime[3] = {0};

    todayRemTime[HR] = 15 - nowTime[HR];
    todayRemTime[MIN] = 30 - nowTime[MIN];
    while(todayRemTime[MIN] < 0)
    {
        todayRemTime[MIN] += 60;
        todayRemTime[HR] -= 1;
    }
#ifdef DEBUG
    printf("DEBUGGING: Today remTime ++ %02d:%02d:%02d ++\n", todayRemTime[HR], todayRemTime[MIN], todayRemTime[SEC]);
#endif

    buffTime[HR] = rem[HR] - (8*remDays) - todayRemTime[HR];
    buffTime[MIN] = rem[MIN] - todayRemTime[MIN] - (30*remDays);
    while(buffTime[MIN]<(-59))
    {
        buffTime[MIN] += 60;
        buffTime[HR]--;
    }
    if(buffTime[HR] > 0 && buffTime[MIN] < 0)
    {
        buffTime[MIN] += 60;
        buffTime[HR]--;
    }

#ifdef DEBUG
    printf("DEBUGGING: extra time ++ %02d:%02d:%02d ++\n", buffTime[HR], buffTime[MIN], buffTime[SEC]);
#endif

    leaveTime[HR] =  15 + buffTime[HR];
    leaveTime[MIN] = 30 + buffTime[MIN]; 
    while(leaveTime[MIN] < 0)
    {
        leaveTime[MIN] += 60;
        leaveTime[HR]--;
    }
    while(leaveTime[MIN] > 59)
    {
        leaveTime[MIN] -= 60;
        leaveTime[HR]++;
    }

    leaveTime[SEC] = 0; 

    printf("LEAVE AT Thursday ***" ANSI_COLOR_LIGHTRED" %02d:%02d:%02d "ANSI_COLOR_RESET"***\n", leaveTime[HR], leaveTime[MIN], leaveTime[MIN]);
}
void backOnSchedule(int* rem, int* nowTime)
{
    // projecting 8HRs per remaining days ex/today
    // Time to leave is 15:30 + remaining - 8:30*remDays
    
    int remDays = 4 - nowTime[DAY]; //remaining days in the week ex/today
    int leaveTime[3] = {0};

    leaveTime[HR] = rem[HR] - (8*remDays);
    leaveTime[MIN] = rem[MIN] - (30*remDays);
    while(leaveTime[MIN]<(-59))
    {
        leaveTime[MIN] += 60;
        leaveTime[HR]--;
    }

    leaveTime[HR] += 15;
    leaveTime[MIN] += 30;
    while(leaveTime[MIN] < 0)
    {
        leaveTime[MIN] += 60;
        leaveTime[HR]--;
    }
    while(leaveTime[MIN] > 59)
    {
        leaveTime[MIN] -= 60;
        leaveTime[HR]++;
    }

    leaveTime[SEC] = 0; 

    printf(ANSI_COLOR_YELLOWBLUE"Preferred"ANSI_COLOR_RESET" leave today at ***" ANSI_COLOR_WHITEBLUE" %02d:%02d:%02d "ANSI_COLOR_RESET"***\n", leaveTime[HR], leaveTime[MIN], leaveTime[MIN]);
    printf("else\n");
    offThursday(rem, nowTime);

}


//return remaining hours after now.
