# wbmsed

cmake_minimum_required(VERSION 3.12)
project(wbmsed
  VERSION 4.4.1
  LANGUAGES C
)

if(NOT DEFINED ENV{GHAASDIR})
  message(FATAL_ERROR "Environment variable GHAASDIR is not set.")
endif()
message(STATUS "GHAASDIR = $ENV{GHAASDIR}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fsigned-char -D_GNU_SOURCE")

add_subdirectory(WBMsed)
