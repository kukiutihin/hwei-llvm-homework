add_library(sim
  sim.c 
)

target_include_directories(sim
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sim
  PRIVATE SDL2::SDL2
)
