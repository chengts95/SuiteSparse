#-------------------------------------------------------------------------------
# SuiteSparse/CAMD/CMakeLists.txt:  cmake for CAMD
#-------------------------------------------------------------------------------

# Copyright (c) 2007-2023, Timothy A. Davis.  All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( CAMD_DATE "Jan 17, 2023" )
set ( CAMD_VERSION_MAJOR 3 )
set ( CAMD_VERSION_MINOR 0 )
set ( CAMD_VERSION_SUB   3 )

message ( STATUS "Building CAMD version: v"
    ${CAMD_VERSION_MAJOR}.
    ${CAMD_VERSION_MINOR}.
    ${CAMD_VERSION_SUB} " (" ${CAMD_DATE} ")" )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../SuiteSparse_config/cmake_modules )

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

find_package ( suitesparseconfig 7.0.0 REQUIRED )

set(SUITESPARSE_CONFIG_STATIC SuiteSparse::suitesparseconfig_static)
set(SUITESPARSE_CONFIG_LIBRARIES SuiteSparse::suitesparseconfig)



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

include_directories ( Source Include ${SUITESPARSE_CONFIG_INCLUDE_DIR} )

#-------------------------------------------------------------------------------
# dynamic camd library properties
#-------------------------------------------------------------------------------

file ( GLOB CAMD_SOURCES "Source/*.c" )

add_library ( camd SHARED ${CAMD_SOURCES} )

set_target_properties ( camd PROPERTIES
    VERSION ${CAMD_VERSION_MAJOR}.${CAMD_VERSION_MINOR}.${CAMD_VERSION_SUB}
    C_STANDARD_REQUIRED 11
    SOVERSION ${CAMD_VERSION_MAJOR}
    PUBLIC_HEADER "Include/camd.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

#-------------------------------------------------------------------------------
# static camd library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( camd_static STATIC ${CAMD_SOURCES} )

    set_target_properties ( camd_static PROPERTIES
        VERSION ${CAMD_VERSION_MAJOR}.${CAMD_VERSION_MINOR}.${CAMD_VERSION_SUB}
        C_STANDARD_REQUIRED 11
        OUTPUT_NAME camd
        SOVERSION ${CAMD_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( camd_static PROPERTIES
            OUTPUT_NAME camd_static )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

target_link_libraries ( camd PUBLIC ${SUITESPARSE_CONFIG_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( camd_static PUBLIC ${SUITESPARSE_CONFIG_STATIC} )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( camd PUBLIC m )
    if ( NOT NSTATIC )
        target_link_libraries ( camd_static PUBLIC m )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# CAMD installation location
#-------------------------------------------------------------------------------
target_include_directories(camd INTERFACE
$<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
install ( TARGETS camd
    EXPORT  ${PROJECT_NAME}
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
# install ( FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/FindAMD.cmake
#     DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse 
#     COMPONENT Development )
if ( NOT NSTATIC )
    install ( TARGETS camd_static
        EXPORT  ${PROJECT_NAME}
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
    target_include_directories(camd_static INTERFACE
        $<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
endif ( )
install (EXPORT  ${PROJECT_NAME}
        FILE  ${PROJECT_NAME}Config.cmake
        NAMESPACE SuiteSparse::
        DESTINATION ${SUITESPARSE_LIBDIR}/cmake)
        install (EXPORT  ${PROJECT_NAME}
        FILE  ${PROJECT_NAME}Config.cmake
        NAMESPACE SuiteSparse::
        DESTINATION ${SUITESPARSE_LIBDIR}/cmake)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION  ${SUITESPARSE_LIBDIR}/cmake/)
    # Generate the version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)
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

