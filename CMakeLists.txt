cmake_minimum_required(VERSION 3.19)
project(SuiteSparse VERSION 7.0.1)
set(PACKAGE_NAME SuiteSparse)
set(LOCAL_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

set(BUILD_SPEX SPEX)

if(MSVC)
    set(BUILD_SPEX "")
endif()

set(TARGETS_LIST SuiteSparse_config
    Mongoose COLAMD AMD BTF CAMD CCOLAMD CHOLMOD
    CXSparse LDL KLU
    UMFPACK
    RBio
    SuiteSparse_GPURuntime GPUQREngine
    SPQR
    GraphBLAS
    ${BUILD_SPEX}
)
option(BLA_STATIC
    "OFF (default): dynamic linking of BLAS.  ON: static linking of BLAS" ON)

set(previous_target "")

include(ExternalProject)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

foreach(TARGET ${TARGETS_LIST})
    ExternalProject_Add(${TARGET}
        PREFIX ${TARGET}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/
        CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_INSTALL_PREFIX:PATH=${LOCAL_INSTALL_PREFIX}/
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/${TARGET}/src/${TARGET}-build --config Release
        INSTALL_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/${TARGET}/src/${TARGET}-build --config Release --target install
        CONFIGURE_COMMAND
        ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DCMAKE_INSTALL_PREFIX:PATH=${LOCAL_INSTALL_PREFIX}/
        -DBLA_STATIC=${BLA_STATIC}
        ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}
    )
 

    if(previous_target)
        add_dependencies(${TARGET} ${previous_target})
    endif()

    set(previous_target ${TARGET})
endforeach()
set(FIND_PACKAGE_COMMANDS "")

# 需要安装的目标列表
set(PROJECT_LIST ${TARGETS_LIST}
)
set(SuiteSparse_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/SuiteSparseConfig.cmake")
set(SuiteSparse_CONFIG_TEMPLATE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SuiteSparseConfig.cmake.in")

foreach(DEP ${PROJECT_LIST})
    set(FIND_PACKAGE_COMMANDS
        "${FIND_PACKAGE_COMMANDS}find_package(${DEP} REQUIRED HINTS 
\"\${CMAKE_CURRENT_LIST_DIR}\")\n")
endforeach()


configure_file(${SuiteSparse_CONFIG_TEMPLATE_FILE} ${SuiteSparse_CONFIG_FILE} @ONLY)

install(FILES ${SuiteSparse_CONFIG_FILE}
    DESTINATION lib/cmake/
)