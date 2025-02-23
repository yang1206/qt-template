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
                PROPERTIES COMPILE_FLAGS "/wd4458 /wd4996 /wd4702"
        )
    endif ()

    # 配置本地库文件
    configure_local_libraries(${TARGET_NAME})
endfunction()

# 配置本地库文件
function(configure_local_libraries TARGET_NAME)
    if (NOT DEFINED ELAWIDGET_ROOT)
        message(FATAL_ERROR "ELAWIDGET_ROOT is not defined")
    endif ()

    set(LIB_NAME "elawidgettools")
    set(LIB_DIR "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}")

    if (WIN32 AND MSVC)
        # 导入库路径
        set(IMPORT_LIB "${LIB_DIR}/${LIB_NAME}${LIB_IMPORT_SUFFIX}")
        # 运行时库路径
        set(RUNTIME_LIB "${LIB_DIR}/${LIB_NAME}${LIB_SHARED_SUFFIX}")

        if (NOT EXISTS "${IMPORT_LIB}")
            message(FATAL_ERROR "Import library not found: ${IMPORT_LIB}")
        endif ()

        if (NOT EXISTS "${RUNTIME_LIB}")
            message(FATAL_ERROR "Runtime library not found: ${RUNTIME_LIB}")
        endif ()

        # 链接导入库
        target_link_libraries(${TARGET_NAME} PRIVATE "${IMPORT_LIB}")

        # 复制运行时DLL到输出目录
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RUNTIME_LIB}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMENT "Copying ${LIB_NAME} runtime library"
        )
    else ()
        # 其他平台的处理保持不变
        get_platform_library_name(${LIB_NAME} LIB_PATH)
        set(LIB_PATH "${LIB_DIR}/${LIB_PATH}")
        if (EXISTS "${LIB_PATH}")
            target_link_libraries(${TARGET_NAME} PRIVATE "${LIB_PATH}")
        endif ()
    endif ()
endfunction()