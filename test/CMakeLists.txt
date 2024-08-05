find_package(Catch2 3.3 CONFIG REQUIRED)

# Include custom CMake modules.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
include(WhirlwindWarnings)

# Add test executable.
add_executable(
  test-whirlwind # cmake-format: sortable
  common/test_version.cpp
  graph/csr_graph.cpp
  graph/forest.cpp
  graph/forest_concepts.cpp
  graph/graph_concepts.cpp
  math/test_math.cpp
  math/test_numbers.cpp
)
target_link_libraries(
  test-whirlwind PRIVATE Catch2::Catch2WithMain whirlwind::warnings
                         whirlwind::whirlwind
)

# Configure Catch2 to prefix all test macros with `CATCH_`.
target_compile_definitions(test-whirlwind PRIVATE CATCH_CONFIG_PREFIX_ALL)

# Don't scan sources for module dependencies unless/until we adopt C++20 modules.
# Scanning for modules may require additional tools not found in common compiler
# distributions.
set_target_properties(test-whirlwind PROPERTIES CXX_SCAN_FOR_MODULES OFF)

# Find and register test cases with CTest.
include(Catch)
catch_discover_tests(test-whirlwind)
