cmake_minimum_required(VERSION 3.16)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(predictor
        main.c
        Makefile
        predictor.c
        predictor.h predictor_util.h predictor_util.c hash_table.c)
