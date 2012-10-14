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
		// move only previous nodes one entry forward and then tree = &tree[1] ?
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

int fbitout(code * letter, FILE * fout) {
	int i = 0;
	int tempch = letter->ch;
	while (i++ < letter->len) {
	// Bit shifting may differ on systems but I am looking for speed
	// One idea is to shift exactly enough bits to fill the byte buffer
		outbuf <<= 1;
		outbuf += tempch & 0x1;
		tempch >>= 1;
		outbuf_len++;
		if (outbuf_len == 8 * sizeof(char)) {
			append_char(outbuf, fout);
			outbuf_len = 0;
		}
	}
}

void append_char(char ch, FILE * fout) {
	int i = 0;
	int j = 0;
	while (writebuf[i] != 0)   // if we save index then we don't have to look for null pointer and fill the array with nulls
		i++;
	if (i == blksize) {
		fwrite(writebuf, sizeof(char), blksize, fout);
		for (j = 0; j < blksize; writebuf[j++] = 0);
		i = 0;
	}
	writebuf[i] = ch;
}

char pad(FILE * fout) {
	int i;
	while (outbuf_len % 8) {
		outbuf *= 2;
		outbuf_len++;
	}
	append_char(outbuf, fout);
	for (i = 0; writebuf[i] != 0; i++)
		;
	fwrite(writebuf, sizeof(char), i, fout);
}

int calc_prob() {
	// Streams opening
	char * buffer = calloc(blksize, sizeof(char));
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
	while (fgets(buffer, blksize, in)) {
		if (opts & STDIN || ~opts & FILEIN)
			fputs(buffer, temp);
		total += create_leafs(&buffer);
	}
	if (ferror(temp))
		error(3, 0, "Couldn't write to temporary file!");
	free(buffer);
	fclose(in);
	// Tree growth
	tip = (hub *) create_tree(chararray);
	return total;
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

compresscode * create_codetable(){
	int i, j = 0;
	compresscode this;
	compresscode *table = calloc(CHAR+1, sizeof(compresscode));
	for (i=0; i<CHAR; i++){
		if (codetable[i]!=NULL){
			this = (compresscode) {.code = codetable[i]->ch, .len = codetable[i]->len, .ch = i};
			table[j++] = this;
		}
	}
	table[CHAR+1].code = j;
	return table;
}

void write_meta(FILE * fout){
/* *
 *  Header file should be:
 *  1 byte padding size (max pad size = 8)
 *  2 bytes for describing graph size
 *  n bytes for graph (array of n chars) // This should be possible with bits also?!
 *  m bytes for characters order (array of m chars)
 *  -------
 *  TOTAL about 2x
 *  REST
*/


	int padsize = outbuf_len;
	outbuf_len = 0;
	fputc(8-padsize, fout);
	compresscode * table = create_codetable();
	fputc(table[CHAR+1].code, fout);
	fwrite(table, sizeof(compresscode), table[CHAR+1].code, fout);

}

void compress() {
	calc_prob();
	calc_codes();
	outbuf = 0;
	writebuf = calloc(blksize, sizeof(char));
	int i, j;
	FILE * in, *out;
	char * buffer = calloc(blksize, sizeof(char));
	if (!buffer)
		error(4, 0, "Cannot allocate memory!");
	if (!writebuf)
		error(4, 0, "Cannot allocate memory!");
	if (opts & FILEIN) {
		in = fopen(inputfile, "r");
		if (!in)
			error(3, 0, "Cannot open input file!");
	} else {
		in = temp;
		rewind(in);
	}
	if (opts & FILEOUT) {
		out = fopen(outputfile, "w");
		if (!out)
			error(3, 0, "Cannot open output file!");
	} else
		out = stdout;
	write_meta(out);
	while (fgets(buffer, blksize, in)) {
		for (j = 0; buffer[j]; j++) {
			fbitout(codetable[(unsigned char) buffer[j]], out);
		}
	}
	pad(out);
	fclose(in);
	fclose(out);
}

