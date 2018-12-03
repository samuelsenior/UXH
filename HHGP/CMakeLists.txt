project(HHGP)

cmake_minimum_required(VERSION 3.1)

#----------------------------------------------------------#
#                    Set C++ Standard                      #
#----------------------------------------------------------#

# Set the C++ standard to C++14 and require C++14 rather than falling back to the next highest version as well keeping the linking to the correct library
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------#
#                    Set Compiler Flags                    #
#----------------------------------------------------------#

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   MESSAGE("-- Compiling using the GNU compiler")
   ##Make sure I'm using GCC and not Clang #Should avoid this way really #Also Needs to be for MPI
   # export CC=/usr/local/bin/gcc-6
   # export CXX=/usr/local/bin/g++-6

   # Set the optimisation level flag
   set(CMAKE_CXX_FLAGS "-std=c++14 -Ofast -fopenmp -mavx")
   set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -fopenmp -mavx")
   #set(CMAKE_CXX_FLAGS "-std=c++14 -g -fopenmp -mavx")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
   MESSAGE("-- Compiling using the Intel compiler")
   ##Make sure I'm using GCC and not Clang #Should avoid this way really #Also Needs to be for MPI
   # export CC=/usr/local/bin/gcc-6
   # export CXX=/usr/local/bin/g++-6

   # Set the optimisation level flag
   set(CMAKE_CXX_FLAGS "-std=c++14 -qopenmp -Ofast -mavx -inline-forceinline")
   # set(CMAKE_CXX_FLAGS "-std=c++14 -qopenmp -O3")#-O3 -fopenmp")
   ## This is the equivalent C flag setting:
   # set(CMAKE_C_FLAGS "-O3 -fopenmp")

endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

#----------------------------------------------------------#
#                    Set version number                    #
#----------------------------------------------------------#

# Get the beginning of each version part line
# version.hpp needs to end in a new line for the subversion part to work correctly
FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp version)
string(FIND "${version}" "\#define _VERSION_MAJOR" major_version_begin)
string(FIND "${version}" "\#define _VERSION_MINOR" minor_version_begin)
string(FIND "${version}" "\#define _VERSION_SUBMINOR" subminor_version_begin)

#----Major Version Number Part----#
# Get the position where the major version number starts
MATH(EXPR tmp "${major_version_begin}+23")
set(major_version_begin ${tmp})

# Get the position where the major version number ends
MATH(EXPR tmp "${minor_version_begin}-1")
set(major_version_end ${tmp})

# Get the length of the major version number
MATH(EXPR tmp "${major_version_end}-${major_version_begin}")
set(major_version_length ${tmp})

# Get a string of the major version number
string(SUBSTRING ${version} ${major_version_begin} ${major_version_length} major_version)

#----Minor Version Number Part----#
# Get the position where the minor version number starts
MATH(EXPR tmp "${minor_version_begin}+23")
set(minor_version_begin ${tmp})

# Get the position where the minor version number ends
MATH(EXPR tmp "${subminor_version_begin}-1")
set(minor_version_end ${tmp})

# Get the length of the minor version number
MATH(EXPR tmp "${minor_version_end}-${minor_version_begin}")
set(minor_version_length ${tmp})

# Get a string of the minor version number
string(SUBSTRING ${version} ${minor_version_begin} ${minor_version_length} minor_version)

#----Subminor Version Number Part----#
# Get the position where the subminor version number starts
MATH(EXPR tmp "${subminor_version_begin}+26")
set(subminor_version_begin ${tmp})

# Get the position where the subminor version number ends
# Assumes the files ends in a "\n#endif\n"
STRING(LENGTH ${version} subminor_version_end)
MATH(EXPR tmp "${subminor_version_end}-9")
set(subminor_version_end ${tmp})

# Get the length of the subminor version number
MATH(EXPR tmp "${subminor_version_end}-${subminor_version_begin}")
set(subminor_version_length ${tmp})

# Get a string of the subminor version number
string(SUBSTRING ${version} ${subminor_version_begin} ${subminor_version_length} subminor_version)

# Set the overall version number
set(VERSION_NUMBER ${major_version}-${minor_version}-${subminor_version})
MESSAGE("-- HPPG_version: " ${VERSION_NUMBER})

#----------------------------------------------------------#
#                    Get Eigen Headers                     #
#----------------------------------------------------------#

##Bring the Eigen headers into the project
#isinclude_directories(Eigen)

#----------------------------------------------------------#
#                    Get MKL Library                       #
#----------------------------------------------------------#

#Intel MKL Library
#find_package(MKL REQUIRED)
#if (NOT MKL_FOUND)
#   message(FATAL_ERROR “PACKAGE MKL not found”)
#endif (NOT MKL_FOUND)
#include_directories(${MKL_INCLUDE_DIR})
#include_directories(${MKL_LIB_DIR})
#set(LIBRARIES ${LIBRARIES} ${MKL_LIBRARIES})
set(LIBRARIES ${LIBRARIES} -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm)

#----------------------------------------------------------#
#                    Get OpenMPI                           #
#----------------------------------------------------------#

#MPI
find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

#----------------------------------------------------------#
#                    Set Sources                           #
#----------------------------------------------------------#

set(SOURCES
   ${SOURCES}
   ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/config_settings.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/DHT.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/grid_rkr.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/grid_tw.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/HHGP.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/HH_source.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/IO.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/keldysh_gas.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/propagation.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/maths_textbook.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../src/physics_textbook.cpp
   #PARENT_SCOPE
)
set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/src/maths_textbook.hpp
   #PARENT_SCOPE
)

#----------------------------------------------------------#
#                    Make Documentation                    #
#----------------------------------------------------------#

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/build/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/build/Doxyfile.out)
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT}  @ONLY)
    message("-- Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("-- Doxygen needs to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

#----------------------------------------------------------#
#                    Put it all Together                   #
#----------------------------------------------------------#

add_library(HHGP_Objects OBJECT ${SOURCES})

add_library(${CMAKE_PROJECT_NAME} STATIC $<TARGET_OBJECTS:HHGP_Objects>)

add_executable(${CMAKE_PROJECT_NAME}-bin $<TARGET_OBJECTS:HHGP_Objects>)
target_link_libraries(${CMAKE_PROJECT_NAME}-bin ${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME}-bin
        PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBRARIES} ${MPI_C_LIBRARIES})

# LIBRARY_OUTPUT_DIRECTORY for shared libraries, ARCHIVE_OUTPUT_DIRECTORY for static library
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(${CMAKE_PROJECT_NAME}-bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(BUILD_POSTFIX ${BUILD_POSTFIX})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME}-v${VERSION_NUMBER}${BUILD_POSTFIX})
set_target_properties(${CMAKE_PROJECT_NAME}-bin PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME}-v${VERSION_NUMBER}${BUILD_POSTFIX})

set(MPI_COMPILE_FLAGS -openmp)
if(MPI_COMPILE_FLAGS)
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(HHGP PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
