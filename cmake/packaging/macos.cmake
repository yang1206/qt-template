# macOS 打包配置模块
include_guard(GLOBAL)

function(configure_macos_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "DragNDrop" PARENT_SCOPE)

    # 设置 Bundle 信息
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024 Your Company")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}")


    # 处理运行时依赖
    get_platform_library_name("elawidgettools" RUNTIME_LIB)
    set(RUNTIME_LIB_PATH "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}/${RUNTIME_LIB}")

    if (EXISTS "${RUNTIME_LIB_PATH}")
        install(FILES "${RUNTIME_LIB_PATH}"
                DESTINATION "${PROJECT_NAME}.app/Contents/Frameworks"
                COMPONENT Runtime
        )
    endif ()

    # 部署依赖
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