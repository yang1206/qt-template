# Qt Template 开发指南

## 目录
- [项目结构](#项目结构)
- [依赖管理](#依赖管理)
- [构建系统](#构建系统)
- [资源管理](#资源管理)
- [打包系统](#打包系统)
- [开发工具](#开发工具)
- [故障排除](#故障排除)

## 项目结构

```plaintext
qt-template/
├── cmake/                  # CMake 模块
│   ├── core/              # 核心构建配置
│   │   ├── compiler.cmake    # 编译器配置
│   │   ├── options.cmake     # 构建选项
│   │   ├── platform.cmake    # 平台特定配置
│   │   └── utilities.cmake   # 通用工具函数
│   ├── features/          # 功能模块
│   │   ├── dependencies.cmake # 依赖管理
│   │   ├── qt.cmake          # Qt 配置
│   │   └── testing.cmake     # 测试配置
│   └── packaging/         # 打包配置
│       ├── common.cmake      # 通用打包配置
│       ├── windows.cmake     # Windows 打包
│       ├── macos.cmake       # macOS 打包
│       └── linux.cmake       # Linux 打包
├── src/                   # 源代码
│   ├── core/             # 核心功能
│   ├── ui/              # 界面相关
│   └── utils/           # 工具类
├── res/                   # 资源文件
│   ├── icon/            # 应用图标
│   └── images/          # 图片资源
├── 3rd/                   # 第三方库
└── tests/                 # 测试代码
```


## 依赖管理

### 添加本地依赖库

1. 在 `3rd` 目录下创建新库目录结构: 

```plaintext
3rd/
└── your_library/
    ├── include/          # 头文件
    └── lib/              # 预编译库文件
        ├── msvc/        # Windows MSVC
        ├── mingw/       # Windows MinGW
        ├── clang/       # macOS
        └── gcc/         # Linux
```
2. 修改 `cmake/features/dependencies.cmake` ：
```cmake
set(LOCAL_DEPS_INCLUDE
    "${THIRD_PARTY_ROOT}/your_library/include"
)

# 链接预编译库
set(LIB_NAME "your_library")
get_platform_library_name(${LIB_NAME} LIB_PATH)
target_link_libraries(${TARGET_NAME} PRIVATE ${LIB_PATH})
```

### 添加 Qt 模块
1. 在 `cmake/features/qt.cmake` 中添加：

```cmake
set(QT_COMPONENTS
    Core
    Widgets
    Gui
    PrintSupport
    # 添加新模块
    Network
    Multimedia
)
```

2. 如果新模块需要部署，修改部署配置：
```cmake
set(DEPLOY_ARGS
    --verbose 2
    # 删除不需要排除的模块
    --no-network  # 如果使用了 Network，删除此行
)
```

### 添加 vcpkg 依赖

1. 修改 vcpkg.json ：
```json
{
  "dependencies": [
    {
      "name": "your-new-dependency"
    }
  ]
}
```
2. 在 `cmake/features/dependencies.cmake` 中配置：
```cmake
find_package(YourNewDependency CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE YourNewDependency::YourNewDependency)
```

## 构建系统
#### 添加新的构建选项

1. 在 `cmake/core/options.cmake` 中添加：

```cmake
option(YOUR_NEW_OPTION "Description" OFF)

```

2. 在需要使用的地方：
```cmake
if(YOUR_NEW_OPTION)
    # 执行操作
endif()
```

### 添加新的预设配置

在 `CMakePresets.json` 中添加：

```json
{
  "configurePresets": [
    {
      "name": "your-preset",
      "inherits": "default-base",
      "generator": "Ninja Multi-Config",
      "cacheVariables": {
        "YOUR_VARIABLE": "value"
      }
    }
  ]
}
```

## 资源管理

### 添加图标资源

1. 将图标文件放入对应目录：
```plaintext
res/icon/
├── windows/
│   └── app.ico
├── macos/
│   └── app.icns
└── linux/
    └── app.png
```

2. 在 res/icon.qrc 中注册：

```xml
<RCC>
    <qresource prefix="/icons">
        <file>icon/windows/app.ico</file>
        <file>icon/macos/app.icns</file>
        <file>icon/linux/app.png</file>
    </qresource>
</RCC>
```
## 打包系统

### Windows 打包配置

在 `cmake/packaging/windows.cmake` 中：

```cmake

# 添加额外文件
install(FILES "${PROJECT_ROOT}/extra/file.txt"
        DESTINATION "extra"
        COMPONENT Runtime)

# 配置快捷方式
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\${PROJECT_NAME}\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'"
)
```

### macOS 打包配置

在 `cmake/packaging/macos.cmake` 中：

```cmake
# 配置 Info.plist
set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${PROJECT_ROOT}/res/macos/Info.plist"
)

# 配置图标
set(MACOSX_BUNDLE_ICON_FILE "app.icns")
```

## 开发工具

### 代码格式化

使用项目的 `.clang-format` 配置：
```bash
# 格式化单个文件
clang-format -i src/your_file.cpp

# 格式化整个项目
find src -iname "*.cpp" -o -iname "*.h" | xargs clang-format -i
```

## 性能优化

1. 启用链接时优化：
```bash
cmake --preset your-preset -DENABLE_LTO=ON
```

2. 使用预编译头文件：

```bash
cmake --preset your-preset -DENABLE_PCH=ON
```

3. 启用 ccache：

```bash
cmake --preset your-preset -DENABLE_CCACHE=ON
```

