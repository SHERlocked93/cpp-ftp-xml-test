cmake_minimum_required(VERSION 3.26)
project(ftp_xml_test)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)
include_directories(./ftp)
include_directories(./xml)

# Locate libcURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

message(${CURL_INCLUDE_DIRS})

add_executable(ftp_xml_test
        main.cpp
        xml/pugixml.cpp
        FtpCtrl.cpp
        ftp/CurlHandle.cpp
        ftp/FTPClient.cpp
)

target_link_libraries(${PROJECT_NAME} curl)
