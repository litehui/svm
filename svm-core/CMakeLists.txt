cmake_minimum_required(VERSION 3.31)
project(svm VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

add_executable(svm main.cpp
        service/show.cpp
        service/show.h
        service/use.cpp
        service/use.h
        service/backupSysEvn.h
        service/backupSysEvn.cpp
        util/ConfigUtil.cpp
        util/ConfigUtil.h
        util/SysEnvUtil.cpp
        util/SysEnvUtil.h
)

target_include_directories(svm PRIVATE ${CMAKE_BINARY_DIR})
