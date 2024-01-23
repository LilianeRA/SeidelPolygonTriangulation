cmake_minimum_required(VERSION 3.5)

project(SeidelPolygonTriangulation)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20) # C++20...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3") # its being ignored in Visual Studio...
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
option(OPTIMIZE_FOR_NATIVE "Build with -march=native" ON)

#--- CMake configuration
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#--- Build example
include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB HEADERS "*.h" "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB SOURCES "*.cpp" "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})




# git status
# git add *
# git add .gitignore
# git config --global user.name "gitusername"
# git config --global user.email "gitemail"
# git commit -a -m "First commit"
# git push
