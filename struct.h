#define CHAR 256

/* 
 *   Leaf and hub.
 *
 *   Leafs are meant for characters and hubs
 *   are for junctions
 *
 */

#ifndef STRUCT_H_
#define STRUCT_H_

	/*
	 * Following structs are for representing different types of possible nodes:
	 * 		leaf - character, endpoint;
	 * 		hub - has two children;
	 * 		node - generic node (dont know if hub of leaf).
	 * Types for previous in same order: 1, 2 (assigned at creation time, we
	 * dont create actual node type variables, just use it in and crawls!)
	 * We want to keep structure the same for joint parts (this means that do not change
	 * the order of items in structures!)
	 */

	typedef struct nodeleaf {
		char type;
		struct nodehub *parent;
		int freq;
		unsigned char ch;
	} leaf;

	typedef struct nodehub {
		char type;
		struct nodehub *parent;
		int freq;
		struct node *left; // as childs can be either leafs or hubs, use generic pointer
		struct node *right;
	} hub;

	typedef struct node {
		char type;
		struct node *parent;
		int freq;
	} node;

	// End of structures concerned

	// Just to ease passing parameters to functions
	typedef struct code {
		int len;
		int ch;
	} code;

	typedef struct compresscode {
		short int code;
		char len;
		char ch;
	} compresscode;

	// Root node
	hub root;

	// Lookup array for nodes
	leaf *chararray[CHAR];
#endif	// STRUCT_H_
