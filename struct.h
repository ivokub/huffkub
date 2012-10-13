#define CHAR 256

/* 
 *   Leaf and hub.
 *
 *   Leafs are meant for characters and hubs
 *   are for junctions
 *
 */

#ifndef STRUCT_VAR
#define STRUCT_VAR
typedef struct nodeleaf {
	struct nodehub *parent;
	int freq;
	unsigned char ch;
} leaf;

typedef struct nodehub {
	struct nodehub *parent;
	int freq;
	void *left; // as childs can be either leafs or hubs, use generic pointer
	void *right;
} hub;

typedef struct node {
	struct node *parent;
	int freq;
} node;

typedef struct code {
	int len;
	int ch;
} code;

// Root node
hub root;

// Lookup array for nodes
leaf *chararray[CHAR];
#endif
