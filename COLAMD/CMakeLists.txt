#-------------------------------------------------------------------------------
# SuiteSparse/COLAMD/CMakeLists.txt:  cmake for COLAMD
#-------------------------------------------------------------------------------

# Copyright (c) 1998-2023, Timothy A. Davis.  All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( COLAMD_DATE "June 16, 2023" )
set ( COLAMD_VERSION_MAJOR 3 )
set ( COLAMD_VERSION_MINOR 0 )
set ( COLAMD_VERSION_SUB   4 )

message ( STATUS "Building COLAMD version: v"
    ${COLAMD_VERSION_MAJOR}.
    ${COLAMD_VERSION_MINOR}.
    ${COLAMD_VERSION_SUB} " (" ${COLAMD_DATE} ")" )

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

project ( colamd
    VERSION "${COLAMD_VERSION_MAJOR}.${COLAMD_VERSION_MINOR}.${COLAMD_VERSION_SUB}"
    LANGUAGES C )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

find_package ( SuiteSparse_config 7.1.0 REQUIRED )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/colamd.h.in"
    "${PROJECT_SOURCE_DIR}/Include/colamd.h"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include )

#-------------------------------------------------------------------------------
# dynamic colamd library properties
#-------------------------------------------------------------------------------

file ( GLOB COLAMD_SOURCES "Source/*.c" )

add_library ( colamd SHARED ${COLAMD_SOURCES} )

set_target_properties ( colamd PROPERTIES
    VERSION ${COLAMD_VERSION_MAJOR}.${COLAMD_VERSION_MINOR}.${COLAMD_VERSION_SUB}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${COLAMD_VERSION_MAJOR}
    PUBLIC_HEADER "Include/colamd.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( colamd
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

#-------------------------------------------------------------------------------
# static colamd library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( colamd_static STATIC ${COLAMD_SOURCES} )

    set_target_properties ( colamd_static PROPERTIES
        VERSION ${COLAMD_VERSION_MAJOR}.${COLAMD_VERSION_MINOR}.${COLAMD_VERSION_SUB}
        OUTPUT_NAME colamd
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        SOVERSION ${COLAMD_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( colamd_static PROPERTIES
            OUTPUT_NAME colamd_static )
    endif ( )

    target_include_directories ( colamd_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

target_link_libraries ( colamd PUBLIC suitesparseconfig )
if ( NOT NSTATIC )
    if ( TARGET suitesparseconfig_static )
        target_link_libraries ( colamd_static PUBLIC suitesparseconfig_static )
    else ( )
        target_link_libraries ( colamd_static PUBLIC suitesparseconfig )
    endif ( )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( colamd PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( colamd_static PUBLIC m )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# COLAMD installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

install ( TARGETS colamd
    EXPORT COLAMDTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
if ( NOT NSTATIC )
    install ( TARGETS colamd_static
        EXPORT COLAMDTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT COLAMDTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/COLAMDTargets.cmake )

# install export target, config and version files for find_package
install ( EXPORT COLAMDTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/COLAMD )

configure_package_config_file (
    Config/COLAMDConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/COLAMDConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/COLAMD )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/COLAMDConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/COLAMDConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/COLAMDConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/COLAMD )

#-------------------------------------------------------------------------------
# Demo library and programs
#-------------------------------------------------------------------------------

option ( DEMO "ON: Build the demo programs.  OFF (default): do not build the demo programs." off )
if ( DEMO )

    #---------------------------------------------------------------------------
    # demo library
    #---------------------------------------------------------------------------

    message ( STATUS "Also compiling the demos in COLAMD/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( colamd_example   "Demo/colamd_example.c" )
    add_executable ( colamd_l_example "Demo/colamd_l_example.c" )

    # Libraries required for Demo programs
    target_link_libraries ( colamd_example   PUBLIC colamd )
    target_link_libraries ( colamd_l_example PUBLIC colamd )

else ( )

    message ( STATUS "Skipping the demos in COLAMD/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

