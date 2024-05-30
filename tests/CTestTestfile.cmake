# CMake Testfile for stuc.h
# Build dir: /stuc/tests/build/
#
# use $ ./run-tests.sh
message("\nStarted testing...\n")

set (failRegex "fail")

if (NOT DEFINED ENV{LIST_OF_TEST_FILES})
	message(WARNING "\n! FAILED TO LOAD ALL TESTS !")
	message("to run all the tests do `./run-tests.sh`\n")

	set (tests "nnForward;nnCost;matDot_Redosljed;matDot_RazliciteVelicine")
else()
	set (tests $ENV{LIST_OF_TEST_FILES})
endif()

string(REPLACE " " ";" tests "${tests}")

foreach(test IN LISTS tests)
	add_test(test_${test} "./build/test_${test}")

	string(REGEX REPLACE "_[^$]*" "" label "${test}")

	set_tests_properties(TEST test_${test}
		PROPERTIES FAIL_REGULAR_EXPRESSION "${failRegex}"
		LABELS "${label}")
endforeach()
