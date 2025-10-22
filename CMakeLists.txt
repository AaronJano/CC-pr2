cmake_minimum_required(VERSION 3.10)
project(TM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Recursivamente encontrar todos los .cpp y .h
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB_RECURSE H_SOURCES "include/*.h")

set(SOURCES ${CPP_SOURCES} ${H_SOURCES})

# Create executable from sources
add_executable(TM ${SOURCES})

# Add the project include directory to header search path
target_include_directories(TM PRIVATE ${PROJECT_SOURCE_DIR}/include)


