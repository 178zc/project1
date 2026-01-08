cmake_minimum_required(VERSION 3.14)
project(MFCApplicationTests)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找GoogleTest
find_package(GTest REQUIRED)

# 包含MFC头文件和库
include_directories(${CMAKE_SOURCE_DIR}/../MFCApplication1)
link_directories(${CMAKE_SOURCE_DIR}/../MFCApplication1)

# 添加可执行文件
add_executable(MFCApplicationTests
    test_main.cpp
    test_document.cpp
    test_view.cpp
    test_edit_operations.cpp
    test_search_replace.cpp
    test_file_operations.cpp
    test_integration.cpp
    test_mfc_mocks.cpp
)

# 链接库
target_link_libraries(MFCApplicationTests
    GTest::gtest
    GTest::gtest_main
    mfc140u.lib  # 根据实际MFC版本调整
    # 其他需要的MFC库
)

# 启用测试
enable_testing()
add_test(NAME AllTests COMMAND MFCApplicationTests)