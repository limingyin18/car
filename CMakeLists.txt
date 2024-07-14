# Global CMake

cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

project("car")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")

add_subdirectory("third_party/spdlog-1.14.1")
add_subdirectory("third_party/SDL-2.30.5")
add_subdirectory("third_party/glm-1.0.1")

# add_subdirectory("third_party")
# add_subdirectory("src/engine")
# add_subdirectory("src/game")

include_directories(src)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC
	spdlog
	SDL2
	glm
)
# compile shaders
# add_subdirectory(shaders)

# test
#add_subdirectory("test")