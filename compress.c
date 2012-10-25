#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "struct.h"
#include "huffkub.h"
#include "compress.h"
#include "bitlib.h"

hub * create_hub(leaf * c1, leaf * c2) {
	hub * thishub;
	thishub = (hub *) malloc(sizeof(hub));
	if (thishub == NULL)
		error(4, 0, "Cannot allocate memory!");
	*thishub = (hub) {.type = 2, .parent = &root, .left = (node *) c1, .right = (node *) c2,
				.freq = c1->freq + c2->freq};
	return thishub;
}

hub * create_tree(leaf ** leafarray) {
	int i;
	int full = 0;
	node ** tree = calloc(CHAR, sizeof(node *));
	hub * thishub;
	memcpy(tree, leafarray, sizeof(void *) * CHAR);
	qsort(tree, CHAR, sizeof(node *), node_cmp_ll);
	while (tree[1] != NULL) {
		thishub = create_hub((leaf *) tree[0], (leaf *) tree[1]);
		tree[0]->parent = (node *) thishub;
		tree[1]->parent = (node *) thishub;
		tree[0] = NULL;
		tree[1] = (node *) thishub;
		qsort(tree, CHAR, sizeof(node *), node_cmp_ll);
	}
	// should free tree array
	return (hub *) tree[0];
}

void fill_node(unsigned char c) {
	leaf *thisnode;
	if ((thisnode = chararray[c]) == NULL) {
		thisnode = (leaf *) malloc(sizeof(leaf));
		if (thisnode == NULL)
			error(4, 0, "Cannot allocate memory!");
		thisnode->type = 1;
                thisnode->parent = &root;
		chararray[c] = thisnode;
		number_nodes++;
	}
	thisnode->ch = c;
	thisnode->freq++;
	root.freq++;
}

int node_cmp_ll(const void *c1, const void *c2) {
	if (*(void **) c1 == NULL) return 1;
	if (*(void **) c2 == NULL) return -1;
	if ((*(node **) c1)->freq > (*(node **) c2)->freq)
		return 1;
	else if ((*(node **) c1)->freq < (*(node **) c2)->freq)
		return -1;
	else
		return 0;
}

code * calc_code(leaf * bytechar) {
	hub * parentnode = bytechar->parent;
	int len = 0, ch = 0;
	code * bytecode = malloc(sizeof(code));
	if (bytecode == NULL)
		error(4, 0, "Cannot allocate memory!");
	*bytecode = (code) {.ch = 0, .len = 0};
	if ((leaf *) bytechar->parent->right == bytechar) {
		bytecode->ch += 1;
	}
	while (parentnode->parent != &root) {
		bytecode->ch *= 2;
		if ((hub *) parentnode->parent->right == parentnode) {
			bytecode->ch += 1;
		}
		bytecode->len++;
		parentnode = parentnode->parent;
	}
	bytecode->len++;
	codetable[bytechar->ch] = bytecode;
	outbuf_len = (outbuf_len + bytecode->len*bytechar->freq) % 8;
	return bytecode;
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

int fbitout(code * letter, FILE * fout){
	int i = 0;
	int tempch = letter->ch;
	while(i++ < letter->len){
		writebit((char) tempch & 0x1);
		tempch >>= 1;
	}
	return 0;
}

int calc_prob() {
	// Streams opening
	char * buffer = calloc(blksize, sizeof(char));
	unsigned char c;
	if (!buffer)
		error(4, 0, "Cannot allocate memory!");
	FILE *in;
	int total = 0;
	if (opts & FILEIN) {
		in = fopen(inputfile, "r");
		if (!in)
			error(3, 0, "Cannot open input file!");
		temp = in;
	} else {
		in = stdin;
		temp = tmpfile();
		if (!temp)
			error(3, 0, "Cannot create temporary file!");
	}
	// Frequency counting and leaf making
	if (test_empty(in)) return 0;
	while ((c = (unsigned char) fgetc(in)) | 1){
		if (feof(in)) break;
		fill_node(c);
		total++;
	}
	if (ferror(temp))
		error(3, 0, "Couldn't write to temporary file!");
	free(buffer);
	fclose(in);
	// Tree growth
	tip = create_tree(chararray);
	return total;
}

int test_empty(FILE * fin){
	if (fgetc(fin) == EOF) return 1;
	else {
		rewind(fin);
		return 0;
	}

}

void calc_codes() {
	int i = 0;
	leaf * currentleaf;
	for (i = 0; i < CHAR; i++) {
		if ((currentleaf = chararray[i]) != NULL) {
			calc_code(currentleaf);
		}
	}
}

void write_meta(FILE * fout){
	if ((opts & VERBOSE) && (opts & FILEOUT)) printf("0");
	if (writebit(0)) error(3, 0, "Write error!");
	int i;
	for (i=0; i<3; i++) writebit(0);
	recursive_write(tip);
	if ((opts & VERBOSE) && (opts & FILEOUT)) printf("\n");
}

void recursive_write(hub * parent){
	if (parent->left->type == 1){
		if ((opts & VERBOSE) && (opts & FILEOUT)) printf("1%c", (signed char) ((leaf *) parent->left)->ch);
		if (writebit(1) || write_char_bit(((leaf *) parent->left)->ch)) error(3, 0, "Write error!");
	} else {
		if ((opts & VERBOSE) && (opts & FILEOUT)) printf("0");
		writebit(0);
		recursive_write((hub *) parent->left);
	}
	if (parent->right->type == 1){
		if ((opts & VERBOSE) && (opts & FILEOUT)) printf("1%c", (signed char) ((leaf *) parent->right)->ch);
		if (writebit(1) || write_char_bit(((leaf *) parent->right)->ch)) error(3, 0, "Write error!");
	} else {
		if ((opts & VERBOSE) && (opts & FILEOUT)) printf("0");
		writebit(0);
		recursive_write((hub *) parent->right);
	}
}

void compress() {
	int nonempty = calc_prob();
	calc_codes();
	outbuf = 0;
	writebuf = calloc(blksize, sizeof(char));
	int i, j;
	FILE * in, *out;
	char * buffer = calloc(blksize, sizeof(char));
	unsigned char c;
	if (!buffer) error(4, 0, "Cannot allocate memory!");
	if (!writebuf)	error(4, 0, "Cannot allocate memory!");
	if (opts & FILEIN) {
		in = fopen(inputfile, "r");
		if (!in) error(3, 0, "Cannot open input file!");
	} else {
		in = temp;
		rewind(in);
	}
	if (opts & FILEOUT) {
		out = fopen(outputfile, "w+");
		if (!out) error(3, 0, "Cannot open output file!");
		if (!nonempty) goto fclose;
	} else
		out = stdout;
	setbitwrite(out);
	write_meta(out);
	while (1 | (c = (unsigned char) fgetc(in))){
		if (feof(in)) break;
		fbitout(codetable[ c], out);
	}
	pad(out);
	writepadsize(out);
fclose:
	fclose(in);
	fclose(out);
}

