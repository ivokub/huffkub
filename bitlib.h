#ifndef BITLIB_H_
#define BITLIB_H_
	char readbit();
	int setbitread(FILE *);
	int writebit(char);
	int write_char_bit(unsigned char);
	void setbitwrite(FILE *);
	char * bitbuffer;
	char * bitbuffer2;
	int eof_reached;
	char bitindex;
	FILE * fbitstream;
	char padsize;
#endif /* BITLIB_H_ */
