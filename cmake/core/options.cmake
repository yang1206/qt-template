# 构建选项配置模块
include_guard(GLOBAL)

# 定义可用的构建类型
set(AVAILABLE_BUILD_TYPES
        Debug # 调试版本：包含调试信息，无优化
        Release # 发布版本：优化级别最高，无调试信息
        RelWithDebInfo # 带调试信息的发布版本：高优化级别，包含调试信息
        MinSizeRel # 最小体积版本：优化目标是最小化代码体积
        Coverage # 代码覆盖率测试版本：用于生成代码覆盖率报告
        ASan # Address Sanitizer：用于检测内存错误
        TSan # Thread Sanitizer：用于检测线程相关错误
        UBSan # Undefined Behavior Sanitizer：用于检测未定义行为
)

# 配置构建选项
function(configure_build_options)
    # 基础选项
    option(BUILD_TESTS "Build test programs" OFF)
    option(ENABLE_PCH "Enable precompiled headers" ON)
    option(ENABLE_CCACHE "Enable ccache support" ON)

    # 设置默认构建类型
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    endif()

    # 设置构建类型的可选值
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${AVAILABLE_BUILD_TYPES})
endfunction()

# 配置构建类型特定选项
function(configure_build_type_options TARGET_NAME)
    if(NOT MSVC)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${TARGET_NAME} PRIVATE -O0 -g)
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${TARGET_NAME} PRIVATE -O3)
            target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)
        elseif(CMAKE_BUILD_TYPE STREQUAL "Coverage")
            target_compile_options(${TARGET_NAME} PRIVATE -O0 -g --coverage)
            target_link_options(${TARGET_NAME} PRIVATE --coverage)
        elseif(CMAKE_BUILD_TYPE STREQUAL "ASan")
            target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=address)
        elseif(CMAKE_BUILD_TYPE STREQUAL "TSan")
            target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=thread)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=thread)
        elseif(CMAKE_BUILD_TYPE STREQUAL "UBSan")
            target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=undefined)
            target_link_options(${TARGET_NAME} PRIVATE -fsanitize=undefined)
        endif()
    endif()
endfunction()