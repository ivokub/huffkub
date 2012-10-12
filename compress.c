#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "struct.h"
#include "huffkub.h"
#include "compress.h"

int create_leafs(char * blob[]) {
	int i;
	int len = strlen(*blob);
	for (i = 0; i < len; i++) {
		fill_node((unsigned char) (*blob)[i]);
	}
	return i;
}

hub * create_hub(leaf * c1, leaf * c2) {
	hub * thishub;
	thishub = (hub *) malloc(sizeof(hub));
	if (thishub == NULL)
		error(4, 0, "Cannot allocate memory!");
	*thishub = (hub) {.parent = &root, .left = c1, .right = c2,
				.freq = c1->freq + c2->freq};
			return thishub;
		}

node * create_tree(leaf ** leafarray) {
	int i;
	node * tree[CHAR];
	hub * thishub;
	memcpy(tree, leafarray, sizeof(void *) * CHAR);
	int left = sort_leaf((leaf **) tree, CHAR);
	while (tree[1] != NULL) {
		thishub = create_hub((leaf *) tree[0], (leaf *) tree[1]);
		tree[0]->parent = (node *) thishub;
		tree[1]->parent = (node *) thishub;
		tree[0] = NULL;
		tree[1] = NULL;
		int k, j = 1;
		// move only previous nodes and then tree = &tree[1] ?
		while (j + 1 < left && (thishub->freq >= tree[j + 1]->freq))
			j++;
		for (k = 2; k <= j; k++)
			tree[k - 2] = tree[k];
		tree[j - 1] = (node *) thishub;
		for (k = j + 1; k < left; k++)
			tree[k - 1] = tree[k];
		tree[left--] = NULL;
	}
	// should free tree array
	return tree[0];
}

void fill_node(unsigned char c) {
	leaf *thisnode;
	if ((thisnode = chararray[c]) == NULL) {
		thisnode = (leaf *) malloc(sizeof(leaf));
		if (thisnode == NULL)
			error(4, 0, "Cannot allocate memory!");
		thisnode->parent = &root;
		chararray[c] = thisnode;
	}
	thisnode->ch = c;
	thisnode->freq++;
	root.freq++;
}

int node_cmp_ll(const void *c1, const void *c2) {
	if ((*(node **) c1)->freq > (*(node **) c2)->freq)
		return 1;
	else if ((*(node **) c1)->freq < (*(node **) c2)->freq)
		return -1;
	else
		return 0;
}

int clean_leaf_array(leaf ** leafarray, int total) {
	int i, freeleaf = 0;
	for (i = 0; i < total; i++) {
		if (leafarray[i] != NULL) {
			if (freeleaf != i) {
				leafarray[freeleaf] = leafarray[i];
				leafarray[i] = NULL;
				freeleaf++;
			}
		}
	}
	return freeleaf;
}

int sort_leaf(leaf * leafarray[], int length) {
	int unique = 0;
	int total = clean_leaf_array(leafarray, length);
	qsort(leafarray, total, sizeof(leaf *), node_cmp_ll);
	while (leafarray[unique] != NULL) {
		unique++;
	}
	return unique;
}

code * calc_code(leaf * bytechar) {
	hub * parentnode = bytechar->parent;
	int len = 0, ch = 0;
	code * bytecode = malloc(sizeof(code));
	if (bytecode == NULL)
		error(4, 0, "Cannot allocate memory!");
	*bytecode = (code) {.ch = 0, .len = 0};
			if (bytechar->parent->right == bytechar) {
				bytecode->ch += 1;
			}
			while (parentnode->parent != &root) {
				bytecode->ch *= 2;
				if (parentnode->parent->right == parentnode) {
					bytecode->ch += 1;
				}
				bytecode->len++;
				parentnode = parentnode->parent;
			}
			bytecode->len++;
			//bytecode->ch = reverse(bytecode->ch, bytecode->len);
			return bytecode;
		}

int reverse(int bytecode, int len) {
	// Ugly, but should be platform independent
	int reversed = 0, i = 0;
	while (i < len) {
		reversed += bytecode % 2;
		bytecode / 2;
		reversed * 2;
		i++;
	}
	return reversed;
}

void print_code(code * bytecode) {
	int i = 0, bitcode = bytecode->ch;

	while (i < bytecode->len) {
		printf("%d", bitcode % 2);
		bitcode /= 2;
		i++;
	}
	printf("\n");
}

int calc_prob() {
	char * buffer = calloc(blksize, sizeof(char));
	if (buffer == NULL)
		error(4, 0, "Cannot allocate memory!");
	FILE *temp, *in;
	int total = 0;
	if (opts & STDIN) {
		in = stdin;
		temp = tmpfile();
		if (!temp)
			error(3, 0, "Cannot create temporary file!");
	} else {
		in = fopen(inputfile, "r");
		if (!in)
			error(3, 0, "Cannot open input file!");
		temp = in;
	}
	while ((fgets(buffer, blksize, in) != NULL)) {
		if (opts & STDIN)
			fputs(buffer, temp);
		total += create_leafs(&buffer);
	}
	if (ferror(temp))
		error(3, 0, "Couldn't write to temporary file!");
	free(buffer);
	if (opts & STDIN)
		fclose(temp);
	fclose(in);
	hub * tip = (hub *) create_tree(chararray);
	return total;
}

