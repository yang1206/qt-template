# ==================== 项目设置模块 ====================
# 该模块负责项目的基本设置，包括构建选项、输出目录配置和依赖管理

# ========== 构建选项 ==========
option(BUILD_TESTS "Build test programs" OFF) # 是否构建测试程序
option(ENABLE_PCH "Enable precompiled headers" ON) # 是否启用预编译头文件
option(ENABLE_CCACHE "Enable ccache support" ON) # 是否启用编译缓存

# ========== 输出目录配置 ==========
set(OUTPUT_ROOT ${CMAKE_BINARY_DIR}) # 设置输出根目录
set(OUTPUT_BIN ${OUTPUT_ROOT}/bin) # 可执行文件输出目录
set(OUTPUT_LIB ${OUTPUT_ROOT}/lib) # 库文件输出目录

# 配置标准输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN}) # 可执行文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB}) # 动态库目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB}) # 静态库目录

# 为每个构建类型配置专门的输出目录
foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_BIN}/${CONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_LIB}/${CONFIG}")
endforeach ()

# ========== 项目路径配置 ==========
set(THIRD_PARTY_ROOT ${PROJECT_ROOT}/3rd) # 第三方库根目录

# 根据编译器类型设置库文件子目录
if (MSVC)
    set(COMPILER_SPECIFIC_DIR "msvc")
elseif (MINGW)
    set(COMPILER_SPECIFIC_DIR "mingw")
elseif (APPLE)
    set(COMPILER_SPECIFIC_DIR "clang")
else ()
    set(COMPILER_SPECIFIC_DIR "gcc")
endif ()

# 更新库目录路径
set(ELAWIDGET_ROOT ${THIRD_PARTY_ROOT}/elawidget) # ElaWidget库目录
set(ELAWIDGET_LIB_DIR ${ELAWIDGET_ROOT}/lib/${COMPILER_SPECIFIC_DIR})
set(QWINDOWKIT_ROOT ${THIRD_PARTY_ROOT}/qwindowkit) # QWindowKit库目录
set(QWINDOWKIT_LIB_DIR ${QWINDOWKIT_ROOT}/lib/${COMPILER_SPECIFIC_DIR})

# ========== 库文件配置 ==========
function(configure_library_paths)
    # 根据不同平台配置库文件后缀
    if (WIN32)
        if (MSVC)
            # MSVC编译器的库文件后缀
            set(LIB_SUFFIXES
                    STATIC_SUFFIX .lib # 静态库后缀
                    SHARED_SUFFIX .dll # 动态库后缀
                    IMPORT_SUFFIX .lib # 导入库后缀
                    CACHE INTERNAL "Library suffixes for MSVC"
            )
        else ()
            # MinGW编译器的库文件后缀
            set(LIB_SUFFIXES
                    STATIC_SUFFIX .a # 静态库后缀
                    SHARED_SUFFIX .dll # 动态库后缀
                    IMPORT_SUFFIX .dll.a # 导入库后缀
                    CACHE INTERNAL "Library suffixes for MinGW"
            )
        endif ()
    elseif (APPLE)
        # macOS平台的库文件后缀
        set(LIB_SUFFIXES
                STATIC_SUFFIX .a # 静态库后缀
                SHARED_SUFFIX .dylib # 动态库后缀
                IMPORT_SUFFIX .dylib # 导入库后缀
                CACHE INTERNAL "Library suffixes for macOS"
        )
    else ()
        # Linux平台的库文件后缀
        set(LIB_SUFFIXES
                STATIC_SUFFIX .a # 静态库后缀
                SHARED_SUFFIX .so # 动态库后缀
                IMPORT_SUFFIX .so # 导入库后缀
                CACHE INTERNAL "Library suffixes for Linux"
        )
    endif ()
endfunction()

# 获取库文件完整路径的辅助函数
function(get_library_path OUT_VAR LIB_NAME)
    # 解析函数参数
    cmake_parse_arguments(PARSE_ARGV 2 ARG "STATIC;SHARED;IMPORT" "BASE_DIR" "")

    # 确保使用编译器特定的子目录
    set(FULL_BASE_DIR "${ARG_BASE_DIR}/${COMPILER_SPECIFIC_DIR}")

    # 根据库类型选择后缀
    if (ARG_STATIC)
        set(SUFFIX ${LIB_SUFFIXES_STATIC_SUFFIX})
    elseif (ARG_SHARED)
        set(SUFFIX ${LIB_SUFFIXES_SHARED_SUFFIX})
    elseif (ARG_IMPORT)
        set(SUFFIX ${LIB_SUFFIXES_IMPORT_SUFFIX})
    endif ()

    # 设置库文件前缀
    if (WIN32 AND NOT MSVC)
        set(PREFIX "lib") # MinGW使用lib前缀
    elseif (UNIX)
        set(PREFIX "lib") # Unix系统使用lib前缀
    else ()
        set(PREFIX "") # MSVC不使用前缀
    endif ()

    # 构建完整的库文件路径
    set(${OUT_VAR} "${FULL_BASE_DIR}/${PREFIX}${LIB_NAME}${SUFFIX}" PARENT_SCOPE)
endfunction()

# ========== 依赖配置 ==========
# vcpkg清单模式配置
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
endif ()

function(configure_dependencies TARGET_NAME)
    # 1. vcpkg依赖配置
    find_package(FFTW3f CONFIG REQUIRED)

    if (TARGET FFTW3::fftw3f)
        target_link_libraries(${TARGET_NAME} PRIVATE FFTW3::fftw3f)
        target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_FFTW3)
    else ()
        message(FATAL_ERROR "FFTW3f target not found. Please check vcpkg installation.")
    endif ()

    # 2. 本地库文件依赖配置
    set(LOCAL_DEPS_INCLUDE
            "${ELAWIDGET_ROOT}/include"
            "${THIRD_PARTY_ROOT}/qcustomplot"
    )

    # 根据平台设置库文件列表
    if (APPLE)
        set(LOCAL_DEPS_LIBS
                "${ELAWIDGET_LIB_DIR}/libelawidgettools.dylib"
        )
    elseif (WIN32)
        if (MSVC)
            set(LOCAL_DEPS_LIBS
                    "${ELAWIDGET_LIB_DIR}/elawidgettools.lib"
            )
        else ()
            set(LOCAL_DEPS_LIBS
                    "${ELAWIDGET_LIB_DIR}/libelawidgettools.a"
            )
        endif ()
    else ()
        set(LOCAL_DEPS_LIBS
                "${ELAWIDGET_LIB_DIR}/libelawidgettools.so"
        )
    endif ()

    # 处理包含目录
    foreach (INC_DIR IN LISTS LOCAL_DEPS_INCLUDE)
        if (EXISTS "${INC_DIR}")
            target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIR})
            get_filename_component(DEP_NAME ${INC_DIR} NAME)
            string(TOUPPER "${DEP_NAME}" DEP_NAME)
            target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_${DEP_NAME})
            message(STATUS "Added include directory: ${INC_DIR}")
        else ()
            message(WARNING "Include directory not found: ${INC_DIR}")
        endif ()
    endforeach ()

    # 处理库文件
    foreach (LIB_PATH IN LISTS LOCAL_DEPS_LIBS)
        if (EXISTS "${LIB_PATH}")
            target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_PATH})
            message(STATUS "Added library: ${LIB_PATH}")
        else ()
            message(WARNING "Library not found: ${LIB_PATH}")
        endif ()
    endforeach ()

    # 3. Qt依赖配置
    target_link_libraries(${TARGET_NAME} PRIVATE ${QT_TARGETS})
endfunction()

# ========== 资源配置 ==========
function(configure_resources TARGET_NAME)
    # 设置图标路径
    set(ICON_PATH "${PROJECT_ROOT}/res/icon")

    # 根据平台配置应用程序图标和资源
    if (WIN32)
        # Windows平台：配置RC文件
        configure_file(
                "${ICON_PATH}/windows/app.rc.in"
                "${CMAKE_BINARY_DIR}/generated/app.rc"
                @ONLY
        )
        configure_file(
                "${ICON_PATH}/windows/app.ico"
                "${CMAKE_BINARY_DIR}/generated/app.ico"
                COPYONLY
        )

        # 修改生成的 RC 文件中的图标路径
        file(READ "${CMAKE_BINARY_DIR}/generated/app.rc" RC_CONTENT)
        string(REPLACE "res/icon/windows/app.ico" "app.ico" RC_CONTENT "${RC_CONTENT}")
        file(WRITE "${CMAKE_BINARY_DIR}/generated/app.rc" "${RC_CONTENT}")
        target_sources(${TARGET_NAME} PRIVATE
                "${CMAKE_BINARY_DIR}/generated/app.rc"
        )
    elseif (APPLE)
        # macOS平台：配置应用程序图标
        set_target_properties(${TARGET_NAME} PROPERTIES
                MACOSX_BUNDLE_ICON_FILE "app.icns"
                RESOURCE "${ICON_PATH}/macos/app.icns"
        )
    else ()
        # Linux平台：配置桌面入口和图标
        configure_file(
                "${ICON_PATH}/linux/${PROJECT_NAME}.desktop.in"
                "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
                @ONLY
        )
        install(FILES
                "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
                DESTINATION "share/applications"
        )
        install(FILES
                "${ICON_PATH}/linux/app.png"
                DESTINATION "share/icons/hicolor/256x256/apps"
                RENAME "${PROJECT_NAME}.png"
        )
    endif ()
endfunction()

# ========== Qt工具配置 ==========
function(configure_qt_auto_tools)
    # 启用Qt自动工具
    set(CMAKE_AUTOUIC ON PARENT_SCOPE) # 自动处理UI文件
    set(CMAKE_AUTOMOC ON PARENT_SCOPE) # 自动处理Q_OBJECT宏
    set(CMAKE_AUTORCC ON PARENT_SCOPE) # 自动处理资源文件


    # 设置 UI 文件搜索路径
    set(CMAKE_AUTOUIC_SEARCH_PATHS
            ${PROJECT_ROOT}/ui
            ${PROJECT_ROOT}/src
            PARENT_SCOPE
    )


    # 查找Qt包
    find_package(Qt6 COMPONENTS Core Widgets Gui PrintSupport REQUIRED)

    # 设置Qt目标
    set(QT_TARGETS
            Qt6::Core
            Qt6::Widgets
            Qt6::Gui
            Qt6::PrintSupport
            CACHE INTERNAL "Qt targets"
    )
endfunction()

# ========== 工具配置函数 ==========
function(configure_ccache)
    if (ENABLE_CCACHE)
        # 查找ccache程序
        find_program(CCACHE_PROGRAM ccache)

        if (CCACHE_PROGRAM)
            # 设置编译器启动器为ccache
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