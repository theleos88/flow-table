#include "flow_table.h"

/* Assuming malloc is done out of it */
int create_table(flow_table_t **table, int nentries, int flowsize){
	assert(nentries <= MAX_NUM_FLOWS);

	/* Maybe I can add malloc if null */
	*table = ((flow_table_t*) malloc (sizeof(flow_table_t)));

	(*table)->n_elements = 0;
	(*table)->max_elements = nentries;
	(*table)->flow_size = flowsize;
	(*table)->in = (index_table_t*) malloc(sizeof(index_table_t));
	(*table)->fl = (entry_table_t*) malloc(  sizeof(entry_table_t) +   (sizeof(flow_node_t) + flowsize - 1)*nentries );

	(*table)->fl->first = 0;
	(*table)->fl->last = 0;

	index_line_t *line = *(*table)->in;
	for (int i=0; i<nentries;i++){
		line->busy=0;
		flow_node_t *n = NODE_POINTER((*table)->fl->payload, i, flowsize );
		n->next = NODE_POINTER((*table)->fl->payload, INCREMENT(i, nentries) , flowsize );
		n->previous = NODE_POINTER((*table)->fl->payload, DECREMENT(i, nentries) , flowsize );
		n->notvalid = 0;
		n->flow_hash = 0;
	}

	return 1;
}

//TODO: Manage timestamps (maybe out of the lib)
//TODO, BUG: Check the line->busy thing. It's buggy

int insert_element(flow_table_t *table, void* element, int size, uint32_t (*hash)(uint8_t*, int)  ){

	uint32_t h = ((int)( (*hash )( (uint8_t*)(element), size)));
	char choice;
	int index = h % table->max_elements;

	index_line_t *line = &((*table->in)[index]);
	flow_node_t *flow;

	if (line->busy == 0){
		choice = 'I';
	} else {
		//printf ("Not empty\n");

		for (int i=0; i<line->busy; i++){

			/* Check hash*/
			if (line->slot[i].hash == h){
				/* Check value*/
				//printf("Colliding or existing...\n");				
				flow = NODE_POINTER(table->fl->payload, line->slot[i].index, size );

				/* Handle existing case*/
				if( memcmp(flow->payload, element, size ) == 0){
					choice = 'U';
					//printf("Existing! --- Update\n");
					break;
				} else {
					//printf("False positive\n");
					continue;
				}
			}
		}
	}

	switch( choice ){
		case 'U' :
		{

			/* Update case */
			flow_node_t *last = NODE_POINTER(table->fl->payload, table->fl->last, size);
			flow_node_t *first = last->previous;

			if (flow == first){
				//printf("Updating first element\n");
				return 1;
			}

			if ( OFFSET(table->fl->payload, flow, table->flow_size) == table->fl->last){
				//printf("Moving last...\n");
				table->fl->last = OFFSET(table->fl->payload, flow->next, table->flow_size);
			} else {
				//printf("Last: %p, First: %p, Current: %p\n", last, first, flow);
				flow->previous->next = flow->next;
				flow->next->previous = flow->previous;

				flow->next = last;
				flow->previous = first;

				first->next = flow;
				last->previous = flow;
			}
			flow->notvalid = 0;

			break;
		}

		default :
		{

			/* The default case is insert */
			line->slot[ line->busy ].hash = h;
			line->slot[ line->busy ].v_q = 0;
			line->slot[ line->busy ].index = table->fl->last;

			//printf("Put element at %d, last %d\n", line->busy, table->fl->last);
			flow_node_t *flow = NODE_POINTER( table->fl->payload, line->slot[ line->busy ].index, size );
			flow->flow_hash = h;
			flow->notvalid = 0;
			memcpy(flow->payload, element, size);

			line->busy = INCREMENT(line->busy, NUM_SLOTS);
			table->fl->last = OFFSET( table->fl->payload, flow->next, table->flow_size);

			if (table->n_elements != table->max_elements){
				table->n_elements++;
			}

			break;
		}

	}

	return 0;
}


flow_node_t *remove_first(flow_table_t *table){
		flow_node_t *last = NODE_POINTER(table->fl->payload, table->fl->last, table->flow_size);

		if (table->n_elements > 0){

			while(last->previous->notvalid){
				last = last->previous;
			}	
			table->fl->last = OFFSET(table->fl->payload, last->previous, table->flow_size);
			table->n_elements--;
			return last->previous;
		} else {
			//Empty flow table
			return NULL;
		}
}

int delete_element(flow_table_t *table, void* element ){
	if(table->n_elements == 0){
		return 0;
	}

	//int node_offset = OFFSET(table->fl->payload, (uint8_t*)element, table->flow_size);
	flow_node_t *last = NODE_POINTER(table->fl->payload, table->fl->last, table->flow_size);
	flow_node_t *first = last->previous;
	flow_node_t *remove = (flow_node_t*)element;

	if (last == remove || first == remove){
		//remove->notvalid=1;
		table->fl->last = OFFSET(table->fl->payload, remove, table->flow_size);
	} else {
		remove->notvalid=1;
	}
	//flow_node_t *remove = NODE_POINTER(table->fl->payload, table->fl->last, table->flow_size);
}

flow_node_t *remove_element_at_position(flow_table_t *table, int pos ){
	if(table->n_elements == 0 || pos > table->max_elements){
		return NULL;
	}

	if (table->fl->last == pos ){
		return NULL;
	} else {
		flow_node_t *node = NODE_POINTER(table->fl->payload, pos, table->flow_size);
		flow_node_t *last = NODE_POINTER(table->fl->payload, table->fl->last, table->flow_size);

		if (node->notvalid == 0){

			if (last->previous == node){				
				table->fl->last = OFFSET(table->fl->payload, last->previous, table->flow_size);				
			}

			node->notvalid=1;
			table->n_elements --;			
			return node;
		} else {
			return NULL;
		}
	}
}
