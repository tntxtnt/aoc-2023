include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/static_analysis.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/sanitize.cmake)

function(target_strip_symbols target_name)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # cl doesn't need stripping
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
    # clang-cl doesn't need stripping
  else()
    target_link_options(${target_name} PRIVATE $<$<CONFIG:RELEASE>:-s>)
  endif()
endfunction()

function(target_enable_strict_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /permissive- /WX)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -pedantic -Werror)
  endif()
endfunction()

function(target_fix_definitions target_name)
  target_compile_definitions(${target_name}
    PRIVATE
      HAVE_SNPRINTF # https://github.com/nlohmann/json/issues/1408#issuecomment-463953575
      NOMINMAX
  )
endfunction()

function(target_enable_unicode_utf8_source target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE "/utf-8")
    target_compile_options(${target_name} PRIVATE "/D_UNICODE")
  endif()
  target_compile_definitions(${target_name} PRIVATE UNICODE)
endfunction()

function(target_fixit target_name)
  target_strip_symbols(${target_name})
  target_enable_strict_warnings(${target_name})
  target_fix_definitions(${target_name})
  target_enable_unicode_utf8_source(${target_name})
  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
endfunction()