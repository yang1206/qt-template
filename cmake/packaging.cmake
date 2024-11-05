# ==================== 打包配置 ====================
# 包含必要的系统库
include(InstallRequiredSystemLibraries)

# 基本包信息
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})

if (WIN32)
    # Windows配置
    # 设置生成器
    set(CPACK_GENERATOR "WIX;ZIP")

    # WiX基本配置
    # 注意: 升级GUID需要是唯一的，你可以通过以下方式生成:
    # 1. Visual Studio: 工具 -> 创建 GUID
    # 2. PowerShell: [guid]::NewGuid()
    # 3. 在线生成器: https://www.guidgenerator.com/
    # 4. Linux: uuidgen
    # 生成后替换下面的占位符GUID
    set(CPACK_WIX_UPGRADE_GUID "00000000-0000-0000-0000-000000000000")
    set(CPACK_WIX_PRODUCT_ICON "${PROJECT_ROOT}/res/icon/windows.ico")
    set(CPACK_WIX_UI_REF "WixUI_InstallDir")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER ${PROJECT_NAME})

    # 配置WiX模板文件
    configure_file(
            "${PROJECT_ROOT}/installer/shortcuts.wxs.in"
            "${CMAKE_CURRENT_BINARY_DIR}/shortcuts.wxs"
            @ONLY
    )
    set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/shortcuts.wxs")

    # 添加程序信息
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://your-website.com")
    set(CPACK_WIX_PROPERTY_ARPCONTACT "your.email@example.com")

    # ZIP配置
    set(CPACK_ZIP_COMPONENT_INSTALL OFF)

    # 安装配置
    include(GNUInstallDirs)

    # 安装主程序
    install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # 安装依赖库
    # ElaWidget DLL
    install(FILES
            "${ELAWIDGET_ROOT}/lib/elawidgettools.dll"
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # vcpkg安装的DLL
    if (VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_DLLS
                "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*.dll"
        )
        install(FILES ${VCPKG_DLLS}
                DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif ()

    # Qt依赖部署
    if (Qt6_FOUND)
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

        install(CODE "
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                    --verbose 1
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    --no-virtualkeyboard
                    --dir \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
                    \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}.exe
            )
        ")
    endif ()

elseif (APPLE)
    # macOS配置
    # 设置生成器
    set(CPACK_GENERATOR "DragNDrop")

    # 基本配置
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_DMG_FORMAT "UDBZ") # 使用更好的压缩
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${PROJECT_ROOT}/installer/dmg_setup.scpt")

    # 安装配置
    install(TARGETS ${PROJECT_NAME}
            BUNDLE DESTINATION . COMPONENT Runtime
            RUNTIME DESTINATION bin COMPONENT Runtime
    )

    # Qt依赖部署
    if (Qt6_FOUND)
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

        install(CODE "
            execute_process(
                COMMAND \"${MACDEPLOYQT_EXECUTABLE}\"
                    \${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app
                    -verbose=1
                    -always-overwrite
            )
        ")
    endif ()

    # vcpkg库安装
    if (VCPKG_INSTALLED_DIR)
        file(GLOB VCPKG_LIBS
                "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/*.dylib"
        )
        install(FILES ${VCPKG_LIBS}
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks
        )
    endif ()

    # 设置包图标
    # set(CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_ROOT}/res/installer/dmg_background.png")
    set(CPACK_DMG_ICON "${PROJECT_ROOT}/app.icns")
endif ()

# 包含CPack模块
include(CPack)
