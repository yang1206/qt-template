# 依赖管理模块
include_guard(GLOBAL)

# 配置依赖项
function(configure_dependencies TARGET_NAME)
    # 确保 FetchContent 模块可用
    include(FetchContent)

    # 配置 Qt 依赖（基础依赖）
    target_link_libraries(${TARGET_NAME} PRIVATE ${QT_TARGETS})

    # 配置 vcpkg 依赖
    configure_vcpkg_dependencies(${TARGET_NAME})

    # 配置 FetchContent 依赖
    configure_fetchcontent_dependencies(${TARGET_NAME})

    # 配置本地依赖
    configure_local_dependencies(${TARGET_NAME})
endfunction()


# 配置 vcpkg 依赖
function(configure_vcpkg_dependencies TARGET_NAME)
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file"
        )
    endif ()

    find_package(FFTW3f CONFIG REQUIRED)
    if (TARGET FFTW3::fftw3f)
        target_link_libraries(${TARGET_NAME} PRIVATE FFTW3::fftw3f)
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_FFTW3)
    else ()
        message(FATAL_ERROR "FFTW3f target not found. Please check vcpkg installation.")
    endif ()
endfunction()


# 配置 FetchContent 依赖
function(configure_fetchcontent_dependencies TARGET_NAME)
    # 示例：添加 fmt 库
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 9.1.0
    )


    # 使用 FetchContent_MakeAvailable 获取所有声明的依赖
    FetchContent_MakeAvailable(fmt )

    # 链接 FetchContent 依赖
    if(TARGET fmt::fmt)
        target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt)
    endif()
endfunction()


# 配置本地库文件
function(configure_local_libraries TARGET_NAME)

endfunction()


# 配置本地依赖
function(configure_local_dependencies TARGET_NAME)
    set(LOCAL_DEPS_INCLUDE
            "${ELAWIDGET_ROOT}/include"
            "${THIRD_PARTY_ROOT}/qcustomplot"
    )

    foreach (INC_DIR IN LISTS LOCAL_DEPS_INCLUDE)
        if (EXISTS "${INC_DIR}")
            target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIR})
            get_filename_component(DEP_NAME ${INC_DIR} NAME)
            string(TOUPPER "${DEP_NAME}" DEP_NAME)
            target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_${DEP_NAME})
        endif ()
    endforeach ()

    # 为第三方库添加 SYSTEM 包含目录，抑制警告
    target_include_directories(${TARGET_NAME} SYSTEM PRIVATE
            ${THIRD_PARTY_ROOT}/qcustomplot
    )

    # 为特定源文件禁用警告
    if (MSVC)
        set_source_files_properties(
                "${THIRD_PARTY_ROOT}/qcustomplot/qcustomplot.cpp"
                PROPERTIES COMPILE_FLAGS "/wd4458 /wd4996 /wd4702"
        )
    endif ()

    # 配置本地库文件
    configure_local_libraries(${TARGET_NAME})
endfunction()