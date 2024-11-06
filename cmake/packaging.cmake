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
    if(WIN32)
        # Windows 平台安装路径
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 可执行文件目录
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 库文件目录（Windows下与bin相同）
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 资源文件目录
    elseif(APPLE)
        # macOS 平台安装路径（.app 包结构）
        set(RUNTIME_INSTALL_DIR "${PROJECT_NAME}.app/Contents/MacOS") # 可执行文件
        set(LIBRARY_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Frameworks") # 框架和库
        set(RESOURCE_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Resources") # 资源文件
    else()
        # Linux 平台安装路径
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # 可执行文件 (/usr/bin)
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}) # 库文件 (/usr/lib)
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}) # 资源文件
    endif()

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
    if(WIN32)
        configure_windows_installation(${TARGET_NAME})
    elseif(APPLE)
        configure_macos_installation(${TARGET_NAME})
    else()
        configure_linux_installation(${TARGET_NAME})
    endif()
endfunction()

# ========== Windows 特定安装配置 ==========
function(configure_windows_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "WIX;ZIP" PARENT_SCOPE) # 使用 WiX 和 ZIP 格式

    # WiX 安装程序特定配置
    set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012") # 升级 GUID
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_ROOT}/res/icon/windows/app.ico") # 安装程序图标
    set(CPACK_WIX_UI_BANNER "${PROJECT_ROOT}/res/icon/windows/banner.png") # 安装程序横幅
    set(CPACK_WIX_UI_DIALOG "${PROJECT_ROOT}/res/icon/windows/dialog.png") # 安装程序对话框

    # 部署 Qt 依赖
    configure_qt_deployment(${TARGET_NAME} "windeployqt"
        ARGS
        --verbose 1
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        --no-virtualkeyboard
    )

    # 安装 vcpkg 依赖
    if(VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_DLLS
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll"
        )
        install(FILES ${VCPKG_DLLS}
            DESTINATION ${RUNTIME_INSTALL_DIR}
        )
    endif()
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
    configure_qt_deployment(${TARGET_NAME} "macdeployqt"
        ARGS
        -verbose=1
        -always-overwrite
        -no-strip
    )

    # 安装 vcpkg 依赖
    if(VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_LIBS
            "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/*.dylib"
        )
        install(FILES ${VCPKG_LIBS}
            DESTINATION ${LIBRARY_INSTALL_DIR}
        )
    endif()
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
    if(Qt6_FOUND)
        find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)

        if(LINUXDEPLOYQT_EXECUTABLE)
            configure_qt_deployment(${TARGET_NAME} "linuxdeployqt"
                ARGS
                -verbose=1
                -no-translations
                -bundle-non-qt-libs
            )
        endif()
    endif()
endfunction()

# ========== Qt 部署配置 ==========
function(configure_qt_deployment TARGET_NAME DEPLOY_TOOL)
    # 解析额外参数
    cmake_parse_arguments(PARSE_ARGV 2 ARG "" "" "ARGS")

    if(Qt6_FOUND)
        # 查找 Qt 部署工具
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(DEPLOYQT_EXECUTABLE ${DEPLOY_TOOL} HINTS "${_qt_bin_dir}")

        if(DEPLOYQT_EXECUTABLE)
            # 根据平台配置部署命令
            if(WIN32)
                # Windows 平台部署
                install(CODE "
                    execute_process(
                        COMMAND \"${DEPLOYQT_EXECUTABLE}\"
                            ${ARG_ARGS}
                            --dir \${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DIR}
                            \${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DIR}/${PROJECT_NAME}.exe
                    )
                ")
            elseif(APPLE)
                # macOS 平台部署
                install(CODE "
                    execute_process(
                        COMMAND \"${DEPLOYQT_EXECUTABLE}\"
                            \${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app
                            ${ARG_ARGS}
                    )
                ")
            else()
                # Linux 平台部署
                install(CODE "
                    execute_process(
                        COMMAND \"${DEPLOYQT_EXECUTABLE}\"
                            \${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DIR}/${PROJECT_NAME}
                            ${ARG_ARGS}
                    )
                ")
            endif()

            message(STATUS "Configured ${DEPLOY_TOOL} deployment")
        else()
            message(WARNING "${DEPLOY_TOOL} not found, Qt deployment may be incomplete")
        endif()
    endif()
endfunction()

# 包含 CPack 模块（必须在所有配置之后）
include(CPack)