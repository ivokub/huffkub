// Todo: 	how to cast pointer to struct and access its element:
// 			eg. if p is pointer to node, then I am using:
//			((leaf *) p)->ch, but there must be a more beautiful way?
//		oneletter_ failing
//		packing output to stdout failing

#include <stdio.h>
#include "struct.h"
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <error.h>
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
	if ((opts & VERBOSE) && (opts & FILEOUT))
		verbose_print();
	return 0;
}

void verbose_print() {
	int i = 0;
	printf("CHAR\tFREQ\tCODE\n");
	for (i = 0; i < CHAR; i++) {
		if (chararray[i] != NULL) {
			printf("%c\t%d\t", chararray[i]->ch, chararray[i]->freq);
			print_code(calc_code(chararray[i]));
		}
	}
}
