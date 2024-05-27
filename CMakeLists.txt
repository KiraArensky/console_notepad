cmake_minimum_required(VERSION 3.26)
project(console_notepad C)

set(CMAKE_C_STANDARD 90)

add_executable(console_notepad main.c
        src/add_note.h
        src/struct.h
        src/open_note.h
        src/check_number.h
        src/add_folder.h
        src/open_folder.h)
