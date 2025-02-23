# Windows 打包配置模块
include_guard(GLOBAL)

function(configure_windows_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "NSIS;ZIP" PARENT_SCOPE)


    # ==================== WiX 配置 ====================
    # UPGRADE_GUID 用于标识应用程序的唯一性，用于升级识别
    # 生成方法：
    # 1. 使用 PowerShell: [guid]::NewGuid()
    # 2. 使用在线工具: https://www.guidgenerator.com/
    # 3. 使用 Visual Studio: 工具 -> 创建 GUID
    # 注意：一旦设置后不要更改，否则 Windows 会将其视为不同的应用程序
    set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012" PARENT_SCOPE)

    # WiX 基本设置
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_ROOT}/res/icon/windows/app.ico" PARENT_SCOPE)
    set(CPACK_WIX_UI_REF "WixUI_Minimal" PARENT_SCOPE)  # 使用最简单的安装界面
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROJECT_NAME}" PARENT_SCOPE)

    set(CPACK_RESOURCE_FILE_LICENSE "" PARENT_SCOPE)

    # 启用 WiX 快捷方式
    set(CPACK_WIX_FEATURE_LEVEL 1 PARENT_SCOPE)
    set(CPACK_WIX_EXTRA_SOURCES "${PROJECT_ROOT}/res/icon/windows/shortcuts.wxs" PARENT_SCOPE)


    # ==================== NSIS 配置 ====================
    # NSIS 基本设置
    set(CPACK_NSIS_MUI_ICON "${PROJECT_ROOT}/res/icon/windows/app.ico" PARENT_SCOPE)
    set(CPACK_NSIS_MUI_UNIICON "${PROJECT_ROOT}/res/icon/windows/app.ico" PARENT_SCOPE)
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe" PARENT_SCOPE)

    # NSIS 安装设置
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON PARENT_SCOPE)
    set(CPACK_NSIS_MODIFY_PATH ON PARENT_SCOPE)

    # NSIS 快捷方式配置
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
            "CreateDirectory '$SMPROGRAMS\\\\${PROJECT_NAME}'
         CreateShortCut '$SMPROGRAMS\\\\${PROJECT_NAME}\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'
         CreateShortCut '$DESKTOP\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'"
            PARENT_SCOPE
    )

    # NSIS 卸载时删除快捷方式
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
            "Delete '$SMPROGRAMS\\\\${PROJECT_NAME}\\\\${PROJECT_NAME}.lnk'
         RMDir '$SMPROGRAMS\\\\${PROJECT_NAME}'
         Delete '$DESKTOP\\\\${PROJECT_NAME}.lnk'"
            PARENT_SCOPE
    )

    #=========== 压缩包配置 ============
    set(CPACK_ARCHIVE_COMPONENT_INSTALL ON PARENT_SCOPE)
    set(CPACK_ZIP_COMPONENT_INSTALL ON PARENT_SCOPE)

    # ==================== 通用配置 ====================
    # 安装目录配置
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}" PARENT_SCOPE)


    # 处理运行时依赖
    if (WIN32 AND MSVC)
        # 部署 elawidgettools
        set(LIB_NAME "elawidgettools")
        set(RUNTIME_LIB "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}/${LIB_NAME}${LIB_SHARED_SUFFIX}")
        if (EXISTS "${RUNTIME_LIB}")
            install(FILES "${RUNTIME_LIB}"
                    DESTINATION ${RUNTIME_INSTALL_DIR}
                    COMPONENT Runtime
            )
        endif ()

        # 部署 vcpkg 依赖
        if (VCPKG_INSTALLED_DIR)
            file(GLOB VCPKG_DLLS
                    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll"
            )
            install(FILES ${VCPKG_DLLS}
                    DESTINATION ${RUNTIME_INSTALL_DIR}
                    COMPONENT Runtime
            )
        endif ()
    endif ()

    # 部署依赖
    configure_qt_deployment(${TARGET_NAME})


    # 创建打包目标
    add_custom_target(create_package
            COMMAND ${CMAKE_COMMAND} -E echo "Creating Windows packages..."
            COMMAND ${CMAKE_COMMAND} --build . --target package
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Creating Windows installers (WiX MSI and NSIS EXE)..."
            VERBATIM
    )
endfunction()