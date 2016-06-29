#include <stdio.h>
#include <math.h>

int averageArray (int* array);

int main() {

	// variables in order to implement moving average
	double average_X = 0;
	double average_Z = 0;

	int counter = 0;
	int index = -1;

	int temp_X;
	int temp_Z;
										// store current value of mag
	int sampleSize_X[20];	 	 		// store 20 samples of mag data
	int sampleSize_Z[20];
	double signalAverage_X = 0.0;		// average signal based on the initial data
	double signalAverage_Z = 0.0;
			// average signal based on the initial data
	int threshold = 0;

    int found = 0;
    double difference_X = 0.0;
    double difference_Z = 0.0;

	// FILE I/O
 	FILE * readFile;
	readFile = fopen("datay.txt", "r");

	FILE * writeFile;
	writeFile = fopen("resulty.txt", "w");

  	if(readFile != NULL) {

		// read a text file
  		while (fscanf(readFile, "%d", &temp_X) != EOF) {

			// implement circular array
			index = index % (sizeof(sampleSize_X) / sizeof(int)) + 1;
			index = index % (sizeof(sampleSize_Z) / sizeof(int)) + 1;
			sampleSize_X[index] = temp_X;				// add new value to array
			sampleSize_Z[index] = temp_Z;

			// after first 20 readings - compute average signal and threshold
            if(counter == 20) {
                signalAverage_X = fabs(averageArray(sampleSize_X) / 20.0);
                signalAverage_Z = fabs(averageArray(sampleSize_Z) / 20.0);
                threshold = 90;				// I got 4 after analying data
            }

			// skip this for the first 20 sample to do above calulation
			if(counter / 20 >= 1) {

				// calucalte moving average
				average_X = averageArray(sampleSize_X) / 20.0;
				average_Z = averageArray(sampleSize_Z) / 20.0;
				// if car is found -- found = 1, else 0

                difference_X = fabs(average_X - signalAverage_Z);		// x axis or y
                difference_Z = fabs(average_Z - signalAverage_Z);		// z axis

                if(difference_X >= threshold || difference_Z >= threshold)
                    found = 1;
                else
                    found= 0;
				//int found = detectCar(threshold, p_min, p_max, signalAverage);


				fprintf(writeFile, "%f\n", average_X);

				average_X = 0;
				average_Z = 0;
			}

			counter++;
		}
	}

	fclose(readFile);
	fclose(writeFile);

	return 0;
}

// sum of array
int averageArray(int* ar) {
	int i;
	int average = 0;
	for(i = 0; i < 20; i++) {
		average += ar[i];
	}
	return average;
}
