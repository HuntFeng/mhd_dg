cmake_minimum_required(VERSION 3.25)
project(1D_FDTD)
set(CMAKE_CXX_STANDARD 20)

find_package(gridformat REQUIRED)
find_package(Kokkos REQUIRED)
find_package(OpenMP REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main INIReader OpenMP::OpenMP_CXX gridformat::gridformat Kokkos::kokkos)
