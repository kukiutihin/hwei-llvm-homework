add_executable(skibc_tests
  parser_test.cpp
  typechecker_test.cpp
)

target_link_libraries(skibc_tests PRIVATE
  skibc_core
  Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(skibc_tests)
