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
	return ( h>>12 )+1;
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
		LOG("NODE: %p *** ", node);
		print_structure(t);
		node = node->previous;
	}
	printf("____\n\n");

}

int check_first(flow_table_t *table, flow_t *expected){
	flow_node_t *node = NODE_POINTER(table->fl->payload, table->fl->last, sizeof(flow_t) );
    node = node->previous;

	flow_t *t = (flow_t*)(node->payload);
	assert ( memcmp(t, expected, sizeof(flow_t)) == 0 );

}

int print_graph_list(flow_table_t *table){
	for (int i=0; i< table->max_elements ;i++){
		flow_node_t *n = NODE_POINTER(table->fl->payload, i, sizeof(flow_t) );
		printf("** CUR: %p\n", n);
		printf("** NEX: %p\n", n->next);
		printf("** PRE: %p\n", n->previous);
		printf("___\n");		
	}
	printf("\n");
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

	LOG("SINGLE: %ld\n", sizeof(flow_node_t)-1+ sizeof(flow_t));
			

	print_graph_list(flowtable);
	for (int i=0; i<MAX_NUM_FLOWS - 1 ; i++) {
		printf("Step %d\n", i);
		f1.ipsrc=i*100+1;
		f1.ipdst=i*200+2;
		f1.type=(i)%11;

		uint32_t h = hash(&f1, sizeof(f1))%flowtable->max_elements;
		printf("Insert at %u\n", h);
		insert_element(flowtable, &f1, sizeof(flow_t), &hash);
		assert( flowtable->n_elements == (i+1) || flowtable->n_elements == flowtable->max_elements );
		print_table(flowtable);

	}

}
