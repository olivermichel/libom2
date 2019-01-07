# libom2

[![Build Status](https://travis-ci.org/olivermichel/libom2.svg?branch=master)](https://travis-ci.org/olivermichel/libom2)

## CMake Automatic Download and Include

    set(LIBOM_VERSION master) # or commit hash
    set(LIBOM_HEADER_LOCATION ${CMAKE_HOME_DIRECTORY}/ext/include/om/om.h)
    
    if(NOT EXISTS ${LIBOM_HEADER_LOCATION})
        file(DOWNLOAD
            https://raw.githubusercontent.com/olivermichel/libom2/${LIBOM_VERSION}/include/om/om.h
            ${LIBOM_HEADER_LOCATION})
        message(STATUS "Downloading libom: /ext/include/om/om.h - done")
    endif()

## Run Unit Tests

    mkdir build && cd build
    cmake ..
    make
    make test

## Generate Doxygen Documentation

*requires Doxygen*

    mkdir build && cd build
    cmake ..
    make
    make doc
