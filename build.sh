#!/bin/bash
CFlags=" -O3 -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -std=c99 src/stuc/libstuc.a -lm"
Raylib="-lraylib"
Debug=""
argc=$#
if [ $argc -eq 1 ] && [ $1 = "dbg" ] 
then 
	# za debug u gdb-u, gf2 i sl.
	Debug="-O0 -ggdb3 -fsanitize=address,undefined"
fi

echo -e "\x1B[37;1mBuilding Library\x1B[37;0m"
pushd src/stuc/
./build-library.sh $1
popd

echo 
echo -e "\x1B[37;1mBuilding Demos\x1B[37;0m"

[ ! -d "./builds/" ] && mkdir "./builds/"

gcc demo/xor.c -DPERFECT -o builds/Xor.out $CFlags $Debug && echo "Finished Building [Xor.out]" || echo "Build Failed [Xor.out]"
gcc demo/loadAndFineTune.c -o builds/FineTuning.out $CFlags $Debug && echo "Finished Building [FineTuning.out]" || echo "Build Failed [FineTuning.out]"
gcc src/gui-app/gui-app.c -o builds/Stuc.out $CFlags $Raylib $Debug && echo "Finished Building [Stuc.out]" || echo "Build Failed [Stuc.out]"

