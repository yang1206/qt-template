# macOS 打包配置模块
include_guard(GLOBAL)

function(configure_macos_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "DragNDrop" PARENT_SCOPE)

    # 显式禁用其他生成器
    set(CPACK_BINARY_STGZ OFF PARENT_SCOPE)
    set(CPACK_BINARY_TGZ OFF PARENT_SCOPE)
    set(CPACK_BINARY_TZ OFF PARENT_SCOPE)

    # 设置 Bundle 信息
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}" PARENT_SCOPE)
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}" PARENT_SCOPE)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}" PARENT_SCOPE)
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024 Your Company" PARENT_SCOPE)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}" PARENT_SCOPE)

    set_target_properties(${TARGET_NAME} PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_COPYRIGHT "Copyright © 2024 Your Company"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.${PROJECT_NAME}"
            MACOSX_BUNDLE_INFO_PLIST "${PROJECT_ROOT}/res/icon/macos/Info.plist.in"
    )

    set(ICON_FILE "${PROJECT_ROOT}/res/icon/macos/app.icns")
    install(FILES "${ICON_FILE}"
            DESTINATION "${PROJECT_NAME}.app/Contents/Resources"
            COMPONENT Runtime
    )


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


    add_custom_target(create_package
            COMMAND ${CMAKE_COMMAND} -E echo "Creating DMG package for macOS..."
            COMMAND ${CMAKE_COMMAND} --build . --target package
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Creating macOS DMG package..."
            VERBATIM
    )
endfunction()