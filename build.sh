#!/bin/bash
CFlags=" -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -lm "
Debug=""
argc=$#
if [ $argc -eq 1 ] && [ $1 = "dbg" ] 
then 
	Debug="-g" 
fi

gcc demo/xor.c -o Xor.out $CFlags $Debug && echo "Finished Building [Xor.out]" || echo "Build Failed [Xor.out]"
gcc demo/loadAndFineTune.c -o FineTuning.out $CFlags $Debug && echo "Finished Building [FineTuning.out]" || echo "Build Failed [FineTuning.out]"

