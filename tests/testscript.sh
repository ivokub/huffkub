#!/bin/bash

echo "$PWD"
echo "Testing packer:"

rm packed/*
rm unpacked/*
for file in `ls plain/`; do
	echo "  File $file:"
	echo -n "   Packing: "
	../huffkub -c -f plain/$file -d packed/$file && echo "Success"
	echo -n "   Unpacking: "
	../huffkub -x -f packed/$file -d unpacked/$file && echo "Success"
	echo -n "   Comparing: "
	cmp unpacked/$file plain/$file && echo "Success"
	SIZE1=$(wc -c plain/$file | cut -d" " -f1)
	SIZE2=$(wc -c packed/$file | cut -d" " -f1)
	echo "   Size before: $SIZE1, size after: $SIZE2"
done

rm packed/*
rm unpacked/*
echo "All tests done"
