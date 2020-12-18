cmake_minimum_required(VERSION 3.12.4)

project (argparser)

option(BUILD_EXAMPLES "Build the examples in {project}/examples directory" ON)
option(BUILD_UNITTEST "Build the unittest in {project}/test directory" ON)
option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dir to include
set(HELLOWORLD_INCLUDE_DIRECTORIES include)

add_compile_options("-Wall" "-W" "-Wextra" "-fPIC")

add_subdirectory(gtest)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# headers to install
file(GLOB_RECURSE HELLOWORLD_PUBLIC_HEADER include/*)

# source for the lib
file(GLOB_RECURSE HELLOWORLD_SOURCE src/*)
add_library(argparser_obj OBJECT ${HELLOWORLD_SOURCE})
target_include_directories(argparser_obj PUBLIC ${HELLOWORLD_INCLUDE_DIRECTORIES})

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

if (${BUILD_UNITTEST})
    add_subdirectory(test)
    enable_testing()
endif()

if (${BUILD_STATIC_LIBS})
    add_library(argparser-static STATIC $<TARGET_OBJECTS:argparser_obj>)

    # install instructions
    set_target_properties(argparser-static PROPERTIES PUBLIC_HEADER "${HELLOWORLD_PUBLIC_HEADER}")
    install(TARGETS argparser-static PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/argparser)
endif()

if (${BUILD_SHARED_LIBS})
    add_library(argparser-dynamic SHARED $<TARGET_OBJECTS:argparser_obj>)

    # install instructions
    set_target_properties(argparser-dynamic PROPERTIES PUBLIC_HEADER "${HELLOWORLD_PUBLIC_HEADER}")
    install(TARGETS argparser-dynamic PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/argparser)
endif()
