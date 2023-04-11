cmake_minimum_required(VERSION 3.25)
project(CacheLab C)

set(CMAKE_C_STANDARD 11)

add_executable(CacheLab main.c)



target_link_libraries(CacheLab PRIVATE m)
