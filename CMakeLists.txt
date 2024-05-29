cmake_minimum_required(VERSION 3.29)
project(RayCaster)

find_package(SDL2 REQUIRED CONFIG)


add_executable(RayCaster main.c)


target_link_libraries(RayCaster PRIVATE SDL2::SDL2)
