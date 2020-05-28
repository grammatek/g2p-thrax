cmake_minimum_required(VERSION 3.12)
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

if(CMAKE_PROJECT_NAME STREQUAL thrax_g2p)
    include(CTest)
endif()

# Split thraxg2p into a library part and the runtime part.
# This way, we can test the library part with unit tests

add_library(thraxg2plib
        RewriteTesterUtils.cpp
        RewriteTesterUtils.h)

target_include_directories(thraxg2plib PUBLIC
        ${FST_INCLUDE}
        ${THRAX_INCLUDE}
        .)

target_link_libraries(thraxg2plib PUBLIC
        ${OPENFST}
        ${THRAX}
        Boost::locale
        Boost::filesystem)

add_executable(thraxg2p main.cpp)

target_include_directories(thraxg2p PRIVATE
        ${FST_INCLUDE}
        ${THRAX_INCLUDE})

target_link_libraries(thraxg2p PRIVATE
        thraxg2plib)

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

if(CMAKE_PROJECT_NAME STREQUAL thrax_g2p AND BUILD_TESTING)
    add_subdirectory(tests)
endif()