cmake_minimum_required(VERSION 3.29)
set(CMAKE_TOOLCHAIN_FILE "/Users/yangjunwei/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(qt_template)

# 添加 vcpkg 的工具链文件（假设 vcpkg 位于 /path/to/vcpkg）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
find_package(fmt REQUIRED PATHS ${CMAKE_BINARY_DIR}/vcpkg_installed/arm64-osx/include/fmt)
find_package(spdlog CONFIG REQUIRED)

add_executable(qt_template main.cpp)

target_link_libraries(qt_template ${CMAKE_BINARY_DIR}/vcpkg_installed/arm64-osx/lib/libfmt.a)
target_link_libraries(qt_template
        Qt::Core
        Qt::Gui
        Qt::Widgets
        fmt::fmt
        spdlog::spdlog
)