cmake_minimum_required(VERSION 3.29)
project(aoc2018)

set(CMAKE_CXX_STANDARD 20)

add_library(utilslib utilslib/utilslib.cpp)

# List of days
set(DAYS day01 day02 day03 day04 day05 day06 day07 day08 day09 day10 day11 day12 day13 day14 day15)

# Loop through each day and create an executable and custom command
foreach (DAY ${DAYS})
    add_executable(${DAY} ${DAY}/${DAY}.cpp utilslib/utilslib.cpp)
    add_custom_command(TARGET ${DAY} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${DAY}/inp.txt $<TARGET_FILE_DIR:${DAY}>/inp.txt
    )
endforeach ()

