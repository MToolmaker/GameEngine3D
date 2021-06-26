cmake_minimum_required(VERSION 3.17)
project(GameEngine3D)

set(CMAKE_CXX_STANDARD 17)
include_directories("${PROJECT_SOURCE_DIR}/include")
add_executable(GameEngine3D main.cpp include/olcPixelGameEngine.h)
target_link_libraries(GameEngine3D -lOpenGL32 -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi)