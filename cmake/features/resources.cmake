# 资源处理模块
include_guard(GLOBAL)

# 配置资源文件
function(configure_resources TARGET_NAME)
    set(ICON_PATH "${PROJECT_ROOT}/res/icon")

    if(WIN32)
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
    elseif(APPLE)
        set_target_properties(${TARGET_NAME} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE "app.icns"
            RESOURCE "${ICON_PATH}/macos/app.icns"
        )
    else()
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
    endif()
endfunction()