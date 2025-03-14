#-------------------------------------------------------------------------------
# SuiteSparse/SuiteSparse_config/CMakeLists.txt:  cmake for SuiteSparse_config
#-------------------------------------------------------------------------------

# SuiteSparse_config, Copyright (c) 2012-2023, Timothy A. Davis.
# All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

# cmake 3.22 is required to find the BLAS
cmake_minimum_required ( VERSION 3.22 )

# version of both SuiteSparse and SuiteSparse_config
set ( SUITESPARSE_DATE "June 16, 2023" )
set ( SUITESPARSE_VERSION_MAJOR 7 )
set ( SUITESPARSE_VERSION_MINOR 1 )
set ( SUITESPARSE_VERSION_SUB   0 )

message ( STATUS "Building SuiteSparse_config version: v"
    ${SUITESPARSE_VERSION_MAJOR}.
    ${SUITESPARSE_VERSION_MINOR}.
    ${SUITESPARSE_VERSION_SUB} " (" ${SUITESPARSE_DATE} ")" )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules )

include ( SuiteSparsePolicy )

if ( NOT NFORTRAN )
    include ( FortranCInterface )
else ( )
    # No Fortran compiler available or enabled, configuration is not automatic.
    set ( FortranCInterface_GLOBAL_MACRO  ${SUITESPARSE_C_TO_FORTRAN} )
    set ( FortranCInterface_GLOBAL__MACRO ${SUITESPARSE_C_TO_FORTRAN} )
endif ( )

#-------------------------------------------------------------------------------
# define the project
#-------------------------------------------------------------------------------

project ( suitesparseconfig
    VERSION "${SUITESPARSE_VERSION_MAJOR}.${SUITESPARSE_VERSION_MINOR}.${SUITESPARSE_VERSION_SUB}"
    LANGUAGES C )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

option ( NOPENMP "ON: do not use OpenMP.  OFF (default): use OpenMP" off )
if ( NOPENMP )
    # OpenMP has been disabled
    message ( STATUS "OpenMP disabled" )
    set ( OPENMP_FOUND false )
else ( )
    find_package ( OpenMP )
endif ( )

include ( SuiteSparseBLAS )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/SuiteSparse_config.h.in"
    "${PROJECT_SOURCE_DIR}/SuiteSparse_config.h"
    NEWLINE_STYLE LF )

configure_file ( "Config/README.md.in"
    "${PROJECT_SOURCE_DIR}/../README.md"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# dynamic suitesparseconfig library properties
#-------------------------------------------------------------------------------

file ( GLOB SUITESPARSECONFIG_SOURCES "*.c" )

add_library ( suitesparseconfig SHARED ${SUITESPARSECONFIG_SOURCES} )
set_target_properties ( suitesparseconfig PROPERTIES
    VERSION ${SUITESPARSE_VERSION_MAJOR}.${SUITESPARSE_VERSION_MINOR}.${SUITESPARSE_VERSION_SUB}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${SUITESPARSE_VERSION_MAJOR}
    PUBLIC_HEADER "SuiteSparse_config.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( suitesparseconfig
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

#-------------------------------------------------------------------------------
# static suitesparseconfig library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( suitesparseconfig_static STATIC ${SUITESPARSECONFIG_SOURCES} )

    set_target_properties ( suitesparseconfig_static PROPERTIES
        VERSION ${SUITESPARSE_VERSION_MAJOR}.${SUITESPARSE_VERSION_MINOR}.${SUITESPARSE_VERSION_SUB}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME suitesparseconfig
        SOVERSION ${SUITESPARSE_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( suitesparseconfig_static PROPERTIES
            OUTPUT_NAME suitesparseconfig_static )
    endif ( )

    target_include_directories ( suitesparseconfig_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

# libm:
if ( NOT WIN32 )
    target_link_libraries ( suitesparseconfig PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( suitesparseconfig_static PUBLIC m )
    endif ( )
endif ( )

# OpenMP:
if ( OPENMP_FOUND )
    message ( STATUS "OpenMP C libraries:      ${OpenMP_C_LIBRARIES} ")
    message ( STATUS "OpenMP C include:        ${OpenMP_C_INCLUDE_DIRS} ")
    message ( STATUS "OpenMP C flags:          ${OpenMP_C_FLAGS} ")
    target_link_libraries ( suitesparseconfig PRIVATE ${OpenMP_C_LIBRARIES} )
    if ( NOT NSTATIC )
        target_link_libraries ( suitesparseconfig_static PUBLIC ${OpenMP_C_LIBRARIES} )
    endif ( )
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} " )
    include_directories ( ${OpenMP_C_INCLUDE_DIRS} )
endif ( )

# BLAS:
if ( BLAS_FOUND )
    # SuiteSparse_config does not itself require the BLAS.  It just needs to
    # know which BLAS is going to be used by the rest of SuiteSparse so it
    # can configure SuiteSparse_config.h properly.
    message ( STATUS "BLAS libraries:      ${BLAS_LIBRARIES} ")
    message ( STATUS "BLAS linker flags:   ${BLAS_LINKER_FLAGS} ")
    message ( STATUS "BLAS include:        ${BLAS_INCLUDE_DIRS} ")
endif ( )

#-------------------------------------------------------------------------------
# suitesparseconfig installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

file ( GLOB SUITESPARSE_CMAKE_MODULES "cmake_modules/*" )

install ( TARGETS suitesparseconfig
    EXPORT SuiteSparse_configTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
install ( FILES
    ${SUITESPARSE_CMAKE_MODULES}
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse
    COMPONENT Development )
if ( NOT NSTATIC )
    install ( TARGETS suitesparseconfig_static
        EXPORT SuiteSparse_configTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT SuiteSparse_configTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/SuiteSparse_configTargets.cmake )

# install export target and config for find_package
install ( EXPORT SuiteSparse_configTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse_config )

configure_package_config_file (
    Config/SuiteSparse_configConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuiteSparse_configConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse_config )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/SuiteSparse_configConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SuiteSparse_configConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuiteSparse_configConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse_config )

include ( SuiteSparseReport )
