# wbmsed

cmake_minimum_required(VERSION 3.12)
project(wbmsed
  VERSION 4.4.1
  LANGUAGES C
)

set(GHAASDIR $ENV{HOME}/tmp/_install/ghaas)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fsigned-char -D_GNU_SOURCE")

message(STATUS "GHAASDIR = ${GHAASDIR}")

add_subdirectory(WBMsed)
