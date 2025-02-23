# Linux 打包配置模块
include_guard(GLOBAL)

function(configure_linux_installation TARGET_NAME)
    # 设置打包生成器
    set(CPACK_GENERATOR "DEB;RPM")

    # DEB 包配置
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

    # RPM 包配置
    set(CPACK_RPM_PACKAGE_LICENSE "Your License")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")

    # 安装桌面集成文件
    install(FILES
            "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
            DESTINATION "share/applications"
    )

    # 处理运行时依赖
    get_platform_library_name("elawidgettools" RUNTIME_LIB)
    set(RUNTIME_LIB_PATH "${ELAWIDGET_ROOT}/lib/${PLATFORM_SPECIFIC_DIR}/${RUNTIME_LIB}")

    if (EXISTS "${RUNTIME_LIB_PATH}")
        install(FILES "${RUNTIME_LIB_PATH}"
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Runtime
        )
    endif ()

    # 部署 Qt 依赖
    if (Qt6_FOUND)
        find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)
        if (LINUXDEPLOYQT_EXECUTABLE)
            configure_qt_deployment(${TARGET_NAME})
        endif ()
    endif ()
endfunction()