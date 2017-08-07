#include "calc.h"

const int LUNCH_BREAK[3] = {0,30, 0};
const int WEEK_WORK_REQ[3] = {40, 0, 0};
const int WORK_DAYS_PER_WEEK = 5;
const long long MIN_DAILY_TIME = (60*60)*(6); //6 hr minimum daily from 8:30 to 14:30
const long long NORMAL_DAILY_TIME = ( ( (60*60)*(8) ) + (60 * 30) );//8:30 nor day | 7:00 to 15:30


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
int avgSec(long long _data, int numDays, long long* _avg)
{
    long long temp = 0;
    temp = _data/numDays;

    *_avg = temp;
    return 0;
}
void calcWorkedSec(long long* _workedTime, int dayX, int dayY, int dayCnt, long long* _inSec, long long* _outSec, long long _nowSec)
{
    long long workedTime = 0;
    workedTime = *_workedTime;
    int today = dayCnt-1;
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

int calcTotalWeekSec(long long* _totalWeekTime)
{
    long long temp = 0;
    temp = ( WEEK_WORK_REQ[SEC] + ( LUNCH_BREAK[SEC] * WORK_DAYS_PER_WEEK) ) + 
           ( 60*(WEEK_WORK_REQ[MIN] + ( LUNCH_BREAK[MIN] * WORK_DAYS_PER_WEEK)) ) +
           ( 3600*( WEEK_WORK_REQ[HR] + ( LUNCH_BREAK[HR] * WORK_DAYS_PER_WEEK)) ) ;

    *_totalWeekTime = temp;
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
int calcMinWeekRemSec(long long* _minLeftWeek, long long _totalWeekTime, long long _myTotalWeekTime, int dayCnt)
{
    long long minLeftWeek = 0;
    int today = dayCnt-1;
    int wday = today%7;
    minLeftWeek = _totalWeekTime - _myTotalWeekTime;
    if(wday < 4) //before thursday, make sure the leftWeek time > remaining days * minimum_daily_sec
    {
        if(minLeftWeek < (MIN_DAILY_TIME*(4-wday)) )
            minLeftWeek += ( (MIN_DAILY_TIME*(4-wday)) - minLeftWeek);
    }

    *_minLeftWeek = minLeftWeek;
    return 0; 
}
int calcLeaveToday(long long* _norLeaveToday, long long* _shLeaveToday, long long _minWeekLeft, int dayCnt)
{
    long long norLeaveToday = 0;
    long long shLeaveToday = 0;
    int today = dayCnt-1;
    int wday = today%7;

    norLeaveToday = _minWeekLeft - ( (4-wday) * (NORMAL_DAILY_TIME) ); // min week left - normal hrs
    shLeaveToday = _minWeekLeft - ( (4-wday) * (MIN_DAILY_TIME) ); // min week left - normal hrs

    *_norLeaveToday = norLeaveToday;
    *_shLeaveToday = shLeaveToday;
    return 0; 
}

// TODO: if i leave at xx:xx:xx
int calcOffThursday(long long* _norLeaveThur, long long* _shLeaveThur, long long* _norNowLeaveThur,long long* _shNowLeaveThur,long long* _shThurRestNor, long long _minWeekLeft, long long _nowSec, int dayCnt)
{
    long long norLeaveThur = 0;
    long long shLeaveThur = 0;
    long long shThurRestNor = 0;
    long long norNowLeaveThur = 0; 
    long long shNowLeaveThur = 0; 
    int today = dayCnt-1;
    int wday = today%7;
    long long restNor = (3-wday) * (NORMAL_DAILY_TIME); // rest days ex/Thursday
    long long restSh = (3-wday) * (MIN_DAILY_TIME);

    long long norOut = (15*(60*60)) + (30*60);
    long long shOut = (14*(60*60)) + (30*60); 

    shThurRestNor = _minWeekLeft - (MIN_DAILY_TIME) - ((3-wday) * NORMAL_DAILY_TIME);
    norLeaveThur = _minWeekLeft - (norOut - _nowSec) - restNor ; 
    shLeaveThur = _minWeekLeft - (shOut - _nowSec) - restSh ; 
    norNowLeaveThur = _minWeekLeft - restNor;
    shNowLeaveThur = _minWeekLeft - restSh;

    *_norLeaveThur = norLeaveThur;
    *_shLeaveThur = shLeaveThur;
    *_norNowLeaveThur = norNowLeaveThur;
    *_shNowLeaveThur = shNowLeaveThur;
    *_shThurRestNor = shThurRestNor;



#ifdef DEBUG
    int temp[3] = {0};
    int temp1[3] = {0};
    int temp2[3] = {0};
    convert_from_sec(temp, _minWeekLeft);
    convert_from_sec(temp1, MIN_DAILY_TIME);
    convert_from_sec(temp2, NORMAL_DAILY_TIME);
    printf("shThurRestNor = %02d:%02d:%02d - %02d:%02d:%02d - (%d * %02d:%02d:%02d)\n", temp[HR], temp[MIN], temp[SEC], temp1[HR], temp1[MIN], temp1[SEC], 2-wday, temp2[HR], temp2[MIN], temp2[SEC]); 
#endif
    return 0; 
}

