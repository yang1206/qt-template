# ==================== 版本控制模块 ====================
# 该模块负责处理版本信息、Git 信息的获取和版本文件的生成

# ========== Git信息获取 ==========
function(get_git_info)
  # 查找 Git 程序
  find_package(Git QUIET)

  if(GIT_FOUND)
    # 获取当前分支名称
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${PROJECT_ROOT}
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE GIT_RESULT
    )

    # 如果获取分支失败，设置为 unknown
    if(NOT GIT_RESULT EQUAL 0)
      set(GIT_BRANCH "unknown")
    endif()

    # 获取最新的提交哈希（短格式）
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${PROJECT_ROOT}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE GIT_RESULT
    )

    # 如果获取提交哈希失败，设置为 unknown
    if(NOT GIT_RESULT EQUAL 0)
      set(GIT_COMMIT_HASH "unknown")
    endif()

    # 获取最新的提交时间（ISO 格式）
    execute_process(
      COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=iso-strict
      WORKING_DIRECTORY ${PROJECT_ROOT}
      OUTPUT_VARIABLE GIT_COMMIT_DATE
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE GIT_RESULT
    )

    # 如果获取提交时间失败，设置为 unknown
    if(NOT GIT_RESULT EQUAL 0)
      set(GIT_COMMIT_DATE "unknown")
    endif()

    # 检查工作目录是否有未提交的修改
    execute_process(
      COMMAND ${GIT_EXECUTABLE} status --porcelain
      WORKING_DIRECTORY ${PROJECT_ROOT}
      OUTPUT_VARIABLE GIT_STATUS
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE GIT_RESULT
    )

    # 设置工作目录状态标志
    if(NOT GIT_RESULT EQUAL 0)
      set(GIT_DIRTY "unknown")
    else()
      if("${GIT_STATUS}" STREQUAL "")
        set(GIT_DIRTY "false")
      else()
        set(GIT_DIRTY "true")
      endif()
    endif()

    # 将获取的 Git 信息设置为父作用域变量
    set(GIT_BRANCH "${GIT_BRANCH}" PARENT_SCOPE)
    set(GIT_COMMIT_HASH "${GIT_COMMIT_HASH}" PARENT_SCOPE)
    set(GIT_COMMIT_DATE "${GIT_COMMIT_DATE}" PARENT_SCOPE)
    set(GIT_DIRTY "${GIT_DIRTY}" PARENT_SCOPE)

  else()
    # 如果未找到 Git，设置所有变量为 unknown
    set(GIT_BRANCH "unknown" PARENT_SCOPE)
    set(GIT_COMMIT_HASH "unknown" PARENT_SCOPE)
    set(GIT_COMMIT_DATE "unknown" PARENT_SCOPE)
    set(GIT_DIRTY "unknown" PARENT_SCOPE)
  endif()
endfunction()

# ========== 版本文件生成 ==========
function(generate_version_file OUTPUT_DIR)
  # 获取最新的 Git 信息
  get_git_info()

  # 获取当前时间戳
  string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

  # 使用模板生成版本头文件
  configure_file(
    ${PROJECT_ROOT}/cmake/templates/version.h.in
    ${OUTPUT_DIR}/version.h
    @ONLY # 只替换 @VAR@ 格式的变量
  )
endfunction()

# ========== 构建时间更新目标 ==========
function(add_build_time_target TARGET_NAME)
  # 设置版本头文件目录
  set(VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
  file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})

  # 生成初始版本文件
  generate_version_file(${VERSION_HEADER_DIR})

  # 创建一个自定义命令来更新版本文件
  # 这个命令会在每次构建时执行
  add_custom_command(
    OUTPUT ${VERSION_HEADER_DIR}/version.h
    COMMAND ${CMAKE_COMMAND}
    -DPROJECT_ROOT=${PROJECT_ROOT}
    -DVERSION_PROJECT_NAME=${VERSION_PROJECT_NAME}
    -DVERSION_PROJECT_VERSION=${VERSION_PROJECT_VERSION}
    -DVERSION_BUILD_TYPE=${VERSION_BUILD_TYPE}
    -DVERSION_COMPILER_ID=${VERSION_COMPILER_ID}
    -DVERSION_COMPILER_VERSION=${VERSION_COMPILER_VERSION}
    -DOUTPUT_DIR=${VERSION_HEADER_DIR}
    -DSCRIPT_MODE=ON
    -P ${PROJECT_ROOT}/cmake/version_control.cmake
    WORKING_DIRECTORY ${PROJECT_ROOT}
    COMMENT "更新版本信息"
    VERBATIM
  )

  # 创建一个自定义目标，依赖于版本文件
  add_custom_target(
    UpdateVersion
    DEPENDS ${VERSION_HEADER_DIR}/version.h
  )

  # 将主目标依赖于版本更新目标
  # 这确保每次构建时都会更新版本信息
  add_dependencies(${TARGET_NAME} UpdateVersion)
endfunction()

# ========== 脚本模式入口点 ==========
# 当在脚本模式下运行时，直接生成版本文件
if(DEFINED SCRIPT_MODE)
  generate_version_file(${OUTPUT_DIR})
  return()
endif()

# ========== 初始化版本文件 ==========
# 设置版本头文件目录并确保目录存在
set(VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})

# 生成初始版本文件
generate_version_file(${VERSION_HEADER_DIR})