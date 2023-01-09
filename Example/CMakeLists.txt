#-------------------------------------------------------------------------------
# SuiteSparse/Example/CMakeLists.txt:  cmake for Example user library, "my"
#-------------------------------------------------------------------------------

# Copyright (c) 2022-2023, Timothy A. Davis, All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

# cmake inserts the date and version number into Include/my.h:
set ( MY_DATE "Jan 17, 2023" )
set ( MY_VERSION_MAJOR 1 )
set ( MY_VERSION_MINOR 2 )
set ( MY_VERSION_PATCH 4 )

message ( STATUS "Building MY library version: v"
    ${MY_VERSION_MAJOR}.
    ${MY_VERSION_MINOR}.
    ${MY_VERSION_PATCH} " (" ${MY_DATE} ")" )

#-------------------------------------------------------------------------------
# policies
#-------------------------------------------------------------------------------

cmake_policy ( SET CMP0042 NEW )    # enable MACOSX_RPATH by default
cmake_policy ( SET CMP0048 NEW )    # VERSION variable policy
cmake_policy ( SET CMP0054 NEW )    # if ( expression ) handling policy
cmake_policy ( SET CMP0104 NEW )    # initialize CUDA architectures

if ( WIN32 )
    set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true )
endif ( )

set ( CMAKE_MACOSX_RPATH TRUE )
enable_language ( C CXX )
include ( GNUInstallDirs )

# set the module path for all Find*.cmake files.
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules
    ${CMAKE_SOURCE_DIR}/../lib/cmake/SuiteSparse
    ${CMAKE_INSTALL_PREFIX}/lib/cmake/SuiteSparse )
# add path to cmake target files (if necessary)
set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    ${CMAKE_SOURCE_DIR}/../lib/cmake )

#-------------------------------------------------------------------------------
# define my project
#-------------------------------------------------------------------------------

project ( my
    VERSION "${MY_VERSION_MAJOR}.${MY_VERSION_MINOR}.${MY_VERSION_PATCH}"
    LANGUAGES C )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

# look for all SuiteSparse packages:
find_package ( SuiteSparse_config 7.0.0 REQUIRED )
find_package ( AMD 3.0.3 REQUIRED )
find_package ( BTF 2.0.3 REQUIRED )
find_package ( CAMD 3.0.3 REQUIRED )
find_package ( CCOLAMD 3.0.3 REQUIRED )
find_package ( COLAMD 3.0.3 REQUIRED )
find_package ( CHOLMOD 4.0.3 REQUIRED )
find_package ( CHOLMOD_CUDA 4.0.3 REQUIRED )
find_package ( CXSparse 4.0.3 REQUIRED )
find_package ( GPUQREngine 2.0.3 REQUIRED )
find_package ( GraphBLAS 7.4.1 REQUIRED )
find_package ( KLU 2.0.3 REQUIRED )
find_package ( KLU_CHOLMOD 2.0.3 REQUIRED )
find_package ( LDL 3.0.3 REQUIRED )
find_package ( Mongoose 3.0.4 REQUIRED )
find_package ( RBio 3.0.3 REQUIRED )
find_package ( SPEX 2.0.3 REQUIRED )
find_package ( SuiteSparse_GPURuntime 2.0.3 REQUIRED )
find_package ( SPQR 3.0.3 REQUIRED )
find_package ( SPQR_CUDA 3.0.3 REQUIRED )
find_package ( UMFPACK 6.1.0 REQUIRED )

# look for all external libaries:
find_package ( OpenMP REQUIRED )
find_package ( GMP 6.1.2 REQUIRED )
find_package ( MPFR 4.0.2 REQUIRED )
include ( SuiteSparseBLAS )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/my.h.in"
    "${PROJECT_SOURCE_DIR}/Include/my.h"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include )

#-------------------------------------------------------------------------------
# my dynamic library properties
#-------------------------------------------------------------------------------

file ( GLOB MY_SOURCES "Source/*.c" )
add_library ( my SHARED ${MY_SOURCES} )
set_target_properties ( my PROPERTIES
    VERSION ${MY_VERSION_MAJOR}.${MY_VERSION_MINOR}.${MY_VERSION_PATCH}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${MY_VERSION_MAJOR}
    PUBLIC_HEADER "Include/my.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

#-------------------------------------------------------------------------------
# my static library properties
#-------------------------------------------------------------------------------

add_library ( my_static STATIC ${MY_SOURCES} )
set_target_properties ( my_static PROPERTIES
    VERSION ${MY_VERSION_MAJOR}.${MY_VERSION_MINOR}.${MY_VERSION_PATCH}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    OUTPUT_NAME my
    SOVERSION ${MY_VERSION_MAJOR} )

if ( MSVC )
    set_target_properties ( my_static PROPERTIES
        OUTPUT_NAME my_static )
endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

# OpenMP:
message ( STATUS "OpenMP C libraries:      ${OpenMP_C_LIBRARIES} ")
message ( STATUS "OpenMP C include:        ${OpenMP_C_INCLUDE_DIRS} ")
message ( STATUS "OpenMP C flags:          ${OpenMP_C_FLAGS} ")
target_link_libraries ( my PUBLIC ${OpenMP_C_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${OpenMP_C_LIBRARIES} )
set ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS} " )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS} " )
include_directories ( ${OpenMP_C_INCLUDE_DIRS} )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( my PUBLIC m )
    target_link_libraries ( my_static PUBLIC m )
endif ( )

# BLAS:
message ( STATUS "BLAS libraries:      ${BLAS_LIBRARIES} ")
message ( STATUS "BLAS include:        ${BLAS_INCLUDE_DIRS} ")
message ( STATUS "BLAS linker flags:   ${BLAS_LINKER_FLAGS} ")
target_link_libraries ( my PUBLIC ${BLAS_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${BLAS_LIBRARIES} )
include_directories ( ${BLAS_INCLUDE_DIRS} )

# LAPACK:
message ( STATUS "LAPACK libraries:    ${LAPACK_LIBRARIES} ")
message ( STATUS "LAPACK include:      ${LAPACK_INCLUDE_DIRS} ")
message ( STATUS "LAPACK linker flags: ${LAPACK_LINKER_FLAGS} ")
target_link_libraries ( my PUBLIC ${LAPACK_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${LAPACK_LIBRARIES} )
include_directories ( ${LAPACK_INCLUDE_DIRS} )

# gmp:
target_link_libraries ( my PUBLIC ${GMP_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${GMP_STATIC} )
include_directories ( ${GMP_INCLUDE_DIR} )

# mpfr:
target_link_libraries ( my PUBLIC ${MPFR_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${MPFR_STATIC} )
include_directories ( ${MPFR_INCLUDE_DIR} )

# amd:
target_link_libraries ( my PRIVATE amd )
if ( TARGET amd_static )
    target_link_libraries ( my_static PUBLIC amd_static )
else ( )
    target_link_libraries ( my_static PUBLIC amd )
endif ( )

# btf:
target_link_libraries ( my PRIVATE btf )
if ( TARGET btf_static )
    target_link_libraries ( my_static PUBLIC btf_static )
else ( )
    target_link_libraries ( my_static PUBLIC btf_static )
endif ( )

# camd:
target_link_libraries ( my PUBLIC camd )
if ( TARGET camd_static )
    target_link_libraries ( my_static PUBLIC camd_static )
else ( )
    target_link_libraries ( my_static PUBLIC camd )
endif ( )

# ccolamd:
target_link_libraries ( my PUBLIC ccolamd )
if ( TARGET ccolamd_static )
    target_link_libraries ( my_static PUBLIC ccolamd_static )
else ( )
    target_link_libraries ( my_static PUBLIC ccolamd )
endif ( )

# cholmod:
target_link_libraries ( my PUBLIC cholmod )
if ( TARGET cholmod_static )
    target_link_libraries ( my_static PUBLIC cholmod_static )
else ( )
    target_link_libraries ( my_static PUBLIC cholmod )
endif ( )

# colamd:
target_link_libraries ( my PUBLIC colamd )
if ( TARGET colamd_static )
    target_link_libraries ( my_static PUBLIC colamd_static )
else ( )
    target_link_libraries ( my_static PUBLIC colamd )
endif ( )

# cxsparse:
target_link_libraries ( my PUBLIC cxsparse )
if ( TARGET cxsparse_static )
    target_link_libraries ( my_static PUBLIC cxsparse_static )
else ( )
    target_link_libraries ( my_static PUBLIC cxsparse )
endif ( )

# gpuqrengine:
target_link_libraries ( my PUBLIC gpuqrengine )
if ( TARGET gpuqrengine_static )
    target_link_libraries ( my_static PUBLIC gpuqrengine_static )
else ( )
    target_link_libraries ( my_static PUBLIC gpuqrengine )
endif ( )

# GraphBLAS:
target_link_libraries ( my PUBLIC ${GRAPHBLAS_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${GRAPHBLAS_STATIC} )
include_directories ( ${GRAPHBLAS_INCLUDE_DIR} )

# klu:
target_link_libraries ( my PUBLIC klu )
if ( TARGET klu_static )
    target_link_libraries ( my_static PUBLIC klu_static )
else ( )
    target_link_libraries ( my_static PUBLIC klu )
endif ( )

# klu_cholmod:
target_link_libraries ( my PUBLIC klu_cholmod )
if ( TARGET klu_cholmod_static )
    target_link_libraries ( my_static PUBLIC klu_cholmod_static )
else ( )
    target_link_libraries ( my_static PUBLIC klu_cholmod )
endif ( )

# ldl:
target_link_libraries ( my PUBLIC ldl )
if ( TARGET ldl_static )
    target_link_libraries ( my_static PUBLIC ldl_static )
else ( )
    target_link_libraries ( my_static PUBLIC ldl )
endif ( )

# mongoose:
target_link_libraries ( my PRIVATE mongoose )
if ( TARGET mongoose_static )
    target_link_libraries ( my_static PUBLIC mongoose_static )
else ( )
    target_link_libraries ( my_static PUBLIC mongoose )
endif ( )

# rbio:
target_link_libraries ( my PUBLIC rbio )
if ( TARGET rbio_static )
    target_link_libraries ( my_static PUBLIC rbio_static )
else ( )
    target_link_libraries ( my_static PUBLIC rbio )
endif ( )

# spex:
target_link_libraries ( my PUBLIC spex )
if ( TARGET spex_static )
    target_link_libraries ( my_static PUBLIC spex_static )
else ( )
    target_link_libraries ( my_static PUBLIC spex )
endif ( )

# spqr:
target_link_libraries ( my PUBLIC spqr )
if ( TARGET spqr_static )
    target_link_libraries ( my_static PUBLIC spqr_static )
else ( )
    target_link_libraries ( my_static PUBLIC spqr )
endif ( )

# suitesparseconfig:
target_link_libraries ( my PRIVATE suitesparseconfig )
if ( TARGET suitesparseconfig_static )
    target_link_libraries ( my_static PUBLIC suitesparseconfig_static )
else ( )
    target_link_libraries ( my_static PUBLIC suitesparseconfig )
endif ( )

# suitesparse_gpuruntime:
target_link_libraries ( my PRIVATE suitesparse_gpuruntime )
if ( TARGET suitesparse_gpuruntime_static )
    target_link_libraries ( my_static PUBLIC suitesparse_gpuruntime_static )
else ( )
    target_link_libraries ( my_static PUBLIC suitesparse_gpuruntime )
endif ( )

# umfpack:
target_link_libraries ( my PUBLIC ${UMFPACK_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${UMFPACK_STATIC} )
include_directories ( ${UMFPACK_INCLUDE_DIR} )

#-------------------------------------------------------------------------------
# installation location
#-------------------------------------------------------------------------------

install ( TARGETS my
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
install ( TARGETS my_static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} )

#-------------------------------------------------------------------------------
# Demo program
#-------------------------------------------------------------------------------

add_executable ( my_demo "Demo/my_demo.c" )
target_link_libraries ( my_demo PUBLIC my )

