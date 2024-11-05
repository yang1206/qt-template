# QT Template

一个基于 Qt6 的跨平台应用模板，集成了现代化的 UI 框架和构建系统。

## 功能特性

- 跨平台支持 (Windows, macOS, Linux)
- 现代化的 UI 框架
  - ElaWidgetTools 提供丰富的自定义控件
  - QCustomPlot 提供高性能图表支持
  - QWindowKit (可选) 提供类原生窗口体验
- 自适应深色/浅色主题
- vcpkg 包管理集成
- CMake 现代化构建系统

## 技术栈

- Qt 6.0+
- CMake 3.16+
- C++17
- vcpkg 包管理器

## 目录结构
```
├── 3rd/                    # 第三方库
│   ├── elawidget/         # ElaWidget库
│   └── qcustomplot/       # QCustomPlot库
├── src/                    # 源代码
├── ui/                     # UI文件
├── CMakeLists.txt         # CMake配置
├── CMakePresets.json      # CMake预设
├── vcpkg.json             # vcpkg依赖配置
└── vcpkg-configuration.json # vcpkg仓库配置
```

## 构建步骤

### Windows
```bash
# 克隆项目
git clone https://github.com/yang1206/qt-template
cd qt-template

# 使用CMake预设构建
cmake --preset default
cmake --build build
```

### macOS
```bash
# 克隆项目
git clone https://github.com/yang1206/qt-template
cd qt-template

# 安装依赖（如果需要）
brew install cmake ninja qt@6

# 使用CMake预设构建
cmake --preset default
cmake --build build
```

### Linux
```bash
# 克隆项目
git clone https://github.com/yang1206/qt-template
cd qt-template

# 安装依赖（Ubuntu为例）
sudo apt update
sudo apt install cmake ninja-build qt6-base-dev

# 使用CMake预设构建
cmake --preset default
cmake --build build
```

## 依赖说明

### Qt组件
- Qt::Core
- Qt::Widgets
- Qt::Gui
- Qt::PrintSupport

### 第三方库
- FFTW3 (通过vcpkg安装)
- ElaWidget (预编译库，已包含在3rd目录)
- QCustomPlot (源码包含在3rd目录)

## 输出目录结构

构建输出将按以下结构组织：
```
build/
├── bin/              # 可执行文件
│   ├── Debug/       # Debug版本
│   └── Release/     # Release版本
└── lib/             # 库文件
    ├── Debug/
    └── Release/
```

## 打包说明

项目支持使用CPack进行打包：

```bash
# 在build目录下执行
cpack
```

打包输出将位于 `build/package` 目录。

## 注意事项

1. 确保vcpkg正确安装并配置了环境变量
2. Windows平台默认使用UTF-8编码
3. 项目使用C++17标准
4. 确保第三方库的依赖路径正确

## 常见问题

1. **找不到Qt**
   - 检查Qt安装路径是否正确
   - 确保Qt版本为Qt6
   - 验证PATH环境变量中包含Qt bin目录

2. **vcpkg相关错误**
   - 确认`VCPKG_ROOT`环境变量设置正确
   - 检查vcpkg.json中的依赖配置
   - 尝试手动运行`vcpkg install`

3. **编译错误**
   - 确保使用了正确的编译器版本
   - 检查是否满足C++17要求
   - 验证所有必需的依赖库是否正确安装

## 许可证

本项目采用 MIT 许可证。详情请见 [LICENSE](LICENSE) 文件。
