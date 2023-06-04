#!/bin/bash
CFlags=" -Wall -Wextra -pedantic -lm "
Debug=""
argc=$#
if [ $argc -eq 1 ] && [ $1 = "dbg" ] 
then 
	Debug="-g" 
fi

gcc src/xor.c -o Xor.out $CFlags $Debug && echo "Finished Building" || echo "Build Failed"

