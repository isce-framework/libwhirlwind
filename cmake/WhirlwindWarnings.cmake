include_guard(GLOBAL)

set(WHIRLWIND_CXX_WARNINGS)
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  set(WHIRLWIND_CXX_WARNINGS
      # cmake-format: sortable
      -Wall
      -Wcast-align
      -Wcast-qual
      -Wconversion
      -Wdouble-promotion
      -Wduplicated-branches
      -Wextra
      -Wlogical-op
      -Wnon-virtual-dtor
      -Wold-style-cast
      -Wpedantic
      -Wshadow
      -Wsign-conversion
      -Wundef
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
  set(WHIRLWIND_CXX_WARNINGS
      # cmake-format: sortable
      -Wall
      -Wbad-function-cast
      -Wcast-function-type
      -Wcast-qual
      -Wconditional-uninitialized
      -Wconversion
      -Wcuda-compat
      -Wdeprecated
      -Wdouble-promotion
      -Wduplicate-decl-specifier
      -Wduplicate-method-arg
      -Wduplicate-method-match
      -Wextra
      -Wextra-semi
      -Wheader-hygiene
      -Wimplicit
      -Winconsistent-missing-destructor-override
      -Wloop-analysis
      -Wnarrowing
      -Wnon-virtual-dtor
      -Wold-style-cast
      -Woverriding-method-mismatch
      -Wpedantic
      -Wpointer-arith
      -Wshadow
      -Wstatic-in-inline
      -Wswitch-enum
      -Wundefined-reinterpret-cast
      -Wunreachable-code-aggressive
      -Wunused-member-function
      -Wunused-template
      -Wvector-conversion
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  set(WHIRLWIND_CXX_WARNINGS /W3)
endif()

# Optionally treat warnings as errors.
if(WHIRLWIND_FATAL_WARNINGS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|(Apple)?Clang)$")
    list(APPEND WHIRLWIND_CXX_WARNINGS -Werror)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    list(APPEND WHIRLWIND_CXX_WARNINGS /WX)
  endif()
endif()

# Define an interface-only target with a list of desired C++ compiler warning flags
# enabled. The flags can then be applied to any compiled target by linking with the
# interface target via `target_link_libraries()`.
add_library(whirlwind-warnings INTERFACE)
add_library(whirlwind::warnings ALIAS whirlwind-warnings)
target_compile_options(
  whirlwind-warnings INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${WHIRLWIND_CXX_WARNINGS}>
)
