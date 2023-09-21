
find_package (glog 0.6.0 REQUIRED)

include_directories("/root/spdlog/spdlog/include/")

add_executable(testStdout testStdout.cc)
target_link_libraries(testStdout xyLog pthread)


add_executable(testCompareWithGlog testCompareWithGlog.cc)
target_link_libraries(testCompareWithGlog xyLog pthread glog::glog)