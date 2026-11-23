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

file(GLOB INTEGRATION_TESTS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/integration/test*.skib67"
)

foreach(test_file ${INTEGRATION_TESTS})
  get_filename_component(test_name ${test_file} NAME_WE)
  add_test(
    NAME "integration.${test_name}"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/run_integration.sh"
            $<TARGET_FILE:skibc>
            "${test_file}"
  )
endforeach()
