# 设置最低 CMake 版本要求
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

# 项目基本信息
project(QT_Template
        VERSION 0.1.0
        DESCRIPTION "Qt Application Template"
        LANGUAGES CXX
)

# 设置基本变量
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRD_PARTY_ROOT ${PROJECT_ROOT}/3rd)
set(QWINDOWKIT_ROOT ${THIRD_PARTY_ROOT}/qwindowkit)

# 设置版本信息变量
set(VERSION_PROJECT_NAME ${PROJECT_NAME})
set(VERSION_PROJECT_VERSION ${PROJECT_VERSION})
set(VERSION_BUILD_TYPE "$<CONFIG>")
set(VERSION_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(VERSION_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})

#加载平台相关配置
include(${PROJECT_ROOT}/cmake/core/platform.cmake)
configure_platform_variables()

# 编译器和构建选项
include(${PROJECT_ROOT}/cmake/core/compiler.cmake)
include(${PROJECT_ROOT}/cmake/core/options.cmake)
include(${PROJECT_ROOT}/cmake/core/utilities.cmake)

# 配置Qt（最基础的依赖）
include(${PROJECT_ROOT}/cmake/features/qt.cmake)
configure_qt_tools()

#初始化基础配置
configure_build_options()
configure_output_directories()

if (ENABLE_CCACHE)
    configure_ccache()
endif ()


# 收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
        CONFIGURE_DEPENDS
        "${PROJECT_ROOT}/src/*.cpp"
        "${PROJECT_ROOT}/src/*.h"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.cpp"
        "${THIRD_PARTY_ROOT}/qcustomplot/*.h"
)



file(GLOB UI_FILES
        "${PROJECT_ROOT}/ui/*.ui"
        "${PROJECT_ROOT}/src/*.ui"
)

set(PROJECT_RESOURCES
        ${PROJECT_ROOT}/res/icon.qrc
        ${PROJECT_ROOT}/src/shared/resources/shared.qrc
        ${PROJECT_ROOT}/style/style.qrc
        ${PROJECT_ROOT}/src/shared/styles/style.qrc
)


# 创建目标
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${UI_FILES} ${PROJECT_RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_ROOT}/src          # 根目录
        ${PROJECT_ROOT}/src/utils    # 工具类
        # 其他子目录...
)

# 配置版本控制
include(${PROJECT_ROOT}/cmake/version_control.cmake)
add_build_time_target(${PROJECT_NAME})

# 添加生成的版本头文件目录到包含路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/generated)


if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
elseif (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}"
    )
endif ()


if (ENABLE_PCH)
    configure_pch(${PROJECT_NAME})
endif ()

#配置目标属性（按依赖顺序）
configure_compiler(${PROJECT_NAME})
configure_platform_options(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_TARGETS})

#配置依赖（Qt已经配置，这里是其他依赖）
include(${PROJECT_ROOT}/cmake/features/dependencies.cmake)
configure_dependencies(${PROJECT_NAME})

#配置资源和测试
include(${PROJECT_ROOT}/cmake/features/resources.cmake)
include(${PROJECT_ROOT}/cmake/features/testing.cmake)
configure_resources(${PROJECT_NAME})
configure_testing()


# 最后配置打包
include(${PROJECT_ROOT}/cmake/packaging/common.cmake)
configure_package_info()
configure_installation(${PROJECT_NAME})

# 更新版本信息的目标（使用现有的 generate_version_file 函数）
add_custom_target(update_version
        COMMAND ${CMAKE_COMMAND}
        -DSCRIPT_MODE=ON
        -DPROJECT_ROOT=${PROJECT_ROOT}
        -DVERSION_PROJECT_NAME=${VERSION_PROJECT_NAME}
        -DVERSION_PROJECT_VERSION=${VERSION_PROJECT_VERSION}
        -DVERSION_BUILD_TYPE=${VERSION_BUILD_TYPE}
        -DVERSION_COMPILER_ID=${VERSION_COMPILER_ID}
        -DVERSION_COMPILER_VERSION=${VERSION_COMPILER_VERSION}
        -DOUTPUT_DIR=${CMAKE_BINARY_DIR}/generated
        -P ${PROJECT_ROOT}/cmake/version_control.cmake
        WORKING_DIRECTORY ${PROJECT_ROOT}
        COMMENT "Updating version information..."
)


include(CPack)