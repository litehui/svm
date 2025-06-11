cmake_minimum_required(VERSION 3.31)
project(svm VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

add_executable(svm main.cpp
        service/use.cpp
        service/use.h
        util/ConfigUtil.cpp
        util/ConfigUtil.h
        service/show.cpp
        service/show.h
        util/sysEnvUtil.cpp
        util/sysEnvUtil.h
)

target_include_directories(svm PRIVATE ${CMAKE_BINARY_DIR})
