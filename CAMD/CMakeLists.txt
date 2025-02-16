#-------------------------------------------------------------------------------
# SuiteSparse/CAMD/CMakeLists.txt:  cmake for CAMD
#-------------------------------------------------------------------------------

# Copyright (c) 2007-2023, Timothy A. Davis.  All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( CAMD_DATE "June 16, 2023" )
set ( CAMD_VERSION_MAJOR 3 )
set ( CAMD_VERSION_MINOR 0 )
set ( CAMD_VERSION_SUB   4 )

message ( STATUS "Building CAMD version: v"
    ${CAMD_VERSION_MAJOR}.
    ${CAMD_VERSION_MINOR}.
    ${CAMD_VERSION_SUB} " (" ${CAMD_DATE} ")" )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/../SuiteSparse_config/cmake_modules )
set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    ${CMAKE_SOURCE_DIR}/../SuiteSparse_config/build )

include ( SuiteSparsePolicy )

#-------------------------------------------------------------------------------
# define the project
#-------------------------------------------------------------------------------

project ( camd
    VERSION "${CAMD_VERSION_MAJOR}.${CAMD_VERSION_MINOR}.${CAMD_VERSION_SUB}"
    LANGUAGES C )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

find_package ( SuiteSparse_config 7.1.0 REQUIRED )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/camd.h.in"
    "${PROJECT_SOURCE_DIR}/Include/camd.h"
    NEWLINE_STYLE LF )
configure_file ( "Config/camd_version.tex.in"
    "${PROJECT_SOURCE_DIR}/Doc/camd_version.tex"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include )

#-------------------------------------------------------------------------------
# dynamic camd library properties
#-------------------------------------------------------------------------------

file ( GLOB CAMD_SOURCES "Source/*.c" )

add_library ( camd SHARED ${CAMD_SOURCES} )

set_target_properties ( camd PROPERTIES
    VERSION ${CAMD_VERSION_MAJOR}.${CAMD_VERSION_MINOR}.${CAMD_VERSION_SUB}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${CAMD_VERSION_MAJOR}
    PUBLIC_HEADER "Include/camd.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( camd
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

#-------------------------------------------------------------------------------
# static camd library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( camd_static STATIC ${CAMD_SOURCES} )

    set_target_properties ( camd_static PROPERTIES
        VERSION ${CAMD_VERSION_MAJOR}.${CAMD_VERSION_MINOR}.${CAMD_VERSION_SUB}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME camd
        SOVERSION ${CAMD_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( camd_static PROPERTIES
            OUTPUT_NAME camd_static )
    endif ( )

    target_include_directories ( camd_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

target_link_libraries ( camd PUBLIC suitesparseconfig )
if ( NOT NSTATIC )
    if ( TARGET suitesparseconfig_static )
        target_link_libraries ( camd_static PUBLIC suitesparseconfig_static )
    else ( )
        target_link_libraries ( camd_static PUBLIC suitesparseconfig )
    endif ( )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( camd PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( camd_static PUBLIC m )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# CAMD installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

install ( TARGETS camd
    EXPORT CAMDTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
if ( NOT NSTATIC )
    install ( TARGETS camd_static
        EXPORT CAMDTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT CAMDTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/CAMDTargets.cmake )

# install export target, config and version files for find_package
install ( EXPORT CAMDTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/CAMD )

configure_package_config_file (
    Config/CAMDConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CAMDConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/CAMD )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/CAMDConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CAMDConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CAMDConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/CAMD )

#-------------------------------------------------------------------------------
# Demo library and programs
#-------------------------------------------------------------------------------

option ( DEMO "ON: Build the demo programs.  OFF (default): do not build the demo programs." off )
if ( DEMO )

    #---------------------------------------------------------------------------
    # demo library
    #---------------------------------------------------------------------------

    message ( STATUS "Also compiling the demos in CAMD/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( camd_demo   "Demo/camd_demo.c" )
    add_executable ( camd_l_demo "Demo/camd_l_demo.c" )
    add_executable ( camd_demo2  "Demo/camd_demo2.c" )
    add_executable ( camd_simple "Demo/camd_simple.c" )

    # Libraries required for Demo programs
    target_link_libraries ( camd_demo   PUBLIC camd )
    target_link_libraries ( camd_l_demo PUBLIC camd )
    target_link_libraries ( camd_demo2  PUBLIC camd )
    target_link_libraries ( camd_simple PUBLIC camd )

else ( )

    message ( STATUS "Skipping the demos in CAMD/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

