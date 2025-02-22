# 依赖管理模块
include_guard(GLOBAL)

# 配置依赖项
function(configure_dependencies TARGET_NAME)
    #配置 Qt 依赖（基础依赖）
    target_link_libraries(${TARGET_NAME} PRIVATE ${QT_TARGETS})

    # vcpkg 配置
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file"
        )
    endif ()

    # 配置 vcpkg 依赖
    find_package(FFTW3f CONFIG REQUIRED)
    if (TARGET FFTW3::fftw3f)
        target_link_libraries(${TARGET_NAME} PRIVATE FFTW3::fftw3f)
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_FFTW3)
    else ()
        message(FATAL_ERROR "FFTW3f target not found. Please check vcpkg installation.")
    endif ()

    # 配置本地依赖
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
            ${ELAWIDGET_ROOT}/include
    )

    # 为特定源文件禁用警告
    if (MSVC)
        set_source_files_properties(
                "${THIRD_PARTY_ROOT}/qcustomplot/qcustomplot.cpp"
                PROPERTIES COMPILE_FLAGS "/WX- /wd4458 /wd4996"
        )
    endif ()

    # 配置本地库文件
    configure_local_libraries(${TARGET_NAME})
endfunction()

# 配置本地库文件
function(configure_local_libraries TARGET_NAME)
    set(ELAWIDGET_LIB_DIR "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}")
    get_platform_library_name("elawidgettools" LIB_NAME)
    set(LIB_PATH "${ELAWIDGET_LIB_DIR}/${LIB_NAME}")

    if (EXISTS "${LIB_PATH}")
        target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_PATH})
    endif ()
endfunction()