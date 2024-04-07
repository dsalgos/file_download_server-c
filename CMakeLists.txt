cmake_minimum_required(VERSION 3.27)
project(file_download_server C)

set(CMAKE_C_STANDARD 11)

add_executable(file_download_server src/org/dsalgos/systemprogramming/client/app/main.c
        ../../../../Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/errno.h
        ../../../../Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/errno.h)
