cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    tease-the-landlord
    VERSION 0.1.0
    DESCRIPTION "欢乐逗地主"
    HOMEPAGE_URL "https://github.com/zymelaii/tease-the-landlord"
    LANGUAGES C CXX
)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(QT_DEPENDENCY Core Gui Widgets)
find_package(QT NAMES Qt6 COMPONENTS ${QT_DEPENDENCY} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_DEPENDENCY} REQUIRED)
set(Qt Qt${QT_VERSION_MAJOR})

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)
list(APPEND SOURCE_FILES ${CMAKE_SOURCE_DIR}/assets/res.qrc)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE ${Qt}::Core ${Qt}::Gui ${Qt}::Widgets
)

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)
