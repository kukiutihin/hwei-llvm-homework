add_library(skibc_core INTERFACE)

target_include_directories(skibc_core
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(skibc
  main.cpp
)

target_link_libraries(skibc PRIVATE skibc_core)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
