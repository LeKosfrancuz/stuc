#!/bin/bash
CFlags=" -O3 -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -std=c99 -lm"
Debug=""
argc=$#
if [ $argc -eq 1 ] && [ $1 = "dbg" ] 
then 
	# za debug u gdb-u, gf2 i sl.
	Debug="-O0 -ggdb3 -fsanitize=address,undefined"
fi

set -xe
OUT=" ./seperate-obj-files/"
[ ! -d $OUT ] && mkdir $OUT

rm libstuc.a -f
rm ${OUT}/* -f

gcc -c stuc-backprop.c 		 -o ${OUT}backprop.o 	$CFlags $Debug
gcc -c stuc-file_IO.c 		 -o ${OUT}file_IO.o 	$CFlags $Debug
gcc -c stuc-finite_diff.c 	 -o ${OUT}finite_diff.o $CFlags $Debug
gcc -c stuc-matrice_operations.c -o ${OUT}matrice_ops.o $CFlags $Debug
gcc -c stuc-nn_user_API.c 	 -o ${OUT}nn_user_API.o $CFlags $Debug


ar rvs libstuc.a ${OUT}backprop.o ${OUT}file_IO.o ${OUT}finite_diff.o ${OUT}matrice_ops.o ${OUT}nn_user_API.o
