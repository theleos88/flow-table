#include "flow_table.h"

/* Assuming malloc is done out of it */
int create_table(flow_table_t **table, int nentries, int flowsize){
	assert(nentries <= MAX_NUM_FLOWS);

	/* Maybe I can add malloc if null */
	*table = ((flow_table_t*) malloc (sizeof(flow_table_t)));

	(*table)->nentries = nentries;
	(*table)->in = (index_table_t*) malloc(sizeof(index_table_t));
	(*table)->fl = (entry_table_t*) malloc(  sizeof(entry_table_t) +   (sizeof(flow_node_t) + flowsize - 1)*nentries );

	(*table)->fl->first = 0;
	(*table)->fl->last = 0;

	index_line_t *line = *(*table)->in;
	for (int i=0; i<nentries;i++){
		line->busy=0;
	}

	return 1;
}

//TODO: Manage timestamps (maybe out of the lib)
//TODO: optimize the insertion
int insert_element(flow_table_t *table, void* element, int size, uint32_t (*hash)(uint8_t*, int)  ){

	uint32_t h = ((int)( (*hash )( (uint8_t*)(element), size)));
	int index = h % table->nentries;

	index_line_t *line = &((*table->in)[index]);

	if (line->busy == 0){
		line->slot[0].hash = h;
		line->slot[0].time_stamp = 0;
		line->slot[0].index = table->fl->last;
		line->busy = 1;

		table->fl->last = (table->fl->last+1) % table->nentries;
		flow_node_t *flow = ((flow_node_t*) ((uint8_t*)(table->fl->payload) + line->slot[0].index*(size + sizeof(flow_node_t)-1 )));

		flow->flow_hash = h;
		memcpy(flow->payload, element, size);

	} else {
		printf ("Not empty\n");
		for (int i=0; i<line->busy; i++){

			/* Check hash*/
			if (line->slot[i].hash == h){
				/* Check value*/
				printf("Colliding or existing...\n");
				flow_node_t *flow = ((flow_node_t*) ((uint8_t*)(table->fl->payload) + line->slot[i].index*(size + sizeof(flow_node_t)-1 )));

				/* Handle existing case*/
				if( memcmp(flow->payload, element, size ) == 0){
					printf("Existing! --- Update\n");
					return EXISTING;
				} else {
					printf("False positive\n");
					continue;
				}
			}
		}

		line->slot[ line->busy ].hash = h;
		line->slot[ line->busy ].time_stamp = 0;
		line->slot[ line->busy ].index = table->fl->last;

		table->fl->last = (table->fl->last+1) % table->nentries;
		flow_node_t *flow = ((flow_node_t*) ((uint8_t*)(table->fl->payload) + line->slot[line->busy].index*(size + sizeof(flow_node_t)-1 )));
		line->busy = (line->busy+1)%NUM_SLOTS;

		flow->flow_hash = h;
		memcpy(flow->payload, element, size);
		printf("New\n");

		return NEWELEM;
	}

	/* Handle not existing case*/

	return FAILURE;
}


/*
int init_table(uint32_t index_table[], flow_table_t *fw, int size, int elems){
	assert(elems <= MAX_NUM_FLOWS);

	int i=0;
	for (i=0;i<elems;i++)
	{
		index_table[i]=0;
	}

	fw = malloc(elems*(sizeof(flow_table_t) + size));

	return 1;
}

int show_table(flow_table_t *fw, int size){
	for (int i=0; i<MAX_NUM_FLOWS;i++){
		for (int j=0; j<size; j++){
			if(j%16 == 0){
				printf("\n");
			}
			printf("%02x",fw[i].payload[j]);
		}
		printf("\n\n");
	}
	printf("*****\n");
}
*/