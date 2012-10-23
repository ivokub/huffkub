/*
 * bitlib.c
 *
 *  Created on: Oct 22, 2012
 *      Author: ivo
 */
#include "bitlib.h"

int setbitread(FILE * fin){
	bitbuffer = calloc(8, sizeof(char));
	char c;
	int i;
	fbitstream = fin;
	c = fgetc(fbitstream);
	if (feof(fbitstream)) return 1;
	for (i=0; i<8; i++){
		bitbuffer[i] = (c & (0x1 << i)) >> i;
	}
	bitindex = 0;
	return 0;
}

char readbit(){
	char c;
	int i;
	if (bitindex == 8){
		c = fgetc(fbitstream);
		if (feof(fbitstream)) return -1;
		for (i=0; i<8; i++){
			bitbuffer[i] = (c & (0x1 << i)) >> i;
		}
		bitindex = 0;
	}
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
	if ((ret = fputc(*bitbuffer, fbitstream)) == EOF) return 1;
	*bitbuffer = 0;
	bitindex = 0;
	return 0;
}


