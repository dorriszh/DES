//
//
//
//

#ifndef engine_h
#define engine_h

#include <stdio.h>
#include "pq.h"


#define BUSY 1
#define IDLE 0
#define PART0 0
#define PART1 1


// A single data structure is used to handle all three event types
//The event data link customer and component

//***************
//***************
//when,who,where
//***************
//                        Key data!!!!!!!!!
struct EventData {
	int 		EventType;          // Type of event ( ARRIVAL, DEPARTURE)
	struct 		Product *Prod; 	// Arriving or departing product;
	int 		CompID;				// ID of component where product is created, arrives, or departs
	double 		Event_begin_time;	//begin time of service or queue
	int 		Pre_compid;		//the ID of previous component

};



//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//Global variable
//********************************************************************
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct Component *comp_list; //component list (the list of workspace)

PrioQ 	*FEL; //future event list (FEL), which is a priority queue

int 	*idx;//mapping index (to Comp ID)
int 	limit_cnt;
int 	prodID; //cnt for generated prod
int 	total_prod;


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////////////////////
//
// Queuing Network Simulation
//
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data structures for event data
//
/////////////////////////////////////////////////////////////////////////////////////////////

// EVENT TYPES:
//  GENERATE: Generation of a new c
//      Parameters: Component where prod is generated
//
//  DEPARTURE: a prod departs from a queue station
//      Parameters: ...

// Event types: Generation of new product, arrival at a component, departure from queue station
//#define     GENERATE    0
#define     ARRIVAL     1
#define     DEPARTURE   2



/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Products and FIFO queue
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Product
struct Product {
	//struct Product *next; // pointer to next product when it is queued in a FIFO
	int 		ProductID;
	double 		Generation_time; //time when material enter system
	double 		Que_time;//total queuing time through system
	int 		Que_cnt;	//Queuing station count
	double 		Cost;
	int tag ;
	int tag_part;	// tag for identifying C1 and C2
};


// Linked List of products (FIFO Queue)


/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Components
//
/////////////////////////////////////////////////////////////////////////////////////////////


// Component types
#define     GENERATOR       0

#define     QUEUE_STATION   1
#define     EXIT            2
#define 	AND 			3
#define  	EXTSUPP			4
#define 	SELE 			5



struct Component {
	int 	ComponentType;  // GENERATOR, QUEUE_STATION, EXIT
	void 	*Comp; // Pointer to information on component (Generator, Exit struct, etc.)
	FF_head_t * Comp_que;
};



// Call this procedure to run the simulation indicating time to end simulation
void RunSim(double EndTime);

// Schedule an event with timestamp ts, event parameters *data
void Schedule(double ts, void *data);

// This function returns the current simulation time
double CurrentTime(void);

// Function defined in the simulation application called by the simulation engine
//  Event handler function: called to process an event
void EventHandler(void *data);

//fork to destination station when DEPARTURE
int fork_ID(int comp_ID);


//function to generate an exponential random number
double randexp(double U);

//function to generate a Gaussian random number parameter {0,1}

double randgauss(double,double );



#endif /* engine_h */

