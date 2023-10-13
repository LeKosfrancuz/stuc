#!/bin/bash
CFlags=" -O3 -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -lm "
Raylib="-lraylib"
Debug=""
argc=$#
if [ $argc -eq 1 ] && [ $1 = "dbg" ] 
then 
	Debug="-g" 
fi

gcc demo/xor.c -o builds/Xor.out $CFlags $Debug && echo "Finished Building [Xor.out]" || echo "Build Failed [Xor.out]"
gcc demo/loadAndFineTune.c -o builds/FineTuning.out $CFlags $Debug && echo "Finished Building [FineTuning.out]" || echo "Build Failed [FineTuning.out]"
gcc src/gui-app.c -o builds/Stuc.out $CFlags $Raylib $Debug && echo "Finished Building [Stuc.out]" || echo "Build Failed [Stuc.out]"

