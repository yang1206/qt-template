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


    # 部署 Qt 依赖
    if (Qt6_FOUND)
        find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)
        if (LINUXDEPLOYQT_EXECUTABLE)
            configure_qt_deployment(${TARGET_NAME})
        endif ()
    endif ()
endfunction()