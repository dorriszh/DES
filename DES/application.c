//
//  main.c
// 6730
//
//
//

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include "engine.h"
//#include "pq.h"


#define CNGFile "config"
#define OUTPUT	"output"
#define Epsilon  (1e-5)


//
// Generator Component
//
struct Generator {
	double IntArrTime;	// mean interarrival time for generated components
	double delta;
	int DestComp;          	// ID of next station components are sent to
	int Countcomp; 			//set Component to unique "Identify  Number"( ComponentID)
	int Entercnt;			//Real component number entering system ;
	double Cost;


};

// Fork Structure
// for distributing of normal components and defects
struct Fork {
	double Proba;
	int DestID;
//	int CompCount;
};

// AND Structure
// for combining components on Assembly
struct And {
	int num;
	int * Source_ID;
//	int CompCount;
};
struct Queue {
	FF_head_t *head;
	int fork;
	int status;          	//0: server is idle,1: server is busy;
	struct Fork *Q_fork; 	//after  service, the probability & destination that the component will go
	double server_time;		//behavior parameter of server
	struct EventData *Now_Event;
	//Now sevicing component infomation
	double Cost;
	double Defective;

};

//
// Exit station
//
struct Exit {
	int Count; 			// number of components that exited at this station
	double total_time;  // total time between  start  & end ,when the component is in system
	double total_cost;


};







 /////////////////////////////////////////////////////////////////////////////////////////////
 //
 // Function prototypes
 //
 /////////////////////////////////////////////////////////////////////////////////////////////

// prototypes for event handlers
//void Generate(struct EventData *);    // generate new component
void Arrival(struct EventData *);        // arrival at a station
void Departure(struct EventData *);      // departure at a station
void NewComponent(int );                	//generate new component





/////////////////////////////////////////////////////////////////////////////////////////////
//
// Event Handlers
// Parameter is a pointer to the data portion of the event
////////////////////////////////////////////////////////////////////////////////////////////




// General Event Handler Procedure defined in simulation engine interface called by simulation engine
void EventHandler(void *data) {
	struct EventData *d;

	//  type cast so the compiler knows the type of information pointed to by the parameter data.
	d = (struct EventData*) data;
	ASSERT (d->EventType == DEPARTURE);//Only process DEPARTURE;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//a negligible (i.e., zero) amount of time to travel between stations.
	//So a "DEPARTURE" trigger a Arrival  or Exit at once
	Departure(d);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}


void Arrival(struct EventData *e) {
		//double ts,inqu_time,ex_time;
	double ts,ex_time;
	//int Comp;                   // ID of station where arrival occurred
	struct Exit *pExit;         // pointer to info on exit station
	struct Queue *queue;
	if (e->EventType != ARRIVAL) {
		fprintf(stderr, "Unexpected event type\n");
		exit(1);
	}

	if (stn_list[e->StationID].StationType == GENERATOR)
		{
			printf("illegal ENTER!!!!\n");
			exit(-1);
		}

	// processing depends on the type of station arrival occurred at
	if (stn_list[e->StationID].StationType == AND)
	{
		//check whther pre_compomnet
	}
	else if (stn_list[e->StationID].StationType == EXIT) {

		//arrival a EXIT station
		ex_time=CurrentTime()-e->Comp->Generation_time;


		printf("ID %d Exit      Station=%d at time %f",e->Comp->CompID,idx[e->StationID], CurrentTime());
		printf(" Enter Time %lf,Sum Time %lf ",e->Comp->Generation_time,CurrentTime()-e->Comp->Generation_time);
		printf("Queuing Time %lf\n",e->Comp->Que_time);


		pExit = (struct Exit*) (stn_list[e->StationID].Stn);


		//statics
		(pExit->Count)++;  		// number of exiting Components
		(pExit->total_cost)+=ex_time;



		//free memory
		free(e->Comp);
		free(e);
		//free(e->Comp);         // release memory of exiting component
	} else if (stn_list[e->StationID].StationType == SERVICE_STATION) {

		//arrival a QUEUE_STATION
		printf("ID %d Arrival   Station=%d at time %f \n",e->Comp->CompID,idx[e->StationID], CurrentTime());
		queue = (struct Queue*) stn_list[e->StationID].Stn;

		e->Event_begin_time=CurrentTime();

		if (queue->status==BUSY)  //check server status
		{ // add to Queue

			FIFO_in(queue->head,e);
			//FF_in
		}else{     // status is IDEL
			//set busy
			queue->status=BUSY;

			queue->Now_Event=e; //set servicing component now

			//start service
			e->EventType = DEPARTURE;
			//set service endtime
			ts = CurrentTime() + randexp(queue->server_time);			// add a  exponential random number
			//e->Comp->Que_time=0;

			printf("ID %d Serviced  Station=%d at time %lf,Queue Time %lf,end time %lf\n",e->Comp->CompID,idx[e->StationID],CurrentTime(),0.0,ts);
			//e->Event_begin_time=CurrentTime();
			//printf("Que %lf\n",e->Comp->Que_time);


			//add to schedle
			Schedule(ts, e);




		}
	}



}

void NewComponent(int compID) {

	//CompID generate a new comp
	struct EventData *d;
	double ts;
	//int forkto;
	//struct Component *cur_comp, *NewComp;
	struct Component  *NewComp;
	struct Generator *pGen;


	pGen = (struct Generator*) stn_list[compID].Stn;
	NewComp = malloc(sizeof(struct Component));
	ASSERT(NewComp!=NULL);

	NewComp->CompID = pGen->Countcomp; // Component ID

	(pGen->Countcomp)++;

	d = malloc(sizeof(struct EventData));
	ASSERT(d!=NULL);


	//init Component
	d->EventType = DEPARTURE;
	d->Comp = NewComp;
	d->StationID = compID;

	d->Comp->Que_cnt=0;
	d->Comp->Que_time=0;

	// to exponential random number
	ts=(pGen->IntArrTime)+pGen->delta*randgauss();
	// Time when component will enter system ;
	if (ts<0.001) ts=0.001;
	ts += CurrentTime()  ;
	//d->Comp->Generation_time=ts;
	////////////////////////////////////////////////////////
	//ts is the actual time when component will  enter system
	///////////////////////////////////////////////////////
	Schedule(ts, d);
}




void Departure(struct EventData *e)

//Event ----Departure
{
	//struct EventData *d;
	double ts,inqu_time;
	int forkto;

	//struct Component *cur_comp,*NewComp;
	//struct Component *NewComp;
	struct EventData * cur_Event;
	struct Generator *pGen;  // pointer to info on generator station




	ASSERT(e->EventType = DEPARTURE) ;


	if (stn_list[e->StationID].StationType == GENERATOR) {
		printf("ID %d Enter     Station=%d at time %lf  \n", e->Comp->CompID,idx[e->StationID],CurrentTime());
		//Component real arrival!!!!!
		//Enter system!!!!!
		pGen = (struct Generator *) stn_list[e->StationID].Stn;
		pGen->Entercnt++;
		e->Comp->Generation_time=CurrentTime();
		//get_ fork_ID
		forkto = fork_ID(e->StationID);
		//cur_comp = e->Comp;



		/////////////////////////////////////////////
		// Create a new component && add to FEL
		NewComponent(e->StationID);
		/////////////////////////////////////////////


		//a negligible (i.e., zero) amount of time to travel between stations.
		//So a "DEPARTURE" trigger a Arrival at once
		e->EventType = ARRIVAL;
		e->StationID=forkto;

		Arrival(e);
		return;

	}

	// Queue Station Departure
	if (stn_list[e->StationID].StationType == SERVICE_STATION) {
		//get _fork_ID;
		forkto = fork_ID(e->StationID);
		printf("ID %d Departure Station=%d at time %lf to %d \n", e->Comp->CompID,idx[e->StationID],CurrentTime(),forkto
					);
		struct Queue *queue = (struct Queue*) stn_list[e->StationID].Stn;



		//station statics


		///////////////////////////////////////////////////////////////////////////////////////
		//Now server is idle. if queue is not empty,begin to service the first component in queue
		////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////

		if (FIFO_isempty(queue->head)) {



			//////////////////////////////
			// no custormer waiting service
			///////////////////////////////
			//printf("in queue\n");
			queue->status = IDLE;
			queue->Now_Event=NULL;



		} else {

			///////////////////////////
			//To service first in queue
			///////////////////////////

			cur_Event=(struct EventData *)FIFO_out(queue->head);

			queue->Now_Event=cur_Event; //set servicing component now;

			//set Event DATA
			cur_Event->EventType = DEPARTURE;
			cur_Event->StationID=e->StationID;
			//printf("111111\n");
			inqu_time=CurrentTime()-cur_Event->Event_begin_time;//queuing time
			//statics queuing

			//component queuing statics
			cur_Event->Comp->Que_time+=inqu_time;
			cur_Event->Comp->Que_cnt++;



			// service time  follow to exponential distribution,get the  random number
			ts = CurrentTime() + randexp(queue->server_time);
			//set the end time of service
			//ts = CurrentTime() +5;

			printf("ID %d Serviced  Station=%d at time %lf,Queue Time %lf,end time %lf ",cur_Event->Comp->CompID,idx[cur_Event->StationID],CurrentTime(),inqu_time,ts);
			printf(" Accu que %lf\n",cur_Event->Comp->Que_time);


			cur_Event->Event_begin_time=CurrentTime(); //start to service,record the starting time

			//from Queue get A component
			//begin serveice ,add departure event to schedule

			Schedule(ts,cur_Event);

		}


		//////////////////////////////////////////
		// a "DEPARTURE" trigger a Arrival at once
		//////////////////////////////////////////


		e->EventType = ARRIVAL;
		e->StationID=forkto;

		Arrival(e);
		return;
	}



	fprintf(stderr,"illegal status");

	ASSERT(false);
	return;

}


int fork_ID(int comp_ID) {
	//selection a fork

	double r_prba;
	int i, fork;
	struct Queue *Comp_P;
	if (stn_list[comp_ID].StationType ==SERVICE_STATION) {


		//random selection
		r_prba = rand() / (RAND_MAX+FLT_MIN); //get random [0,1)
		Comp_P = (struct Queue*) stn_list[comp_ID].Stn;
		/*if(Comp_P->fork==1){
			printf("aaaaa");
		};
		*/

		for (i = 0; i < Comp_P->fork-1 ; i++) {
			if (r_prba >= 0 && r_prba < (Comp_P->Q_fork)[i ].Proba)
				break;
			r_prba = r_prba - Comp_P->Q_fork[i].Proba;
		}
		// fork to final station  if no selection first K-1;
		fork = (Comp_P->Q_fork[i]).DestID;

		/*if (comp_list[fork].StationType == GENERATOR) { //do not fork to a GENERATOR
			printf("CompID:%d fork to a Generator %d\n", idx[comp_ID],idx[fork]);
			exit(-1);
		}
		*/
		return fork;
	}
	if (stn_list[comp_ID].StationType ==GENERATOR) {

		return ((struct Generator*) stn_list[comp_ID].Stn)->DestComp;
	}

	return -1;		//ERROR
}

/*
int check_ID(int i,int comp_ID,int cngline)
{
	if(comp_ID<0 || comp_ID>=cngline)

			{
				printf("Config file in line  %d Error Station:%d!!\n",i,comp_ID);
				printf("Please code station between 0 & %d\n",cngline-1);
				printf("Press any key!!!");
				setbuf(stdin,NULL);
				getchar();
				return -1;
			}
	return 0;
}
*/



void freedata( void * p_data) //free EventData memory
{
	struct EventData *data;
	data=(struct EventData *)p_data;
	if(data!=NULL)
	{
		free(data->Comp);
		free(data);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////





int main(int argc, char *argv[]) {

	int cngline = 0,*tmp,preidx;
	struct Generator *G;
	struct Queue *Q;
	struct Fork *p_fork;
	struct Exit *E;
	int i = 0, j, comp_ID, dest_ID, fork;
	char stn_type;
	//double avg_gen, avg_serv_time,sum_prob;
	double avg_gen, sum_prob,avg_delta;
	int **graph; //adjancet martix of DES topology graph
	//char prob[100];
	FILE *fp;
	int Err_input=0;



	void *head; //pointer to  FIFO queue header
	srand((unsigned) time(NULL)); //random seed

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	if(argc!=4) {
		printf("useage: %s [runsim] [configfile] [outputfile]\n",argv[0]);
		exit(-1);
	}


	/////////////////////////////////////
	/////////////input config  &&init data
	/////////////////////////////////////

	FEL=PQ_Create(); //create a priority queue  for FEL

	fp = fopen(argv[2], "r");
	ASSERT(fp!=NULL);

	fscanf(fp, "%d", &cngline);
	stn_list = malloc(sizeof(struct Station) * cngline);

	//init topology adjancent martrix

	graph=malloc(sizeof(int *)*cngline);//alloction memory for graph


		for (i=0;i<cngline;i++)
		{
			graph[i]=malloc(sizeof(int)*cngline);
			for (j=0;j<cngline;j++)
				graph[i][j]=BLANK; //set to BLANK
		}




	//////mapping comp_ID to index
	//////////////////////////////
	idx=malloc(sizeof(int)*cngline);
	tmp=malloc(sizeof(int)*cngline);
	i=0;
	while (i<cngline)
	{
		Err_input=fscanf(fp, "%d%*[^\n]\n", &idx[i] );
		if(Err_input==-1) printf("Config file input  line error!!!\n");
		ASSERT(Err_input!=-1);
		//printf("%d",idx[i]);
		i++;;
	}

	merge_sort(0,cngline-1,idx,tmp);
	free(tmp);



	preidx=idx[0];
	for (i=1;i<cngline;i++)//check repeat station declaration
			{
		if(idx[i]==preidx){
			printf("station  ID:%d  declaration repeat!",i);
			return -1;
		}
		preidx=idx[i];
	}



	rewind(fp);
	i=0;
	fscanf(fp, "%*[^\n]\n");//skip one  input line;
	while (i < cngline) {
		fscanf(fp, "%d %c", &comp_ID, &stn_type);
		//ASSERT(comp_ID == i);
		//if (check_ID(i,comp_ID,cngline)==-1) return -1;


		comp_ID=bfind(idx,cngline,comp_ID);//comp_ID mapping;
		ASSERT(comp_ID!=-1);

		stn_list[comp_ID].StationType = stn_type;
		if (stn_type == 'G') {

			//GENERATOR
			stn_list[comp_ID].StationType=GENERATOR;
			G = malloc(sizeof(struct Generator));
			fscanf(fp, "%lf", &avg_gen);
			fscanf(fp, "%lf", &avg_delta);
			fscanf(fp, "%d", &dest_ID);
			//if (check_ID(i,dest_ID,cngline)==-1) return -1;
			dest_ID=bfind(idx,cngline,dest_ID);//dest_ID mapping;
			ASSERT(dest_ID!=-1);
			if(avg_gen<Epsilon)
			{
				printf("Generator mean interarrival  time too smaller!!!\n");
				ASSERT(avg_gen>Epsilon);
			}

			G->Countcomp=0;
			G->Entercnt=0;
			G->DestComp = dest_ID;
			graph[comp_ID][dest_ID]=WHITE;


			G->IntArrTime = avg_gen;
			G->delta=avg_delta;
			stn_list[comp_ID].Stn = G;


			//generate a seed event
			///////////////////////
			//~~~~~~~~~~~~~~~~~~~~~
			NewComponent(comp_ID);
			//~~~~~~~~~~~~~~~~~~~~~
			//////////////////////




		}



		else if (stn_type == 'Q') {  //QUEUE
			stn_list[comp_ID].StationType=SERVICE_STATION;
			Q = (struct Queue*) malloc(sizeof(struct Queue));
			head = FIFO_creat();
			Q->head=head;
			fscanf(fp, "%lf", &Q->server_time);
			fscanf(fp, "%d", &fork);
			Q->fork = fork;
			p_fork = (struct Fork*) malloc(sizeof(struct Fork) * fork);

			Q->status=IDLE;	//init status of Server  is IDLE
			sum_prob=0.0;


			//Create the fork
			/////////////////
			for (j = 0; j < fork; j++)
			{
				fscanf(fp, "%lf", &p_fork[j].Proba);

				//p_fork[j].Proba=atof(prob);
				sum_prob+=p_fork[j].Proba;
			}
			//if(fabs(sum_prob-1)>Epsilon) printf("aaaaa%d\n",comp_ID);

			ASSERT(fabs(sum_prob-1)<Epsilon);//check if sum probabilty =1
			for (j = 0; j < fork; j++)
			{
				fscanf(fp, "%d", &(p_fork[j].DestID));

				p_fork[j].DestID=bfind(idx,cngline,p_fork[j].DestID);
				ASSERT(p_fork[j].DestID!=-1);
				if(p_fork[j].Proba<Epsilon) {
					printf(" Comp_ID %d fork to %d probabilty is 0\n",
							idx[comp_ID], idx[p_fork[j].DestID]);

								}
/*				if(comp_list[idx[p_fork[j].DestID]].StationType==GENERATOR)
				{
					printf("CompID:%d fork to a Generator %d\n",idx[comp_ID],idx[p_fork[j].DestID]);
					exit(-1);
				}
*/
				//if (check_ID(i,p_fork[j].DestID,cngline)==-1) return -1;
				graph[comp_ID][p_fork[j].DestID]=WHITE;
			}



			//init statics
			Q->Q_fork = p_fork;


			Q->Now_Event=NULL;
			stn_list[comp_ID].Stn = Q;

		} else if (stn_type == 'E') {
			stn_list[comp_ID].StationType=EXIT;
			E = malloc(sizeof(struct Exit));
			stn_list[comp_ID].Stn = E;

			//init status
			E->Count=0;
			E->total_time=0;


		} else if(stn_type == 'A')
		{
			//process "AND" 
		}
		else if(stn_type == 'A')
		{
			//process "EXT_SUPP"
		}
		else
		{
			fprintf(stderr, "illgeal compment type");
			ASSERT(true);
		}

		i++;

	}
	fclose(fp);





	///////////////////////////////////////////////////////////////////////
	//// Start simulation/////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
	RunSim(atoi(argv[1]));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//////////////////////////////////////////////////////////////////////






	printf("Run end\n");

	//////////////////////////////////////////////////
	//after simulation, output statics  and free memory
	///////////////////////////////////////////////////

	//free(comp_list); //free comp_list memory && output statics;
	fp=fopen(argv[3],"w");
	for (i=0;i<cngline;i++) //free Station memory
	{
		if (stn_list[i].StationType==EXIT)
				{

								E=stn_list[i].Stn ;

					fprintf(fp,"Comp:%d EXIT\n",idx[i]);
					fprintf(fp,"  Number: %d\n",E->Count);
				}

		if (stn_list[i].StationType==SERVICE_STATION)
		{//statics queuing data of the station

			FIFO_free(((struct Queue *) stn_list[i].Stn)->head,freedata);
			free(((struct Queue*)  stn_list[i].Stn)->Q_fork);
			//FIFO_free(Q->head, freedata);
			//free(Q->Q_fork);
		};
		free(stn_list[i].Stn);

	};


	free(stn_list); //free comp_list memory
	PQ_free(FEL,freedata); //free PQ memory
	free(idx);

	return 0;

}

