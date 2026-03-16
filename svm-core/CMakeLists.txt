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
        service/add.cpp
        service/add.h
        service/delete.cpp
        service/delete.h
        service/config.cpp
        service/config.h
        service/sdkconfig.cpp
        service/sdkconfig.h
        util/ConfigUtil.cpp
        util/ConfigUtil.h
        util/SysEnvUtil.cpp
        util/SysEnvUtil.h
        util/StringUtil.cpp
        util/StringUtil.h
        util/FileUtil.cpp
        util/FileUtil.h
)

target_include_directories(svm PRIVATE ${CMAKE_BINARY_DIR})
