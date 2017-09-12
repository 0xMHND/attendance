


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

#define FILENAME "randomWeeks"
#define ONEWEEK 7
#define NUM_WEEK 13

enum time{HR, MIN, SEC};

void formatStr( char* str, int cnt)
{
    
    int inTime[3] = {0};
    inTime[HR] = (rand()%6) + 6; //from 6am to 12pm
    inTime[MIN] = rand()%60;
    inTime[SEC] = rand()%60;
    int outTime[3] = { 0 };
    while(outTime[HR] < inTime[HR])
    {
       outTime[HR] = ( rand()%9 ) + 14; //from 2pm to 10pm
    }
    outTime[MIN] = rand()%60;
    while( (outTime[HR]==inTime[HR]) && (outTime[MIN]<=inTime[MIN]) )
    {
       inTime[MIN] = rand()%40;
       outTime[MIN] = rand()%60;
    }
    outTime[SEC] = rand()%60;

    switch(cnt)
    {
        case 0:
            sprintf( str, "S %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 1:
            sprintf( str, "M %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 2:
            sprintf( str, "T %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 3:
            sprintf( str, "W %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 4:
            sprintf( str, "R %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 5:
            sprintf( str, "F %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
            break;
        case 6:
            sprintf( str, "U %d:%d:%d %d:%d:%d\n", inTime[HR], inTime[MIN], inTime[SEC], outTime[HR], outTime[MIN], outTime[SEC]);
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

    int numWeeks = rand()%20 + 1; //between 1 and 21 weeks

    FILE *fp = fopen( FILENAME, "w");

    int weekCnt = 0;


    while(weekCnt < numWeeks)
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
