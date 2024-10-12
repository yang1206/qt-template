cmake_minimum_required(VERSION 3.29)

project(qt_template)
set(CMAKE_PREFIX_PATH "C:/QT/6.7.2/mingw_64")
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

## 第三方库的根目录
set(LIBRARY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rd")
#
## 设置 board SDK 的路径
set(BOARD_INCLUDE_DIR "${LIBRARY_ROOT}/xdma/include")
set(BOARD_LIB_DIR "${LIBRARY_ROOT}/xdma/lib/x64")  # 如果你使用的是 x64 平台
set(BOARD_DLL_DIR "${LIBRARY_ROOT}/xdma/dll/x64")  # 添加 DLL 目录
## 将头文件路径包含到项目中
include_directories(${BOARD_INCLUDE_DIR} ${BOARD_UTILS_DIR})
#
## 将库文件路径包含到项目中
link_directories(${BOARD_LIB_DIR})


# 查找项目主代码源文件
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc" "src/*.c" "src/*.h")

set(SOURCES ${UTIL_SOURCES})

# 查找项目源文件、头文件等
file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE STAMPS "*.stamp")

# 对文件进行分组
source_group("src" FILES ${SOURCES})
source_group("header" FILES ${HEADERS})
source_group("forms" FILES ${FORMS})
source_group("res" FILES ${RESOURCES})
source_group("stamps" FILES ${STAMPS})

# 设置自动生成的 UI 文件的搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/qt_ui)

set(QT_SRC
        qt_src/BoardController.h
        qt_src/BoardController.cpp
        qt_src/BoardHardwareInterface.cpp
        qt_src/BoardHardwareInterface.h
        qt_src/Pub.cpp
        qt_src/Pub.h
)
set(QT_UI qt_ui/mainwindow.ui)
set(RES
        styles.qrc
)

SET(SRC
        ${QT_SRC}
        ${QT_UI}
        ${RES}
        ${PROJECT_SOURCES}
)
# 收集依赖的 Qt 库
set(QT_LIB
        Core
        Widgets
        Gui
)

# 查找 Qt6 包
find_package(Qt6 COMPONENTS REQUIRED ${QT_LIB})

# 组合连接时的名字
list(TRANSFORM QT_LIB PREPEND "Qt6::" OUTPUT_VARIABLE QT_LIB_LINK)
foreach (item ${QT_LIB_LINK})
    get_target_property(LibLocation ${item} LOCATION)
    get_filename_component(LibDir ${LibLocation} DIRECTORY)
    list(APPEND QT_LIB_LINK ${LibLocation})
endforeach ()

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SRC})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIB_LINK}
        QTXdmaApi
        pthreadVC2
)

# 设置项目的属性
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE OFF
)

# 设置运行时 DLL 搜索路径
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${BOARD_DLL_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
