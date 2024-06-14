cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    tease-the-landlord
    VERSION 0.1.0
    DESCRIPTION "欢乐逗地主"
    HOMEPAGE_URL "https://github.com/zymelaii/tease-the-landlord"
    LANGUAGES C CXX
)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)
list(APPEND SOURCE_FILES ${CMAKE_SOURCE_DIR}/assets/res.rc)

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SOURCE_FILES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC UNICODE _UNICODE)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE msimg32.lib)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -static)

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)
