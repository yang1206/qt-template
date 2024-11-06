# QT Template

一个基于 Qt6 的现代化跨平台应用模板，集成了完整的构建系统和丰富的开发工具支持。

## 功能特性

- 完整的跨平台支持 (Windows, macOS, Linux)
- 现代化的构建系统
  - CMake 预设配置
  - vcpkg 包管理集成
  - ccache 编译加速支持
  - 预编译头文件支持
- 丰富的开发工具支持
  - 多种构建类型支持（Debug/Release/RelWithDebInfo等）
  - 代码分析工具集成（Address/Thread/UB Sanitizer）
  - 代码覆盖率测试支持
- 自动化的依赖管理
  - Qt 组件自动配置
  - 第三方库自动检测和链接
  - 平台特定依赖处理
- 完整的应用打包支持
  - Windows (NSIS 安装包)
  - macOS (.app 和 .dmg)
  - Linux (DEB 和 RPM)

## 项目结构

```
├── cmake/                  # CMake模块目录
│   ├── build_config.cmake     # 构建配置
│   ├── packaging.cmake        # 打包配置
│   ├── project_settings.cmake # 项目设置
│   ├── templates/            # 模板文件
│   └── version_control.cmake  # 版本控制
├── 3rd/                    # 第三方库
│   ├── elawidget/         # ElaWidget库
│   ├── qwindowkit/        # QWindowKit库
│   └── qcustomplot/       # QCustomPlot库
├── src/                    # 源代码
├── ui/                     # UI文件
├── res/                    # 资源文件
│   └── icon/              # 应用图标
├── tests/                  # 测试代码
├── CMakeLists.txt         # 主CMake配置
├── CMakePresets.json      # CMake预设
└── vcpkg.json             # vcpkg依赖配置
```

## 构建类型

支持以下构建类型：
- Debug：调试版本
- Release：发布版本
- RelWithDebInfo：带调试信息的发布版本
- MinSizeRel：最小体积版本
- Coverage：代码覆盖率测试版本
- ASan：内存错误检测版本
- TSan：线程错误检测版本
- UBSan：未定义行为检测版本

## 构建步骤

### 前置要求
- CMake 3.16+
- Qt 6.0+
- C++17 兼容的编译器
  - GCC 9.0+
  - Clang 10.0+
  - MSVC 19.20+
  - AppleClang 12.0+

### Windows
```bash
cmake --preset windows-release
cmake --build --preset windows-release
```

### macOS
```bash
cmake --preset macos-release
cmake --build --preset macos-release
```

### Linux
```bash
cmake --preset linux-release
cmake --build --preset linux-release
```

## 依赖项

### 必需依赖
- Qt 6.0+ 组件：
  - Qt::Core
  - Qt::Widgets
  - Qt::Gui
  - Qt::PrintSupport
- FFTW3 (通过 vcpkg 安装)

### 可选依赖
- ccache (用于加速编译)
- ElaWidget (UI 组件库)
- QWindowKit (窗口管理)
- QCustomPlot (图表支持)

## 版本控制

项目包含完整的版本信息追踪：
- 项目版本号
- Git 提交信息
- 构建时间戳
- 编译器信息
- 构建类型

## 打包说明

### Windows
```bash
cmake --build --preset windows-release --target package
```

### macOS
```bash
cmake --build --preset macos-release --target package
```

### Linux
```bash
cmake --build --preset linux-release --target package
```

## 开发工具支持

### 代码分析
- Address Sanitizer：`cmake --preset asan`
- Thread Sanitizer：`cmake --preset tsan`
- UB Sanitizer：`cmake --preset ubsan`

### 性能优化
- LTO 支持（Release 模式）
- 预编译头文件支持
- ccache 编译缓存

## 许可证

本项目采用 MIT 许可证。详情请见 [LICENSE](LICENSE) 文件。
