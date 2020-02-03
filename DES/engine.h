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


// A single data structure is used to handle all three event types
//an  event data link between station and component

//***************
//***************
//when,who,where
//***************
//                        Key data!!!!!!!!!
struct EventData {
	int 		EventType;          // Type of event ( ARRIVAL, DEPARTURE)
	struct 		Component *Comp; 	// Arriving or departing component;
	int 		StationID;				// ID of station where component is created, arrives, or departs
	double 		Event_begin_time;	//begin time of service(producing)
};



//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//Gobal variable
//********************************************************************
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct Station *stn_list; //station list

PrioQ 	*FEL; //future event list (FEL)  is a priority queue

int 	*idx;//map index to Stn ID


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////////////////////
//
// Queueing Network Simulation
//
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data structures for event data
//
/////////////////////////////////////////////////////////////////////////////////////////////

// EVENT TYPES:
//  GENERATE: Generation of a new component
//      Parameters: Station where component is generated
//
//  DEPARTURE: a component departs from a queue station
//      Parameters: ...
//
//	RELEASE: a component enter the station when service is not yet end
//
// Event types: Generate new component, arrival at a station, departure from queue station
//#define     GENERATE    0
#define     ARRIVAL     1
#define     DEPARTURE   2
#define		RELEASE		3



/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Customers and FIFO queue
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Component
struct Component {
	//struct Component *next; // pointer to next component when it is queued in a FIFO
	int 		CompID;
	double 		Generation_time; //time when component enter system
	double 		Que_time;//total queuing time through system
	int 		Que_cnt;	//Queuing station count
	double 		Cost;
};


// Linked List of components (FIFO Queue)


/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Stations
//
/////////////////////////////////////////////////////////////////////////////////////////////


// Component types
#define     GENERATOR       0

#define     SERVICE_STATION   1
#define     EXIT            2
#define 	AND 			3
#define  	EXTSUPP			4



struct Station {
	int 	StationType;  // GENERATOR, SERVICE_STATION, EXIT
	void 	*Stn; // Pointer to information on station (Generator, Exit struct, etc.)
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

//fork to desition station when DEPARTURE
int fork_ID(int comp_ID);


//function to generate an exponential random number
double randexp(double U);

//function to generate a Gauss random number parameter {0,1}

double randgauss( );



#endif /* engine_h */

