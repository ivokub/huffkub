all: huffkub

huffkub: huffkub.o compress.o extract.o bitlib.o
	gcc -g huffkub.o compress.o extract.o bitlib.o -o huffkub

huffkub.o: huffkub.c struct.h huffkub.h
	gcc -g -c huffkub.c

compress.o: compress.c struct.h huffkub.h
	gcc -g -c compress.c

extract.o: extract.c huffkub.h
	gcc -g -c extract.c
	
bitlib.o: bitlib.c bitlib.h
	gcc -g -c bitlib.c

clean:
	rm huffkub.o compress.o extract.o huffkub
