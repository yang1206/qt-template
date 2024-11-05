cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0077 NEW)

project(QT_Template VERSION 0.1 LANGUAGES CXX)

# 基础设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 指定编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# IDE文件分组
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# 编译器选项
if(MSVC)
        add_compile_options("/utf-8" "/MP")
        add_definitions(-DUNICODE -D_UNICODE)
endif()

# 第三方库根目录
set(LIBRARY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rd")

# 源文件收集
file(GLOB_RECURSE PROJECT_SOURCES
        "src/*.cpp"
        "src/*.h"
        "${LIBRARY_ROOT}/qcustomplot/*.h"
        "${LIBRARY_ROOT}/qcustomplot/*.cpp"
)

# UI文件和资源文件
set(QT_UI "${CMAKE_CURRENT_SOURCE_DIR}/ui/mainwindow.ui")
set(QT_RES
        "${LIBRARY_ROOT}/elawidget/include/ElaWidgetTools.qrc" # elawidget 的资源文件
        "${CMAKE_CURRENT_SOURCE_DIR}/res/icon.qrc" # 项目自身的资源文件
)

# 设置 UI 文件的搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

# Qt配置
set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR}) # 确保设置了 Qt6_DIR 环境变量
set(QT_COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport # QCustomPlot 需要
)
find_package(Qt6 COMPONENTS ${QT_COMPONENTS} REQUIRED)

# 生成可执行文件
if(WIN32)
        set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
        add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES} ${QT_UI} ${QT_RES} ${APP_ICON_RESOURCE_WINDOWS})
elseif(APPLE)
        set(MACOSX_BUNDLE_ICON_FILE app.icns)
        set(APP_ICON_MACOSX "${CMAKE_CURRENT_SOURCE_DIR}/app.icns")
        set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources"
        )
        add_executable(${PROJECT_NAME} MACOSX_BUNDLE
                ${PROJECT_SOURCES}
                ${QT_UI}
                ${QT_RES}
                ${APP_ICON_MACOSX}
        )
        set_target_properties(${PROJECT_NAME} PROPERTIES
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_GUI_IDENTIFIER "com.yang1206.${PROJECT_NAME}"
                MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
                MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
                MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        )
else()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${QT_UI} ${QT_RES})
endif()

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
        "${LIBRARY_ROOT}/elawidget/include"
        "${LIBRARY_ROOT}/qcustomplot"
        "${LIBRARY_ROOT}/qwindowkit/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_BINARY_DIR}"
)

# 查找 vcpkg 库
find_package(FFTW3f CONFIG REQUIRED)

# 设置库文件后缀
if(WIN32)
        if(MSVC)
                set(LIB_SUFFIX ".lib")
                set(SHARED_LIB_SUFFIX ".dll")
        else()
                set(LIB_SUFFIX ".a")
                set(SHARED_LIB_SUFFIX ".dll")
        endif()
elseif(APPLE)
        set(LIB_SUFFIX ".dylib")
        set(SHARED_LIB_SUFFIX ".dylib")
else()
        set(LIB_SUFFIX ".so")
        set(SHARED_LIB_SUFFIX ".so")
endif()

# 设置库文件路径
set(ELAWIDGET_LIB "${LIBRARY_ROOT}/elawidget/lib/libelawidgettools${SHARED_LIB_SUFFIX}")
set(QWINDOWKIT_CORE_LIB "${LIBRARY_ROOT}/qwindowkit/lib/libQWKCore${SHARED_LIB_SUFFIX}")
set(QWINDOWKIT_WIDGETS_LIB "${LIBRARY_ROOT}/qwindowkit/lib/libQWKWidgets${SHARED_LIB_SUFFIX}")

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
        "${ELAWIDGET_LIB}" # 使用完整路径
        FFTW3::fftw3f
        "${QWINDOWKIT_CORE_LIB}" # 使用完整路径
        "${QWINDOWKIT_WIDGETS_LIB}" # 使用完整路径
)

# 复制动态库
if(WIN32)
        set(SHARED_LIBS
                "${ELAWIDGET_LIB}"
                "${QWINDOWKIT_CORE_LIB}"
                "${QWINDOWKIT_WIDGETS_LIB}"
        )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SHARED_LIBS}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
elseif(APPLE)
        set(SHARED_LIBS
                "${ELAWIDGET_LIB}"
                "${QWINDOWKIT_CORE_LIB}"
                "${QWINDOWKIT_WIDGETS_LIB}"
        )

        # macOS 特定的库安装路径
        set(BUNDLE_LIB_DIR "${PROJECT_NAME}.app/Contents/Frameworks")

        # 复制动态库到应用程序包中
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../Frameworks
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SHARED_LIBS}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/../Frameworks
        )

        # 修复库的安装名称
        foreach(LIB ${SHARED_LIBS})
                get_filename_component(LIB_NAME ${LIB} NAME)
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND install_name_tool -change ${LIB}
                        "@executable_path/../Frameworks/${LIB_NAME}"
                        $<TARGET_FILE:${PROJECT_NAME}>
                )
        endforeach()
endif()

# 安装配置
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
        LIBRARY DESTINATION lib
)

if(WIN32)
        # 安装Qt插件
        get_target_property(QtCore_LOCATION Qt6::Core LOCATION)
        get_filename_component(QT_BIN_DIR ${QtCore_LOCATION} DIRECTORY)
        get_filename_component(QT_ROOT_DIR ${QT_BIN_DIR} DIRECTORY)
        install(FILES "${QT_ROOT_DIR}/plugins/platforms/qwindows${SHARED_LIB_SUFFIX}"
                DESTINATION bin/platforms
        )
endif()

# CPack打包配置
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")
set(CPACK_GENERATOR "ZIP")
include(CPack)
