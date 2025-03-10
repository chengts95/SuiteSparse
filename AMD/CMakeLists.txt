#-------------------------------------------------------------------------------
# SuiteSparse/AMD/CMakeLists.txt:  cmake for AMD
#-------------------------------------------------------------------------------

# Copyright (c) 1996-2022, Timothy A. Davis, Patrick Amestoy, Iain Duff.
# All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( AMD_DATE "June 16, 2023" )
set ( AMD_VERSION_MAJOR 3 )
set ( AMD_VERSION_MINOR 0 )
set ( AMD_VERSION_SUB   4 )

message ( STATUS "Building AMD version: v"
    ${AMD_VERSION_MAJOR}.
    ${AMD_VERSION_MINOR}.
    ${AMD_VERSION_SUB} " (" ${AMD_DATE} ")" )

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

if ( WIN32 )
    # disable Fortran in AMD when compiling on Windows
    set ( NFORTRAN true )
endif ( )

if ( NOT NFORTRAN )
    # Fortan is available and enabled
    project ( amd
        VERSION "${AMD_VERSION_MAJOR}.${AMD_VERSION_MINOR}.${AMD_VERSION_SUB}"
        LANGUAGES C Fortran )
else ( )
    # no Fortran compiler available; do not compile Source/*.f or Demo/*.f
    project ( amd
        VERSION "${AMD_VERSION_MAJOR}.${AMD_VERSION_MINOR}.${AMD_VERSION_SUB}"
        LANGUAGES C )
endif ( )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

find_package ( SuiteSparse_config 7.1.0 REQUIRED )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/amd.h.in" "${PROJECT_SOURCE_DIR}/Include/amd.h"
    NEWLINE_STYLE LF )
configure_file ( "Config/amd_version.tex.in" "${PROJECT_SOURCE_DIR}/Doc/amd_version.tex"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include )

#-------------------------------------------------------------------------------
# dynamic amd library properties
#-------------------------------------------------------------------------------

if ( NOT NFORTRAN )
    file ( GLOB AMD_SOURCES "Source/*.c" "Source/*.f" )
else ( )
    file ( GLOB AMD_SOURCES "Source/*.c" )
endif ( )

add_library ( amd SHARED ${AMD_SOURCES} )
set_target_properties ( amd PROPERTIES
    VERSION ${AMD_VERSION_MAJOR}.${AMD_VERSION_MINOR}.${AMD_VERSION_SUB}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${AMD_VERSION_MAJOR}
    PUBLIC_HEADER "Include/amd.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( amd
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

#-------------------------------------------------------------------------------
# static amd library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( amd_static STATIC ${AMD_SOURCES} )
    set_target_properties ( amd_static PROPERTIES
        VERSION ${AMD_VERSION_MAJOR}.${AMD_VERSION_MINOR}.${AMD_VERSION_SUB}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME amd
        SOVERSION ${AMD_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( amd_static PROPERTIES
            OUTPUT_NAME amd_static )
    endif ( )

    target_include_directories ( amd_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

# suitesparseconfig:
target_link_libraries ( amd PUBLIC suitesparseconfig )
if ( NOT NSTATIC )
    if ( TARGET suitesparseconfig_static )
        target_link_libraries ( amd_static PUBLIC suitesparseconfig_static )
    else ( )
        target_link_libraries ( amd_static PUBLIC suitesparseconfig )
    endif ( )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( amd PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( amd_static PUBLIC m )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# AMD installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

install ( TARGETS amd
    EXPORT AMDTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
if ( NOT NSTATIC )
    install ( TARGETS amd_static
        EXPORT AMDTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT AMDTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/AMDTargets.cmake )

# install export target, config and version files for find_package
install ( EXPORT AMDTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/AMD )

configure_package_config_file (
    Config/AMDConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AMDConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/AMD )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/AMDConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AMDConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AMDConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/AMD )

#-------------------------------------------------------------------------------
# Demo library and programs
#-------------------------------------------------------------------------------

option ( DEMO "ON: Build the demo programs.  OFF (default): do not build the demo programs." off )
if ( DEMO )

    #---------------------------------------------------------------------------
    # demo library
    #---------------------------------------------------------------------------

    message ( STATUS "Also compiling the demos in AMD/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( amd_demo      "Demo/amd_demo.c" )
    add_executable ( amd_l_demo    "Demo/amd_l_demo.c" )
    add_executable ( amd_demo2     "Demo/amd_demo2.c" )
    add_executable ( amd_simple    "Demo/amd_simple.c" )
    if ( NOT NFORTRAN )
        add_executable ( amd_f77demo   "Demo/amd_f77demo.f" )
        add_executable ( amd_f77simple "Demo/amd_f77simple.f" )
    endif ( )

    # Libraries required for Demo programs
    target_link_libraries ( amd_demo      PUBLIC amd )
    target_link_libraries ( amd_l_demo    PUBLIC amd )
    target_link_libraries ( amd_demo2     PUBLIC amd )
    target_link_libraries ( amd_simple    PUBLIC amd )
    if ( NOT NFORTRAN )
        target_link_libraries ( amd_f77demo   PUBLIC amd )
        target_link_libraries ( amd_f77simple PUBLIC amd )
    endif ( )

else ( )

    message ( STATUS "Skipping the demos in AMD/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

