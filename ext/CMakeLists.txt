function(whirlwind_check_submodule NAME)
  if(NOT IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${NAME})
    message(
      FATAL_ERROR
        "The ${NAME} submodule was not found. Did you forget to clone with"
        " `--recursive`? If so, use `git submodule update --init --recursive` to add"
        " the missing submodule(s)."
    )
  endif()
endfunction()

whirlwind_check_submodule(generator)
whirlwind_check_submodule(range-v3)

add_library(std-generator INTERFACE)
add_library(std::generator ALIAS std-generator)
target_include_directories(
  std-generator INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generator/include>
)

add_library(range-v3 INTERFACE)
add_library(range-v3::range-v3 ALIAS range-v3)
target_include_directories(
  range-v3 INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/range-v3/include>
)
target_compile_options(
  range-v3 INTERFACE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
                     $<$<COMPILE_LANG_AND_ID:CUDA,MSVC>:-Xcompiler=/permissive->
)
