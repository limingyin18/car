# Global CMake

cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_COLOR_DIAGNOSTICS ON)
# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")


project("Finsler")

# third party

set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory("third_party/assimp-5.4.2")
add_subdirectory("third_party/spdlog-1.14.1")
add_subdirectory("third_party/SDL-2.30.5")
add_subdirectory("third_party/meshoptimizer-0.21")
add_subdirectory("third_party/Catch2-3.7.1")

## glad
add_library(glad "third_party/glad/glad.c")
target_include_directories(glad PUBLIC "third_party/glad")

## imgui
set(IMGUI_DIR "third_party/imgui-1.91.2")
file(GLOB srcImgui "${IMGUI_DIR}/*.cpp")
add_library(imgui
    "${srcImgui}"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp")
target_include_directories(imgui PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")
target_link_libraries(imgui SDL2 glad)

## Eigen
add_subdirectory("third_party/eigen")

# source
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC
    src
    third_party
    "third_party/glm-1.0.1"
    )
target_link_libraries(${PROJECT_NAME} PUBLIC
	spdlog
	SDL2
	assimp
    meshoptimizer
    glad
    imgui
    eigen
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
	-DGLM_ENABLE_EXPERIMENTAL
    -DDLL_EXPORT
)

# app
SET(APP_NAME "app")
file(GLOB_RECURSE APP_SRC "app/*.cpp")
add_executable(${APP_NAME} ${APP_SRC})
target_include_directories(${APP_NAME} PUBLIC "src")
target_link_libraries(${APP_NAME} PUBLIC
    ${PROJECT_NAME}
)

# compile shaders
# add_subdirectory(shaders)

# test
SET(TEST_NAME "unit_test")
file(GLOB_RECURSE TEST_SRC "test/*.cpp")
add_executable(${TEST_NAME} ${TEST_SRC})
target_include_directories(${TEST_NAME} PUBLIC "src")
target_link_libraries(${TEST_NAME} PUBLIC
    ${PROJECT_NAME}
    Catch2::Catch2WithMain
)