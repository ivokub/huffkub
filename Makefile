compiled = huffkub.o compress.o extract.o bitlib.o
args = -g

all: huffkub

huffkub: $(compiled)
	gcc $(args) $(compiled) -o huffkub

huffkub.o: huffkub.c struct.h huffkub.h
	gcc $(args) -c huffkub.c

compress.o: compress.c struct.h huffkub.h
	gcc $(args) -c compress.c

extract.o: extract.c huffkub.h
	gcc $(args) -c extract.c
	
bitlib.o: bitlib.c bitlib.h
	gcc $(args) -c bitlib.c

clean:
	rm $(compiled)

distclean: clean
	rm huffkub

