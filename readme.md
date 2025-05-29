# Qt Template

一个现代化的 Qt6 应用程序模板，提供完整的 CMake 构建系统和开发工具链支持。

[![Windows Build](https://github.com/yang1206/qt-template/actions/workflows/windows-build.yml/badge.svg)](https://github.com/yang1206/qt-template/actions/workflows/windows-build.yml)
[![macOS Build](https://github.com/yang1206/qt-template/actions/workflows/macos-build.yml/badge.svg)](https://github.com/yang1206/qt-template/actions/workflows/macos-build.yml)

## 特性

- 🚀 现代化 CMake 构建系统
  - 预设配置支持 (Windows MSVC/MinGW, macOS, Linux)
  - vcpkg 包管理集成
  - 多配置生成器支持
  - 预编译头文件优化

- 📦 完整的依赖管理
  - Qt6 组件自动配置
  - vcpkg 依赖自动处理
  - 平台特定库管理

- 🛠 开发工具集成
  - ccache 编译加速
  - IDE 工具链配置
  - 代码分析工具支持

- 📱 跨平台打包系统
  - Windows: NSIS 安装包
  - macOS: DMG 打包
  - Linux: DEB/RPM 支持

## 快速开始

### 前置要求

- CMake 3.16+
- Qt 6.0+
- vcpkg
- 支持的编译器:
  - Windows: MSVC 2019+ 或 MinGW-w64
  - macOS: AppleClang 12.0+
  - Linux: GCC 9.0+ 或 Clang 10.0+

### 克隆项目

```bash
git clone --recursive https://github.com/yang1206/qt-template.git
cd qt-template
```

## 构建类型

支持以下构建类型：

- Debug：调试版本
- Release：发布版本
- RelWithDebInfo：带调试信息的发布版本

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

## 开发指南

详细的开发指南请参考 [开发指南文档](docs/development-guide.md)。

## 许可证

本项目采用 MIT 许可证。详情请见 [LICENSE](LICENSE) 文件。
