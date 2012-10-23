#include <stdio.h>
#include <stdlib.h>

#ifndef BITLIB_H_
#define BITLIB_H_
	char readbit();
	int setbitread(FILE *);
	int writebit(char);
	int write_char_bit(unsigned char);
	void setbitwrite(FILE *);
	char * bitbuffer;
	char bitindex;
	FILE * fbitstream;
#endif /* BITLIB_H_ */
