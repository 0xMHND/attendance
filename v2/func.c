#include <stdio.h>
#include <stdlib.h>
#include "func.h"

#define WORK_DAY_SEC ( (8*60*60) + (30*60) ) //8 hr and 30 min
#define ONE_WEEK_TOTAL_SEC (5*WORK_DAY_SEC)

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
double avg_in(WEEK_t *w, uint16_t index){
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

void readHome(WEEK_t *w) {

    int week_num = 0;
    uint8_t dayIndex = 0;
    char day = 'X';
    char temp_str[256];
    char buf[LINE_MAX];
    int temp[4] = {0};

    FILE* fp = fopen(FILE_HOME, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if(buf[1]=='E') //if first word is WEEK
        { 
            sscanf(buf, "WEEK%d %s",  &week_num, temp_str);
        }
        else{
            sscanf(buf, "%c %d:%d %d:%d", &day,
                                                &temp[0],
                                                &temp[1],
                                                &temp[2],
                                                &temp[3]);
            dayIndex = return_day_number(day);
            if(dayIndex == -1)
                printf("error reading day index");

            w[week_num-1].days[dayIndex].day = day;
            w[week_num-1].days[dayIndex].homeOUT = 60*temp[1] + 3600*temp[0];
            w[week_num-1].days[dayIndex].homeIN =  60*temp[3] + 3600*temp[2];
    //        printf(" %c     %d   %d\n", w[week_num-1].days[dayIndex].day, w[week_num-1].days[dayIndex].homeOUT, w[week_num-1].days[dayIndex].homeIN);
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 0;
            temp[3] = 0;
        }
    }
    fclose(fp);
}
void readAttendance(WEEK_t *w, uint16_t* week_index) {

    uint16_t weekCnt = 0;
    uint8_t dayIndex = 0;
    char day = 'X';
    char buf[LINE_MAX];
    char str[256];
    int temp[6] = {0};

    FILE* fp = fopen(FILE_ATTENDANCE, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    memset(str, 0, sizeof(str)); 
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if(buf[1]=='E') //if first word is WEEK
        { 
            //TODO : realloc if hit max days 
            sscanf(buf, "WEEK%d %d-%d-%d",  (int*)&w[weekCnt].week_num,
                                            (int*)&w[weekCnt].week_month,
                                            (int*)&w[weekCnt].week_day,
                                            (int*)&w[weekCnt].week_year);  
            weekCnt++;
        }

        else{
            sscanf(buf, "%c %d:%d:%d %d:%d:%d NOTE:%s", &day,
                                                &temp[0],
                                                &temp[1],
                                                &temp[2],
                                                &temp[3],
                                                &temp[4],
                                                &temp[5],
                                                str);
            dayIndex = return_day_number(day);
            if(dayIndex == -1)
                printf("error reading day index");

            memcpy(w[weekCnt-1].days[dayIndex].note, str, sizeof(str));
            w[weekCnt-1].days[dayIndex].day = day;
            w[weekCnt-1].days[dayIndex].workIn = temp[2] + 60*temp[1] + 3600*temp[0];
            w[weekCnt-1].days[dayIndex].workOut = temp[5] + 60*temp[4] + 3600*temp[3];
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 0;
            temp[3] = 0;
            temp[4] = 0;
            temp[5] = 0;
            memset(str, 0, sizeof(str)); 
        }
 //       printf("%c %d %d\n", w[weekCnt-1].days[dayIndex].day,w[weekCnt-1].days[dayIndex].workIn, w[weekCnt-1].days[dayIndex].workOut);
    }
    fclose(fp);

    *week_index = weekCnt;
}



int return_day_number(char day){
    switch(day){
        case 'S':
            return 0;
        case 'M':
            return 1;
        case 'T':
            return 2;
        case 'W':
            return 3;
        case 'R':
            return 4;
        case 'F':
            return 5;
        case 'U':
            return 6;
        default:
            return -1;
    }
}


void printStats(WEEK_t *stats, uint16_t week_index){
    uint16_t days_cnt = 0;
    uint8_t hr = 0;
    uint8_t min = 0;
    uint8_t sec = 0;

    for(int i=0; i<week_index; i++)
    {
        int32_t total_sec = 0;
        int8_t rem_hr = 0;
        int8_t rem_min = 0;
        int8_t rem_sec = 0;
        printf("week[%d] %d-%d-%d\n", stats[i].week_num, stats[i].week_month, stats[i].week_day, stats[i].week_year);
        for(int j=0; j<7; j++)
        {
            if(stats[i].days[j].workIn > 0)
                days_cnt++;
            hr = stats[i].days[j].workIn / 3600;
            min = (stats[i].days[j].workIn % 3600) / 60;
            sec = (stats[i].days[j].workIn % 3600) % 60;
            printf("%c %02d:%02d:%02d ", stats[i].days[j].day, hr, min, sec);
            hr = stats[i].days[j].workOut / 3600;
            min = (stats[i].days[j].workOut % 3600) / 60;
            sec = (stats[i].days[j].workOut % 3600) % 60;
            printf(" %02d:%02d:%02d", hr, min, sec);
            hr = stats[i].days[j].homeOUT / 3600;
            min = (stats[i].days[j].homeOUT % 3600) / 60;
            printf("   home %02d:%02d", hr, min);
            hr = stats[i].days[j].homeIN / 3600;
            min = (stats[i].days[j].homeIN % 3600) / 60;
            printf(" %02d:%02d", hr, min);
            hr = (stats[i].days[j].workOut- stats[i].days[j].workIn)/ 3600;
            min = ((stats[i].days[j].workOut- stats[i].days[j].workIn)% 3600)/60;
            sec = ((stats[i].days[j].workOut - stats[i].days[j].workIn)% 3600) % 60;
            printf("   worked %02d:%02d:%02d\n", hr, min, sec);
            total_sec += (stats[i].days[j].workOut - stats[i].days[j].workIn);
        }
        printf("total_sec = %d ONE_WEEK: %d\n", total_sec, ONE_WEEK_TOTAL_SEC);
        total_sec = ONE_WEEK_TOTAL_SEC - total_sec;
        printf("total_sec = %d ONE_WEEK: %d\n", total_sec, ONE_WEEK_TOTAL_SEC);
        rem_hr = (total_sec)/ 3600;
        rem_min = ((total_sec)% 3600)/60;
        rem_sec = ((total_sec)% 3600) % 60;
        printf("--------- total left: %d:%d:%d ---------\n", rem_hr, rem_min, rem_sec);
    }
    printf("(%d) weeks and %d worked days\n", week_index, days_cnt);
}

void plot_stats(WEEK_t * w, uint16_t index, time_t time_now){

    uint16_t x_index = index * 7;
    uint64_t xvals[x_index];
    uint64_t** yvals = malloc( sizeof(uint64_t*) * COLUMNS);

    yvals[0] = malloc( sizeof(uint64_t) * x_index); //homeOUT
    yvals[1] = malloc( sizeof(uint64_t) * x_index); //workIn
    yvals[2] = malloc( sizeof(uint64_t) * x_index); //workOut
    yvals[3] = malloc( sizeof(uint64_t) * x_index); //workIN

    uint16_t y_index = 0;
    for(int i=0; i < index; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(w[i].days[j].homeOUT > 0)
                yvals[0][y_index] = w[i].days[j].homeOUT;
            else
                yvals[0][y_index] = 0;

            yvals[1][y_index] = w[i].days[j].workIn;
            yvals[2][y_index] = w[i].days[j].workOut;

            if(w[i].days[j].homeIN > 0)
                yvals[3][y_index] = w[i].days[j].homeIN;
            else
                yvals[3][y_index] = 0;

            xvals[y_index] = y_index+1;
            y_index++;
        }
    }

    plot_data(xvals, yvals, x_index, COLUMNS);

}
