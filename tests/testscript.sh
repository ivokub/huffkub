#!/bin/bash

echo "$PWD"
echo "Testing packer:"

rm packed/*
rm unpacked/*
for file in `ls plain/`; do
	echo "	Packing $file:"
	../huffkub -c -f plain/$file -d packed/$file && echo "		Success"
done

for file in `ls packed/`; do
	echo "	Unpacking $file:"
        ../huffkub -x -f packed/$file -d unpacked/$file && echo "		Success"
done

for file in `ls plain/`; do
	echo "	Comparing $file:"
        cmp unpacked/$file plain/$file && echo "		Success:"
done

echo "All tests done"
