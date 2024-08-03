# CMake>=3.25 is required for `add_subdirectory(... SYSTEM)`, which is convenient for
# working with Git submodules. The maximum version may be updated occasionally but
# should not exceed the latest CMake version tested by the CI system.
cmake_minimum_required(VERSION 3.25...3.30)

# This project follows a CalVer (calendar date-based versioning) convention with two
# version numbers. The first is the "epoch" -- the calendar date of the latest
# API-breaking release in YYYYMMDD format. Each release with the same epoch number
# should be API-compatible. The second version number is the "patch" number, which is
# incremented whenever backwards-compatible patches are released, starting over from 0
# whenever the epoch is updated.
project(
  whirlwind
  VERSION 20240715.0
  LANGUAGES CXX
)

# Optionally build the test suite. Defaults to ON if this is the main (top-level) CMake
# project; otherwise OFF.
option(WHIRLWIND_TEST "Build the test suite" ${PROJECT_IS_TOP_LEVEL})

# Add third-party submodules.
add_subdirectory(ext SYSTEM)

# Create a `version.hpp` file in the source tree from the input `version.hpp.in`
# template file when CMake configures the project.
configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/include/whirlwind/common/version.hpp.in
  ${CMAKE_CURRENT_LIST_DIR}/include/whirlwind/common/version.hpp @ONLY
)

add_library(whirlwind INTERFACE)
add_library(whirlwind::whirlwind ALIAS whirlwind)
target_compile_features(whirlwind INTERFACE cxx_std_20)
target_include_directories(
  whirlwind INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
)
target_link_libraries(whirlwind INTERFACE range-v3::range-v3 std::generator std::mdspan)

# When compiling with GCC<11, we need to add the `-fcoroutines` option. With Clang<17,
# we need `-fcoroutines-ts`.
target_compile_options(
  whirlwind
  INTERFACE
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,GCC>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,11>>:-fcoroutines>
    $<$<AND:$<COMPILE_LANG_AND_ID:CXX,Clang>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,17>>:-fcoroutines-ts>
)

if(WHIRLWIND_TEST)
  include(CTest)
  add_subdirectory(test)
endif()
