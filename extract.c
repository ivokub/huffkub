#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#include "extract.h"
#include "huffkub.h"
#include "bitlib.h"
#include "compress.h"

void extract(){
	FILE * in, * out;
	int i;
	if (opts & FILEIN) {
		in = fopen(inputfile, "r");
		if (!in)
			error(3, 0, "Cannot open input file!");
	} else {
		in = stdin;
	}
	if (opts & FILEOUT) {
		out = fopen(outputfile, "w");
		if (!out)
			error(3, 0, "Cannot open output file!");
	} else
		out = stdout;
	if (setbitread(in) == 1)
		goto close;
	read_meta(in);
	searchpointer = 0;

	rep_bits(in, out, ctable);
close:
	fclose(in);
	fclose(out);
}

void reverse(compresscode * ctable, int arraylen){
	int i, j;
	short int code = 0;
	for (i = 0; i < arraylen; i++){
		for (j = 0; j < ctable[i].len; j++){
			code <<= 1;
			code += ctable[i].code & 0x1;
			ctable[i].code >>= 1;
		}
		ctable[i].code = code;
		code = 0;
	}
}

void read_meta(FILE * fin){
	char c;
	recursive_tree((hub *) NULL, -1);
}

void recursive_tree(hub * parent, int direction){
	unsigned char c = readbit();
	leaf * thisleaf;
	hub * thishub;
	int i;
	if (c == -1) error(3, 0, "Read error!");
	switch (direction){
		case -1:
			tip2 = malloc(sizeof(hub));
			tip2->parent = &root;
			recursive_tree(tip2, 0);
			recursive_tree(tip2, 1);
			break;
		case 0:
			if (c == 1){
				thisleaf = malloc(sizeof(leaf));
				thisleaf->parent = parent;
				thisleaf->type = 1;
				parent->left = (node *) thisleaf;
				thisleaf->ch = 0;
				for (i=0; i<8; i++){
					if ((c = readbit()) == -1) error(3, 0, "Read error!");
					thisleaf->ch >>= 1;
					thisleaf->ch |= c<<7;
				}
				if ((opts & VERBOSE) && (opts & FILEOUT)) {
					printf("%c\t", thisleaf->ch);
					print_code(calc_code(thisleaf));
				}
			} else {
				thishub = malloc(sizeof(hub));
				thishub->parent = parent;
				parent->left = (node *) thishub;
				thishub->type = 2;
				recursive_tree(thishub, 0);
				recursive_tree(thishub, 1);
			}
			break;
		case 1:
			if (c == 1){
				thisleaf = malloc(sizeof(leaf));
				thisleaf->parent = parent;
				thisleaf->type = 1;
				parent->right = (node *) thisleaf;
				thisleaf->ch = 0;
				for (i=0; i<8; i++){
					if ((c = readbit()) == -1) error(3, 0, "Read error!");
					thisleaf->ch >>= 1;
					thisleaf->ch |= c<<7;
				}
				if ((opts & VERBOSE) && (opts & FILEOUT)) {
					printf("%c\t", thisleaf->ch);
					print_code(calc_code(thisleaf));
				}

			} else {
				thishub = malloc(sizeof(hub));
				thishub->type = 2;
				thishub->parent = parent;
				parent->right = (node *) thishub;
				recursive_tree(thishub, 0);
				recursive_tree(thishub, 1);
			}
			break;
		default:
			break;
	}

}

void sort_compresstable(compresscode ctable[], int len){
	qsort(ctable, len, sizeof(compresscode), cmp_compresscode);
	maxlen = ctable[len-1].len;
	minlen = ctable[0].len;
}

int cmp_compresscode(const void * c1, const void * c2){
	if (((compresscode *) c1)->len > ((compresscode *) c2)->len) return 1;
	else if (((compresscode *) c1)-> len < ((compresscode *) c2)->len) return -1;
	else return 0;
}
char find_char(compresscode * ctable, int code, int len){
	int i = searchpointer;
	if (len>=minlen){
		while (ctable[i].len<len) i++;
		for (; ctable[i].len == len; i++){
			if (ctable[i].code == code) return ctable[i].ch;
		}
	}
	return 0;
}

void rep_bits(FILE * fin, FILE * fout, compresscode * ctable){
	int ch = 0;
	int k=0;
	char c;
	char bit;
	node * currentlocation = (node *) tip2;
	while (1){
		if ((bit = readbit()) == -2) error(3, 0, "Read error!");
		if (bit == -1) break;
		if (bit){
			currentlocation = ((hub *) currentlocation)->right;
		} else {
			currentlocation = ((hub *) currentlocation)->left;
		}
		if (currentlocation->type == 1){
			fputc((signed char) ((leaf *) currentlocation)->ch, fout);
			currentlocation = (node *) tip2;
		}
	}
}
