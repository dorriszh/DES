#include <stdio.h>
#include <stdlib.h>

#include "pq.h"

#ifdef PriQueue

//
//    header node is a dummy£º,do not store data ,only to  count the queue;
//    data   store after the header node


//    init the queue
PrioQ* PQ_Create() {
	PrioQ *queue;
	queue = (PrioQ*) malloc(sizeof(PrioQ));    //    Allocate a dummy
	ASSERT(queue!=NULL);
	/*if (queue == NULL) {        //    memory allocation success?
	 printf("memory allocation failed...");
	 exit(-1);
	 }*/
	queue->Next = NULL; //    init queue to NULL
	queue->data=NULL;

	queue->val.Qcount = 0; //  Count set to 0,queue item is empty
	//printf("create success ...\n");
	return queue;
}

//    is the queue empty?
bool PQ_IsEmp(PrioQ *queue) {
	if (queue == NULL) {
		printf("ERROR NULL QUEUE.....\n");
		exit(-1);
	}
	if (queue->Next == NULL) {
		printf("queue is empty...\n");
		return true;
	} else {
		//printf("queue is not empty...\n");
		return false;
	}

}
//   
//    insert the key in the queue ,location defined  by order of the key
int PQ_Insert(PrioQ *queue, double key, void *data) {
	PrioQ *New, *P;
	New = (PrioQ*) malloc(sizeof(PrioQ)); //    allocation memmory for queue item
	ASSERT(New!=NULL);
	/*if (New == NULL) {
	 printf("alloction failed for key :%f...", key);
	 return -1;
	 }*/
	New->val.key = key;    //    set key & data  to item of node
	New->data = data;
	P = queue;
	while (P->Next != NULL)
		if (P->Next->val.key <= key) { //find the location to be inserted
			P = P->Next;
		} else
			break;
	New->Next = P->Next; // SET the pointer 
	P->Next = New;
	queue->val.Qcount++; //set the count 

	//printf("insert  %f success...\n", key);
	return 0;

}

//
//    delete first data item
//    if success £¬return the key ,else  return -1
void* PQ_Delete(PrioQ *PQ, double *key) {
	PrioQ *P;
	void *data;
	if (PQ_IsEmp(PQ)) {
		printf("Queue is empty,delete fail\n");
		return NULL;

	}
	P = PQ->Next;    	//    temp pointer
	*key = P->val.key; 	//    get the key
	data = P->data;
	PQ->Next = P->Next;
	PQ->val.Qcount--;
	free(P);    		//    free the memory
	//printf("Delete success %f\n", *key);
	return data;

}
//    visit all around
void PQ_print(PrioQ *queue) {
	PrioQ *P;
	int i = 1;

	if (PQ_IsEmp(queue)) {
		printf(" It is empty!!! \n");
		return;
	}
	P = queue->Next;    //all around from first data item £¨no dummy node £©

	printf("Queue list is£º\n");
	while (P != NULL) {
		printf("%d     %f \n", i++, P->val.key);
		P = P->Next;
	}
	printf("\n");
}

//    Free Queue

void PQ_free(PrioQ *queue, void (*freedata)(void * data)) {
	//free memory of PQ queue. if freedata is not NULL ,call freedata free the data space
	PrioQ *P, *Q;
	P = queue->Next; //First Node

	//free the first data node
	while (P != NULL) {
		Q = P->Next;
		if (freedata!=NULL)
			freedata(P->data);
		free(P);
		P = Q;
	}
	//free(queue); //free dummy
	//printf("Free Queue Success...\n");
}

//return the item number in the queue
int PQ_count(PrioQ *queue) {
	//printf("count is %d \n", queue->val.Qcount);
	return queue->val.Qcount;
}
#endif

#ifdef FIFOQueue0
void* FIFO_creat() {
	FF_head_t *FF_p;
	FF_p = malloc(sizeof(FF_head_t));
	ASSERT(FF_p!=NULL);
	FF_p->header = NULL;  //set header node & init
	FF_p->tail = NULL;
	FF_p->cnt=0;
	return FF_p;
}
void FIFO_in(FF_head_t *FF_p, void *inqueue) {
	FF_node_t *FF_node_item;
	FF_node_item = malloc(sizeof(FF_node_t));
	FF_node_item->next=NULL;  //set  FIFO data node
	FF_p->cnt++;
	FF_node_item->data = inqueue;
	if (FF_p->tail == NULL) {

		//a empty queqe, set header & tail
		FF_p->header = FF_node_item;
		FF_p->tail = FF_node_item;
	} else {
		//not empty,add to tail
		FF_p->tail->next = FF_node_item; //add to tail
		FF_p->tail = FF_node_item;  //set tail

	}
}
void* FIFO_out(FF_head_t *FF_p)
//return  a term from FIFO,if empty ,return NULL
{
	FF_node_t *ret_node;
	void *ret_data;
	if (FF_p->tail == NULL)	// Queue is empty
		return NULL;
	FF_p->cnt--;
	ret_node = FF_p->header;
	FF_p->header = FF_p->header->next;	//set new header
	if (FF_p->header == NULL)
		FF_p->tail = NULL;	//after out ,it is empty, set tail
	ret_data = ret_node->data;
	free(ret_node);	//free Queue item
	return ret_data; //return pointer to  Queue data

}
int  FIFO_count(FF_head_t *FF_p)
{
	return(FF_p->cnt);
}
bool FIFO_isempty(FF_head_t *FF_p) {
	if (FF_p->tail == NULL)
		return true;
	return false;

}

void FIFO_free(FF_head_t  * P,void(*freedata)(void * data))
//free memory of FIFO queue. if freedata is not NULL ,call freedata free the data space
{
	FF_node_t *Q,*N;
	Q=P->header;

	while (Q!=NULL)
	{
		N=Q->next;
		if (freedata!=NULL) freedata(Q->data);  //call function pointer,release user data
		free(Q);
		Q=N;

	}
	free(P);
}

#endif



int bfind (int *idx ,int len,int key)//Binary search for comp_ID
{
	int l,r,mid;
	l=0;
	r=len-1;
	while (l<=r)
	{
		mid=l+(r-l)/2;
		if (idx[mid]==key) return mid;
		else	if (key>idx[mid])
		{
			l=mid+1;    //right
		}
		else
		{
			r=mid-1;    //left
		}
	}
	return -1;
}

void merge_sort(int i,int j, int * idx,int *tmp) //merge sort data in idx ,tmp is temporary  space
{
	int k = 0;

	if (i == j)
			return;
	k = (i + j - 1) / 2;
	merge_sort(i, k, idx, tmp);//sort i to k
	merge_sort(k + 1, j, idx, tmp);//sort k+1 to j

	int l = i;
	int m = i;
	int n = k + 1;

	while (m < k + 1 && n < j + 1) {//smaller to  tmp array
		if (idx[m] <= idx[n]) {

			tmp[l++] = idx[m++];
		} else {

			tmp[l++] = idx[n++];
		}
	}

	while (m < k + 1) {// (K+1 to j) sequence have exhausted,the rest of (i to k) to tmp

		tmp[l++] = idx[m++];
	}

	while (n < j + 1) {// (i to k) sequence have exhausted,the rest of (k+1 to j) to tmp

		tmp[l++] = idx[n++];
	}

	for (m = i; m <= j; m++) {//copy tmp to original degree array

		idx[m] = tmp[m];
	}

}

int connect(int **graph, int n, int i, void *FIFO_q)
//Check DES topology connectivity between station & EXIT,Breath First  search
{
	int j, *e_in;
	for (j = 0; j < n; j++)
		if (graph[j][i] == WHITE) {
			graph[j][i] = BLACK;
			e_in = malloc(sizeof(int));
			*e_in = j; //get vex
			FIFO_in(FIFO_q, e_in);
			//j,i->FIFO;
		}
	e_in = (int*) (FIFO_out(FIFO_q));
	if (e_in != NULL) {
		connect(graph, n, *e_in, FIFO_q);
		free(e_in);
	}
	return 0;
}

int connect0(int **graph, int n, int i, void *FIFO_q)
//Check DES topology connectivity between station & GENERATOR,Breath First  search
{
	int j, *e_in;
	for (j = 0; j < n; j++)
		if (graph[i][j] == WHITE || graph[i][j] == BLACK) {
			graph[i][j] = GRAY;
			e_in = malloc(sizeof(int));
			*e_in = j; //get vex
			FIFO_in(FIFO_q, e_in);
			//j,i->FIFO;
		}
	e_in = (int*) (FIFO_out(FIFO_q));
	if (e_in != NULL) {
		connect0(graph, n, *e_in, FIFO_q);
		free(e_in);
	}
	return 0;
}


void free_conn( void * p) //free connect queue vex  memory
{

		free(p);

}

