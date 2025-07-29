cmake_minimum_required(VERSION 3.22)
project(cpprtw)

find_package(stb REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/render_system.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE stb::stb)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)