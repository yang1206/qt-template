# 打包通用配置模块
include_guard(GLOBAL)

include(InstallRequiredSystemLibraries)
include(GNUInstallDirs)

# 配置基本包信息
function(configure_package_info)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME} PARENT_SCOPE)
    set(CPACK_PACKAGE_VENDOR "Your Company" PARENT_SCOPE)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION} PARENT_SCOPE)
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME} PARENT_SCOPE)
    set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>" PARENT_SCOPE)
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_ROOT}/LICENSE" PARENT_SCOPE)
    set(CPACK_RESOURCE_FILE_README "${PROJECT_ROOT}/README.md" PARENT_SCOPE)
endfunction()

# 配置安装规则
function(configure_installation TARGET_NAME)
    if (WIN32)
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
    elseif (APPLE)
        set(RUNTIME_INSTALL_DIR "${PROJECT_NAME}.app/Contents/MacOS")
        set(LIBRARY_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Frameworks")
        set(RESOURCE_INSTALL_DIR "${PROJECT_NAME}.app/Contents/Resources")
    else ()
        set(RUNTIME_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
        set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
        set(RESOURCE_INSTALL_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})
    endif ()

    set(RUNTIME_INSTALL_DIR ${RUNTIME_INSTALL_DIR} PARENT_SCOPE)
    set(LIBRARY_INSTALL_DIR ${LIBRARY_INSTALL_DIR} PARENT_SCOPE)
    set(RESOURCE_INSTALL_DIR ${RESOURCE_INSTALL_DIR} PARENT_SCOPE)

    install(TARGETS ${TARGET_NAME}
            RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}
            BUNDLE DESTINATION . COMPONENT Runtime
            LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    )

    # 根据平台选择特定的安装配置

    set(PACKAGING_DIR "${PROJECT_ROOT}/cmake/packaging")
    if (WIN32)
        include("${PACKAGING_DIR}/windows.cmake")
        configure_windows_installation(${TARGET_NAME})
    elseif (APPLE)
        include("${PACKAGING_DIR}/macos.cmake")
        configure_macos_installation(${TARGET_NAME})
    else ()
        include("${PACKAGING_DIR}/linux.cmake")
        configure_linux_installation(${TARGET_NAME})
    endif ()

    # 添加文档安装
    install(FILES
            "${PROJECT_ROOT}/README.md"
            "${PROJECT_ROOT}/LICENSE"
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endfunction()