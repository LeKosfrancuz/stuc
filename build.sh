#!/bin/bash
CFlags=" -O3 -Wall -Wextra -pedantic -Wstrict-prototypes -Wold-style-definition -std=c99 src/stuc/libstuc.a -lm"
Raylib="-L./src/gui-app/external/lib -lraylib -I./src/gui-app/external"
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
echo -e "\x1B[37;1mBuilding Raylib\x1B[37;0m"
pushd src/gui-app/external/raylib
make remake_release_path
make -j10
popd

echo
echo -e "\x1B[37;1mBuilding Demos\x1B[37;0m"

[ ! -d "./builds/" ] && mkdir "./builds/"

gcc demo/xor.c -DPERFECT -o builds/Xor.out $CFlags $Debug && echo "Finished Building [Xor.out]" || echo "Build Failed [Xor.out]"
gcc demo/zbrajalo.c -DPERFECT -DFINITE_DIFF -o builds/Zbrajalo_diff.out $CFlags $Debug && echo "Finished Building [Zbrajalo_diff.out]" || echo "Build Failed [Zbrajalo_diff.out]"
gcc demo/zbrajalo.c -DPERFECT -DBACKPROP -o builds/Zbrajalo_bp.out $CFlags $Debug && echo "Finished Building [Zbrajalo_bp.out]" || echo "Build Failed [Zbrajalo_bp.out]"
gcc demo/nonsensical_ian_numbers.c -o builds/IanNums.out $CFlags $Debug && echo "Finished Building [IanNums.out]" || echo "Build Failed [IanNums.out]"
gcc demo/loadAndFineTune.c -o builds/FineTuning.out $CFlags $Debug && echo "Finished Building [FineTuning.out]" || echo "Build Failed [FineTuning.out]"

SCENES="src/gui-app/scene_nnBuilder.c src/gui-app/scene_imageRecogniser.c src/gui-app/scene_tData.c"
gcc src/gui-app/gui-app.c $SCENES -o builds/Stuc.out $CFlags $Raylib $Debug && echo "Finished Building [Stuc.out]" || echo "Build Failed [Stuc.out]"
