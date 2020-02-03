//
//
//
//
//
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "engine.h"
//#include "pq.h"


double Now = 0.0;

//#include "pq.h"
/*struct Event {
	double timestamp;       	// event timestamp
	void *AppData;            	// pointer to application defined event parameters
	struct Event *Next;        	// priority queue pointer
	};
*/






// Return current simulation time

double CurrentTime(void) {
	return (Now);
}






// Schedule new event in FEL
// Priority queue is implemented as a timestamp ordered linear list
void Schedule(double ts, void *data) {

	PQ_Insert(FEL, ts, data);


}

// Function to execute simulation up to a specified time (EndTime)
void RunSim(double EndTime) {
	void  *e;

	printf("Initial event list:\n");


	// Main scheduler loop

	///////////////////////////////////////////
	///////////////////////////////////////////
	//************** PQ_Delete    set "Now"****
	///////////////////////////////////////////
	//////////////////////////////////////////
	while ((e = PQ_Delete(FEL,&Now)) != NULL) {

		if (Now > EndTime)
			break;
		EventHandler(e);


	}
	Now=EndTime;  // To record queuing data
}


// Gaussian random number generator
#define PI		3.14159265358979323846
double randgauss()
{
	//Box-Muller alogrithm -->Gauss random
	// https://en.wikipedia.org/wiki/Box¨CMuller_transform
	static double U, V;
	static int phase = 0;
	double Z;

	if (phase == 0)
	{
		U = rand() / (RAND_MAX + 1.0);
		V = rand() / (RAND_MAX + 1.0);
		Z = sqrt(-2.0 * log(U)) * sin(2.0 * PI * V);
	}
	else
	{
		Z = sqrt(-2.0 * log(U)) * cos(2.0 * PI * V);
	}

	phase = 1 - phase;
	return Z;
}

//exponential random number generator
double randexp(double U) {
	/*double aa;
	 aa=rand() ;
	 aa=aa/ (RAND_MAX + 1.0);
	 aa=log(1-aa);
	 return -U *aa;
	 */
	 return -U*log(1-rand()/(RAND_MAX+FLT_MIN));//Uniform random [0,1)->exponential random number
}

