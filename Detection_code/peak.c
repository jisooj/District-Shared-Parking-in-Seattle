#include <stdio.h>
#include <math.h>

int averageArray (int* array);
int findMin(double currentValue, double currentMin);
int findMax(double currentValue, double currentMax);
int detectCar(int threshold, double* min, double* max, double averageSignal);


int main() {

	// variables in order to implement moving average
	int average = 0;
	int counter = 0;
	int index = -1;

	int temp;									// store current value of mag
	int sampleSize[20];	 	 		// store 20 samples of mag data
	double signalAverage;		// average signal based on the initial data
	int threshold;

	double min = 40000.0;		// default min value
	double max = -40000.0;		// default max value
	double* p_min  = &min;
	double* p_max = &max;


	// FILE I/O
 	FILE * readFile;
	readFile = fopen("data2.txt", "r");

	FILE * writeFile;
	writeFile = fopen("result2.txt", "w");

  	if(readFile != NULL) {

		// read a text file
  		while (fscanf(readFile, "%d", &temp) != EOF) {

			// implement circular array
			index = index % (sizeof(sampleSize) / sizeof(int)) + 1;
			sampleSize[index] = temp;				// add new value to array

			// after first 20 readings - compute average signal and threshold
			if(counter == 20) {
				signalAverage = (averageArray(sampleSize) / 20.0);
				threshold = fabs(signalAverage) / 4;				// I got 4 after analying data
			}

			// skip this for the first 20 sample to do above calulation
			if(counter / 20 >= 1) {

				// calucalte moving average
				average = averageArray(sampleSize) / 20.0;

				// calculate current min and max
				min = findMin(average, min);
				max = findMax(average, max);

				// if car is found -- found = 1, else 0
				int found = detectCar(threshold, p_min, p_max, signalAverage);

				//printf("Z value = %f, Min = %f, Max = %f, Average Signal = %f, Found = %d\n", average, min, max, signalAverage, found) ;
				fprintf(writeFile, "%d\n", found);

				average = 0;
			}
			counter++;
		}
	}

	return 0;
}

// if car is found or not
int detectCar(int threshold, double* p_min, double* p_max, double averageSignal) {
	int difference = (int)fabs(fabs(*p_max) - fabs(*p_min));
	if(difference > threshold) {
		*p_max = averageSignal;
		*p_min = averageSignal;
		return 1;
	} else
		return 0;
}

// finds max of two number
int findMax(double currentValue, double currentMax) {
	if(currentValue > currentMax)
		return currentValue;
	else
		return currentMax;
}

// finds minimum of two values
int findMin(double currentValue, double currentMin) {
	if(currentValue < currentMin)
		return currentValue;
	else
		return currentMin;
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
