add_executable(app
  app.c 
)

target_link_libraries(app
  PRIVATE sim
)

set(LL_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/ll)
file(MAKE_DIRECTORY ${LL_OUTPUT_DIR})

add_custom_command(
  OUTPUT ${LL_OUTPUT_DIR}/app.ll
  COMMAND ${CMAKE_C_COMPILER} -emit-llvm -S -O2
          "$<LIST:TRANSFORM,$<TARGET_PROPERTY:sim,INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>"
          ${CMAKE_CURRENT_SOURCE_DIR}/app.c
          -o ${LL_OUTPUT_DIR}/app.ll
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/app.c
  COMMAND_EXPAND_LISTS
)
add_custom_target(emit-ll ALL 
  DEPENDS
    ${LL_OUTPUT_DIR}/app.ll
)

