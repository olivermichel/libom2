cmake_minimum_required(VERSION 3.10)
project(libom2)

set(CMAKE_CXX_STANDARD 11)

set(CATCH_VERSION 2.2.2)

if (NOT EXISTS ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
    file(DOWNLOAD
            https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp
            ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
endif ()

enable_testing()

add_executable(test_runner test/test_runner.cc include/om/om.h
        test/async/poll_test.cc
        test/net/ip4_addr_test.cc
        test/net/mac_addr_test.cc
        test/net/socket_test.cc)

target_include_directories(test_runner PUBLIC test/include)
target_include_directories(test_runner PUBLIC include)

add_custom_target(doc
        COMMAND doxygen libom.doxyfile
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/doc)