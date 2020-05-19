cmake_minimum_required(VERSION 3.16)
project(sim_g2p)

set(CMAKE_CXX_STANDARD 17)

find_library(OPENFST NAMES fst)
find_library(THRAX NAMES thrax)

find_path(FST_INCLUDE fst)
find_path(THRAX_INCLUDE thrax)

add_executable(simg2p main.cpp RewriteTesterUtils.cpp RewriteTesterUtils.h)
target_include_directories(simg2p PRIVATE ${FST_INCLUDE} ${THRAX_INCLUDE})
target_link_libraries(simg2p PRIVATE ${THRAX} ${OPENFST})