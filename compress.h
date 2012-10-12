int sort_leaf(leaf * [], int);
int node_cmp_ll(const void *c1, const void *c2);
int clean_leaf_array(leaf * [], int);

int create_leafs(char * []);
node * create_tree(leaf **);
hub * create_hub(leaf *, leaf *);

int calc_prob();
code * calc_code(leaf *);
int reverse(int , int);
void fill_node(unsigned char);

void print_code(code *);
