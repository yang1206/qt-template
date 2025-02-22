# 测试模块
include_guard(GLOBAL)

# 配置测试
function(configure_testing)
    if(BUILD_TESTS)
        enable_testing()
        
        # 收集测试源文件
        file(GLOB_RECURSE TEST_SOURCES
            "${PROJECT_ROOT}/tests/*.cpp"
            "${PROJECT_ROOT}/tests/*.h"
        )

        # 创建测试可执行文件
        add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

        # 配置测试目标
        target_include_directories(${PROJECT_NAME}_tests PRIVATE
            ${PROJECT_ROOT}/src
            ${PROJECT_ROOT}/tests
        )

        # 链接测试依赖
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
            ${QT_TARGETS}
            FFTW3::fftw3f
        )

        # 配置编译器选项
        configure_compiler(${PROJECT_NAME}_tests)
        configure_build_type_options(${PROJECT_NAME}_tests)
    endif()
endfunction()