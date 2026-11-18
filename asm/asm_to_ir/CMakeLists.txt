add_executable(lifter
  main.cpp
)

target_include_directories(lifter
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lifter PRIVATE
  foonathan::lexy::core
  foonathan::lexy::file
)
