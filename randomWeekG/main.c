


/// TODO :  write a random generator for in/out time for a week 
//          from Sunday to Saturday.
//          - out time > in time
//          - all values '+'
//          FEATURE ;
//              * still going week. 
//              - complete week.
//
//

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define FILENAME "randomWeek"
#define ONEWEEK 7
#define NUM_WEEK 13

enum time{HR, MIN, SEC};

void formatStr( char* str, int cnt)
{
    
    int inTime[3] = {0};
    inTime[HR] = rand()%24;
    inTime[MIN] = rand()%60;
    inTime[SEC] = rand()%60;
    int outTime[3] = { 0 };
    while(outTime[HR] < inTime[HR])
    {
       inTime[MIN] = rand()%20;
       outTime[HR] = rand()%24;
    }
    while( (outTime[HR]==inTime[HR]) && (outTime[MIN]<=inTime[MIN]) )
    {
       inTime[HR] = rand()%40;
       outTime[MIN] = rand()%60;
    }
    outTime[MIN] = rand()%60;
    outTime[SEC] = rand()%60;

    switch(cnt)
    {
        case 0:
            sprintf( str, "S %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Sunday\n");
            break;
        case 1:
            sprintf( str, "M %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Monday\n");
            break;
        case 2:
            sprintf( str, "T %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Tuesday\n");
            break;
        case 3:
            sprintf( str, "W %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Wednesday\n");
            break;
        case 4:
            sprintf( str, "R %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Thursday\n");
            break;
        case 5:
            sprintf( str, "F %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//            strcpy( str,"Friday\n");
            break;
        case 6:
            sprintf( str, "U %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
//          strcpy( str,"Saturday\n");
            break;
        default:
            printf("error");
            sprintf( str, "error reading daysCnt");
            break;
    }
}

int main(){

//    char* str = malloc(sizeof(char)*25);
    char str[80];
    char w_str[80];

    srand((unsigned) time(NULL));

    FILE *fp = fopen( FILENAME, "w");

    int weekCnt = 0;


    while(weekCnt < NUM_WEEK)
    {
        sprintf(w_str, "WEEK%d\n", ++weekCnt);
        fputs(w_str, fp);

        for(int i=0; i<ONEWEEK; i++)
        {  
            formatStr(str, i);
            if(fputs( str , fp ) == EOF) 
            {
                perror("ERR writing to file");
                return(EXIT_FAILURE);
            }
        }
    }

    fclose(fp);

    return 0;
}
