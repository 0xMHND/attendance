#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "func.h"


int main(int argc, char** argv){

    WEEK_t * stats = malloc( sizeof(WEEK_t) * WEEK_MAX);
    uint16_t week_index = 0;

    readAttendance(stats, &week_index);
    readHome(stats);

    printStats(stats, week_index);
    return 0;
}
