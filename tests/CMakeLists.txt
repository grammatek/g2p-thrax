add_executable(thraxg2p_test TestThraxG2P.cpp)
target_link_libraries(thraxg2p_test PRIVATE thraxg2plib CppUTest CppUTestExt dl)

add_test(NAME thraxg2p_test COMMAND thraxg2p_test --far=${CMAKE_SOURCE_DIR}/g2p.far --test_file=${CMAKE_CURRENT_SOURCE_DIR}/test_transcripts.txt)
