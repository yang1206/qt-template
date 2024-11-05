cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

# ==================== 项目基本信息 ====================
project(QT_Template
        VERSION 0.1.0                    # 项目版本
        DESCRIPTION "Qt Application Template"  # 项目描述
        LANGUAGES CXX                    # 使用的编程语言
)

# ==================== 构建选项 ====================
# 是否构建测试程序
option(BUILD_TESTS "Build test programs" OFF)
# 是否启用预编译头以加快编译速度
option(ENABLE_PCH "Enable precompiled headers" ON)
# 是否启用ccache以加快重复编译
option(ENABLE_CCACHE "Enable ccache support" ON)

# ==================== 编译器和构建设置 ====================
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制使用指定的C++标准
set(CMAKE_CXX_EXTENSIONS OFF)        # 禁用编译器特定扩展

# Qt自动处理设置
set(CMAKE_AUTOMOC ON)  # 自动处理Qt的元对象系统
set(CMAKE_AUTORCC ON)  # 自动处理Qt资源文件
set(CMAKE_AUTOUIC ON)  # 自动处理Qt的UI文件

# 启用ccache支持，加快重复编���
if (ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if (CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif ()
endif ()

# 编译器特定配置
if (MSVC)
    add_compile_options(
            /utf-8             # 使用UTF-8编码
            /MP               # 启用多处理器编译
            /W4              # 启用更高级别的警告
            /permissive-     # 启用标准符合模式
    )
    # Windows特定的宏定义
    add_definitions(
            -DUNICODE        # 使用Unicode字符集
            -D_UNICODE
            -DWIN32_LEAN_AND_MEAN  # 精简Windows头文件
            -DNOMINMAX            # 避免Windows的min/max宏
    )
else ()
    # GCC/Clang警告配置
    add_compile_options(
            -Wall            # 启用所有常见警告
            -Wextra          # 启用额外警告
    )
endif ()

# ==================== 输出目录配置 ====================
# 设置构建输出的根目录
set(OUTPUT_ROOT ${CMAKE_BINARY_DIR})
set(OUTPUT_BIN ${OUTPUT_ROOT}/bin)  # 可执行文件输出目录
set(OUTPUT_LIB ${OUTPUT_ROOT}/lib)  # 库文件输出目录

# 设置默认输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB})

# 为不同的构建类型(Debug/Release等)设置输出目录
foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_BIN}/${CONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}")
endforeach ()

# ==================== IDE配置 ====================
# 启用IDE中的文件夹组织
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# 将CMake生成的目标放入特定文件夹
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# ==================== 项目路径配置 ====================
# 定义重要路径变量
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRD_PARTY_ROOT ${PROJECT_ROOT}/3rd)
set(ELAWIDGET_ROOT ${THIRD_PARTY_ROOT}/elawidget)
set(QWINDOWKIT_ROOT ${THIRD_PARTY_ROOT}/qwindowkit)

# 设置UI文件搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_ROOT}/ui)

# 设置项目包含目录
set(PROJECT_INCLUDES
        ${ELAWIDGET_ROOT}/include
        ${QWINDOWKIT_ROOT}/include/QWINDOWKIT
        ${THIRD_PARTY_ROOT}/qcustomplot
        ${PROJECT_ROOT}/src
        ${PROJECT_ROOT}/ui
        ${CMAKE_CURRENT_BINARY_DIR}  # 添加生成的文件目录
)

# ==================== 源文件收集 ====================
# 收集所有源文件
file(GLOB_RECURSE PROJECT_SOURCES
        "${PROJECT_ROOT}/src/*.cpp"
        "${PROJECT_ROOT}/src/*.h"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.cpp"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.h"
)

# 收集UI文件
file(GLOB UI_FILES "${PROJECT_ROOT}/ui/*.ui")

# 收集资源文件
set(PROJECT_RESOURCES
        ${ELAWIDGET_ROOT}/include/ElaWidgetTools.qrc
        ${PROJECT_ROOT}/res/icon.qrc
)

# ==================== Qt配置 ====================
# 定义需要的Qt模块
set(QT_MODULES
        Core
        Widgets
        Gui
        PrintSupport
)

# 查找Qt包
find_package(Qt6 COMPONENTS ${QT_MODULES} REQUIRED)
# 转换Qt模块名为完整的目标名称
list(TRANSFORM QT_MODULES PREPEND "Qt6::" OUTPUT_VARIABLE QT_TARGETS)

# ==================== 目标配置 ====================
# 根据平台配置可执行文件
if (WIN32)
    # Windows平台配置
    set(APP_ICON ${PROJECT_ROOT}/app.rc)
    add_executable(${PROJECT_NAME} WIN32
            ${UI_FILES}
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES}
            ${APP_ICON}
    )
elseif (APPLE)
    # macOS平台配置
    set(APP_ICON ${PROJECT_ROOT}/app.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
            ${UI_FILES}
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES}
            ${APP_ICON}
    )
    # 设置macOS应用程序包属性
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yang1206.${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
else ()
    # Linux平台配置
    add_executable(${PROJECT_NAME}
            ${UI_FILES}
            ${PROJECT_SOURCES}
            ${PROJECT_RESOURCES}
    )
endif ()

# 配置预编译头以加快编译速度
if (ENABLE_PCH)
    target_precompile_headers(${PROJECT_NAME} PRIVATE
            <vector>
            <string>
            <memory>
            <QWidget>
            <QString>
    )
endif ()

# ==================== 依赖配置 ====================
# 配置ElaWidget和QWindowKit库
if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # MinGW配置
        set(ELAWIDGET_LIB "${ELAWIDGET_ROOT}/lib/libelawidgettools.a")
        set(QWINDOWKIT_LIBS
            "${QWINDOWKIT_ROOT}/lib/libQWKCore.dll.a"
            "${QWINDOWKIT_ROOT}/lib/libQWKWidgets.dll.a"
        )
        # 复制运行时DLL
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ELAWIDGET_ROOT}/lib/elawidgettools.dll"
                "${QWINDOWKIT_ROOT}/lib/QWKCore.dll"
                "${QWINDOWKIT_ROOT}/lib/QWKWidgets.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    else ()
        # MSVC配置
        set(ELAWIDGET_LIB "${ELAWIDGET_ROOT}/lib/elawidgettools.lib")
        set(QWINDOWKIT_LIBS
            "${QWINDOWKIT_ROOT}/lib/QWKCore.lib"
            "${QWINDOWKIT_ROOT}/lib/QWKWidgets.lib"
        )
    endif ()
else ()
    # Unix系统配置
    set(ELAWIDGET_LIB "${ELAWIDGET_ROOT}/lib/libelawidgettools.${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(QWINDOWKIT_LIBS
        "${QWINDOWKIT_ROOT}/lib/libQWKCore.${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${QWINDOWKIT_ROOT}/lib/libQWKWidgets.${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
endif ()

# 查找FFTW库
find_package(FFTW3f CONFIG REQUIRED)

# 配置包含目录和链接库
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${QT_TARGETS}
        ${ELAWIDGET_LIB}
        ${QWINDOWKIT_LIBS}
        FFTW3::fftw3f
)

# ==================== 部署配置 ====================
if (WIN32)
    # 配置Qt部署工具选项
    set(DEPLOY_ARGS
            --no-quick-import
            --no-translations
            --no-opengl-sw
            --no-system-d3d-compiler
            --verbose 2
    )

    # 部署Qt依赖
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND Qt6::windeployqt ${DEPLOY_ARGS} $<TARGET_FILE:${PROJECT_NAME}>
    )
endif ()

# ==================== 安装配置 ====================
# 包含GNUInstallDirs以使用标准安装目录
include(GNUInstallDirs)
# 设置安装前缀
set(CMAKE_INSTALL_PREFIX "${OUTPUT_ROOT}/install" CACHE PATH "Installation directory")

# 安装目标文件
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装依赖库
if (WIN32)
    install(FILES
            ${ELAWIDGET_ROOT}/lib/elawidgettools.dll
            ${QWINDOWKIT_ROOT}/lib/QWKCore.dll
            ${QWINDOWKIT_ROOT}/lib/QWKWidgets.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()

# 安装资源文件
install(DIRECTORY ${PROJECT_ROOT}/res/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
)

# ==================== 包含打包配置 ====================
include(${PROJECT_ROOT}/cmake/packaging.cmake)

# ==================== 测试配置 ====================
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()