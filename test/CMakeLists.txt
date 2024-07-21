find_package(Catch2 3 CONFIG REQUIRED)

# Add test executable.
add_executable(
  test-whirlwind # cmake-format: sortable
  common/test_version.cpp graph/digraph.cpp math/test_math.cpp math/test_numbers.cpp
)
target_link_libraries(
  test-whirlwind PRIVATE whirlwind::whirlwind Catch2::Catch2WithMain
)

# Find and register test cases from `test-whirlwind` with CTest.
include(CTest)
include(Catch)
catch_discover_tests(test-whirlwind)
