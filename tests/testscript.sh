#!/bin/bash

echo "Testing packer:"

rm packed_1/*
rm packed_2/*
rm packed_3/*
rm packed_4/*
rm unpacked_1/*
rm unpacked_2/*
rm unpacked_3/*
rm unpacked_4/*

for file in `ls plain/`; do
	echo -e "  File \e[00;31m$file\e[00m:"
        echo -e "    \e[01;33mPacking: "
	echo -n "       file to file: " && ../huffkub -c -f plain/$file -d packed_1/$file && echo "Success"
        echo -n "       stdin to file: " && ../huffkub -c -i -d packed_2/$file < plain/$file && echo "Success"
        echo -n "       file to stdout: " && ../huffkub -c -f plain/$file -o > packed_3/$file && echo "Success"
        echo -n "       stdin to stdout: " && ../huffkub -c -i -o < plain/$file > packed_4/$file && echo "Success"
        echo -e "   \e[01;37mComparing packed files: "
        echo -n "       1<>2: " && cmp packed_1/$file packed_2/$file && echo "Success"
        echo -n "       1<>3: " && cmp packed_1/$file packed_3/$file && echo "Success"
        echo -n "       1<>4: " && cmp packed_1/$file packed_4/$file && echo "Success"
	echo -e "    \e[01;34mUnpacking: "
        echo -n "       file to file: " && ../huffkub -x -f packed_1/$file -d unpacked_1/$file && echo "Success"
        echo -n "       stdin to file: " && ../huffkub -x -i -d unpacked_2/$file < packed_1/$file && echo "Success"
        echo -n "       file to stdout: " && ../huffkub -x -o -f packed_1/$file > unpacked_3/$file && echo "Success"
        echo -n "       stdin to stdout: " && ../huffkub -x -i -o < packed_1/$file > unpacked_4/$file && echo "Success"
	echo -e "    \e[00;36mComparing: "
	echo -n "       original<>1: " && cmp unpacked_1/$file plain/$file && echo "Success"
        echo -n "       original<>2: " && cmp unpacked_2/$file plain/$file && echo "Success"
        echo -n "       original<>3: " && cmp unpacked_3/$file plain/$file && echo "Success"
        echo -n "       original<>4: " && cmp unpacked_4/$file plain/$file && echo "Success"
	SIZE1=$(wc -c plain/$file | cut -d" " -f1)
	SIZE2=$(wc -c packed_1/$file | cut -d" " -f1)
	echo -e "   \e[00;35mSize before: $SIZE1, size after: $SIZE2\e[00m"
done

echo -e "\e[00mAll tests done"
