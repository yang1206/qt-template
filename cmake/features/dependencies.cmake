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
    FetchContent_MakeAvailable(fmt)

    # 链接 FetchContent 依赖
    if (TARGET fmt::fmt)
        target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt)
    endif ()
endfunction()


# 配置本地库文件
function(configure_local_libraries TARGET_NAME)
    set(QWINDOWKIT_LIB_DIR "${QWINDOWKIT_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}")
    set(QWINDOWKIT_LIBS "QWKCore" "QWKWidgets")
    set(DEBUG_SUFFIX_GENEX "$<$<CONFIG:Debug>:d>$<$<NOT:$<CONFIG:Debug>>:>")

    if (WIN32)
        foreach (LIB_NAME ${QWINDOWKIT_LIBS})
            # 构建库文件名（带有条件后缀）
            set(IMPORT_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_NAME}${DEBUG_SUFFIX_GENEX}${LIB_IMPORT_SUFFIX}")
            set(RUNTIME_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_NAME}${DEBUG_SUFFIX_GENEX}${LIB_SHARED_SUFFIX}")

            # 检查Debug和Release库文件是否存在
            set(DEBUG_IMPORT_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_NAME}d${LIB_IMPORT_SUFFIX}")
            set(RELEASE_IMPORT_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_NAME}${LIB_IMPORT_SUFFIX}")

            if (EXISTS "${DEBUG_IMPORT_LIB}" AND EXISTS "${RELEASE_IMPORT_LIB}")
                # 使用生成表达式链接适当的库
                target_link_libraries(${TARGET_NAME} PRIVATE "${IMPORT_LIB}")

                # 复制运行时DLL到输出目录
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${QWINDOWKIT_LIB_DIR}/${LIB_NAME}$<$<CONFIG:Debug>:d>${LIB_SHARED_SUFFIX}"
                        $<TARGET_FILE_DIR:${TARGET_NAME}>
                        COMMENT "Copying ${LIB_NAME} runtime library"
                )

                message(STATUS "Configured QWindowKit library: ${LIB_NAME}")
            else ()
                message(WARNING "QWindowKit library not found: ${LIB_NAME}")
                message(STATUS "Debug import: ${DEBUG_IMPORT_LIB}")
                message(STATUS "Release import: ${RELEASE_IMPORT_LIB}")
            endif ()
        endforeach ()
    else ()
        # 其他平台的处理方式（macOS/Linux）
        foreach (LIB_NAME ${QWINDOWKIT_LIBS})
            # 检查Debug和Release库文件是否存在
            set(DEBUG_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_PREFIX}${LIB_NAME}d${LIB_SHARED_SUFFIX}")
            set(RELEASE_LIB "${QWINDOWKIT_LIB_DIR}/${LIB_PREFIX}${LIB_NAME}${LIB_SHARED_SUFFIX}")
            
            # 根据当前配置选择正确的库文件
            if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                # Debug模式 - 优先使用Debug库，如果不存在则使用Release库
                if (EXISTS "${DEBUG_LIB}")
                    message(STATUS "Linking Debug QWindowKit library: ${DEBUG_LIB}")
                    target_link_libraries(${TARGET_NAME} PRIVATE "${DEBUG_LIB}")
                elseif (EXISTS "${RELEASE_LIB}")
                    message(STATUS "Debug library not found, linking Release QWindowKit library: ${RELEASE_LIB}")
                    target_link_libraries(${TARGET_NAME} PRIVATE "${RELEASE_LIB}")
                else()
                    message(WARNING "QWindowKit library not found: ${LIB_NAME}")
                endif()
            else()
                # Release模式 - 使用Release库
                if (EXISTS "${RELEASE_LIB}")
                    message(STATUS "Linking Release QWindowKit library: ${RELEASE_LIB}")
                    target_link_libraries(${TARGET_NAME} PRIVATE "${RELEASE_LIB}")
                else()
                    message(WARNING "QWindowKit library not found: ${LIB_NAME}")
                endif()
            endif()
            
            # 对于macOS，需要设置rpath以便可执行文件能找到动态库
            if (APPLE)
                set_target_properties(${TARGET_NAME} PROPERTIES
                    INSTALL_RPATH "@executable_path/../Frameworks"
                    BUILD_WITH_INSTALL_RPATH TRUE
                )
                
                # 复制动态库到应用程序包中
                if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND EXISTS "${DEBUG_LIB}")
                    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks"
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEBUG_LIB}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks"
                        COMMENT "Copying ${LIB_NAME} Debug library to app bundle"
                    )
                elseif (EXISTS "${RELEASE_LIB}")
                    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks"
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RELEASE_LIB}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/../Frameworks"
                        COMMENT "Copying ${LIB_NAME} Release library to app bundle"
                    )
                endif()
            endif()
        endforeach ()
    endif ()
endfunction()


# 配置本地依赖
function(configure_local_dependencies TARGET_NAME)
    set(LOCAL_DEPS_INCLUDE
            "${QWINDOWKIT_ROOT}/include"
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