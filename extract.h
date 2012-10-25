void extract();
void read_meta();
void sort_compresstable(compresscode *, int);
int cmp_compresscode(const void *, const void *);
void replace(FILE *, FILE *, compresscode *);
void replace_bits(FILE *, char *, compresscode *);
void reverse(compresscode *, int);
void recursive_tree(hub *, int);
void rep_bits(FILE *, FILE *, compresscode *);
#ifndef EXTRACT_H_
#define EXTRACT_H_
	int maxlen;
	int minlen;
	int searchpointer;
	int replacepointer;
	int inputend;
	char * replacebuffer;
	compresscode * ctable;
	hub * tip2;
	int filesize, readbytes;
	char arraylen;
#endif	//EXTRACT_H_
