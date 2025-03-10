# Qt 功能配置模块
include_guard(GLOBAL)

# Qt 组件配置
set(QT_COMPONENTS
        Core
        Widgets
        Gui
        PrintSupport
)

# 配置 Qt 自动工具
function(configure_qt_tools)
    set(CMAKE_AUTOUIC ON PARENT_SCOPE)
    set(CMAKE_AUTOMOC ON PARENT_SCOPE)
    set(CMAKE_AUTORCC ON PARENT_SCOPE)

    set(CMAKE_AUTOUIC_SEARCH_PATHS
            ${PROJECT_ROOT}/ui
            ${PROJECT_ROOT}/src
            PARENT_SCOPE
    )

    find_package(Qt6 COMPONENTS ${QT_COMPONENTS} REQUIRED)

    set(QT_TARGETS "")
    foreach (COMPONENT ${QT_COMPONENTS})
        list(APPEND QT_TARGETS Qt6::${COMPONENT})
    endforeach ()

    set(QT_TARGETS ${QT_TARGETS} CACHE INTERNAL "Qt targets")
endfunction()

# 配置 Qt 部署
function(configure_qt_deployment TARGET_NAME)
    if (NOT TARGET ${TARGET_NAME})
        return()
    endif ()

    get_target_property(TARGET_LIBS ${TARGET_NAME} LINK_LIBRARIES)
    if (NOT TARGET_LIBS MATCHES "Qt6::")
        return()
    endif ()

    # 平台特定的部署配置
    if (WIN32)
        _configure_qt_windows_deployment(${TARGET_NAME})
    elseif (APPLE)
        _configure_qt_macos_deployment(${TARGET_NAME})
    elseif (UNIX)
        _configure_qt_linux_deployment(${TARGET_NAME})
    endif ()
endfunction()


# 内部函数：Windows Qt 部署
function(_configure_qt_windows_deployment TARGET_NAME)

    # 复制 Qt 库 ，如果你使用了下面的库，请从下方列表中删除
    set(DEPLOY_ARGS
            --verbose 2
            --no-compiler-runtime
            --no-quick-import # 不复制 QtQuick 相关文件
            --no-opengl-sw # 不复制软件 OpenGL 库
            --no-system-d3d-compiler # 不复制 D3D 编译器
            --no-translations # 不复制翻译文件
            --no-qml # 不复制 QML 文件
            --no-quick # 不复制 Quick 文件
            --no-sql # 不复制 SQL 文件
            --no-network # 不复制网络文件
            --force
    )

    # 构建时部署
    if(ENABLE_QT_DEPLOY)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND Qt6::windeployqt
                ${DEPLOY_ARGS}
                "$<TARGET_FILE:${TARGET_NAME}>"
                COMMENT "Deploying Qt dependencies for ${TARGET_NAME}..."
        )
    endif()

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
endfunction()


# 内部函数：macOS Qt 部署
function(_configure_qt_macos_deployment TARGET_NAME)
    # 首先找到 macdeployqt 工具
    get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
    set(MACDEPLOYQT_EXECUTABLE "${QT_BIN_DIR}/macdeployqt")

    if (NOT EXISTS "${MACDEPLOYQT_EXECUTABLE}")
        message(FATAL_ERROR "macdeployqt not found at: ${MACDEPLOYQT_EXECUTABLE}")
    endif ()

    # 确保在安装完成后执行 macdeployqt
    install(CODE "
        message(STATUS \"Deploying Qt dependencies for ${TARGET_NAME}...\")
        execute_process(
            COMMAND \"${MACDEPLOYQT_EXECUTABLE}\"
                \"\${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}.app\"
                -verbose=2
                -always-overwrite
            RESULT_VARIABLE DEPLOY_RESULT
            OUTPUT_VARIABLE DEPLOY_OUTPUT
            ERROR_VARIABLE DEPLOY_ERROR
        )

        if(NOT DEPLOY_RESULT EQUAL 0)
            message(FATAL_ERROR \"Failed to deploy Qt dependencies:\n\${DEPLOY_OUTPUT}\n\${DEPLOY_ERROR}\")
        endif()
    " COMPONENT Runtime)
endfunction()

# 内部函数：Linux Qt 部署
function(_configure_qt_linux_deployment TARGET_NAME)
    find_program(LINUXDEPLOYQT_EXECUTABLE linuxdeployqt)
    if (LINUXDEPLOYQT_EXECUTABLE)
        install(CODE "
            execute_process(
                COMMAND ${LINUXDEPLOYQT_EXECUTABLE}
                    \"\${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_DIR}/$<TARGET_FILE_NAME:${TARGET_NAME}>\"
                    -verbose=2
                    -always-overwrite
            )
        ")
    endif ()
endfunction()