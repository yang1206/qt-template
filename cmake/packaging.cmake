# ==================== 打包配置模块 ====================
# 该模块负责处理安装和打包相关的配置，包括不同平台的安装规则和打包选项

# 包含必要的系统库
include(InstallRequiredSystemLibraries) # 包含系统运行时库
include(GNUInstallDirs) # 包含标准安装目录定义

# ========== 基本包信息 ==========
# 设置包的基本信息
set(CPACK_PACKAGE_NAME ${PROJECT_NAME}) # 包名称
set(CPACK_PACKAGE_VENDOR "Your Company") # 发布者
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION}) # 包描述
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION}) # 包版本
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME}) # 安装目录

# ========== 安装配置 ==========
function(configure_installation TARGET_NAME)
    # 根据不同平台设置安装目录
    if (WIN32)
        # Windows 平台安装路径
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 可执行文件目录
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 库文件目录（Windows下与bin相同）
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 资源文件目录
    elseif (APPLE)
        # macOS 平台安装路径（.app 包结构）
        set(RUNTIME_INSTALL_DIR "${PROJECT_NAME}.app/Contents/MacOS") # 可执行文件
        set(LIBRARY_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Frameworks") # 框架和库
        set(RESOURCE_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Resources") # 资源文件
    else ()
        # Linux 平台安装路径
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 可执行文件 (/usr/bin)
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}) # 库文件 (/usr/lib)
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}) # 资源文件
    endif ()

    # 安装主程序
    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR} # 可执行文件安装规则
            BUNDLE DESTINATION . COMPONENT Runtime # macOS Bundle 安装规则
            LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR} # 库文件安装规则
    )

    # 安装依赖库
    install(FILES ${RUNTIME_DEPENDENCIES}
            DESTINATION ${LIBRARY_INSTALL_DIR}
    )

    # 根据平台配置特定的安装规则
    if (WIN32)
        configure_windows_installation(${TARGET_NAME})
    elseif (APPLE)
        configure_macos_installation(${TARGET_NAME})
    else ()
        configure_linux_installation(${TARGET_NAME})
    endif ()
endfunction()

# ========== Windows 特定安装配置 ==========
function(configure_windows_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "WIX;ZIP" PARENT_SCOPE) # 使用 WiX 和 ZIP 格式

    # WiX 安装程序特定配置
    # WiX基本配置
    # 注意: 升级GUID需要是唯一的，你可以通过以下方式生成:
    # 1. Visual Studio: 工具 -> 创建 GUID
    # 2. PowerShell: [guid]::NewGuid()
    # 3. 在线生成器: https://www.guidgenerator.com/
    # 4. Linux: uuidgen
    # 生成后替换下面的占位符GUID
    set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012") # 升级 GUID
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_ROOT}/res/icon/windows/app.ico") # 安装程序图标
    set(CPACK_WIX_UI_BANNER "${PROJECT_ROOT}/res/icon/windows/banner.png") # 安装程序横幅
    set(CPACK_WIX_UI_DIALOG "${PROJECT_ROOT}/res/icon/windows/dialog.png") # 安装程序对话框

    # 部署 Qt 依赖
    configure_qt_deployment(${TARGET_NAME})

    # 安装 vcpkg 依赖
    if (VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_DLLS
                "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll"
        )
        install(FILES ${VCPKG_DLLS}
                DESTINATION ${RUNTIME_INSTALL_DIR}
        )
    endif ()
endfunction()

# ========== macOS 特定安装配置 ==========
function(configure_macos_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "DragNDrop" PARENT_SCOPE) # 使用 DMG 格式

    # 设置 Bundle 信息
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024 Your Company")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}")

    # 部署 Qt 依赖
    configure_qt_deployment(${TARGET_NAME})

    # 安装 vcpkg 依赖
    if (VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_LIBS
                "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/*.dylib"
        )
        install(FILES ${VCPKG_LIBS}
                DESTINATION ${LIBRARY_INSTALL_DIR}
        )
    endif ()
endfunction()

# ========== Linux 特定安装配置 ==========
function(configure_linux_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "DEB;RPM" PARENT_SCOPE) # 使用 DEB 和 RPM 格式

    # DEB 包配置
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # 自动检测依赖

    # RPM 包配置
    set(CPACK_RPM_PACKAGE_LICENSE "Your License")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")

    # 安装桌面集成文件
    install(FILES
            "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
            DESTINATION "share/applications"
    )

    # 部署 Qt 依赖
    if (Qt6_FOUND)
        find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)

        if (LINUXDEPLOYQT_EXECUTABLE)
            configure_qt_deployment(${TARGET_NAME})
        endif ()
    endif ()
endfunction()

# ========== Qt 部署配置 ==========
function(configure_qt_deployment TARGET_NAME)
    # 检查目标是否使用 Qt6
    get_target_property(TARGET_LIBS ${TARGET_NAME} LINK_LIBRARIES)

    if (NOT TARGET_LIBS MATCHES "Qt6::")
        message(STATUS "Target ${TARGET_NAME} does not use Qt6, skipping Qt deployment")
        return()
    endif ()

    if (APPLE)
        # macOS 平台：只在安装/打包时执行 macdeployqt
        install(CODE "
            message(STATUS \"Deploying Qt for installation of ${TARGET_NAME}...\")
            execute_process(
                COMMAND \"$<TARGET_FILE:Qt6::macdeployqt>\"
                    \"\${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}.app\"
                    -verbose=2
                    -always-overwrite
            )
        ")

        # 运行时不执行 macdeployqt
        return()
    endif ()

    # Windows 和 Linux 平台的处理保持不变
    # 最小化部署（只复制必要的文件）
    set(DEPLOY_ARGS
            --verbose 2
            --no-quick-import # 不复制 QtQuick 相关文件
            --no-translations # 不复制翻译文件
            --no-opengl-sw # 不复制软件 OpenGL 库
            --no-system-d3d-compiler # 不复制 D3D 编译器
            --no-virtualkeyboard # 不复制虚拟键盘
    )

    if (WIN32)
        # Windows 平台：使用 Qt 提供的目标
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND Qt6::windeployqt
                ${DEPLOY_ARGS}
                "$<TARGET_FILE:${TARGET_NAME}>"
                COMMENT "Deploying Qt dependencies for ${TARGET_NAME}..."
        )

        # 安装时部署
        install(CODE "
            message(STATUS \"Deploying Qt for installation of ${TARGET_NAME}...\")
            execute_process(
                COMMAND \"$<TARGET_FILE:Qt6::windeployqt>\"
                    ${DEPLOY_ARGS}
                    --dir \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
                    \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${TARGET_NAME}.exe\"
            )
        ")

    elseif (APPLE)
        # macOS 平台：使用 macdeployqt
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND Qt6::macdeployqt
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/../.."
                -verbose=2
                -always-overwrite
                COMMENT "Deploying Qt dependencies for ${TARGET_NAME}..."
        )

        install(CODE "
            message(STATUS \"Deploying Qt for installation of ${TARGET_NAME}...\")
            execute_process(
                COMMAND \"$<TARGET_FILE:Qt6::macdeployqt>\"
                    \"\${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}.app\"
                    -verbose=2
                    -always-overwrite
            )
        ")

    else ()
        # Linux 平台：使用 linuxdeployqt
        find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)

        if (LINUXDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${LINUXDEPLOYQT_EXECUTABLE}
                    "$<TARGET_FILE:${TARGET_NAME}>"
                    -verbose=2
                    -no-translations
                    -bundle-non-qt-libs
                    COMMENT "Deploying Qt dependencies for ${TARGET_NAME}..."
            )

            install(CODE "
                message(STATUS \"Deploying Qt for installation of ${TARGET_NAME}...\")
                execute_process(
                    COMMAND \"${LINUXDEPLOYQT_EXECUTABLE}\"
                        \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${TARGET_NAME}\"
                        -verbose=2
                        -no-translations
                        -bundle-non-qt-libs
                )
            ")
        endif ()
    endif ()
endfunction()

# 包含 CPack 模块（必须在所有配置之后）
include(CPack)

# 运行时依赖配置
function(configure_runtime_dependencies TARGET_NAME)
    # 1. 处理自定义运行时依赖
    if (WIN32)
        set(RUNTIME_DLLS
                "${ELAWIDGET_LIB_DIR}/elawidgettools.dll"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RUNTIME_DLLS}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying runtime DLLs to build directory"
        )

        install(FILES ${RUNTIME_DLLS}
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
        )
    elseif (APPLE)
        set(RUNTIME_LIBS
                "${ELAWIDGET_LIB_DIR}/libelawidgettools.dylib"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RUNTIME_LIBS}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying runtime libraries to build directory"
        )

        install(FILES ${RUNTIME_LIBS}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
        )
    else ()
        set(RUNTIME_LIBS
                "${ELAWIDGET_LIB_DIR}/libelawidgettools.so"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RUNTIME_LIBS}
                $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMENT "Copying runtime libraries to build directory"
        )

        install(FILES ${RUNTIME_LIBS}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
        )
    endif ()

    # 处理 Qt 依赖
    configure_qt_deployment(${TARGET_NAME})
endfunction()