# 通用工具函数模块
include_guard(GLOBAL)

# 配置输出目录
function(configure_output_directories)
    set(OUTPUT_ROOT ${CMAKE_BINARY_DIR})
    set(OUTPUT_BIN ${OUTPUT_ROOT}/bin)
    set(OUTPUT_LIB ${OUTPUT_ROOT}/lib)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB} PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB} PARENT_SCOPE)

    # 为每个构建类型配置专门的输出目录
    foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_BIN}/${CONFIG}" PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}" PARENT_SCOPE)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}" PARENT_SCOPE)
    endforeach ()
endfunction()

# 配置 ccache
function(configure_ccache)
    if (ENABLE_CCACHE)
        find_program(CCACHE_PROGRAM ccache)
        if (CCACHE_PROGRAM)
            set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" PARENT_SCOPE)
            message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
        else ()
            message(STATUS "ccache not found, disabling ccache support")
            set(ENABLE_CCACHE OFF CACHE BOOL "Enable ccache support" FORCE)
        endif ()
    endif ()
endfunction()


function(configure_pch TARGET_NAME)
    if (ENABLE_PCH)
        # 获取Qt包含路径
        get_target_property(QT_INCLUDE_DIRS Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)

        # 设置预编译头文件列表
        target_precompile_headers(${TARGET_NAME} PRIVATE
                <vector>
                <string>
                <memory>
                <QWidget>
                <QString>
        )

        # 为预编译头文件添加Qt包含路径
        target_include_directories(${TARGET_NAME} PRIVATE ${QT_INCLUDE_DIRS})

        message(STATUS "Enabled precompiled headers for target: ${TARGET_NAME}")
        message(STATUS "Qt include directories: ${QT_INCLUDE_DIRS}")
    endif ()
endfunction()