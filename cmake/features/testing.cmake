# 测试模块
include_guard(GLOBAL)

# 配置测试
function(configure_testing)
    if(BUILD_TESTS)
        enable_testing()

        # 显式列出测试源文件
        set(TEST_SOURCES
                # tests/test_main.cpp
        )

        add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

        target_include_directories(${PROJECT_NAME}_tests PRIVATE
                ${PROJECT_ROOT}/src
                ${PROJECT_ROOT}/tests
        )

        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
                ${QT_TARGETS}
                FFTW3::fftw3f
        )

        configure_compiler(${PROJECT_NAME}_tests)
    endif()
endfunction()