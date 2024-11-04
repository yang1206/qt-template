cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

project(QT_Template VERSION 0.1 LANGUAGES CXX)

# 基础设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Debug/Release 分别输出到不同目录
foreach (OUTPUT_CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUT_CONFIG} OUTPUT_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/bin/${OUTPUT_CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUT_CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_CONFIG} ${CMAKE_BINARY_DIR}/lib/${OUTPUT_CONFIG})
endforeach ()

# IDE文件分组
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# 编译器选项
if (MSVC)
    add_compile_options("/utf-8")
    add_definitions(-DUNICODE -D_UNICODE)
    add_compile_options(/MP)
endif ()

# 第三方库根目录
set(LIBRARY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rd")

# include目录
include_directories(
        SYSTEM
        "${CMAKE_SOURCE_DIR}/3rd/elawidget/include"
        "${CMAKE_SOURCE_DIR}/3rd/qcustomplot"
)

# 库目录
link_directories(
        SYSTEM
        "${CMAKE_SOURCE_DIR}/3rd/elawidget/lib"
)

# 源文件收集
file(GLOB_RECURSE PROJECT_SOURCES
        "src/*.cpp"
        "src/*.h"
        "${CMAKE_SOURCE_DIR}/3rd/qcustomplot/*.h"
        "${CMAKE_SOURCE_DIR}/3rd/qcustomplot/*.cpp"
)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE FORMS "*.ui")

# 文件分组
source_group("src" FILES ${SOURCES})
source_group("header" FILES ${HEADERS})
source_group("forms" FILES ${FORMS})
source_group("res" FILES ${RESOURCES})

# UI文件搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)

set(QT_UI ui/mainwindow.ui)

set(RES
        ${CMAKE_SOURCE_DIR}/3rd/elawidget/include/ElaWidgetTools.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/icon.qrc
)
SET(SRC
        ${QT_UI}
        ${RES}
        ${PROJECT_SOURCES}
)


# Qt库
set(QT_LIB
        Core
        Widgets
        Gui
        PrintSupport
)

find_package(Qt6 COMPONENTS REQUIRED ${QT_LIB})

# Qt库链接
list(TRANSFORM QT_LIB PREPEND "Qt6::" OUTPUT_VARIABLE QT_LIB_LINK)

foreach (item ${QT_LIB_LINK})
    get_target_property(LibLocation ${item} LOCATION)
    get_filename_component(LibDir ${LibLocation} DIRECTORY)
    list(APPEND QT_LIB_LINK ${LibLocation})
endforeach ()

if (WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
endif ()

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SRC} ${APP_ICON_RESOURCE_WINDOWS})

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # MinGW 版本
        message(STATUS "Using MinGW compiler")
        set(ELAWIDGET_LIB "${CMAKE_CURRENT_SOURCE_DIR}/3rd/elawidget/lib/libelawidgettools.a")

        # 确保 DLL 文件被复制到输出目录
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/3rd/elawidget/lib/elawidgettools.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    else ()
        # MSVC 版本
        message(STATUS "Using MSVC compiler")
        set(ELAWIDGET_LIB "${CMAKE_CURRENT_SOURCE_DIR}/3rd/elawidget/lib/elawidgettools.lib")
    endif ()
else ()
    if (APPLE)
        set(ELAWIDGET_LIB "${CMAKE_CURRENT_SOURCE_DIR}/3rd/elawidget/lib/libelawidgettools.dylib")
    else ()
        set(ELAWIDGET_LIB "${CMAKE_CURRENT_SOURCE_DIR}/3rd/elawidget/lib/libelawidgettools.a")
    endif ()
endif ()

# 查找 vcpkg 库
find_package(FFTW3f CONFIG REQUIRED)
# 链接 vcpkg与第三方库
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${QT_LIB_LINK}
        ${ELAWIDGET_LIB}
        FFTW3::fftw3f
)

# 项目属性
set_target_properties(${PROJECT_NAME} PROPERTIES
        AUTORCC ON
        WIN32_EXECUTABLE OFF
)

# Qt部署配置
if (WIN32)
    set(DeployArg
            --no-quick-import
            --no-translations
            --no-opengl-sw
            --no-system-d3d-compiler
            --verbose 2 # 添加详细输出以便调试
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND Qt6::windeployqt
            ${DeployArg} $<TARGET_FILE:${PROJECT_NAME}>
    )
endif ()

# DLL搜索路径
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${LIBRARY_ROOT}/elawidget/lib $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# 安装配置
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

if (WIN32)
    # 查找Qt插件目录
    find_program(QT_QMAKE_EXECUTABLE qmake HINTS ${LibDir})
    execute_process(
            COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
            OUTPUT_VARIABLE QT_PLUGINS_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # 安装Qt插件
    install(FILES "${QT_PLUGINS_DIR}/platforms/qwindows.dll"
            DESTINATION bin/platforms
    )
endif ()

# 安装主程序和依赖库
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${QT_LIB_LINK} DESTINATION bin)

# CPack打包配置
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")
set(CPACK_GENERATOR "ZIP")
include(CPack)