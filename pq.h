#define bool 		_Bool
//#define bool int
#define false 		0
#define true 		1
#define BLANK		0
#define WHITE 		1
#define BLACK		2
#define GRAY 		3
#define ASSERT(cond) {if(!(cond)) {fprintf(stderr,"Error in file %s line %d,assert ("#cond ")failed!",__FILE__,__LINE__);exit(-1);}}

#define PriQueue

#define FIFOQueue0


typedef struct Node {
	union {
		double key;
		int Qcount;
	} val;  // Union  header:count ;  queue item: key

	void *data;        //    point to  data
	struct Node *Next;
} PrioQ;

//    function

PrioQ* PQ_Create();     //Create queue
bool PQ_IsEmp(PrioQ*); // Is empty?
int PQ_Insert(PrioQ*, double, void*); //Insert
void* PQ_Delete(PrioQ*, double*);
void PQ_print(PrioQ*);
void PQ_free(PrioQ*, void (*)(void *));
int PQ_count(PrioQ*);



typedef struct FF_node {   //node data of FIFO queue
	struct FF_node *next;
	void *data;

} FF_node_t;

typedef struct FF_que {  //data structure for FIFO queue header
	FF_node_t *header;
	FF_node_t *tail;
	int cnt;
} FF_head_t;

//FIFO function
bool FIFO_isempty(FF_head_t *);
void* FIFO_out(FF_head_t *);

void FIFO_in(FF_head_t *, void *);

void* FIFO_creat();

void FIFO_free(FF_head_t  * ,void(*)(void * ));
int  FIFO_count(FF_head_t *);


int bfind (int *,int ,int ); //Binary search
void merge_sort(int ,int , int * ,int *); //merge  sort
int connect(int ** ,int ,int ,void *);  //check connectivity of station and EXIT
int connect0(int ** ,int ,int ,void *); //check connectivity GENERATOR and this station
void free_conn( void * ); //free data of a connected FIFO queue
