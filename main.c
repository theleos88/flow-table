#include <stdio.h>
#include "hdr/flow_table.h"

typedef struct n{
	uint32_t ipsrc;
	uint32_t ipdst;
	uint32_t prtsrc;
	uint32_t prtdst;
	uint32_t type;
} flow_t;

uint32_t hash(uint8_t *elem, int size){
	uint32_t h = 0;
	while(size>0){
		h = (h ^ *elem) + ((h<<26)+(h>>6));
		size--;
	}
	return ( h>>12 )/4;
}

int print_structure(flow_t s){
	printf("|IPSRC: %d - IPDST: %d - PRTSRC-DST: %d/%d - PROTO: %d|\n", s.ipsrc, s.ipdst, s.prtsrc, s.prtdst, s.type);
}

int print_test_data(flow_table_t *t){
	int index = 6;

	index_line_t *line = &((*t->in)[index]);
	printf("Busy: %d\n", line->busy);
	for (int i=0; i<line->busy; i++){
		printf("Hash: %08x\n", line->slot[i].hash);
		printf("Index: %d\n", line->slot[i].index);
		printf("TS: %08x\n", line->slot[i].time_stamp);

		int ref = line->slot[i].index;

		flow_node_t *flow = ((flow_node_t*) ((uint8_t*)(t->fl->payload) + line->slot[i].index*(sizeof(flow_t) + sizeof(flow_node_t)-1 )));
		flow_t myflow = *((flow_t*)(flow->payload));
		print_structure(myflow);
	}

}

int print_table(flow_table_t *table){
	printf("ELEMENTS: %d\n", table->n_elements);
	printf("___\n");
	flow_node_t *node = NODE_POINTER(table->fl->payload, table->fl->last, sizeof(flow_t) );
	node = node->previous;

	for (int i=0; i<table->n_elements; i++){
		flow_t t = *(flow_t*)(node->payload);
		print_structure(t);
		node = node->previous;
	}

}

int main(int argc, char** argv){

	flow_t f1;
	f1.ipsrc=1;
	f1.ipdst=1;
	f1.type=1;

	flow_t f2 = f1;
	f2.ipsrc=55;

	flow_t f3 = f1;
	f3.ipsrc = 66;
	f3.prtdst = 125;


	index_table_t mytable;

	flow_table_t *flowtable;
	create_table(&flowtable, 10, sizeof(flow_t));

	printf("Ipsrc: %d - Index: %d\n",f1.ipsrc, ( hash( (uint8_t*)&f1, sizeof(f1))  ) % flowtable->max_elements);
	printf("Ipsrc: %d - Index: %d\n",f2.ipsrc, ( hash( (uint8_t*)&f2, sizeof(f2))  ) % flowtable->max_elements);
	printf("Ipsrc: %d - Index: %d\n",f3.ipsrc, ( hash( (uint8_t*)&f3, sizeof(f3))  ) % flowtable->max_elements);

/*
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		

*/

	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f1, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f2, sizeof(f1), &hash) );		
	printf("HASH insert: %08x\n", insert_element (flowtable, (void*)&f3, sizeof(f1), &hash) );		

	print_test_data(flowtable);
	printf("\n\n\n\n\n");
	print_table(flowtable);

	/*
	index_node_t *t =&mytable[10][2];
	t->hash=1;
	t->index=2;
	t->time_stamp=3;

	index_node_t *pnt = mytable[10];
	for (int i=0;i<NUM_SLOTS;i++){
		index_node_t data= *(pnt+i);
		printf("Hash %d ", data.hash);
		printf("Index %d ", data.index);
		printf("TIME_STAMP %lu ", data.time_stamp);
		printf("\n****\n");
	}



	f2.ipsrc=5;
	f2.ipdst=5;
	f2.type=5;

	//printf("HASH F1: %u\n", hash((uint8_t*)(&f1), sizeof(f1)) );
	//printf("HASH F2: %u\n", hash((uint8_t*)(&f2), sizeof(f2)) );

	for (int i=0;i<10;i++){
			printf("HASH insert: %08x\n", insert_element (&flowtable, (void*)&i, sizeof(i), &hash) );		
	}

*/

	//flow_table_t *mytable;
	//uint32_t myindex[NEL];

	//init_table(myindex, mytable, sizeof(flow_t), NEL);
	//show_table(mytable, sizeof(flow_t));
}
