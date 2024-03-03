#!/bin/bash

S="\x1B[32;1m" # Success colour
F="\x1B[31;1m" # Fail colour
N="\x1B[37;0m" # Normal colour
B="\x1B[37;1m" # Bold colour
C="\x1B[36;1m" # Cyan colour

# nnForward i Cost tests
TESTS="nnForward nnCost"

# Test matricnih operacija
TESTS="$TESTS matDot_Redosljed matDot_RazliciteVelicine matRow_tocnostKopiranja matRow_promjenaOriginalnihPodataka"

#Relu Tests
TESTS="$TESTS activationDerivRELU_manjeOdNula activationDerivRELU_veceOdNula activationDerivRELU_jednakoNula"

#Sigmoid Tests
TESTS="$TESTS activationDerivSIGMOID_minusJedan activationDerivSIGMOID_jednaPolovina activationDerivSIGMOID_plusJedan"

#Tanh Tests
TESTS="$TESTS activationDerivSIN_0 activationDerivSIN_45 activationDerivSIN_90 activationDerivSIN_180"

#STUC_AT Tests
TESTS="$TESTS stucAT_input stucAT_output stucAT_wrongInIndex stucAT_wrongOutIndex stucAT_funcArgOutput"

echo -e "${B}Building Tests...$N\n"
for i in $TESTS; do 
    gcc test_${i}.c -o build/test_${i} ../src/stuc/libstuc.a -lm -ggdb && echo -e "Built test [$C$i$N] ${S}succesfuly$N" || echo -e "${F}Failed$N to build test [$C$i$N]"
done

LIST_OF_TEST_FILES=$TESTS ctest -j4 $1 $2 $3 $4 $5 $6
