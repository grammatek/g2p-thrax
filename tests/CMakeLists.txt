# Unit testing

# CppUTest, fetch it externally, so we don't need it to be part of the git repository
include(FetchContent)
FetchContent_Declare(
        CppUTest
        GIT_REPOSITORY https://github.com/cpputest/cpputest.git
        GIT_TAG        latest-passing-build
)

# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
FetchContent_MakeAvailable(CppUTest)

add_executable(thraxg2p_test TestThraxG2P.cpp)
target_link_libraries(thraxg2p_test PRIVATE thraxg2plib CppUTest CppUTestExt dl)
add_dependencies(thraxg2p_test g2p_compile)

add_test(NAME thraxg2p_test COMMAND thraxg2p_test --rules=G2P --far=${GRAMMAR_ARCHIVE} --test_file=${CMAKE_CURRENT_SOURCE_DIR}/test_transcripts.txt)
add_test(NAME thraxg2p_test_north COMMAND thraxg2p_test --rules=G2P --far=${GRAMMAR_ARCHIVE_NORTH} --test_file=${CMAKE_CURRENT_SOURCE_DIR}/test_transcripts_north.txt)
add_test(NAME thraxg2p_test_northeast COMMAND thraxg2p_test --rules=G2P --far=${GRAMMAR_ARCHIVE_NORTHEAST} --test_file=${CMAKE_CURRENT_SOURCE_DIR}/test_transcripts_northeast.txt)
add_test(NAME thraxg2p_test_south COMMAND thraxg2p_test --rules=G2P --far=${GRAMMAR_ARCHIVE_SOUTH} --test_file=${CMAKE_CURRENT_SOURCE_DIR}/test_transcripts_south.txt)

