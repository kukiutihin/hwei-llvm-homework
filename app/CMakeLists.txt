add_executable(app
  app.c 
  sim.c 
  start.c
)

target_include_directories(app
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(app
  PRIVATE SDL2::SDL2
)

set(LL_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/ll)
file(MAKE_DIRECTORY ${LL_OUTPUT_DIR})

add_custom_command(
  OUTPUT ${LL_OUTPUT_DIR}/app.ll
  COMMAND ${CMAKE_C_COMPILER} -emit-llvm -S -O2 ${CMAKE_CURRENT_SOURCE_DIR}/app.c -o ${LL_OUTPUT_DIR}/app.ll
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/app.c
)

add_custom_target(emit-ll ALL 
  DEPENDS
    ${LL_OUTPUT_DIR}/app.ll
)
