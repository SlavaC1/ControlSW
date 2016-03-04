project (red_demo C)

cmake_minimum_required (VERSION 2.8)

if (USE_DEMOMA AND USE_DEMOMA STREQUAL "1")
  message("Build is using DEMOMA batchcode")
  add_definitions (-DUSE_DEMOMA)
  set (hasp_library ${red_demo_SOURCE_DIR}/src/libhasp_windows_demo.lib)
else ()
  message("Build is using Stratasys batchcode")
  set (hasp_library ${red_demo_SOURCE_DIR}/src/libhasp_windows_92199.lib)
endif ()

# no warning for unsafe CRT functions
if (MSVC)
  add_definitions (-D_CRT_SECURE_NO_DEPRECATE)
endif ()

# include directories
include_directories (${red_demo_SOURCE_DIR}/src)

set (red_demoSrcs
  src/red_demo.c
  src/hasp_io_buffer
  src/REDApp_utils.c
)

add_executable (red_demo ${red_demoSrcs})

add_library(hasp STATIC IMPORTED)

set_target_properties(hasp PROPERTIES
  IMPORTED_LOCATION ${hasp_library})

target_link_libraries (red_demo
  hasp
)
