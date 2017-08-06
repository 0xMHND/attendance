
#define LINE_MAX 50
#define WEEK_MAX 52
#define WEEK_NUM 1 
extern const int LUNCH_BREAK[3];
extern const int WEEK_WORK_REQ[3];
extern const int WORK_DAYS_PER_WEEK;
extern const long long MIN_DAILY_TIME;
extern const long long NORMAL_DAILY_TIME;
enum time{HR, MIN, SEC, DAY};

int convert_from_sec(int* n, long long s);
int convert_to_sec(int* n, long long* s);
int avgSec(long long _data, int numDays, long long* _avg);
void calcWorkedSec(long long* _workedTime, int dayX, int dayY, int dayCnt, long long* _inSec, long long* _outSec, long long _nowSec);
int calcTotalWeekSec(long long* _totalWeekTime);
int calcTotalSec(long long* _result, long long _avg, int _period);
int calcWeekRemSec(long long* _leftWeek, long long _totalWeekTime, long long _myTotalWeekTime);
int calcMinWeekRemSec(long long* _minLeftWeek, long long _totalWeekTime, long long _myTotalWeekTime, int dayCnt);
int calcLeaveToday(long long* _norLeaveToday, long long* _shLeaveToday, long long _minWeekLeft, int dayCnt);
int calcOffThursday(long long* _norLeaveThur, long long* _shLeaveThur, long long* _norNowLeaveThur,long long* _shNowLeaveThur, long long _minWeekLeft, long long _nowSec, int dayCnt);
