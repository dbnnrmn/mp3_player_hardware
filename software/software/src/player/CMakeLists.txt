cmake_minimum_required(VERSION 3.2)
project(player C CXX)

set(SOURCE_FILES
    player.h
    player.c)

add_library(player SHARED STATIC ${SOURCE_FILES})


message("${SOURCE_FILES}")