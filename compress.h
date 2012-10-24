int sort_leaf(leaf *[], int);					// Sort leafs using qsort
int node_cmp_ll(const void *c1, const void *c2);// Leaf compare function (using probabilities)
int clean_leaf_array(leaf *[], int);			// Sort out null pointers (so qsort would be faster)
int create_leafs(char *[]);						// From string to leafs
hub * create_tree(leaf **);					// Calculate tree using Huffmann algorithm
hub * create_hub(leaf *, leaf *);				// Crete parent hub for two leafs
int calc_prob();								// Open streams, calculate trees
void fill_node(unsigned char);					// Character frequency counter and leaf creater
void print_code(code *);						// Code printing for verbose
int fbitout(code *, FILE *);					// Bit-by-bit output writing
void append_char(char, FILE *);					// Writebuffer appender and streamout writer
void calc_codes();								// Calculate all coding codes at once
//char pad(FILE *);								// Write padding (to achieve full byte)
void compress();								// Put everything together
void recursive_write(hub *);


#ifndef COMPRESS_VAR
	#define COMPRESS_VAR
	hub * tip;

	char outbuf;
	short int outbuf_len;
	char * writebuf;
	FILE * temp;
	code * calc_code(leaf *);
	code * codetable[CHAR];
#endif
