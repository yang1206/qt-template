# 构建选项配置模块
include_guard(GLOBAL)

# 定义可用的构建类型
set(AVAILABLE_BUILD_TYPES
        Debug # 调试版本：包含调试信息，无优化
        Release # 发布版本：优化级别最高，无调试信息
)

# 配置构建选项
function(configure_build_options)
    # 基础选项
    option(ENABLE_PCH "Enable precompiled headers" ON)
    option(ENABLE_CCACHE "Enable ccache support" ON)
    option(ENABLE_LTO "Enable Link Time Optimization in Release builds" ON)

    # 检查生成器类型
    if (CMAKE_CONFIGURATION_TYPES)
        # 多配置生成器（如 Visual Studio, Ninja Multi-Config）
        message(STATUS "Using multi-configuration generator: ${CMAKE_GENERATOR}")
        set(CMAKE_CONFIGURATION_TYPES "${AVAILABLE_BUILD_TYPES}" CACHE STRING "Configuration types" FORCE)
    else ()
        # 单配置生成器（如 Unix Makefiles, Ninja）
        message(STATUS "Using single-configuration generator: ${CMAKE_GENERATOR}")
        if (NOT CMAKE_BUILD_TYPE)
            message(STATUS "No build type specified, defaulting to Debug")
            set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
        endif ()
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${AVAILABLE_BUILD_TYPES})
    endif ()
endfunction()

# 配置构建类型特定选项
function(configure_build_type_options TARGET_NAME)
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
    if (NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
        if (MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE
                    $<$<CONFIG:Debug>:/Od /Zi>
                    $<$<CONFIG:Release>:/O2>
            )
            target_compile_definitions(${TARGET_NAME} PRIVATE
                    $<$<CONFIG:Debug>:_DEBUG>
                    $<$<CONFIG:Release>:NDEBUG>
            )
        else ()
            target_compile_options(${TARGET_NAME} PRIVATE
                    $<$<CONFIG:Debug>:-O0 -g>
                    $<$<CONFIG:Release>:-O3>
            )
            target_compile_definitions(${TARGET_NAME} PRIVATE
                    $<$<CONFIG:Release>:NDEBUG>
            )
        endif ()
    endif ()
endfunction()