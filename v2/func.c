#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"

#define OFFICIAL_WORK_DAY_SEC (8*60*60) //8 hr
#define WORK_DAY_SEC ( OFFICIAL_WORK_DAY_SEC + (30*60) ) //8 hr and 30 min
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
//                printf("week %d day %d, work-home %d\n", i, j, w[i].days[j].homeIN - w[i].days[j].workOut);
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
                if(w[i].days[j].workIn < w[i].days[j].homeOUT)
                printf("w[%d].day[%d]: in %d --- out %d\n", i, j, w[i].days[j].workIn, w[i].days[j].homeOUT);
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
    char note[256];
    int temp[6] = {0};

    FILE* fp = fopen(FILE_ATTENDANCE, "r");
    if (fp == NULL)
       exit(EXIT_FAILURE);

    memset(note, 0, sizeof(note)); 
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
                                                        note);
            dayIndex = return_day_number(day);
            if(dayIndex == -1)
                printf("error reading day index");

            memcpy(w[weekCnt-1].days[dayIndex].note, note, sizeof(note));
            w[weekCnt-1].days[dayIndex].day = day;
            w[weekCnt-1].days[dayIndex].workIn = temp[2] + 60*temp[1] + 3600*temp[0];
            w[weekCnt-1].days[dayIndex].workOut = temp[5] + 60*temp[4] + 3600*temp[3];
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 0;
            temp[3] = 0;
            temp[4] = 0;
            temp[5] = 0;
            memset(note, 0, sizeof(note)); 
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

            /********** WORK *********/
            if(stats[i].days[j].workIn || stats[i].days[j].workOut)
            {
                printf("%c", stats[i].days[j].day);

                if(stats[i].days[j].workIn)
                {
                    hr = stats[i].days[j].workIn / 3600;
                    min = (stats[i].days[j].workIn % 3600) / 60;
                    sec = (stats[i].days[j].workIn % 3600) % 60;
                    printf(" %02d:%02d:%02d ", hr, min, sec);
                }
                else
                {
                    printf("         ");
                }
                if(stats[i].days[j].workOut)
                {
                    hr = stats[i].days[j].workOut / 3600;
                    min = (stats[i].days[j].workOut % 3600) / 60;
                    sec = (stats[i].days[j].workOut % 3600) % 60;
                    printf(" %02d:%02d:%02d", hr, min, sec);
                }
                else
                {
                    printf("         ");
                }
                /********** HOME *********/
                printf("   home");

                if(stats[i].days[j].homeOUT)
                {
                    hr = stats[i].days[j].homeOUT / 3600;
                    min = (stats[i].days[j].homeOUT % 3600) / 60;
                    printf(" %02d:%02d", hr, min);
                }
                else
                {
                    printf("      ");
                }
                if(stats[i].days[j].homeIN)
                {
                    hr = stats[i].days[j].homeIN / 3600;
                    min = (stats[i].days[j].homeIN % 3600) / 60;
                    printf(" %02d:%02d", hr, min);
                }
                else
                {
                    printf("      ");
                }
                
                /********** TOTAL WORKED *********/
                hr = (stats[i].days[j].workOut- stats[i].days[j].workIn)/ 3600;
                min = ((stats[i].days[j].workOut- stats[i].days[j].workIn)% 3600)/60;
                sec = ((stats[i].days[j].workOut - stats[i].days[j].workIn)% 3600) % 60;
                printf("   worked %02d:%02d:%02d", hr, min, sec);

                if(strlen(stats[i].days[j].note) > 0)
                {
                    if(!strncmp("off", stats[i].days[j].note, 3))
                    {
                        //total_sec += WORK_DAY_SEC;
                    }
                    else if(!strncmp("half", stats[i].days[j].note, 4))
                    {
                        total_sec += OFFICIAL_WORK_DAY_SEC/2 + (30*60);
                    }
                    else if(!strncmp("tr", stats[i].days[j].note, 2))
                    {
                        //total_sec += WORK_DAY_SEC;
                    }
                    else if(!strncmp("@2", stats[i].days[j].note, 2))
                    {
                        total_sec += (60*60) + (30*60);
                    }
                }
                total_sec += (stats[i].days[j].workOut - stats[i].days[j].workIn);


                /********** NOTE *********/
                //if(stats[i].days[j].note)
                //    printf(" %s", stats[i].days[j].note);

                printf("\n");
            }
        }
        total_sec = ONE_WEEK_TOTAL_SEC - total_sec;
        rem_hr = (total_sec)/ 3600;
        rem_min = ((total_sec)% 3600)/60;
        rem_sec = ((total_sec)% 3600) % 60;
        printf("--------- total left: %02d:%02d:%02d ---------\n", rem_hr, rem_min, rem_sec);
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
