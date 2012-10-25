#define PACK 0x1
#define UNPACK 0x2
#define STDIN 0x4
#define STDOUT 0x8
#define FILEIN 0x10
#define FILEOUT 0x20
#define VERBOSE 0x40

#ifndef VARIABLES
#define VARIABLES
int blksize;
short int opts;
char inputfile[32], outputfile[32];
#endif

void parse_args(int, char **);
int main(int, char **);
void verbose_print();
