# ==================== 构建和编译器配置模块 ====================
# 该模块负责处理所有与构建和编译器相关的配置，包括构建类型、编译器选项等

# ========== 构建类型配置 ==========
# 定义可用的构建类型
set(AVAILABLE_BUILD_TYPES
  Debug # 调试版本：包含调试信息，无优化
  Release # 发布版本：优化级别最高，无调试信息
  RelWithDebInfo # 带调试信息的发布版本：高优化级别，包含调试信息
  MinSizeRel # 最小体积版本：优化目标是最小化代码体积
  Coverage # 代码覆盖率测试版本：用于生成代码覆盖率报告
  ASan # Address Sanitizer：用于检测内存错误
  TSan # Thread Sanitizer：用于检测线程相关错误
  UBSan # Undefined Behavior Sanitizer：用于检测未定义行为
)

# 如果没有设置构建类型，默认使用 Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

# 设置构建类型的可选值（在 CMake GUI 中显示）
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${AVAILABLE_BUILD_TYPES})

# ========== 编译器版本检查 ==========
function(check_compiler_version)
  # GCC 编译器版本检查
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
      message(FATAL_ERROR "GCC version must be at least 9.0!")
    endif()

  # Clang 编译器版本检查
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
      message(FATAL_ERROR "Clang version must be at least 10.0!")
    endif()

  # AppleClang 编译器版本检查
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12.0)
      message(FATAL_ERROR "AppleClang version must be at least 12.0!")
    endif()

  # MSVC 编译器版本检查
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
      message(FATAL_ERROR "MSVC version must be at least 19.20!")
    endif()
  endif()
endfunction()

# ========== 编译器和构建配置函数 ==========
function(configure_compiler_and_build TARGET_NAME)
  # 检查编译器版本
  check_compiler_version()

  # 设置 C++ 标准和编译器特性
  set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17 # 使用 C++17 标准
    CXX_STANDARD_REQUIRED ON # 强制要求 C++17
    CXX_EXTENSIONS OFF # 禁用编译器特定扩展
    WINDOWS_EXPORT_ALL_SYMBOLS ON # Windows DLL 导出所有符号
  )

  # 根据编译器类型配置特定选项
  if(MSVC)
    configure_msvc_options(${TARGET_NAME})
  else()
    configure_gcc_clang_options(${TARGET_NAME})
  endif()

  # 配置构建类型特定选项
  configure_build_type_options(${TARGET_NAME})

  # 配置平台特定选项
  configure_platform_options(${TARGET_NAME})

  # 输出配置信息
  message(STATUS "Configured compiler options for target: ${TARGET_NAME}")
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
  message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
  message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
endfunction()

# ========== MSVC 特定配置 ==========
function(configure_msvc_options TARGET_NAME)
  target_compile_options(${TARGET_NAME} PRIVATE
    /utf-8 # 使用 UTF-8 编码
    /MP # 多处理器编译
    /W4 # 警告级别 4（较严格）
    /WX # 将警告视为错误
    /permissive- # 标准符合模式
    /Zc:__cplusplus # 启用正确的 __cplusplus 宏
    /Zc:preprocessor # 使用新的预处理器
    /EHsc # 标准异常处理
    /wd4251 # 忽略 DLL 接口警告
    /wd4275 # 忽略 DLL 接口警告
  )
endfunction()

# ========== GCC/Clang 特定配置 ==========
function(configure_gcc_clang_options TARGET_NAME)
  target_compile_options(${TARGET_NAME} PRIVATE
    -Wall # 启用所有常见警告
    -Wextra # 启用额外警告
    -Wpedantic # 严格遵循标准
    -Wno-extra-semi # 忽略多余分号警告
    -Wno-deprecated-declarations # 忽略弃用声明警告

    # GCC 特定选项
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>

    # Clang 特定选项
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
  )
endfunction()

# ========== 构建类型特定配置 ==========
function(configure_build_type_options TARGET_NAME)
  if(NOT MSVC)
    # Debug 构建类型配置
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(${TARGET_NAME} PRIVATE -O0 -g)

    # Release 构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
      target_compile_options(${TARGET_NAME} PRIVATE -O3)

      # 链接时优化 (LTO) 配置
      if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET_NAME} PRIVATE -flto -fno-fat-lto-objects)
        set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-flto")
      elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${TARGET_NAME} PRIVATE -flto=thin)
        set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-flto=thin")
      endif()

      target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)

    # RelWithDebInfo 构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      target_compile_options(${TARGET_NAME} PRIVATE -O2 -g)
      target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)

    # MinSizeRel 构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
      target_compile_options(${TARGET_NAME} PRIVATE -Os)
      target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)

    # Coverage 构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "Coverage")
      target_compile_options(${TARGET_NAME} PRIVATE -O0 -g --coverage)
      target_link_options(${TARGET_NAME} PRIVATE --coverage)

    # ASan（地址检查器）构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "ASan")
      target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=address -fno-omit-frame-pointer)
      target_link_options(${TARGET_NAME} PRIVATE -fsanitize=address)

    # TSan（线程检查器）构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "TSan")
      target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=thread)
      target_link_options(${TARGET_NAME} PRIVATE -fsanitize=thread)

    # UBSan（未定义行为检查器）构建类型配置
    elseif(CMAKE_BUILD_TYPE STREQUAL "UBSan")
      target_compile_options(${TARGET_NAME} PRIVATE -O1 -g -fsanitize=undefined)
      target_link_options(${TARGET_NAME} PRIVATE -fsanitize=undefined)
    endif()
  endif()
endfunction()

# ========== 平台特定配置 ==========
function(configure_platform_options TARGET_NAME)
  # 平台特定的链接选项
  if(APPLE)
    # macOS 平台：启用死代码剥离
    target_link_options(${TARGET_NAME} PRIVATE "LINKER:-dead_strip")
  elseif(UNIX AND NOT APPLE)
    # Linux 平台：优化链接选项
    target_link_options(${TARGET_NAME} PRIVATE
      "LINKER:--as-needed" # 只链接必要的库
      "LINKER:--gc-sections" # 移除未使用的代码段
    )
  endif()

  # 添加平台特定的预处理器定义
  target_compile_definitions(${TARGET_NAME} PRIVATE
    $<$<BOOL:${WIN32}>:NOMINMAX> # 禁用 Windows 的 min/max 宏
    $<$<BOOL:${WIN32}>:_CRT_SECURE_NO_WARNINGS> # 禁用 CRT 安全警告
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN> # 精简 Windows 头文件
  )
endfunction()