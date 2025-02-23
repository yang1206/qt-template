# 平台配置模块
include_guard(GLOBAL)

# 配置平台特定选项
function(configure_platform TARGET_NAME)
    if (APPLE)

        set(PLATFORM_SPECIFIC_DIR "clang" PARENT_SCOPE)
        set(LIB_PREFIX "lib" PARENT_SCOPE)
        set(LIB_SHARED_SUFFIX ".dylib" PARENT_SCOPE)
        set(LIB_IMPORT_SUFFIX "" PARENT_SCOPE)
        set(NEED_IMPORT_LIB FALSE PARENT_SCOPE)

    elseif (WIN32)
        if (MSVC)
            set(PLATFORM_SPECIFIC_DIR "msvc" PARENT_SCOPE)
            set(LIB_PREFIX "" PARENT_SCOPE)
            set(LIB_SHARED_SUFFIX ".dll" PARENT_SCOPE)
            set(LIB_IMPORT_SUFFIX ".lib" PARENT_SCOPE)
            set(NEED_IMPORT_LIB TRUE PARENT_SCOPE)
        else ()
            set(PLATFORM_SPECIFIC_DIR "mingw" PARENT_SCOPE)
            set(LIB_PREFIX "lib" PARENT_SCOPE)
            set(LIB_SHARED_SUFFIX ".dll" PARENT_SCOPE)
            set(LIB_IMPORT_SUFFIX ".dll.a" PARENT_SCOPE)
            set(NEED_IMPORT_LIB TRUE PARENT_SCOPE)
        endif ()

        target_compile_definitions(${TARGET_NAME} PRIVATE
                NOMINMAX
                _CRT_SECURE_NO_WARNINGS
                WIN32_LEAN_AND_MEAN
        )
    else ()
        target_link_options(${TARGET_NAME} PRIVATE
                "LINKER:--as-needed"
                "LINKER:--gc-sections"
        )

        set(PLATFORM_SPECIFIC_DIR "gcc" PARENT_SCOPE)
        set(LIB_PREFIX "lib" PARENT_SCOPE)
        set(LIB_SHARED_SUFFIX ".so" PARENT_SCOPE)
        set(LIB_IMPORT_SUFFIX "" PARENT_SCOPE)
        set(NEED_IMPORT_LIB FALSE PARENT_SCOPE)
    endif ()

    # 设置全局变量供其他模块使用
    set(PLATFORM_SPECIFIC_DIR ${PLATFORM_SPECIFIC_DIR} PARENT_SCOPE)
    set(LIB_PREFIX ${LIB_PREFIX} PARENT_SCOPE)
    set(LIB_SHARED_SUFFIX ${LIB_SHARED_SUFFIX} PARENT_SCOPE)
    set(LIB_STATIC_SUFFIX ${LIB_STATIC_SUFFIX} PARENT_SCOPE)
endfunction()


function(configure_platform_variables)
    # 设置平台特定目录
    if (APPLE)
        set(PLATFORM_SPECIFIC_DIR "clang" PARENT_SCOPE)
        set(LIB_PREFIX "lib" PARENT_SCOPE)
        set(LIB_SHARED_SUFFIX ".dylib" PARENT_SCOPE)
        set(LIB_STATIC_SUFFIX ".a" PARENT_SCOPE)
    elseif (WIN32)
        if (MSVC)
            set(PLATFORM_SPECIFIC_DIR "msvc" PARENT_SCOPE)
            set(LIB_PREFIX "" PARENT_SCOPE)
            set(LIB_SHARED_SUFFIX ".dll" PARENT_SCOPE)
            set(LIB_IMPORT_SUFFIX ".lib" PARENT_SCOPE)
            set(LIB_STATIC_SUFFIX ".lib" PARENT_SCOPE)
            set(USE_IMPORT_LIB TRUE PARENT_SCOPE)
        else ()
            set(PLATFORM_SPECIFIC_DIR "mingw" PARENT_SCOPE)
            set(LIB_PREFIX "lib" PARENT_SCOPE)
            set(LIB_SHARED_SUFFIX ".dll" PARENT_SCOPE)
            set(LIB_STATIC_SUFFIX ".a" PARENT_SCOPE)
        endif ()
    else ()
        set(PLATFORM_SPECIFIC_DIR "gcc" PARENT_SCOPE)
        set(LIB_PREFIX "lib" PARENT_SCOPE)
        set(LIB_SHARED_SUFFIX ".so" PARENT_SCOPE)
        set(LIB_STATIC_SUFFIX ".a" PARENT_SCOPE)
    endif ()
endfunction()


# 配置平台特定选项
function(configure_platform_options TARGET_NAME)
    if (APPLE)
        target_link_options(${TARGET_NAME} PRIVATE "LINKER:-dead_strip")
    elseif (WIN32)
        target_compile_definitions(${TARGET_NAME} PRIVATE
                NOMINMAX
                _CRT_SECURE_NO_WARNINGS
                WIN32_LEAN_AND_MEAN
        )
    else ()
        target_link_options(${TARGET_NAME} PRIVATE
                "LINKER:--as-needed"
                "LINKER:--gc-sections"
        )
    endif ()
endfunction()


# 获取平台特定的库文件名
function(get_platform_library_name BASE_NAME OUTPUT_VAR)
    set(${OUTPUT_VAR} "${LIB_PREFIX}${BASE_NAME}${LIB_SHARED_SUFFIX}" PARENT_SCOPE)
endfunction()


