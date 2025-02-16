#-------------------------------------------------------------------------------
# GraphBLAS/CMakeLists.txt:  cmake script for GraphBLAS
#-------------------------------------------------------------------------------

# SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# See the User Guide for details on how to compile SuiteSparse:GraphBLAS.

#-------------------------------------------------------------------------------
# get the version
#-------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.16 )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules )

include ( GraphBLAS_version )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

# CUDA is under development for now, and not deployed in production:
  set ( ENABLE_CUDA false )
# set ( ENABLE_CUDA true )

#-------------------------------------------------------------------------------
# SuiteSparse policies
#-------------------------------------------------------------------------------

# GraphBLAS takes a long time to build, so do not build the static library
# by default
set ( NSTATIC_DEFAULT_ON true )

include ( SuiteSparsePolicy )
include ( GraphBLAS_JIT_paths )

#-------------------------------------------------------------------------------
# define the project
#-------------------------------------------------------------------------------

if ( SUITESPARSE_CUDA )
    # FOR NOW: do not compile FactoryKernels when developing the CUDA kernels
    set ( COMPACT on )
    message ( STATUS "GraphBLAS CUDA JIT: enabled")
    project ( graphblas
            VERSION "${GraphBLAS_VERSION_MAJOR}.${GraphBLAS_VERSION_MINOR}.${GraphBLAS_VERSION_SUB}"
            LANGUAGES CUDA C )
else ( )
    message ( STATUS "GraphBLAS CUDA JIT: disabled")
    project ( graphblas
            VERSION "${GraphBLAS_VERSION_MAJOR}.${GraphBLAS_VERSION_MINOR}.${GraphBLAS_VERSION_SUB}"
            LANGUAGES C )
endif ( )

#-------------------------------------------------------------------------------
# find OpenMP, CUDA, RMM, and cpu_features
#-------------------------------------------------------------------------------

if ( SUITESPARSE_CUDA )
    # with CUDA and RMM
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DSUITESPARSE_CUDA" )
    add_subdirectory ( CUDA )
    set ( GB_CUDA graphblascuda  ${CUDA_LIBRARIES} )
    set ( GB_RMM rmm_wrap ${CUDA_LIBRARIES} )
    add_subdirectory ( rmm_wrap )
    include_directories ( "rmm_wrap" ${CUDA_INCLUDE_DIRS}
        "/usr/local/cuda/include/cub" )
    link_directories ( "CUDA" "${CUDA_LIBRARIES}"
        "/usr/local/cuda/lib64/stubs" "rmm_wrap" "/usr/local/cuda/lib64" )
else ( )
    # without CUDA and RMM
    set ( CMAKE_RMM_FLAG " " )
    set ( GB_CUDA )
    set ( GB_RMM )
endif ( )


option ( NOPENMP "ON: do not use OpenMP.  OFF (default): use OpenMP" off )
if ( NOPENMP )
    # OpenMP has been disabled.
    set ( OPENMP_FOUND false )
else ( )
    find_package ( OpenMP )
endif ( )

if ( NOT GBNCPUFEAT )
    # default: enable Google's cpu_features package
    message ( STATUS "cpu_features (by google.com): enabled " )
    include_directories ( "cpu_features/include" "cpu_features" "cpu_features/src" "cpu_features/include/internal" )
else ( )
    # disable Google's cpu_features package
    message ( STATUS "cpu_features (by google.com): disabled" )
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBNCPUFEAT " )
endif ( )

if ( DEFINED GBX86 )
    # default: this is detected automatically, but can be set here also
    if ( GBX86 )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBX86=1 " )
    else ( )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBX86=0 " )
    endif ( )
endif ( )

if ( DEFINED GBAVX2 )
    # default: this is detected automatically, but can be set here also
    if ( GBAVX2 )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBAVX2=1 " )
    else ( )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBAVX2=0 " )
    endif ( )
endif ( )

if ( DEFINED GBAVX512F )
    # default: this is detected automatically, but can be set here also
    if ( GBAVX512F )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBAVX512F=1 " )
    else ( )
        set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DGBAVX512F=0 " )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# determine build type
#-------------------------------------------------------------------------------

# select "true" to build both dynamic and static libraries:
# set ( NOT NSTATIC true )
# set ( NOT NSTATIC false )
# or use cmake with -DNSTATIC=1

if ( NOT NSTATIC )
    message ( STATUS "Building all GraphBLAS libraries (static and dynamic)" )
else ( )
    message ( STATUS "Building dynamic GraphBLAS library" )
endif ( )

#-------------------------------------------------------------------------------
# Configure Include/GraphBLAS.h and documentation with version number
#-------------------------------------------------------------------------------

configure_file ( "Config/GraphBLAS.h.in"
    "${PROJECT_SOURCE_DIR}/Include/GraphBLAS.h"
    NEWLINE_STYLE LF )

configure_file ( "Config/GraphBLAS_version.tex.in"
    "${PROJECT_SOURCE_DIR}/Doc/GraphBLAS_version.tex"
    NEWLINE_STYLE LF )
configure_file ( "Config/GraphBLAS_API_version.tex.in"
    "${PROJECT_SOURCE_DIR}/Doc/GraphBLAS_API_version.tex"
    NEWLINE_STYLE LF )
configure_file ( "Config/README.md.in"
    "${PROJECT_SOURCE_DIR}/README.md"
    NEWLINE_STYLE LF )

# for CUDA
configure_file ( "CUDA/Config/GB_cuda_common_jitFactory.hpp.in"
    "${PROJECT_SOURCE_DIR}/CUDA/GB_cuda_common_jitFactory.hpp"
    NEWLINE_STYLE LF )

#-------------------------------------------------------------------------------
# include directories for both graphblas and the demos
#-------------------------------------------------------------------------------

if ( COMPACT )
    # do not compile the FactoryKernels
    include_directories ( Source/Template Source Include Source/Shared
        Config xxHash Config lz4 zstd zstd/zstd_subset JITpackage
        Source/Factories Demo/Include rmm_wrap Source/JitKernels )
else ( )
    include_directories ( Source/Template Source Include Source/Shared
        Config xxHash Config lz4 zstd zstd/zstd_subset JITpackage
        Source/FactoryKernels
        Source/Factories Demo/Include rmm_wrap Source/JitKernels )
endif ( )

#-------------------------------------------------------------------------------
# compiler options
#-------------------------------------------------------------------------------

include ( GraphBLAS_compiler_options )

#-------------------------------------------------------------------------------
# dynamic graphblas library properties
#-------------------------------------------------------------------------------

# Notes from Sebastien Villemot (sebastien@debian.org):
# SOVERSION policy: if a binary compiled against the old version of the shared
# library needs recompiling in order to work with the new version, then a
# SO_VERSION increase # is needed. Otherwise not.  Examples of the changes that
# require a SO_VERSION increase:
#
#   - a public function or static variable is removed
#   - the prototype of a public function changes
#   - the integer value attached to a public #define or enum changes
#   - the fields of a public structure are modified
#
# Examples of changes that do not require a SO_VERSION increase:
#
#   - a new public function or static variable is added
#   - a private function or static variable is removed or modified
#   - changes in the internals of a structure that is opaque to the calling
#       program (i.e. is only a pointer manipulated through public functions of
#       the library)
#   - a public enum is extended (by adding a new item at the end, but without
#       changing the already existing items)

if ( NJIT )
    if ( COMPACT )
        # no JIT, do not compile the FactoryKernels
        file ( GLOB GRAPHBLAS_SOURCES "Source/*.c" "Config/*.c" "PreJIT/*.c" )
    else ( )
        # no JIT, but do compile the FactoryKernels
        file ( GLOB GRAPHBLAS_SOURCES "Source/*.c" "Config/*.c" "PreJIT/*.c"
            "Source/FactoryKernels/*.c" )
    endif ( )
else ( )
    if ( COMPACT )
        # with the JIT; but do not compile the FactoryKernels
        file ( GLOB GRAPHBLAS_SOURCES "Source/*.c" "Config/*.c" "PreJIT/*.c" 
            "JITpackage/*.c" )
    else ( )
        # with the JIT and the FactoryKernels
        file ( GLOB GRAPHBLAS_SOURCES "Source/*.c" "Config/*.c" "PreJIT/*.c"
            "JITpackage/*.c" "Source/FactoryKernels/*.c" )
    endif ( )
endif ( )

add_library ( graphblas SHARED ${GRAPHBLAS_SOURCES} )

set_target_properties ( graphblas PROPERTIES
    VERSION ${GraphBLAS_VERSION_MAJOR}.${GraphBLAS_VERSION_MINOR}.${GraphBLAS_VERSION_SUB}
    SOVERSION ${GraphBLAS_VERSION_MAJOR}
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    PUBLIC_HEADER "Include/GraphBLAS.h"
    WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_include_directories ( graphblas
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

if ( SUITESPARSE_CUDA )
    add_dependencies ( graphblas graphblascuda )
#   add_dependencies ( graphblas rmm_wrap )
endif ( )

if ( WIN32 )
    # Compiling the graphblas dll on Windows: export the dll symbols
    target_compile_definitions ( graphblas PRIVATE GB_DLL_EXPORT )
endif ( )

#-------------------------------------------------------------------------------
# static graphblas library properties
#-------------------------------------------------------------------------------

if ( NOT NSTATIC )

    add_library ( graphblas_static STATIC ${GRAPHBLAS_SOURCES} )

    set_target_properties ( graphblas_static PROPERTIES
       VERSION ${GraphBLAS_VERSION_MAJOR}.${GraphBLAS_VERSION_MINOR}.${GraphBLAS_VERSION_SUB}
       OUTPUT_NAME graphblas
       SOVERSION ${GraphBLAS_VERSION_MAJOR}
        C_STANDARD 11
        C_STANDARD_REQUIRED ON )

    if ( MSVC )
        set_target_properties ( graphblas_static PROPERTIES
            OUTPUT_NAME graphblas_static )
    endif ( )

    target_include_directories ( graphblas_static
        INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )

    if ( SUITESPARSE_CUDA )
        add_dependencies ( graphblas_static graphblascuda )
#       add_dependencies ( graphblas_static rmm_wrap )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# cpu_features settings
#-------------------------------------------------------------------------------

if ( NOT GBNCPUFEAT )
    if ( UNIX )
        # look for requirements for cpu_features/src/hwcaps.c
        include ( CheckIncludeFile )
        include ( CheckSymbolExists )
        check_include_file ( dlfcn.h HAVE_DLFCN_H )
        if ( HAVE_DLFCN_H )
            message ( STATUS "cpu_feautures has dlfcn.h" )
            target_compile_definitions ( graphblas PRIVATE HAVE_DLFCN_H )
            if ( NOT NSTATIC )
                target_compile_definitions ( graphblas_static PRIVATE HAVE_DLFCN_H )
            endif ( )
        else ( )
            message ( STATUS "cpu_feautures without dlfcn.h" )
        endif ( )
        check_symbol_exists ( getauxval "sys/auxv.h" HAVE_STRONG_GETAUXVAL )
        if ( HAVE_STRONG_GETAUXVAL )
            message ( STATUS "cpu_feautures has getauxval from sys/auxv.h" )
            target_compile_definitions ( graphblas PRIVATE HAVE_STRONG_GETAUXVAL )
            if ( NOT NSTATIC )
                target_compile_definitions ( graphblas_static PRIVATE HAVE_STRONG_GETAUXVAL )
            endif ( )
        else ( )
            message ( STATUS "cpu_feautures doesn't have getauxval from sys/auxv.h" )
        endif ( )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# select the math library (not required for Microsoft Visual Studio)
#-------------------------------------------------------------------------------

# libm:
if ( NOT WIN32 )
    target_link_libraries ( graphblas PRIVATE m )
    if ( NOT NSTATIC )
        target_link_libraries ( graphblas_static PUBLIC m )
    endif ( )
endif ( )

# libdl
if ( NOT WIN32 )
    target_link_libraries ( graphblas PRIVATE dl )
    if ( NOT NSTATIC )
        target_link_libraries ( graphblas_static PUBLIC dl )
    endif ( )
endif ( )

# atomic
include ( SuiteSparseAtomic )
if ( LIBATOMIC_REQUIRED )
    target_link_libraries ( graphblas PRIVATE atomic )
    if ( NOT NSTATIC )
        target_link_libraries ( graphblas_static PUBLIC atomic )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# add the OpenMP, IPP, CUDA, BLAS, etc libraries
#-------------------------------------------------------------------------------

if ( OPENMP_FOUND )
    message ( STATUS "CMAKE OpenMP libraries:    " ${OpenMP_C_LIBRARIES} )
    message ( STATUS "CMAKE OpenMP include:      " ${OpenMP_C_INCLUDE_DIRS} )
    message ( STATUS "CMAKE OpenMP C flags:      " ${OpenMP_C_FLAGS} )
    target_link_libraries ( graphblas PRIVATE OpenMP::OpenMP_C )
    if ( NOT NSTATIC )
        target_link_libraries ( graphblas_static PUBLIC OpenMP::OpenMP_C )
    endif ( )
else ( )
    message  ( WARNING
    "WARNING:  OpenMP was not found (or was disabled with NOPENMP).  See the "
    "GrapBLAS user guide on the consequences of compiling GraphBLAS without "
    "OpenMP. GraphBLAS will work but may not be thread-safe, since it relies "
    "on '#pragma omp flush' to ensure the work performed by one user thread is "
    "available to another, in GrB_wait.  If OpenMP is not in use, the "
    "thread-safety of GrB_wait becomes the responsibilty of the user "
    "application (perhaps through a pthreads construct).  Compiling GraphBLAS "
    "without OpenMP is not recommended for installation in a package manager "
    "(Linux, conda-forge, spack, brew, vcpkg, etc). " )
    # check for __thread, __declspec(thread), and _Thread_local keywords
    include ( SuiteSparse__thread )
    if ( NOT HAVE_KEYWORD__DECLSPEC_THREAD AND NOT HAVE_KEYWORD__THREAD )
        message  ( WARNING "The C compiler supports neither the __thread "
        "nor the __declspec(thread) keyword for thread-local-storage; "
        "GxB_Context_engage will return GrB_NOT_IMPLEMENTED." )
    endif ( )
endif ( )

# get the current library list, before linking with cuda and rmm
get_target_property ( GB_CMAKE_LIBRARIES graphblas LINK_LIBRARIES )
if ( NOT GB_CMAKE_LIBRARIES )
    set ( GB_CMAKE_LIBRARIES "" )
endif ( )

if ( SUITESPARSE_CUDA )
    target_link_libraries ( graphblas PRIVATE ${GB_CUDA} ${GB_RMM} )
    if ( NOT NSTATIC )
        target_link_libraries ( graphblas_static PUBLIC ${GB_CUDA} ${GB_RMM} )
    endif ( )
endif ( )

#-------------------------------------------------------------------------------
# print final C flags
#-------------------------------------------------------------------------------

message ( STATUS "CMAKE C flags: ${CMAKE_C_FLAGS} " )

#-------------------------------------------------------------------------------
# Demo programs
#-------------------------------------------------------------------------------

option ( DEMO "ON: Build the demo programs.  OFF (default): do not build the demo programs." off )
if ( DEMO )

    message ( STATUS "Also compiling the demos in GraphBLAS/Demo" )

    #---------------------------------------------------------------------------
    # Demo programs
    #---------------------------------------------------------------------------

    add_executable ( openmp_demo   "Demo/Program/openmp_demo.c" )
    add_executable ( openmp2_demo  "Demo/Program/openmp2_demo.c" )
    add_executable ( complex_demo  "Demo/Program/complex_demo.c" )
    add_executable ( kron_demo     "Demo/Program/kron_demo.c" )
    add_executable ( simple_demo   "Demo/Program/simple_demo.c" )
    add_executable ( wildtype_demo "Demo/Program/wildtype_demo.c" )
    add_executable ( reduce_demo   "Demo/Program/reduce_demo.c" )
    add_executable ( import_demo   "Demo/Program/import_demo.c" )
    add_executable ( wathen_demo   "Demo/Program/wathen_demo.c" )
    add_executable ( context_demo  "Demo/Program/context_demo.c" )
    add_executable ( gauss_demo    "Demo/Program/gauss_demo.c" )

    # Libraries required for Demo programs
    target_link_libraries ( openmp_demo   PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( openmp2_demo  PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( complex_demo  PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( kron_demo     PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( simple_demo   PUBLIC graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( wildtype_demo PUBLIC graphblas ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( reduce_demo   PUBLIC graphblas ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( import_demo   PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    target_link_libraries ( wathen_demo   PUBLIC graphblas graphblasdemo ${GB_CUDA} ${GB_RMM} )
    if ( OPENMP_FOUND )
        target_link_libraries ( openmp_demo   PUBLIC OpenMP::OpenMP_C )
        target_link_libraries ( openmp2_demo  PUBLIC OpenMP::OpenMP_C )
        target_link_libraries ( reduce_demo   PUBLIC OpenMP::OpenMP_C )
        target_link_libraries ( wathen_demo   PUBLIC OpenMP::OpenMP_C )
    endif ( )
else ( )

    message ( STATUS "Skipping the demos in GraphBLAS/Demo" )

endif ( )

#-------------------------------------------------------------------------------
# installation location
#-------------------------------------------------------------------------------

include ( CMakePackageConfigHelpers )

install ( TARGETS graphblas
    EXPORT GraphBLASTargets
    LIBRARY DESTINATION ${SUITESPARSE_LIBDIR}
    ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR}
    RUNTIME DESTINATION ${SUITESPARSE_BINDIR}
    PUBLIC_HEADER DESTINATION ${SUITESPARSE_INCLUDEDIR} )
if ( NOT NSTATIC )
    install ( TARGETS graphblas_static
        EXPORT GraphBLASTargets
        ARCHIVE DESTINATION ${SUITESPARSE_LIBDIR} )
endif ( )

# create (temporary) export target file during build
export ( EXPORT GraphBLASTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/GraphBLASTargets.cmake )

# install export target and config for find_package
install ( EXPORT GraphBLASTargets
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/GraphBLAS )

configure_package_config_file (
    Config/GraphBLASConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GraphBLASConfig.cmake
    INSTALL_DESTINATION ${SUITESPARSE_LIBDIR}/cmake/GraphBLAS )

write_basic_package_version_file (
    ${CMAKE_CURRENT_BINARY_DIR}/GraphBLASConfigVersion.cmake
    COMPATIBILITY SameMajorVersion )

install ( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GraphBLASConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GraphBLASConfigVersion.cmake
    DESTINATION ${SUITESPARSE_LIBDIR}/cmake/GraphBLAS )

#-------------------------------------------------------------------------------
# configure the JITs
#-------------------------------------------------------------------------------

include ( GraphBLAS_JIT_configure )

configure_file ( "Config/GB_config.h.in"
    "${PROJECT_SOURCE_DIR}/Config/GB_config.h"
    NEWLINE_STYLE LF )

include ( GraphBLAS_PreJIT )

#-------------------------------------------------------------------------------
# dump all variables, for debugging only
#-------------------------------------------------------------------------------

# uncomment this line or add -DDUMP=true to your cmake call to dump all cmake variables:
# set ( DUMP true )

if ( DUMP )
    message("====")
    message("Begin dumping all CMake variables in alphabetic order")
    message("====")
    get_cmake_property ( _variableNames VARIABLES )
    list ( SORT _variableNames )
    foreach ( _variableName ${_variableNames} )
        message ( STATUS "${_variableName}=${${_variableName}}" )
    endforeach ( )
    message("====")
    message("End dumping all CMake variables in alphabetic order")
    message("====")
endif ( )

#-------------------------------------------------------------------------------
# report status
#-------------------------------------------------------------------------------

include ( SuiteSparseReport )
