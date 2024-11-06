# ==================== 主配置文件 ====================
# 设置最低 CMake 版本要求
cmake_minimum_required(VERSION 3.16)

# 启用新的变量设置行为
cmake_policy(SET CMP0077 NEW)

# ==================== 项目基本信息 ====================
project(QT_Template
        VERSION 0.1.0
        DESCRIPTION "Qt Application Template"
        LANGUAGES CXX
)

# 设置基本变量
# 项目根目录路径
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# 版本信息相关变量
set(VERSION_PROJECT_NAME ${PROJECT_NAME})
set(VERSION_PROJECT_VERSION ${PROJECT_VERSION})
set(VERSION_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(VERSION_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(VERSION_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})

# ==================== 包含配置模块 ====================
# 包含各个功能模块的 CMake 文件
include(${PROJECT_ROOT}/cmake/project_settings.cmake) # 项目基本设置
include(${PROJECT_ROOT}/cmake/build_config.cmake) # 构建配置
include(${PROJECT_ROOT}/cmake/version_control.cmake) # 版本控制
include(${PROJECT_ROOT}/cmake/packaging.cmake) # 打包配置

# ==================== 初始化配置 ====================
# 配置 Qt 自动工具（moc、uic、rcc）
configure_qt_auto_tools()

# 配置库文件路径
configure_library_paths()

# 配置 ccache（如果启用）
if (ENABLE_CCACHE)
    configure_ccache()
endif ()

# ==================== 源文件配置 ====================
# 收集所有源文件
file(GLOB_RECURSE PROJECT_SOURCES
        "${PROJECT_ROOT}/src/*.cpp"
        "${PROJECT_ROOT}/src/*.h"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.cpp"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.h"
)

# 收集所有 UI 文件
file(GLOB UI_FILES
        "${PROJECT_ROOT}/ui/*.ui"
        "${PROJECT_ROOT}/src/*.ui"
)

# 收集资源文件
set(PROJECT_RESOURCES
        ${PROJECT_ROOT}/res/icon.qrc
        ${ELAWIDGET_ROOT}/include/ElaWidgetTools.qrc
)

# 设置 Qt 自动工具选项
set(CMAKE_AUTOUIC ON) # 自动处理 UI 文件
set(CMAKE_AUTOMOC ON) # 自动处理 Q_OBJECT 宏
set(CMAKE_AUTORCC ON) # 自动处理资源文件

# 根据平台配置可执行文件类型
if (WIN32)
    # Windows 平台：创建 GUI 应用程序（无控制台窗口）
    add_executable(${PROJECT_NAME} WIN32
            ${PROJECT_SOURCES}
            ${UI_FILES}
            ${PROJECT_RESOURCES}
    )
elseif (APPLE)
    # macOS 平台：创建应用程序包（.app）
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
            ${PROJECT_SOURCES}
            ${UI_FILES}
            ${PROJECT_RESOURCES}
    )
else ()
    # Linux 平台：创建普通可执行文件
    add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
            ${UI_FILES}
            ${PROJECT_RESOURCES}
    )
endif ()

# 配置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_ROOT}/src # 项目源码目录
        ${THIRD_PARTY_ROOT}/qcustomplot # 第三方库目录
        ${ELAWIDGET_ROOT}/include # ElaWidget 包含目录
        ${QWINDOWKIT_ROOT}/include # QWindowKit 包含目录
        ${CMAKE_BINARY_DIR} # 构建目录（用于生成的文件）
        ${CMAKE_BINARY_DIR}/generated # 生成的文件目录
        ${CMAKE_CURRENT_BINARY_DIR} # 当前构建目录
)

# 配置编译器和构建选项
configure_compiler_and_build(${PROJECT_NAME})

# 配置 Qt 依赖（在预编译头之前）
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_TARGETS})

# 配置预编译头（如果启用）
if (ENABLE_PCH)
    configure_pch(${PROJECT_NAME})
endif ()

# 配置项目依赖项
configure_dependencies(${PROJECT_NAME})

# 配置资源文件
configure_resources(${PROJECT_NAME})

configure_runtime_dependencies(${PROJECT_NAME})


# ==================== 测试配置 ====================
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# ==================== 版本信息配置 ====================
add_build_time_target(${PROJECT_NAME})

# ==================== 安装和打包配置 ====================
configure_installation(${PROJECT_NAME})
