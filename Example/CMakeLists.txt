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
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cmake/SuiteSparse
    ${CMAKE_INSTALL_PREFIX}/lib/cmake/SuiteSparse )

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
find_package ( suitesparseconfig 7.0.0 REQUIRED )
set(Suitesparseconfig_STATIC SuiteSparse::suitesparseconfig_static)
set(Suitesparseconfig_LIBRARIES SuiteSparse::suitesparseconfig)

set(SUITESPARSE_CONFIG_STATIC SuiteSparse::suitesparseconfig_static)
set(SUITESPARSE_CONFIG_LIBRARIES SuiteSparse::suitesparseconfig)


find_package ( amd 3.0.3 REQUIRED )
set(AMD_STATIC SuiteSparse::amd_static)
set(AMD_LIBRARIES SuiteSparse::amd)

find_package ( btf 2.0.3 REQUIRED )
set(BTF_STATIC SuiteSparse::btf_static)
set(BTF_LIBRARIES SuiteSparse::btf)
find_package ( camd 3.0.3 REQUIRED )
set(CAMD_STATIC SuiteSparse::camd_static)
set(CAMD_LIBRARIES SuiteSparse::camd)
find_package ( ccolamd 3.0.3 REQUIRED )
set(CCOLAMD_STATIC SuiteSparse::ccolamd_static)
set(CCOLAMD_LIBRARIES SuiteSparse::ccolamd)
find_package ( colamd 3.0.3 REQUIRED )
set(COLAMD_STATIC SuiteSparse::colamd_static)
set(COLAMD_LIBRARIES SuiteSparse::colamd)
find_package ( cholmod 4.0.3 REQUIRED )
set(CHOLMOD_STATIC SuiteSparse::cholmod_static)
set(CHOLMOD_LIBRARIES SuiteSparse::cholmod)
find_package ( CHOLMOD_CUDA 4.0.3 REQUIRED )
find_package ( cxsparse 4.0.3 REQUIRED )
set(CXSparse_STATIC SuiteSparse::cxsparse_static)
set(CXSparse_LIBRARIES SuiteSparse::cxsparse)
find_package ( gpuqrengine 2.0.3 REQUIRED )
set(GPUQREngine_STATIC SuiteSparse::gpuqrengine_static)
set(GPUQREngine_LIBRARIES SuiteSparse::gpuqrengine)
find_package ( graphblas 7.4.1 REQUIRED )
set(GraphBLAS_STATIC SuiteSparse::graphblas_static)
set(GraphBLAS_LIBRARIES SuiteSparse::graphblas)
find_package ( klu 2.0.3 REQUIRED )
set(KLU_STATIC SuiteSparse::klu_static)
set(KLU_LIBRARIES SuiteSparse::klu)
find_package ( KLU_CHOLMOD 2.0.3 REQUIRED )
find_package ( ldl 3.0.3 REQUIRED )
set(LDL_STATIC SuiteSparse::ldl_static)
set(LDL_LIBRARIES SuiteSparse::ldl)
find_package ( mongoose 3.0.4 REQUIRED )
set(Mongoose_STATIC SuiteSparse::mongoose_static)
set(Mongoose_LIBRARIES SuiteSparse::mongoose)
find_package ( rbio 3.0.3 REQUIRED )
set(RBio_STATIC SuiteSparse::rbio_static)
set(RBio_LIBRARIES SuiteSparse::rbio)
find_package ( spex 2.0.3 REQUIRED )
set(SPEX_STATIC SuiteSparse::spex_static)
set(SPEX_LIBRARIES SuiteSparse::spex)
find_package ( SuiteSparse_GPURuntime 2.0.3 REQUIRED )
find_package ( spqr 3.0.3 REQUIRED )
set(SPQR_STATIC SuiteSparse::spqr_static)
set(SPQR_LIBRARIES SuiteSparse::spqr)
find_package ( SPQR_CUDA 3.0.3 REQUIRED )
find_package ( umfpack 6.1.0 REQUIRED )
set(UMFPACK_STATIC SuiteSparse::umfpack_static)
set(UMFPACK_LIBRARIES SuiteSparse::umfpack)

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
    C_STANDARD_REQUIRED 11
    SOVERSION ${MY_VERSION_MAJOR}
    PUBLIC_HEADER "Include/my.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

#-------------------------------------------------------------------------------
# my static library properties
#-------------------------------------------------------------------------------

add_library ( my_static STATIC ${MY_SOURCES} )
set_target_properties ( my_static PROPERTIES
    VERSION ${MY_VERSION_MAJOR}.${MY_VERSION_MINOR}.${MY_VERSION_PATCH}
    C_STANDARD_REQUIRED 11
    OUTPUT_NAME my
    SOVERSION ${MY_VERSION_MAJOR} )

if ( WIN32 )
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
target_link_libraries ( my PUBLIC ${AMD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${AMD_STATIC} )
include_directories ( ${AMD_INCLUDE_DIR} )

# btf:
target_link_libraries ( my PUBLIC ${BTF_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${BTF_STATIC} )
include_directories ( ${BTF_INCLUDE_DIR} )

# camd:
target_link_libraries ( my PUBLIC ${CAMD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${CAMD_STATIC} )
include_directories ( ${CAMD_INCLUDE_DIR} )

# ccolamd:
target_link_libraries ( my PUBLIC ${CCOLAMD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${CCOLAMD_STATIC} )
include_directories ( ${CCOLAMD_INCLUDE_DIR} )

# cholmod:
target_link_libraries ( my PUBLIC ${CHOLMOD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${CHOLMOD_STATIC} )
include_directories ( ${CHOLMOD_INCLUDE_DIR} )

# colamd:
target_link_libraries ( my PUBLIC ${COLAMD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${COLAMD_STATIC} )
include_directories ( ${COLAMD_INCLUDE_DIR} )

# cxsparse:
target_link_libraries ( my PUBLIC ${CXSPARSE_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${CXSPARSE_STATIC} )
include_directories ( ${CXSPARSE_INCLUDE_DIR} )

# gpuqrengine:
target_link_libraries ( my PUBLIC ${GPUQRENGINE_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${GPUQRENGINE_STATIC} )
include_directories ( ${GPUQRENGINE_INCLUDE_DIR} )

# GraphBLAS:
target_link_libraries ( my PUBLIC ${GRAPHBLAS_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${GRAPHBLAS_STATIC} )
include_directories ( ${GRAPHBLAS_INCLUDE_DIR} )

# klu:
target_link_libraries ( my PUBLIC ${KLU_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${KLU_STATIC} )
include_directories ( ${KLU_INCLUDE_DIR} )

# klu_cholmod:
target_link_libraries ( my PUBLIC ${KLU_CHOLMOD_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${KLU_CHOLMOD_STATIC} )
include_directories ( ${KLU_CHOLMOD_INCLUDE_DIR} )

# ldl:
target_link_libraries ( my PUBLIC ${LDL_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${LDL_STATIC} )
include_directories ( ${LDL_INCLUDE_DIR} )

# mongoose:
target_link_libraries ( my PUBLIC ${MONGOOSE_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${MONGOSE_STATIC} )
include_directories ( ${MONGOOSE_INCLUDE_DIR} )

# rbio:
target_link_libraries ( my PUBLIC ${RBIO_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${RBIO_STATIC} )
include_directories ( ${RBIO_INCLUDE_DIR} )

# spex:
target_link_libraries ( my PUBLIC ${SPEX_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${SPEX_STATIC} )
include_directories ( ${SPEX_INCLUDE_DIR} )

# spqr:
target_link_libraries ( my PUBLIC ${SPQR_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${SPQR_STATIC} )
include_directories ( ${SPQR_INCLUDE_DIR} )

# suitesparseconfig:
target_link_libraries ( my PUBLIC ${SUITESPARSE_CONFIG_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${SUITESPARSE_CONFIG_STATIC} )
include_directories ( ${SUITESPARSE_CONFIG_INCLUDE_DIR} )

# suitesparse_gpuruntime:
target_link_libraries ( my PUBLIC ${SUITESPARSE_GPURUNTIME_LIBRARIES} )
target_link_libraries ( my_static PUBLIC ${SUITESPARSE_GPURUNTIME_STATIC} )
include_directories ( ${SUITESPARSE_GPURUNTIME_INCLUDE_DIR} )

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

