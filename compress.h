int sort_leaf(leaf *[], int);
int node_cmp_ll(const void *c1, const void *c2);
int clean_leaf_array(leaf *[], int);

int create_leafs(char *[]);
node * create_tree(leaf **);
hub * create_hub(leaf *, leaf *);

int calc_prob();
FILE * temp;
code * calc_code(leaf *);
code * codetable[CHAR];
int reverse(int, int);
void fill_node(unsigned char);

void print_code(code *);

int fbitout(code *, FILE *);
char outbuf;
short int outbuf_len;
char * writebuf;

void append_char(char, FILE *);

char * create_graph(hub *);
void calc_codes();
char pad(FILE *);

void compress();
hub * tip;
