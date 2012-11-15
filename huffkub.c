// Todo: 	how to cast pointer to struct and access its element:
// 			eg. if p is pointer to node, then I am using:
//			((leaf *) p)->ch, but there must be a more beautiful way?
//		oneletter_ failing

#include <stdio.h>
#include "struct.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include "huffkub.h"
#include "compress.h"
#include "extract.h"

void parse_args(int argc, char **argv) {
	opts = 0;
	int opt;
	while ((opt = getopt(argc, argv, "cxiof:d:hv")) != -1) {
		switch (opt) {
		case 'c':
			if (opts & UNPACK)
				error(2, 0, "-c and -x are exclusive");
			opts |= PACK;
			break;
		case 'x':
			if (opts & PACK)
				error(2, 0, "-c and -x are exclusive");
			opts |= UNPACK;
			break;
		case 'i':
			if (opts & FILEIN)
				error(2, 0, "-f and -i are exclusive");
			opts |= STDIN;
			break;
		case 'o':
			if (opts & FILEOUT)
				error(2, 0, "-d and -o are exclusive");
			opts |= STDOUT;
			break;
		case 'f':
			if (opts & STDIN)
				error(2, 0, "-f and -i are exclusive");
			opts |= FILEIN;
			strncpy(inputfile, optarg, 128);
			break;
		case 'd':
			if (opts & STDOUT)
				error(2, 0, "-d and -o are exclusive");
			opts |= FILEOUT;
			strncpy(outputfile, optarg, 128);
			break;
		case 'v':
			opts |= VERBOSE;
			break;
		case 'h':
		default:
			printf("Simple file packer using Huffman algorithm.\n\n"
				"Usage:\thuffkub [-c] [-x] [-i] [-o] [-f inputfile] [-d outputfile]\n\n"
				"Argument usage:\n\t -c\t\tPack (exclusive with -x)\n"
				"\t -x\t\tUnpack (exclusive with -c)\n"
				"\t -i\t\tRead input from stdin (exclusive with -f)\n"
				"\t -o\t\tWrite output to stdout (exclusive with -d)\n"
				"\t -f FILE\tRead input from FILE (exclusive with -i)\n"
				"\t -d FILE\tWrite output to FILE (exclusive with -o)\n"
				"\t -v\t\tVerbose output (only when -o is not set)\n"
				"\t -h\t\tPrint this help message\n");
			exit(0);
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Use -h for usage\n");
		exit(0);
	}

	blksize = 1024;
	parse_args(argc, argv);

	if (opts & PACK) {
		compress();
	}
	if (opts & UNPACK) {
		extract();
	}
	if (opts & VERBOSE) {
		print_tree1((node* ) tip);
		verbose_print();
	}
	return 0;
}

void verbose_print() {
	int i = 0;
	fprintf(stderr, "/*\nCHAR\tFREQ\tCODE\n");
	for (i = 0; i < CHAR; i++) {
		if (chararray[i] != NULL) {
			fprintf(stderr, "%c\t%d\t", chararray[i]->ch, chararray[i]->freq);
			print_code(calc_code(chararray[i]));
		}
	}
	fprintf(stderr, "\n*/");
}

void print_tree(node * rr) {
	leaf *l,*r;
	if (rr->type == 2) {
		fprintf(stderr, "\t\"%p\" [shape=record,label=\"%p|%d\"]\n ", rr, rr, rr->freq);
		if ( (((hub *) rr)->left)->type == 1){
			l = ( (leaf *) ((hub *) rr)->left);
			if (sanitize(l->ch))
				fprintf(stderr, "\t\"%p\" [shape=record,label=\"char(%d)|%d\"]\n ", l, (int) l->ch, l->freq);
			else
				fprintf(stderr, "\t\"%p\" [shape=record,label=\"%c|%d\"]\n ", l, l->ch, l->freq);
			fprintf(stderr, "\t\"%p\" -> \"%p\";\n", rr, l);
		} else {
			fprintf(stderr, "\t\"%p\" -> \"%p\";\n", rr, ((hub *) rr)->left);
		}
		print_tree(((hub *) rr)->left);
		if ( (((hub *) rr)->right)->type == 1){
			r = ( (leaf *) ((hub *) rr)->right);
			if (sanitize(r->ch))
				fprintf(stderr, "\t\"%p\" [shape=record,label=\"char(%d)|%d\"]\n ", r, (int) r->ch, r->freq);
			else
				fprintf(stderr, "\t\"%p\" [shape=record,label=\"%c|%d\"]\n ", r, r->ch, r->freq);
			fprintf(stderr, "\t\"%p\" -> \"%p\";\n", rr, r);
		} else {
			fprintf(stderr, "\t\"%p\" -> \"%p\";\n", rr, ((hub *) rr)->right);
		}
		print_tree(((hub *) rr)->right);
	}
}
void print_tree1(node * rr) {
	fprintf(stderr, "digraph bt {\n"
				"\trandkir = LR;\n");
	print_tree(rr);
	fprintf(stderr, "}\n");
}

int sanitize(unsigned char c) {
	switch ((signed char) c){
		case 10:
		case '"':
		case '|':
		case ' ':
		case '<':
		case '>':
		case 0:
			return 1;
			break;
		default:
			return 0;
	}
}
