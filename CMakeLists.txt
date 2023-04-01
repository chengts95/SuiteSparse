cmake_minimum_required(VERSION 3.19)

if(WIN32)
  set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
  set(CMAKE_TOOLCHAIN_FILE D:/vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")
  set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
else()
  set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
  set(CMAKE_TOOLCHAIN_FILE ../vcpkg/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "Vcpkg toolchain file")
endif()
#set(ENV{ENABLE_CUDA} FALSE)

project(SuiteSparse)
set(PACKAGE_NAME SuiteSparse)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(BUILD_SPEX "SPEX")
if(MSVC)
set(BUILD_SPEX "")

endif()
# 需要安装的目标列表
set(TARGETS_LIST SuiteSparse_config
 Mongoose COLAMD  AMD  BTF CAMD CCOLAMD CHOLMOD
  CXSparse LDL KLU 
  UMFPACK 
  RBio 
  SuiteSparse_GPURuntime GPUQREngine 
  SPQR  
  GraphBLAS 
  ${BUILD_SPEX} 
 )
list(APPEND ${CMAKE_MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/lib/cmake)
list(APPEND ${CMAKE_PREFIX_PATH} ${CMAKE_INSTALL_PREFIX}/lib/cmake)
set(TOP_CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH})
# set(TARGETS_LIST SuiteSparse_config Mongoose AMD)




add_compile_definitions(BLAS32)
# 初始化一个空的变量以保存先前的目标
set(previous_target "")
include(ExternalProject)
foreach(TARGET ${TARGETS_LIST})
  # 添加子目录
  ExternalProject_Add(${TARGET}
    PREFIX ${TARGET}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/
    CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${TOP_CMAKE_PREFIX_PATH}
    -DBLAS32
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/${TARGET}/src/${TARGET}-build --config Release
    INSTALL_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/${TARGET}/src/${TARGET}-build --config Release --target install
    CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DBLA_STATIC=ON
    -DNGPL=ON
    -DCMAKE_PREFIX_PATH=${TOP_CMAKE_PREFIX_PATH}
    -DENABLE_CUDA=$ENV{ENABLE_CUDA}
     ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}
    
  )

  #add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${TARGET} )


   if(previous_target)
     add_dependencies(${TARGET} ${previous_target})
   endif()

  set(previous_target ${TARGET})
endforeach()








