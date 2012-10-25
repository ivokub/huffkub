huffkub
=======

Huffkub packs stuff

Building
--------

Just use `make`. To clean working directory, use `make clean`.
After building run tests:

	cd tests
	./testscript.sh

Usage
-----

From program help

	Simple file packer using Huffman algorithm.

	Usage:	huffkub [-c] [-x] [-i] [-o] [-f inputfile] [-d outputfile]

	Argument usage:
		 -c		Pack (exclusive with -x)
		 -x		Unpack (exclusive with -c)
		 -i		Read input from stdin (exclusive with -f)
		 -o		Write output to stdout (exclusive with -d)
		 -f FILE	Read input from FILE (exclusive with -i)
		 -d FILE	Write output to FILE (exclusive with -o)
		 -v		Verbose output (only when -o is not set)
		 -h		Print this help message


