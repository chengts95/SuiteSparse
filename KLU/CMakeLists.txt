#-------------------------------------------------------------------------------
# SuiteSparse/KLU/CMakeLists.txt:  cmake for KLU
#-------------------------------------------------------------------------------

# KLU, Copyright (c) 2004-2022, University of Florida.  All Rights Reserved.
# Authors: Timothy A. Davis and Ekanathan Palamadai.
# SPDX-License-Identifier: LGPL-2.1+

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.19 )

set ( KLU_DATE "June 16, 2023" )
set ( KLU_VERSION_MAJOR 2 )
set ( KLU_VERSION_MINOR 0 )
set ( KLU_VERSION_SUB   4 )

message ( STATUS "Building KLU version: v"
    ${KLU_VERSION_MAJOR}.
    ${KLU_VERSION_MINOR}.
    ${KLU_VERSION_SUB} " (" ${KLU_DATE} ")" )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/../SuiteSparse_config/cmake_modules )
set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    ${CMAKE_SOURCE_DIR}/../AMD/build
    ${CMAKE_SOURCE_DIR}/../BTF/build
    ${CMAKE_SOURCE_DIR}/../CAMD/build
    ${CMAKE_SOURCE_DIR}/../CCOLAMD/build
    ${CMAKE_SOURCE_DIR}/../CHOLMOD/build
    ${CMAKE_SOURCE_DIR}/../COLAMD/build
    ${CMAKE_SOURCE_DIR}/../SuiteSparse_config/build )

include ( SuiteSparsePolicy )

#-------------------------------------------------------------------------------
# define the project
#-------------------------------------------------------------------------------

project ( klu
    VERSION "${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}"
    LANGUAGES C )

#-------------------------------------------------------------------------------
# find library dependencies
#-------------------------------------------------------------------------------

find_package ( SuiteSparse_config 7.1.0 REQUIRED )
find_package ( BTF 2.0.4 REQUIRED )
find_package ( COLAMD 3.0.4 REQUIRED )
find_package ( AMD 3.0.4 REQUIRED )

option ( NCHOLMOD "ON: do not use CHOLMOD.  OFF (default): use CHOLMOD" off )

if ( NOT NCHOLMOD )
    # look for CHOLMOD (optional fill-reducing orderings)
    find_package ( CHOLMOD 4.0.4 )
    find_package ( CHOLMOD_CUDA 4.0.4 )
    # look for CHOLMOD's dependencies: AMD and COLAMD are required.  CAMD and
    # CCOLAMD are optional, but must be found if CHOLMOD was built with them.
    find_package ( CAMD 3.0.4 )
    find_package ( CCOLAMD 3.0.4 )
    if ( NOT CHOLMOD_FOUND OR NOT AMD_FOUND OR NOT COLAMD_FOUND )
        # CHOLMOD not found so disable it
        set ( NCHOLMOD true )
    endif ( )
endif ( )

if ( NCHOLMOD )
    # tell KLU that CHOLMOD is not available
    message ( STATUS "CHOLMOD not found or not requested" )
    add_compile_definitions ( NCHOLMOD )
else ( )
    message ( STATUS "Using CHOLMOD for addtional pre-ordering options" )
endif ( )

#-------------------------------------------------------------------------------
# configure files
#-------------------------------------------------------------------------------

configure_file ( "Config/klu.h.in"
    "${PROJECT_SOURCE_DIR}/Include/klu.h"
    NEWLINE_STYLE LF )
configure_file ( "Config/klu_version.tex.in"
    "${PROJECT_SOURCE_DIR}/Doc/klu_version.tex"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories
#-------------------------------------------------------------------------------

include_directories ( Source Include User )

#-------------------------------------------------------------------------------
# dynamic klu library properties
#-------------------------------------------------------------------------------

file ( GLOB KLU_SOURCES "Source/*.c" )

add_library ( klu SHARED ${KLU_SOURCES} )

set_target_properties ( klu PROPERTIES
    VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    SOVERSION ${KLU_VERSION_MAJOR}
    PUBLIC_HEADER "Include/klu.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( klu 
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

#-------------------------------------------------------------------------------
# static klu library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( klu_static STATIC ${KLU_SOURCES} )

    set_target_properties ( klu_static PROPERTIES
        VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        OUTPUT_NAME klu
        SOVERSION ${KLU_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( klu_static PROPERTIES
            OUTPUT_NAME klu_static )
    endif ( )

    target_include_directories ( klu_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

endif ( )

#-------------------------------------------------------------------------------
# klu_cholmod library properties
#-------------------------------------------------------------------------------

if ( NOT NCHOLMOD )

    file ( GLOB KLU_CHOLMOD_SOURCES "User/*.c" )

    add_library ( klu_cholmod SHARED ${KLU_CHOLMOD_SOURCES} )

    set_target_properties ( klu_cholmod PROPERTIES
        VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        SOVERSION ${KLU_VERSION_MAJOR}
        PUBLIC_HEADER "User/klu_cholmod.h" )

    target_include_directories ( klu_cholmod 
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

    if ( NOT NSTATIC )
        add_library ( klu_cholmod_static STATIC ${KLU_CHOLMOD_SOURCES} )

        set_target_properties ( klu_cholmod_static PROPERTIES
            VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
            C_STANDARD 11
            C_STANDARD_REQUIRED ON
            OUTPUT_NAME klu_cholmod
            SOVERSION ${KLU_VERSION_MAJOR} )

        if ( MSVC )
            set_target_properties ( klu_cholmod_static PROPERTIES
                OUTPUT_NAME klu_cholmod_static )
        endif ( )

        target_include_directories ( klu_cholmod_static 
            INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

    endif ( )

endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

# suitesparseconfig:
target_link_libraries ( klu PUBLIC suitesparseconfig )
if ( NOT NSTATIC )
    if ( TARGET suitesparseconfig_static )
        target_link_libraries ( klu_static PUBLIC suitesparseconfig_static )
    else ( )
        target_link_libraries ( klu_static PUBLIC suitesparseconfig )
    endif ( )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( klu PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( klu_static PUBLIC m )
    endif ( )
endif ( )

# amd:
target_link_libraries ( klu PUBLIC amd )
if ( NOT NSTATIC )
    if ( TARGET amd_static )
        target_link_libraries ( klu_static PUBLIC amd_static )
    else ( )
        target_link_libraries ( klu_static PUBLIC amd )
    endif ( )
endif ( )

# colamd:
target_link_libraries ( klu PUBLIC colamd )
if ( NOT NSTATIC )
    if ( TARGET colamd_static )
        target_link_libraries ( klu_static PUBLIC colamd_static )
    else ( )
        target_link_libraries ( klu_static PUBLIC colamd )
    endif ( )
endif ( )

# btf:
target_link_libraries ( klu PUBLIC btf )
if ( NOT NSTATIC )
    if ( TARGET btf_static )
        target_link_libraries ( klu_static PUBLIC btf_static )
    else ( )
        target_link_libraries ( klu_static PUBLIC btf )
    endif ( )
endif ( )

if ( NOT NCHOLMOD )

    target_link_libraries ( klu_cholmod PUBLIC
        cholmod cholmod_cuda )
    if ( NOT NSTATIC )
       if ( TARGET cholmod_static )
            target_link_libraries ( klu_static PUBLIC cholmod_static )
            target_link_libraries ( klu_cholmod_static PUBLIC cholmod_static )
        else ( )
            target_link_libraries ( klu_static PUBLIC cholmod )
            target_link_libraries ( klu_cholmod_static PUBLIC cholmod )
        endif ( )
        if ( TARGET cholmod_cuda_static )
            target_link_libraries ( klu_cholmod_static PUBLIC cholmod_cuda_static )
        else ( )
            target_link_libraries ( klu_cholmod_static PUBLIC cholmod_cuda )
        endif ( )
    endif ( )

    # klu:
    target_link_libraries ( klu_cholmod PUBLIC klu btf )
    if ( NOT NSTATIC )
        if ( TARGET btf_static )
            target_link_libraries ( klu_cholmod_static PUBLIC btf_static )
        else ( )
            target_link_libraries ( klu_cholmod_static PUBLIC btf )
        endif ( )
    endif ( )

endif ( )

#-------------------------------------------------------------------------------
# KLU installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

install ( TARGETS klu
    EXPORT KLUTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )

if ( NOT NSTATIC )
    install ( TARGETS klu_static
        EXPORT KLUTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT KLUTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/KLUTargets.cmake )

# install export target, config and version files for find_package
install ( EXPORT KLUTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU )

configure_package_config_file (
    Config/KLUConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KLUConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/KLUConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KLUConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/KLUConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU )

if ( NOT NCHOLMOD )
    install ( TARGETS klu_cholmod
        EXPORT KLU_CHOLMODTargets
        LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
        RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
        PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
    if ( NOT NSTATIC )
        install ( TARGETS klu_cholmod_static
            EXPORT KLU_CHOLMODTargets
            ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
    endif ( )

    # create (temporary) export target file during build
    export ( EXPORT KLU_CHOLMODTargets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/KLU_CHOLMODTargets.cmake )

    # install export target, config and version files for find_package
    install ( EXPORT KLU_CHOLMODTargets
        DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU_CHOLMOD )

    configure_package_config_file (
        Config/KLU_CHOLMODConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/KLU_CHOLMODConfig.cmake
        INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU_CHOLMOD )

    write_basic_package_version_file (
        ${CMAKE_CURRENT_BINARY_DIR}/KLU_CHOLMODConfigVersion.cmake
        COMPATIBILITY SameMajorVersion )

    install ( FILES
        ${CMAKE_CURRENT_BINARY_DIR}/KLU_CHOLMODConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/KLU_CHOLMODConfigVersion.cmake
        DESTINATION ${SUITESPARSE_LIBDIR}/cmake/KLU_CHOLMOD )

endif ( )

#-------------------------------------------------------------------------------
# Demo library and programs
#-------------------------------------------------------------------------------

option ( DEMO "ON: Build the demo programs.  OFF (default): do not build the demo programs." off )
if ( DEMO )

    #---------------------------------------------------------------------------
    # demo library
    #---------------------------------------------------------------------------

    message ( STATUS "Also compiling the demos in KLU/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( klu_simple  "Demo/klu_simple.c" )
    if ( NOT NCHOLMOD )
        add_executable ( kludemo     "Demo/kludemo.c" )
        add_executable ( kluldemo    "Demo/kluldemo.c" )
    endif ( )

    # Libraries required for Demo programs
    target_link_libraries ( klu_simple  PUBLIC klu )
    if ( NOT NCHOLMOD )
        target_link_libraries ( kludemo  PUBLIC klu_cholmod ${CHOLMOD_LIBRARIES} )
        target_link_libraries ( kluldemo PUBLIC klu_cholmod ${CHOLMOD_LIBRARIES} )
        if ( NOT WIN32 )
            target_link_libraries ( kludemo  PUBLIC m )
            target_link_libraries ( kluldemo PUBLIC m )
        endif ( )
    endif ( )

else ( )

    message ( STATUS "Skipping the demos in KLU/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

