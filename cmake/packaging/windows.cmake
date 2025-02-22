# Windows 打包配置模块
include_guard(GLOBAL)

function(configure_windows_installation TARGET_NAME)
    set(CPACK_GENERATOR "WIX;ZIP" PARENT_SCOPE)

    # WiX 配置
    set(CPACK_WIX_UPGRADE_GUID "12345678-1234-1234-1234-123456789012")
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_ROOT}/res/icon/windows/app.ico")
    set(CPACK_WIX_UI_BANNER "${PROJECT_ROOT}/res/icon/windows/banner.png")
    set(CPACK_WIX_UI_DIALOG "${PROJECT_ROOT}/res/icon/windows/dialog.png")

    # 处理运行时依赖
    get_platform_library_name("elawidgettools" RUNTIME_LIB)
    set(RUNTIME_LIB_PATH "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}/${RUNTIME_LIB}")

    if (EXISTS "${RUNTIME_LIB_PATH}")
        install(FILES "${RUNTIME_LIB_PATH}"
                DESTINATION ${RUNTIME_INSTALL_DIR}
                COMPONENT Runtime
        )
    endif ()

    # 部署依赖
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