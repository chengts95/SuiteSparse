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

set ( KLU_DATE "Jan 17, 2023" )
set ( KLU_VERSION_MAJOR 2 )
set ( KLU_VERSION_MINOR 0 )
set ( KLU_VERSION_SUB   3 )

message ( STATUS "Building KLU version: v"
    ${KLU_VERSION_MAJOR}.
    ${KLU_VERSION_MINOR}.
    ${KLU_VERSION_SUB} " (" ${KLU_DATE} ")" )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
    # ${CMAKE_CURRENT_SOURCE_DIR}/../BTF/cmake_modules
    # ${CMAKE_CURRENT_SOURCE_DIR}/../AMD/cmake_modules
    # ${CMAKE_CURRENT_SOURCE_DIR}/../COLAMD/cmake_modules
    # ${CMAKE_CURRENT_SOURCE_DIR}/../CAMD/cmake_modules
    # ${CMAKE_CURRENT_SOURCE_DIR}/../CCOLAMD/cmake_modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHOLMOD/cmake_modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../SuiteSparse_config/cmake_modules )

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

find_package ( suitesparseconfig 7.0.0 REQUIRED )

set(SUITESPARSE_CONFIG_STATIC SuiteSparse::suitesparseconfig_static)
set(SUITESPARSE_CONFIG_LIBRARIES SuiteSparse::suitesparseconfig)


find_package ( btf 2.0.3 REQUIRED )
set(BTF_STATIC SuiteSparse::btf_static)
set(BTF_LIBRARIES SuiteSparse::btf)
find_package ( colamd 3.0.3 REQUIRED )
set(COLAMD_STATIC SuiteSparse::colamd_static)
set(COLAMD_LIBRARIES SuiteSparse::colamd)
find_package ( amd 3.0.3 REQUIRED )
set(AMD_STATIC SuiteSparse::amd_static)
set(AMD_LIBRARIES SuiteSparse::amd)


option ( NCHOLMOD "ON: do not use CHOLMOD.  OFF (default): use CHOLMOD" off )

if ( NOT NCHOLMOD )
    # look for CHOLMOD (optional fill-reducing orderings)
    find_package ( cholmod 4.0.3 )
    set(CHOLMOD_STATIC SuiteSparse::cholmod_static)
    set(CHOLMOD_LIBRARIES SuiteSparse::cholmod)
    find_package ( CHOLMOD_CUDA 4.0.3 )
    # look for CHOLMOD's dependencies: AMD and COLAMD are required.  CAMD and
    # CCOLAMD are optional, but must be found if CHOLMOD was built with them.
    find_package ( camd 3.0.3 )
    set(CAMD_STATIC SuiteSparse::camd_static)
    set(CAMD_LIBRARIES SuiteSparse::camd)
    find_package ( ccolamd 3.0.3 )
    set(CCOLAMD_STATIC SuiteSparse::ccolamd_static)
    set(CCOLAMD_LIBRARIES SuiteSparse::ccolamd)
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

include_directories ( Source Include User ${SUITESPARSE_CONFIG_INCLUDE_DIR}
    ${AMD_INCLUDE_DIR} ${COLAMD_INCLUDE_DIR} ${BTF_INCLUDE_DIR} )

#-------------------------------------------------------------------------------
# dynamic klu library properties
#-------------------------------------------------------------------------------

file ( GLOB KLU_SOURCES "Source/*.c" )

add_library ( klu SHARED ${KLU_SOURCES} )

set_target_properties ( klu PROPERTIES
    VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
    C_STANDARD_REQUIRED 11
    SOVERSION ${KLU_VERSION_MAJOR}
    PUBLIC_HEADER "Include/klu.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

#-------------------------------------------------------------------------------
# static klu library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )
    add_library ( klu_static STATIC ${KLU_SOURCES} )

    set_target_properties ( klu_static PROPERTIES
        VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
        C_STANDARD_REQUIRED 11
        OUTPUT_NAME klu
        SOVERSION ${KLU_VERSION_MAJOR} )

    if ( MSVC )
        set_target_properties ( klu_static PROPERTIES
            OUTPUT_NAME klu_static )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# klu_cholmod library properties
#-------------------------------------------------------------------------------

if ( NOT NCHOLMOD )

    file ( GLOB KLU_CHOLMOD_SOURCES "User/*.c" )

    add_library ( klu_cholmod SHARED ${KLU_CHOLMOD_SOURCES} )
    include_directories ( ${CHOLMOD_INCLUDE_DIR} )

    set_target_properties ( klu_cholmod PROPERTIES
        VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
        C_STANDARD_REQUIRED 11
        SOVERSION ${KLU_VERSION_MAJOR}
        PUBLIC_HEADER "User/klu_cholmod.h" )

    if ( NOT NSTATIC )
        add_library ( klu_cholmod_static STATIC ${KLU_CHOLMOD_SOURCES} )

        set_target_properties ( klu_cholmod_static PROPERTIES
            VERSION ${KLU_VERSION_MAJOR}.${KLU_VERSION_MINOR}.${KLU_VERSION_SUB}
            C_STANDARD_REQUIRED 11
            OUTPUT_NAME klu_cholmod
            SOVERSION ${KLU_VERSION_MAJOR} )

        if ( MSVC )
            set_target_properties ( klu_cholmod_static PROPERTIES
                OUTPUT_NAME klu_cholmod_static )
        endif ( )
    endif ( )

endif ( )

#-------------------------------------------------------------------------------
# add the library dependencies
#-------------------------------------------------------------------------------

# suitesparseconfig:
target_link_libraries ( klu PUBLIC ${SUITESPARSE_CONFIG_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( klu_static PUBLIC ${SUITESPARSE_CONFIG_STATIC} )
endif ( )

# libm:
if ( NOT WIN32 )
    target_link_libraries ( klu PUBLIC m )
    if ( NOT NSTATIC )
        target_link_libraries ( klu_static PUBLIC m )
    endif ( )
endif ( )

# amd:
target_link_libraries ( klu PUBLIC ${AMD_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( klu_static PUBLIC ${AMD_STATIC} )
endif ( )

# colamd:
target_link_libraries ( klu PUBLIC ${COLAMD_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( klu_static PUBLIC ${COLAMD_STATIC} )
endif ( )

# btf:
target_link_libraries ( klu PUBLIC ${BTF_LIBRARIES} )
if ( NOT NSTATIC )
    target_link_libraries ( klu_static PUBLIC ${BTF_STATIC} )
endif ( )

if ( NOT NCHOLMOD )

    # cholmod:
    # link with CHOLMOD and its dependencies, both required and optional
    target_link_libraries ( klu PUBLIC
        ${CHOLMOD_LIBRARIES} ${CHOLMOD_CUDA_LIBRARIES}
        ${AMD_LIBRARIES} ${COLAMD_LIBRARIES}
        ${CAMD_LIBRARIES} ${CCOLAMD_LIBRARIES} )
    target_link_libraries ( klu_cholmod PUBLIC
        ${CHOLMOD_LIBRARIES} ${CHOLMOD_CUDA_LIBRARIES}
        ${AMD_LIBRARIES} ${COLAMD_LIBRARIES}
        ${CAMD_LIBRARIES} ${CCOLAMD_LIBRARIES} )
    if ( NOT NSTATIC )
        target_link_libraries ( klu_static PUBLIC
            ${CHOLMOD_STATIC} ${CHOLMOD_CUDA_STATIC}
            ${AMD_STATIC} ${COLAMD_STATIC}
            ${CAMD_STATIC} ${CCOLAMD_STATIC} )
        target_link_libraries ( klu_cholmod_static PUBLIC
            ${CHOLMOD_STATIC} ${CHOLMOD_CUDA_STATIC}
            ${AMD_STATIC} ${COLAMD_STATIC}
            ${CAMD_STATIC} ${CCOLAMD_STATIC} )
    endif ( )

    # klu:
    target_link_libraries ( klu_cholmod PUBLIC klu ${BTF_LIBRARIES} )
    if ( NOT NSTATIC )
        target_link_libraries ( klu_cholmod_static PUBLIC
            klu_static ${BTF_STATIC} )
    endif ( )

endif ( )

#-------------------------------------------------------------------------------
# KLU installation location
#-------------------------------------------------------------------------------
target_include_directories( klu INTERFACE
$<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
install ( TARGETS klu
    EXPORT ${PROJECT_NAME}Config
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
# install ( FILES
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/FindKLU.cmake
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/FindKLU_CHOLMOD.cmake
#     DESTINATION ${SUITESPARSE_LIBDIR}/cmake/SuiteSparse 
#     COMPONENT Development )

if ( NOT NSTATIC )
target_include_directories( klu_static INTERFACE
$<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
    install ( TARGETS klu_static
        EXPORT ${PROJECT_NAME}Config
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

if ( NOT NCHOLMOD )
target_include_directories(klu_cholmod INTERFACE
$<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)

    install ( TARGETS klu_cholmod
        EXPORT ${PROJECT_NAME}Config
        LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
        RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
        PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
    if ( NOT NSTATIC )
    target_include_directories( klu_cholmod_static INTERFACE
        $<INSTALL_INTERFACE:${SUITESPARSE_INCLUDEDIR}>)
        install ( TARGETS klu_cholmod_static
        EXPORT ${PROJECT_NAME}Config
            ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
    endif ( )
endif ( )



install (EXPORT  ${PROJECT_NAME}Config
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
        target_link_libraries ( kludemo  PUBLIC klu_cholmod )
        target_link_libraries ( kluldemo PUBLIC klu_cholmod )
    endif ( )

else ( )

    message ( STATUS "Skipping the demos in KLU/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )

