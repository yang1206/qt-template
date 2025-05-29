# 编译器配置模块
include_guard(GLOBAL)

# 检查编译器版本
function(check_compiler_version)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
            message(FATAL_ERROR "GCC version must be at least 9.0!")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
            message(FATAL_ERROR "Clang version must be at least 10.0!")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12.0)
            message(FATAL_ERROR "AppleClang version must be at least 12.0!")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
            message(FATAL_ERROR "MSVC version must be at least 19.20!")
        endif ()
    endif ()
endfunction()

# 配置编译器选项
function(configure_compiler TARGET_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES
            CXX_STANDARD 20 # 使用 C++20 标准
            CXX_STANDARD_REQUIRED ON # 强制要求 C++20
            CXX_EXTENSIONS OFF # 禁用编译器特定扩展
            WINDOWS_EXPORT_ALL_SYMBOLS ON # Windows DLL 导出所有符号
    )

    if (MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
                /utf-8
                /MP
                /W4
                /permissive-
                /Zc:__cplusplus
                /Zc:preprocessor
                /EHsc
                /diagnostics:caret    # 更好的错误提示
                /Zc:inline           # 删除未使用的函数
                /Zc:throwingNew      # 假设 new 总是抛出异常
        )
    else ()
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wno-deprecated-declarations
                -fdiagnostics-show-option     # 显示警告选项
                -fdiagnostics-color=always    # 彩色输出
        )
        if(MINGW)
        target_compile_options(${TARGET_NAME} PRIVATE -Wa,-mbig-obj)
        endif()
    endif ()

    if (APPLE)
        target_compile_options(${TARGET_NAME} PRIVATE
                -fstrict-vtable-pointers  # 虚表指针优化
                -fstack-protector-strong  # 栈保护
        )
    elseif (UNIX AND NOT APPLE)
        target_compile_options(${TARGET_NAME} PRIVATE
                -fstack-clash-protection  # 栈碰撞保护
                -fcf-protection=full      # 控制流保护
        )
    endif ()

    if (ENABLE_LTO)
        include(CheckIPOSupported)
        check_ipo_supported(RESULT LTO_SUPPORTED)
        if (LTO_SUPPORTED)
            set_property(TARGET ${TARGET_NAME} PROPERTY
                    INTERPROCEDURAL_OPTIMIZATION $<CONFIG:Release>)
        endif ()
    endif ()
endfunction()