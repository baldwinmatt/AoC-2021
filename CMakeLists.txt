cmake_minimum_required(VERSION 3.10)

# set the project name
project(AoC)
project(AoC VERSION 2021)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-missing-field-initializers -Wno-unused-function -ftemplate-depth-128 -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -ggdb -Og")

include_directories(${CMAKE_SOURCE_DIR})

macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

subdirlist(SUBDIRS ${CMAKE_SOURCE_DIR})

foreach(subdir ${SUBDIRS})
  if (subdir MATCHES Day)
    add_subdirectory(${subdir})
  endif()
endforeach()

