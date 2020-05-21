cmake_minimum_required(VERSION 3.15)
project(thrax_g2p)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(default_build_type "Release")

find_library(OPENFST NAMES fst)
find_library(THRAX NAMES libthrax.a)

find_path(FST_INCLUDE fst)
find_path(THRAX_INCLUDE thrax)

find_package(Boost REQUIRED
        COMPONENTS locale filesystem)

add_executable(thraxg2p main.cpp RewriteTesterUtils.cpp RewriteTesterUtils.h)
target_include_directories(thraxg2p PRIVATE ${FST_INCLUDE} ${THRAX_INCLUDE})
target_link_libraries(thraxg2p PUBLIC ${OPENFST} ${THRAX} Boost::locale Boost::filesystem)