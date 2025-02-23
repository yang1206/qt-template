# 资源处理模块
include_guard(GLOBAL)

# 配置资源文件
function(configure_resources TARGET_NAME)
    set(ICON_PATH "${PROJECT_ROOT}/res/icon")

    if (WIN32)
        configure_file(
                "${ICON_PATH}/windows/app.rc.in"
                "${CMAKE_BINARY_DIR}/generated/app.rc"
                @ONLY
        )
        configure_file(
                "${ICON_PATH}/windows/app.ico"
                "${CMAKE_BINARY_DIR}/generated/app.ico"
                COPYONLY
        )

        file(READ "${CMAKE_BINARY_DIR}/generated/app.rc" RC_CONTENT)
        string(REPLACE "res/icon/windows/app.ico" "app.ico" RC_CONTENT "${RC_CONTENT}")
        file(WRITE "${CMAKE_BINARY_DIR}/generated/app.rc" "${RC_CONTENT}")

        target_sources(${TARGET_NAME} PRIVATE
                "${CMAKE_BINARY_DIR}/generated/app.rc"
        )
    elseif (APPLE)
        set(ICON_FILE "${ICON_PATH}/macos/app.icns")
        if (NOT EXISTS "${ICON_FILE}")
            message(FATAL_ERROR "Icon file not found: ${ICON_FILE}")
        endif ()

        # 正确设置 Bundle 图标
        set_target_properties(${TARGET_NAME} PROPERTIES
                MACOSX_BUNDLE_ICON_FILE "app.icns"
        )

        # 将图标文件添加到目标并设置正确的包位置
        target_sources(${TARGET_NAME} PRIVATE "${ICON_FILE}")
        # 开发时复制图标文件到构建目录
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET_NAME}>/Resources"
                COMMAND ${CMAKE_COMMAND} -E copy
                "${ICON_FILE}"
                "$<TARGET_BUNDLE_CONTENT_DIR:${TARGET_NAME}>/Resources/app.icns"
                COMMENT "Copying app icon for development..."
        )
    else ()
        configure_file(
                "${ICON_PATH}/linux/${PROJECT_NAME}.desktop.in"
                "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
                @ONLY
        )

        install(FILES
                "${CMAKE_BINARY_DIR}/generated/${PROJECT_NAME}.desktop"
                DESTINATION "share/applications"
        )
        install(FILES
                "${ICON_PATH}/linux/app.png"
                DESTINATION "share/icons/hicolor/256x256/apps"
                RENAME "${PROJECT_NAME}.png"
        )
    endif ()
endfunction()