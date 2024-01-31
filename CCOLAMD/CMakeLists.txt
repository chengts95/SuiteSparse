#-------------------------------------------------------------------------------
# SuiteSparse/CCOLAMD/CMakeLists.txt:  cmake for CCOLAMD
#-------------------------------------------------------------------------------

# Copyright (c) 2005-2023, Timothy A. Davis.  All Rights Reserved.
# SPDX-License-Identifier: BSD-3-clause

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( CCOLAMD_DATE "Jan 17, 2023" )
set ( CCOLAMD_VERSION_MAJOR 3 )
set ( CCOLAMD_VERSION_MINOR 0 )
set ( CCOLAMD_VERSION_SUB   3 )

message ( STATUS "Building CCOLAMD version: v"
    ${CCOLAMD_VERSION_MAJOR}.
    ${CCOLAMD_VERSION_MINOR}.
    ${CCOLAMD_VERSION_SUB} " (" ${CCOLAMD_DATE} ")" )

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

project ( ccolamd
    VERSION "${CCOLAMD_VERSION_MAJOR}.${CCOLAMD_VERSION_MINOR}.${CCOLAMD_VERSION_SUB}"
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

configure_file ( "Config/ccolamd.h.in"
    "${PROJECT_SOURCE_DIR}/Include/ccolamd.h"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include ${SUITESPARSE_CONFIG_INCLUDE_DIR} )

#-------------------------------------------------------------------------------
# dynamic ccolamd library properties
#-------------------------------------------------------------------------------

file ( GLOB CCOLAMD_SOURCES "Source/*.c" )

add_library ( ccolamd SHARED ${CCOLAMD_SOURCES} )

set_target_properties ( ccolamd PROPERTIES
    VERSION ${CCOLAMD_VERSION_MAJOR}.${CCOLAMD_VERSION_MINOR}.${CCOLAMD_VERSION_SUB}
    C_STANDARD_REQUIRED 11
    SOVERSION ${CCOLAMD_VERSION_MAJOR}
    PUBLIC_HEADER "Include/ccolamd.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

#-------------------------------------------------------------------------------
# static ccolamd library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( ccolamd_static STATIC ${CCOLAMD_SOURCES} )

    set_target_properties ( ccolamd_static PROPERTIES
        VERSION ${CCOLAMD_VERSION_MAJOR}.${CCOLAMD_VERSION_MINOR}.${CCOLAMD_VERSION_SUB}
        OUTPUT_NAME ccolamd
        C_STANDARD_REQUIRED 11
        SOVERSION ${CCOLAMD_VERSION_MAJOR} )

    if ( WIN32 )
        set_target_properties ( ccolamd_static PROPERTIES
            OUTPUT_NAME ccolamd_static )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

target_link_libraries ( ccolamd PUBLIC ${SUITESPARSE_CONFIG_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( ccolamd_static PUBLIC ${SUITESPARSE_CONFIG_STATIC} )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( ccolamd PUBLIC m )
    if ( NOT NSTATIC )
        target_link_libraries ( ccolamd_static PUBLIC m )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# CCOLAMD installation location
#-------------------------------------------------------------------------------
target_include_directories(ccolamd INTERFACE
$<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)

install ( TARGETS ccolamd
    EXPORT ${PROJECT_NAME}
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
# install ( FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/FindCOLAMD.cmake
#     DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse 
#     COMPONENT Development )
if ( NOT NSTATIC )
    install ( TARGETS ccolamd_static
    EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
    target_include_directories(ccolamd_static INTERFACE
        $<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
endif ( )

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

    message ( STATUS "Also compiling the demos in CCOLAMD/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( ccolamd_example   "Demo/ccolamd_example.c" )
    add_executable ( ccolamd_l_example "Demo/ccolamd_l_example.c" )

    # Libraries required for Demo programs
    target_link_libraries ( ccolamd_example   PUBLIC ccolamd )
    target_link_libraries ( ccolamd_l_example PUBLIC ccolamd )

else ( )

    message ( STATUS "Skipping the demos in CCOLAMD/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

