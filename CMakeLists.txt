# Global CMake

cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_C_COMPILER "clang-cl")
#set(CMAKE_CXX_COMPILER "clang-cl")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")


# add_subdirectory("third_party/glm-1.0.1")
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory("third_party/assimp-5.4.2")
add_subdirectory("third_party/spdlog-1.14.1")
add_subdirectory("third_party/SDL-2.30.5")

# add_subdirectory("third_party")
# add_subdirectory("src/engine")
# add_subdirectory("src/game")

project("car")
include_directories(src)
include_directories("third_party/glm-1.0.1")

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC
	spdlog
	SDL2
	assimp
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
	-DGLM_ENABLE_EXPERIMENTAL
)
# compile shaders
# add_subdirectory(shaders)

# test
#add_subdirectory("test")