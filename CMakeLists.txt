cmake_minimum_required(VERSION 3.12)
project(thrax_g2p)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(default_build_type "Release")

# find dependencies for compilation
find_library(OPENFST NAMES fst)
find_library(THRAX NAMES libthrax.a)
find_program(THRAXCOMPILER NAMES thraxcompiler)

find_path(FST_INCLUDE fst)
find_path(THRAX_INCLUDE thrax)

find_package(Boost REQUIRED
        COMPONENTS locale filesystem)

# add custom rule for compiling fst archive file
set(GRAMMAR ${CMAKE_SOURCE_DIR}/grammars/g2p.grm)
set(GRAMMAR_ARCHIVE  ${CMAKE_SOURCE_DIR}/grammars/g2p.far)

add_custom_command(OUTPUT ${GRAMMAR_ARCHIVE}
        PRE_BUILD
        COMMAND ${THRAXCOMPILER} --input_grammar=${GRAMMAR} --output_far=${GRAMMAR_ARCHIVE}
        DEPENDS ${GRAMMAR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling grammar ${GRAMMAR} => ${GRAMMAR_ARCHIVE}"
        VERBATIM
        )
add_custom_target(g2p_compile DEPENDS ${GRAMMAR_ARCHIVE})

# add custom rule for dialects
set(GRAMMAR_NORTH ${CMAKE_SOURCE_DIR}/grammars/g2p-north.grm)
set(GRAMMAR_ARCHIVE_NORTH  ${CMAKE_SOURCE_DIR}/grammars/g2p-north.far)

# General compilation options
add_compile_options(
        "-Wall" "-W" "-Wpedantic" "-Wextra" "-fexceptions" "-Wno-unused-parameter" "-Wno-sign-compare" "-Wno-extra-semi"
        "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)

if(CMAKE_PROJECT_NAME STREQUAL thrax_g2p)
    include(CTest)
endif()

# Split thraxg2p into a library part and the runtime part.
# This way, we can test the library part with unit tests

add_library(thraxg2plib STATIC
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
        thraxg2plib
        dl)
add_dependencies(thraxg2p g2p_compile)

if(CMAKE_PROJECT_NAME STREQUAL thrax_g2p AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
