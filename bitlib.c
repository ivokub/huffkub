#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitlib.h"

int setbitread(FILE * fin){
	bitbuffer = calloc(8, sizeof(char));
	bitbuffer2 = calloc(8, sizeof(char));
	char c;
	int i;
	eof_reached = 0;
	fbitstream = fin;
	c = fgetc(fbitstream);
	if (feof(fbitstream)) return 1;
	for (i=0; i<8; i++){
		bitbuffer[i] = (c & (0x1 << i)) >> i;
	}
	c = fgetc(fbitstream);
	if (feof(fbitstream)) return 1;
	for (i=0; i<8; i++){
		bitbuffer2[i] = (c & (0x1 << i)) >> i;
	}
	bitindex = 0;
	for (i=0; i<3; i++){
		padsize <<= 1;
		padsize |= readbit();
	}
	return 0;
}

char readbit(){
	char c;
	int i;
	if (bitindex == 8 && !eof_reached){
		c = fgetc(fbitstream);
		if (feof(fbitstream)) {
			eof_reached = 1;
		}
		memcpy(bitbuffer, bitbuffer2, 8*sizeof(char));
		if (!eof_reached){
			for (i=0; i<8; i++){
				bitbuffer2[i] = (c & (0x1 << i)) >> i;
				}
		}
		bitindex = 0;
	}
	if (eof_reached && padsize+bitindex == 8) return -1;
	return bitbuffer[7-bitindex++];

}

void setbitwrite(FILE * fout){
	bitbuffer = malloc(sizeof(char));
	fbitstream = fout;
	int i;
	*bitbuffer = 0;
	bitindex = 0;
}

int writebit(char c){
	int ret;
	*bitbuffer |= (0x1&c)<<(7-bitindex++);
	if (bitindex == 8){
		if ((ret = fputc(*bitbuffer, fbitstream)) == EOF) return 1;
		*bitbuffer = 0;
		bitindex = 0;
	}
	return 0;
}

int write_char_bit(unsigned char c){
	int i;
	for (i=0; i<8; i++){
		if (writebit(c & 0x1)) return 1;
		c >>= 1;
	}
	return 0;
}

char pad(FILE * fout){
	int ret;
	if (bitindex)
	if ((ret = fputc(*bitbuffer, fbitstream)) == EOF) return 1;
	*bitbuffer = 0;
	padsize = 8-bitindex;
	bitindex = 0;
	return 0;
}



