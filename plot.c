#include "plot.h"

//config filename
#define CONFIG "plot.cfg"

/***************************
 * input: 	x values to graph
 * 			y values to graph
 * output:	This function returns 0 on success
 * description:	This function takes x and y values and creates
 * 				a PNG image showing the graphical representation of that data
***************************/
int plot_data(uint64_t * xvals, uint64_t ** yvals, int size, int yValCnt)
{
	char buf[BUF_SIZE];

	memset(buf, '\0', BUF_SIZE);
	snprintf(buf, BUF_SIZE, "%s/chart.dat", LOG_PATH);

	//Open data file for the chart
	FILE * temp = fopen(buf, "w");
	if (temp == NULL) {
		fprintf(stderr,"Could not open/create chart.dat file: %s.\n", strerror(errno));
		return 1;
	}
	memset(buf, '\0', BUF_SIZE);

	//Write data to the data file
	for (int i=0; i < size; i++) {
        fprintf(temp, "%ld ", xvals[i]);
        for(int j=0; j<yValCnt; j++)
        {
//            if( (yvals[j][i]!=0))
//            {
                fprintf(temp, "%ld ", yvals[j][i]); //Write the data to a temporary file
//            }
        }
        fprintf(temp, "\n"); //Write the data to a temporary file
	}
	if (fclose(temp) == EOF)
		 fprintf(stderr,"Could not close chart.dat file pointer: %s.\n", strerror(errno));

	//Open the config file that holds chart options
	FILE * cfg = fopen(CONFIG, "r");
	if (cfg == NULL) {
		fprintf(stderr,"Could not open %s: %s.\n", CONFIG, strerror(errno));
		return 1;
	}
	
	//Open the GNUplot program and get it ready to feed it instructions
	FILE * gnuplotPipe = popen ("gnuplot", "w");
	if (gnuplotPipe == NULL) {
		fprintf(stderr,"Could not open GNUplot.\n");
		return 1;
	}
	
	//set output to be a PNG image file
	fprintf(gnuplotPipe, "%s\n", "set terminal png truecolor");
	//set output name
	fprintf(gnuplotPipe, "set output \'%schart.png\'\n", LOG_PATH);

	//This loops goes through every line of the config file
	//and feeds them into GNUplot
	for (int i = 0; !feof(cfg); i++) {
		memset(buf, '\0', BUF_SIZE); 	//clear buffer

		//get next line into the buffer
		if (fgets(buf, BUF_SIZE, cfg) == NULL && !feof(cfg)) {
			fprintf(stderr, "Could not read line %d in file %s", i, CONFIG);
			return 1;
		}

		//if end of file is reached, break out of the loop
		if (feof(cfg))
			break;

		//send command to GNUplot
		fprintf(gnuplotPipe, "%s\n", buf);
	}
	//fprintf(gnuplotPipe, "plot \'%s/chart.dat\' ls 2\n", LOG_PATH);

	//close GNUplot pipe
	if (pclose(gnuplotPipe) == -1) {
		fprintf(stderr, "Could not close pipe to GNUplot");
	}
	return 0;
}
