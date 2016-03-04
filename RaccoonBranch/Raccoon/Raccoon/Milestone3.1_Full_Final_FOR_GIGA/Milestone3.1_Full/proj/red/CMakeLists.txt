project (red_demo C)

cmake_minimum_required (VERSION 2.8)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions (-DDEBUG)
endif ()

if (USE_DEMOMA AND USE_DEMOMA STREQUAL "1")
  message("Build is using DEMOMA batchcode")
  add_definitions (-DUSE_DEMOMA)
  set (hasp_library ${red_demo_SOURCE_DIR}/src/libhasp_windows_demo.lib)
else ()
  message("Build is using Stratasys batchcode")
  set (hasp_library ${red_demo_SOURCE_DIR}/src/libhasp_windows_92199.lib)
endif ()

set (openssl_library ${red_demo_SOURCE_DIR}/../openssl/lib/VC/static/libeay32MD.lib)
set (cunit_library  ${red_demo_SOURCE_DIR}/../cunit/lib/libcunit.lib)

# no warning for unsafe CRT functions
if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_DEPRECATE)
endif ()

# include directories
include_directories (${red_demo_SOURCE_DIR}/src)
include_directories (${red_demo_SOURCE_DIR}/../openssl/include)
include_directories (${red_demo_SOURCE_DIR}/../cunit/include)

set (red_demoSrcs
  src/red_demo.c
  src/REDApp_utils.c
  src/hasp_io_buffer.c
  src/hasp_vm_api.c
)

add_executable (red_demo ${red_demoSrcs})

add_library(hasp STATIC IMPORTED)

set_target_properties(hasp PROPERTIES
  IMPORTED_LOCATION ${hasp_library})


add_library(openssl STATIC IMPORTED)
set_target_properties(openssl PROPERTIES
  IMPORTED_LOCATION ${openssl_library})

add_library(cunit STATIC IMPORTED)

set_target_properties(cunit PROPERTIES
  IMPORTED_LOCATION ${cunit_library})

target_link_libraries (red_demo
  hasp
  openssl
  cunit
)
