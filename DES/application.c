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
#define LIMIT_yes 1
#define LIMIT_no 0

//
// Generator Component
//
struct Generator {
	double IntArrTime;	// mean interarrival time for generated components
	double Delta;
	int DestComp;          	// ID of next component products are sent to
	int Countprod; 		//set Product to unique "Identify  Number"(ProductID)
	int Entercnt;			//Real product number entering system ;
	double Cost;
	int Limit_type;	//if true ,Next Queue >n number  ,generator do not depature
	int Limit_status;	// is wait next station to 0;
	int Discard;
	int Line;
	FF_head_t *Next_que;
//int mult;

};

// Fork Structure
//
struct Fork {
	double Proba;
	int DestID;
//	int CompCount;
};
struct Select {
	int Route_true_ID;
//	FF_head_t * R1_Que;
	int Route_false_ID;
//	FF_head_t * R2_Que;

	int status;
//	int CompCount;
};
struct And {
	int num;
	int *Source_ID;
	int *Sele;
	FF_head_t **And_Que;
	int DestID;
//	int CompCount;
};
struct Queue {
	FF_head_t *head;
	int fork;
	int status;          	//0: server is idle,1: server is busy;
	struct Fork *Q_fork; //after  service, the probability & destination that the prod will go
	double server_time;		//behavior parameter of server
	double server_delta;
	struct EventData *Now_Event;
	//Now, sevicing prod's infomation
	double Cost;
	double Defective;

};

//
// Exit component
//
struct Exit {
	int Count; 			// number of products that exited at this component
	double total_time; // total time between  start  & end ,when the prod is in system
	double total_cost;
	int tag;

};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////////////////////////////////

// prototypes for event handlers
//void Generate(struct EventData *);    // generate new prod
void Arrival(struct EventData*);        // arrival at a component
void Departure(struct EventData*);      // departure at a component
void NewProdt(int);                	//generate new prod

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
	ASSERT(d->EventType == DEPARTURE);                //Only process DEPARTURE;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//a negligible (i.e., zero) amount of time to travel between components.
	//So a "DEPARTURE" suddenly triggers an Arrival  or Exit
	Departure(d);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}

void Arrival(struct EventData *e) {
	//double ts,inqu_time,ex_time;
	double ts, ex_time;
	//int Comp;                   // ID of component where arrival occurred
	struct Exit *pExit;         // pointer to info on exit component
	struct Queue *queue;
	if (e->EventType != ARRIVAL) {
		fprintf(stderr, "Unexpected event type\n");
		exit(1);
	}

	if (comp_list[e->CompID].ComponentType == GENERATOR) {
		printf("illegal ENTER!!!!\n");
		exit(-1);
	}

	// processing depends on the type of component that arrival occurred at
	if (comp_list[e->CompID].ComponentType == SELE)

	{
		struct Select *pSele;
		pSele = (struct Select*) (comp_list[e->CompID].Comp);
//		if ((e->Prod)->tag == true) {
//			e->Prod->tag = false;
//			e->Prod->tag_part = pSele->status;
//			pSele->status = pSele->status == true ? false : true;
//
//		}

		if (e->Prod->tag_part) {
			e->Pre_compid = e->CompID;
			e->CompID = pSele->Route_true_ID;
			e->EventType = ARRIVAL;
			e->Event_begin_time = CurrentTime();
			Arrival(e);

		} else {
			e->Pre_compid = e->CompID;
			e->CompID = pSele->Route_false_ID;
			e->EventType = ARRIVAL;
			e->Event_begin_time = CurrentTime();
			Arrival(e);
		}

	}

	else if (comp_list[e->CompID].ComponentType == AND) {
		int source_id = e->Pre_compid;
		int i;
		int de_flag;
		struct And *pAnd;
		struct EventData *cur_Event, *and_Event;
		pAnd = comp_list[e->CompID].Comp;
		//find "And" Queue , append to

		for (i = 0; i < pAnd->num; i++) {
			if (pAnd->Source_ID[i] == source_id
					&& pAnd->Sele[i] == e->Prod->tag_part) {
				FIFO_in(pAnd->And_Que[i], e);
				break;
			}
		}
		de_flag = true;
		//check  to assembly
		for (i = 0; i < pAnd->num; i++) {
					printf("-----%d  %d\n",i,FIFO_count(pAnd->And_Que[i]));
		}
		for (i = 0; i < pAnd->num; i++) {
			//printf("-----%d  %d\n",i,FIFO_count(pAnd->And_Que[i]));
			if (FIFO_count(pAnd->And_Que[i]) == 0) {
				de_flag = false;
				break;
			}
		}
		if (de_flag == true)

		{ //to next station
			cur_Event = (struct EventData*) FIFO_out(pAnd->And_Que[0]);
			cur_Event->Event_begin_time = CurrentTime();
			cur_Event->Pre_compid = e->CompID;
			cur_Event->CompID = pAnd->DestID;
			cur_Event->EventType = ARRIVAL;
			for (i = 1; i < pAnd->num; i++) {
				and_Event = (struct EventData*) FIFO_out(pAnd->And_Que[i]); //get parts from Queue
				cur_Event->Prod->Que_time += and_Event->Prod->Que_time;
				cur_Event->Prod->Cost += and_Event->Prod->Cost; //sum cost
				free(and_Event->Prod);
				free(and_Event);
			}

			Arrival(cur_Event);

		}

	} else if (comp_list[e->CompID].ComponentType == EXIT) {

		//arrival at an EXIT component
		ex_time = CurrentTime() - e->Prod->Generation_time;

		printf("ID %d Exit      Component=%d at time %f", e->Prod->ProductID,
				idx[e->CompID], CurrentTime());
		printf(" Enter Time %lf,Sum Time %lf ", e->Prod->Generation_time,
				CurrentTime() - e->Prod->Generation_time);
		printf("Queuing Time %lf\n", e->Prod->Que_time);

		pExit = (struct Exit*) (comp_list[e->CompID].Comp);

		//statistical info
		(pExit->Count)++;  		// number of exiting Prods
		(pExit->total_time) += ex_time;
		pExit->total_cost += e->Prod->Cost;
		if (pExit->tag == 1)
			total_prod--;   //count acceptable product

		//free memory
		free(e->Prod);
		free(e);
		//free(e->Cust);         // release memory of exiting prod
	} else if (comp_list[e->CompID].ComponentType == QUEUE_STATION) {

		//arrival at a QUEUE_STATION
		printf("ID %d Arrival   Component=%d at time %f \n", e->Prod->ProductID,
				idx[e->CompID], CurrentTime());
		queue = (struct Queue*) comp_list[e->CompID].Comp;

		e->Event_begin_time = CurrentTime();

		if (queue->status == BUSY)  //check server's status
		{ // add to Queue

			FIFO_in(queue->head, e);
			//FF_in
		} else {     // status is IDLE
			//set to busy
			queue->status = BUSY;

			queue->Now_Event = e; //set servicing prod now

			//start service
			e->EventType = DEPARTURE;
			//set service endtime
			// add an  gauss  random number
			ts = CurrentTime()
					+ randgauss(queue->server_time, queue->server_delta);
			e->Prod->Cost += queue->Cost;


			//e->Prod->Que_time=0;

			printf(
					"ID %d Serviced  Component=%d at time %lf,Queue Time %lf,end time %lf\n",
					e->Prod->ProductID, idx[e->CompID], CurrentTime(), 0.0, ts);
			//e->Event_begin_time=CurrentTime();
			//printf("Que %lf\n",e->Cust->Que_time);

			//add to schedule
			Schedule(ts, e);

		}
	}

}

void NewProdt(int compID) {

	//CompID generate a new prod
	struct EventData *d;
	double ts;
	//int forkto;
	//struct Customer *cur_custom, *NewCust;
	struct Product *NewProd;
	struct Generator *pGen;

	pGen = (struct Generator*) comp_list[compID].Comp;
	NewProd = malloc(sizeof(struct Product));
	ASSERT(NewProd!=NULL);
	//if (pGen->Countprod>pGen->mult*total_prod) return;

	NewProd->ProductID = customID++; // Product ID

	(pGen->Countprod)++;

	NewProd->tag = false;
	NewProd->tag_part = pGen->Line;
	NewProd->Cost = pGen->Cost;

	d = malloc(sizeof(struct EventData));
	ASSERT(d!=NULL);

	//init Product
	d->EventType = DEPARTURE;
	d->Prod = NewProd;
	d->CompID = compID;

	d->Prod->Que_cnt = 0;
	d->Prod->Que_time = 0;

	// to gauss  random number
	ts = randgauss(pGen->IntArrTime, pGen->Delta);
	// Time when prod will enter system ;
	//if (ts<0.001) ts=0.001;
	ts += CurrentTime();
	//d->Cust->Generation_time=ts;
	////////////////////////////////////////////////////////
	//ts is the actual time when product will  enter system
	///////////////////////////////////////////////////////
	Schedule(ts, d);
}

void Departure(struct EventData *e)

//Event ----Departure
{
	//struct EventData *d;
	double ts, inqu_time;
	int forkto;

	//struct Customer *cur_custom,*NewCust;
	//struct Customer *NewCust;
	struct EventData *cur_Event;
	struct Generator *pGen;  // pointer to info on generator component

	ASSERT(e->EventType = DEPARTURE);

	e->Pre_compid = e->CompID;

	if (comp_list[e->CompID].ComponentType == GENERATOR) {
		printf("ID %d Enter     Component=%d at time %lf  \n",
				e->Prod->ProductID, idx[e->CompID], CurrentTime());
		//product's real arrival!!!!!
		//Enter the system!!!!!
		pGen = (struct Generator*) comp_list[e->CompID].Comp;
		pGen->Entercnt++;
		e->Prod->Generation_time = CurrentTime();
		//get_ fork_ID
		forkto = fork_ID(e->CompID);
		//cur_custom = e->Cust;

		/////////////////////////////////////////////
		// Create a new product && add to FEL
		NewProdt(e->CompID);
		/////////////////////////////////////////////

		//a negligible (i.e., zero) amount of time to travel between components.
		//So a "DEPARTURE" suddenly triggers an Arrival
		e->EventType = ARRIVAL;
		e->CompID = forkto;
		if (pGen->Limit_type == false) {

			Arrival(e);
		} else {
			if (FIFO_count(pGen->Next_que) == 0)
				pGen->Limit_status = false; // Queqe is null ,set status false
			if (FIFO_count(pGen->Next_que)
					<= limit_cnt&& pGen->Limit_status==false) {
				Arrival(e);
			} else {
				//discard part
				pGen->Limit_status = true;
				pGen->Discard++;
				free(e->Prod);
				free(e);
				//free Eventdata;
			}

		}
		return;

	}

	// Queue Station Departure
	if (comp_list[e->CompID].ComponentType == QUEUE_STATION) {
		//get _fork_ID;
		forkto = fork_ID(e->CompID);
		printf("ID %d Departure Component=%d at time %lf to %d \n",
				e->Prod->ProductID, idx[e->CompID], CurrentTime(), forkto);
		struct Queue *queue = (struct Queue*) comp_list[e->CompID].Comp;

		//station's statistical info

		///////////////////////////////////////////////////////////////////////////////////////
		//Now server is idle. if queue is not empty,begin to service the first prod in queue
		////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////

		if (FIFO_isempty(queue->head)) {

			//////////////////////////////
			// no prod is waiting service
			///////////////////////////////
			//printf("in queue\n");
			queue->status = IDLE;
			queue->Now_Event = NULL;

		} else {

			///////////////////////////
			//To service the first one in queue
			///////////////////////////

			cur_Event = (struct EventData*) FIFO_out(queue->head);

			queue->Now_Event = cur_Event; //set servicing prod now;

			//set Event DATA
			cur_Event->EventType = DEPARTURE;
			cur_Event->CompID = e->CompID;
			//printf("111111\n");
			inqu_time = CurrentTime() - cur_Event->Event_begin_time; //queuing time
			//statistical info of queuing

			//product queuing statics
			cur_Event->Prod->Que_time += inqu_time;
			cur_Event->Prod->Que_cnt++;

			// service time  follows exponential distribution
			// get the random number
			//ts = CurrentTime() + randexp(queue->server_time);
			ts = CurrentTime()
					+ randgauss(queue->server_time, queue->server_delta);
			cur_Event->Prod->Cost += queue->Cost;
			//set the end time of service
			//ts = CurrentTime() +5;

			printf(
					"ID %d Serviced  Component=%d at time %lf,Queue Time %lf,end time %lf ",
					cur_Event->Prod->ProductID, idx[cur_Event->CompID],
					CurrentTime(), inqu_time, ts);
			printf(" Accu que %lf\n", cur_Event->Prod->Que_time);

			cur_Event->Event_begin_time = CurrentTime(); //start to service,record the starting time

			//from Queue get A prod
			//begin service ,add departure event to schedule

			Schedule(ts, cur_Event);

		}

		//////////////////////////////////////////
		// a "DEPARTURE" trigger an Arrival at once
		//////////////////////////////////////////

		e->EventType = ARRIVAL;
		e->CompID = forkto;

		Arrival(e);
		return;
	}

	fprintf(stderr, "illegal status");

	ASSERT(false);
	return;

}

int fork_ID(int comp_ID) {
	//select a fork

	double r_prba;
	int i, fork;
	struct Queue *Comp_P;
	if (comp_list[comp_ID].ComponentType == QUEUE_STATION) {

		//random selection
		r_prba = rand() / (RAND_MAX + FLT_MIN); //get random [0,1)
		Comp_P = (struct Queue*) comp_list[comp_ID].Comp;
		/*if(Comp_P->fork==1){
		 printf("aaaaa");
		 };
		 */

		for (i = 0; i < Comp_P->fork - 1; i++) {
			if (r_prba >= 0 && r_prba < (Comp_P->Q_fork)[i].Proba)
				break;
			r_prba = r_prba - Comp_P->Q_fork[i].Proba;
		}
		// fork to final component  if no selection on first K-1;
		fork = (Comp_P->Q_fork[i]).DestID;

		/*if (comp_list[fork].ComponentType == GENERATOR) { //do not fork to a GENERATOR
		 printf("CompID:%d fork to a Generator %d\n", idx[comp_ID],idx[fork]);
		 exit(-1);
		 }
		 */
		return fork;
	}
	if (comp_list[comp_ID].ComponentType == GENERATOR) {

		return ((struct Generator*) comp_list[comp_ID].Comp)->DestComp;
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

void freedata(void *p_data) //free memory of EventData
{
	struct EventData *data;
	data = (struct EventData*) p_data;
	if (data != NULL) {
		free(data->Prod);
		free(data);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

	int cngline = 0, *tmp, preidx;
	struct Generator *G;
	struct Queue *Q;
	struct Fork *p_fork;
	struct Exit *E;
	int i = 0, j, comp_ID, dest_ID, fork;
	char comp_type;
	//double avg_gen, avg_serv_time,sum_prob;
	double avg_gen, sum_prob, avg_delta;
	int **graph; //adjacent matrix of DES topology graph
	//char prob[100];
	FILE *fp;
	int Err_input = 0;
	customID = 0;
	total_prod = 100;
	limit_cnt = 100000;

	void *head; //pointer to  FIFO queue header
	srand((unsigned) time(NULL)); //random seed

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	if (argc != 4) {
		printf("usage: %s [runsim] [configfile] [outputfile]\n", argv[0]);
		exit(-1);
	}

	while(1)
	{
		setbuf(stdin, NULL);
		setbuf(stdout, NULL);
		printf("\nInput the product number:");
		if(scanf("%d",&total_prod)==1) break;

	}

	/////////////////////////////////////
	/////////////input config &&init data
	/////////////////////////////////////

	FEL = PQ_Create(); //create a priority queue  for FEL

	fp = fopen(argv[2], "r");
	ASSERT(fp!=NULL);

	fscanf(fp, "%d", &cngline);
	comp_list = malloc(sizeof(struct Component) * cngline);

	//init topology adjacent matrix

	graph = malloc(sizeof(int*) * cngline); //allocate memory for graph

	for (i = 0; i < cngline; i++) {
		graph[i] = malloc(sizeof(int) * cngline);
		for (j = 0; j < cngline; j++)
			graph[i][j] = BLANK; //set to BLANK
	}

	//////mapping comp_ID to index
	//////////////////////////////
	idx = malloc(sizeof(int) * cngline);
	tmp = malloc(sizeof(int) * cngline);
	i = 0;
	while (i < cngline) {
		Err_input = fscanf(fp, "%d%*[^\n]\n", &idx[i]);
		if (Err_input == -1)
			printf("Config file input  line error!!!\n");
		ASSERT(Err_input != -1);
		//printf("%d",idx[i]);
		i++;
		;
	}

	merge_sort(0, cngline - 1, idx, tmp);
	free(tmp);

	preidx = idx[0];
	for (i = 1; i < cngline; i++)	//check duplicate component declaration
			{
		if (idx[i] == preidx) {
			printf("component  ID:%d  duplicate declaration!", i);
			return -1;
		}
		preidx = idx[i];
	}

	rewind(fp);
	i = 0;
	fscanf(fp, "%*[^\n]\n");	//skip one  input line;
	while (i < cngline) {
		fscanf(fp, "%d %c", &comp_ID, &comp_type);
		//ASSERT(comp_ID == i);
		//if (check_ID(i,comp_ID,cngline)==-1) return -1;

		comp_ID = bfind(idx, cngline, comp_ID);		//comp_ID mapping;
		ASSERT(comp_ID != -1);

		comp_list[comp_ID].ComponentType = comp_type;
		if (comp_type == 'G') {

			//GENERATOR
			comp_list[comp_ID].ComponentType = GENERATOR;
			G = (struct Generator*) malloc(sizeof(struct Generator));
			fscanf(fp, "%lf", &avg_gen);
			fscanf(fp, "%lf", &avg_delta);
			fscanf(fp, "%lf", &G->Cost);
			fscanf(fp, "%d", &dest_ID);
			fscanf(fp, "%d", &G->Line);
			//int limit_type;
			G->Limit_status = false;
			G->Limit_type = false;
//			//fscanf(fp, "%d", &limit_type);
////			if (limit_type == 1) {
////				G->Limit_type = true;
////
////			} else
//				G->Limit_type = false;


//			fscanf(fp,"%d",&G->mult);

			//if (check_ID(i,dest_ID,cngline)==-1) return -1;
			dest_ID = bfind(idx, cngline, dest_ID);		//dest_ID mapping;
			ASSERT(dest_ID != -1);
			if (avg_gen < Epsilon) {
				printf("Generator mean interarrival  time too smaller!!!\n");
				ASSERT(avg_gen>Epsilon);
			}

			G->Countprod = 0;
			G->Entercnt = 0;
			G->DestComp = dest_ID;
			graph[comp_ID][dest_ID] = WHITE;

			G->IntArrTime = avg_gen;
			G->Delta = avg_delta;
			comp_list[comp_ID].Comp = G;
			G->Next_que = NULL;

			//generate a seed event
			///////////////////////
			//~~~~~~~~~~~~~~~~~~~~~
			NewProdt(comp_ID);
			//~~~~~~~~~~~~~~~~~~~~~
			//////////////////////

		}

		else if (comp_type == 'Q') {  //QUEUE
			comp_list[comp_ID].ComponentType = QUEUE_STATION;
			Q = (struct Queue*) malloc(sizeof(struct Queue));
			head = FIFO_creat();
			comp_list[comp_ID].Comp_que = head;
			Q->head = head;
			fscanf(fp, "%lf", &Q->server_time);
			fscanf(fp, "%lf", &Q->server_delta);
			fscanf(fp, "%lf", &Q->Cost);
			fscanf(fp, "%d", &fork);
			Q->fork = fork;
			p_fork = (struct Fork*) malloc(sizeof(struct Fork) * fork);

			Q->status = IDLE;	//init status of Server  -- IDLE
			sum_prob = 0.0;

			//Create the fork
			/////////////////
			for (j = 0; j < fork; j++) {
				fscanf(fp, "%lf", &p_fork[j].Proba);

				//p_fork[j].Proba=atof(prob);
				sum_prob += p_fork[j].Proba;
			}
			//if(fabs(sum_prob-1)>Epsilon) printf("aaaaa%d\n",comp_ID);

			ASSERT(fabs(sum_prob-1)<Epsilon);	//check if sum of probability =1
			for (j = 0; j < fork; j++) {
				fscanf(fp, "%d", &(p_fork[j].DestID));

				p_fork[j].DestID = bfind(idx, cngline, p_fork[j].DestID);
				ASSERT(p_fork[j].DestID != -1);
				if (p_fork[j].Proba < Epsilon) {
					printf(" Comp_ID %d fork to %d probabilty is 0\n",
							idx[comp_ID], idx[p_fork[j].DestID]);

				}
				/*				if(comp_list[idx[p_fork[j].DestID]].ComponentType==GENERATOR)
				 {
				 printf("CompID:%d fork to a Generator %d\n",idx[comp_ID],idx[p_fork[j].DestID]);
				 exit(-1);
				 }
				 */
				//if (check_ID(i,p_fork[j].DestID,cngline)==-1) return -1;
				graph[comp_ID][p_fork[j].DestID] = WHITE;

			}

			//init statistical data
			Q->Q_fork = p_fork;

			Q->Now_Event = NULL;

			comp_list[comp_ID].Comp = Q;

		} else if (comp_type == 'E') {
			comp_list[comp_ID].ComponentType = EXIT;
			E = malloc(sizeof(struct Exit));
			comp_list[comp_ID].Comp = E;

			//init status
			E->Count = 0;
			E->total_time = 0;
			fscanf(fp, "%d", &E->tag);

		} else if (comp_type == 'A') {
			comp_list[comp_ID].ComponentType = AND;
			struct And *A;
			int id;
			int k;

			A = (struct And*) malloc(sizeof(struct And));
			fscanf(fp, "%d", &A->DestID);
			A->DestID = bfind(idx, cngline, A->DestID);
			fscanf(fp, "%d", &A->num);
			A->Source_ID = (int*) malloc(sizeof(int) * A->num);
			A->And_Que = (FF_head_t**) malloc(sizeof(FF_head_t*) * A->num);
			A->Sele = (int*) malloc(sizeof(int) * A->num);
			for (k = 0; k < A->num; k++) {
				fscanf(fp, "%d", &id);
				id = bfind(idx, cngline, id);
				A->Source_ID[k] = id;
				A->Sele[k] = 0;
				for (j = k - 1; j >= 0; j--) {
					if (A->Source_ID[j] == id)
						A->Sele[k]++;
				}
				head = FIFO_creat();
				A->And_Que[k] = head;

			}
			comp_list[comp_ID].Comp = A;

		}

		else if (comp_type == 'S') {
			comp_list[comp_ID].ComponentType = SELE;
			struct Select *S;

			S = (struct Select*) malloc(sizeof(struct Select));
			fscanf(fp, "%d", &S->Route_true_ID);
			S->Route_true_ID = bfind(idx, cngline, S->Route_true_ID);
			fscanf(fp, "%d", &S->Route_false_ID);
			S->Route_false_ID = bfind(idx, cngline, S->Route_false_ID);
			S->status = true;
			comp_list[comp_ID].Comp = S;

		} else {
			fprintf(stderr, "illegal component type");
			ASSERT(true);
		}

		i++;

	}
	for (i = 0; i < cngline; i++) {
		if (comp_list[i].ComponentType == GENERATOR) {
			struct Generator *pGen;
			pGen = comp_list[i].Comp;
			if (pGen->Limit_type == true) {
				if (comp_list[pGen->DestComp].ComponentType == QUEUE_STATION) {
					pGen->Next_que = comp_list[pGen->DestComp].Comp_que;
				} else if (comp_list[pGen->DestComp].ComponentType == AND) {
					struct And *pAnd;

					pAnd = comp_list[pGen->DestComp].Comp;
					for (j = 0; j < pAnd->num; j++) {
						if (pAnd->Source_ID[j] == i) {
							pGen->Next_que = pAnd->And_Que[j];
							break;
						}
					}
				}
			}
			//fill Generator que

		}

	}

	fclose(fp);

	///////////////////////////////////////////////////////////////////////
	//// Start simulation/////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////
	RunSim(atoi(argv[1]));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//////////////////////////////////////////////////////////////////////

	//printf("Run end %.2lf\n",CurrentTime());

	//////////////////////////////////////////////////
	//after simulation, output statics  and free memory
	///////////////////////////////////////////////////

	//free(comp_list); //free comp_list memory && output statics;
	double cost_sum = 0;
	fp = fopen(argv[3], "w");
	for (i = 0; i < cngline; i++) //free Component memory
			{
		if (comp_list[i].ComponentType == EXIT) {

			E = comp_list[i].Comp;

			fprintf(fp, "Comp:%d EXIT\n", idx[i]);
			fprintf(fp, "  Number: %d\n", E->Count);
			fprintf(fp, "Cost:%.2lf\n", E->total_cost);
			cost_sum += E->total_cost;
		}

		if (comp_list[i].ComponentType == QUEUE_STATION) { //statics queuing data of the station

			FIFO_free(((struct Queue*) comp_list[i].Comp)->head, freedata);
			free(((struct Queue*) comp_list[i].Comp)->Q_fork);
			//FIFO_free(Q->head, freedata);
			//free(Q->Q_fork);
		};
		free(comp_list[i].Comp);

	};
	fprintf(fp, " Total Cost:%.2lf\n", cost_sum);
	fprintf(fp, " Time is %.2lf", CurrentTime());
	printf(" Total Cost:%.2lf\n", cost_sum);

	free(comp_list); //free comp_list memory
	PQ_free(FEL, freedata); //free PQ memory
	free(idx);

	return 0;

}

