cmake_minimum_required(VERSION 3.22)

project(EXEC VERSION 1.0)

add_executable(${PROJECT_NAME}
  src/screensaversequential.cpp
)

# Find SDL2 library
find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  OpenMP::OpenMP_CXX
)
