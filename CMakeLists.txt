
cmake_minimum_required(VERSION 3.5)
project(libom2)

set(CMAKE_CXX_STANDARD 11)

set(CATCH_VERSION 2.13.8)

if (NOT EXISTS ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
    file(DOWNLOAD
            https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp
            ${CMAKE_HOME_DIRECTORY}/test/include/catch.h)
endif ()

enable_testing()

add_executable(test_runner test/test_runner.cc
        test/async/poll_test.cc
        test/concurrency/queue_test.cc
        test/concurrency/thread_joiner_test.cc
        test/concurrency/thread_pool_test.cc
        test/etc/etc_test.cc
        test/file/file_test.cc
        test/file/simple_binary_reader_test.cc
        test/file/simple_binary_writer_test.cc
        test/net/arp_header_test.cc
        test/net/ethernet_header_test.cc
        test/net/icmp_header_test.cc
        test/net/ip4_addr_test.cc
        test/net/ip4_flow_key_test.cc
        test/net/ip4_header_test.cc
        test/net/mac_addr_test.cc
        test/net/net_test.cc
        test/net/packet_header_test.cc
        test/net/socket_test.cc
        test/net/tcp_header_test.cc
        test/net/udp_header_test.cc
        test/sys/sys_test.cc)

target_include_directories(test_runner PUBLIC test/include)
target_include_directories(test_runner PUBLIC include)

target_link_libraries(test_runner pthread)

add_test(NAME arp_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner arp_header)
add_test(NAME etc WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner etc)
add_test(NAME ethernet_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner ethernet_header)
add_test(NAME file WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner file)
add_test(NAME icmp_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner icmp_header)
add_test(NAME ip4_addr WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner ip4_addr)
add_test(NAME ip4_flow_key WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner ip4_flow_key)
add_test(NAME ip4_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner ip4_header)
add_test(NAME mac_addr WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner mac_addr)
add_test(NAME net WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner net)
add_test(NAME packet_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner packet_header)
add_test(NAME poll WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner poll)
add_test(NAME simple_binary_reader WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner simple_binary_reader)
add_test(NAME simple_binary_writer WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner simple_binary_writer)
add_test(NAME socket WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner socket)
add_test(NAME tcp_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner tcp_header)
add_test(NAME thread_joiner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner thread_joiner)
add_test(NAME thread_pool WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner thread_pool)
add_test(NAME udp_header WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner udp_header)
add_test(NAME sys WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner sys)
add_test(NAME queue WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND test_runner queue)

add_custom_target(doc
        COMMAND doxygen libom.doxyfile
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/doc)
