#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_NUM_FLOWS 256 // Variable, it can change
#define NUM_SLOTS 4 //Fit in the cache line

/*******   MACRO          *****/

#define NODE_POINTER(base, i, size)  ( ((flow_node_t*) ((uint8_t*)(base) + i*(size + sizeof(flow_node_t)-1 ))) )
//#define OFF_FORMULA(flowsize) ((sizeof(flow_node_t)-1+ flowsize ))
#define OFFSET(startptr, offptr, flowsize) (( (int)  ( ((uint8_t*)(offptr) - (uint8_t*)(startptr)))/((sizeof(flow_node_t))-1+ (flowsize) )  ))


#define INCREMENT(i, max_value) ( (i+1) % max_value )
#define DECREMENT(i, max_value) (((max_value+i-1)%max_value))


#define LOG(fmt, ...)  fprintf(stderr, "%d:%s(): " fmt, __LINE__, __func__, ## __VA_ARGS__)


/*********************************/
#define EXISTING 1
#define NEWELEM 2
#define REMOVED 3
#define FAILURE (-1)

typedef struct _index_n{
	uint32_t hash;
	uint32_t index;
	uint32_t v_q;
}index_node_t;

typedef struct _index_l{
	uint8_t busy;
	index_node_t slot[NUM_SLOTS];
} index_line_t;

typedef index_line_t index_table_t[MAX_NUM_FLOWS];

typedef struct _flow_t{
	uint16_t flow_pos[2];
	uint8_t notvalid;
	struct _flow_t *next;
	struct _flow_t *previous;
	uint8_t payload[1];
} flow_node_t;

typedef struct _entry_table_t{
	uint16_t first;	//Not needed;
	uint16_t last;
	uint8_t payload[1];
} entry_table_t;

typedef struct _general{
	uint16_t n_elements;
	uint16_t max_elements;
	uint16_t flow_size;
	index_table_t *in;
	entry_table_t *fl;
} flow_table_t;

int create_table(flow_table_t **table, int nentries, int flowsize);
int insert_element(flow_table_t *table, void* element, int size, uint32_t (*hash)(uint8_t*, int)  );
int delete_element(flow_table_t *table, void* element );
flow_node_t *remove_element_at_position(flow_table_t *table, int pos);
flow_node_t *remove_first(flow_table_t *table);
flow_node_t *get_first_ptr(flow_table_t *table);


//int init_table(uint32_t index_table[], flow_table_t *fw, int size, int elems);
//int show_table(flow_table_t *fw, int size);
