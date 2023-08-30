project(EasyAsioHost)
cmake_minimum_required(VERSION 3.00)

#====================================================================================

add_library(asio_sdk_lib)
target_include_directories(asio_sdk_lib PUBLIC
        asio_sdk/common
        asio_sdk/host
        asio_sdk/host/pc
)
target_sources(asio_sdk_lib PRIVATE
        asio_sdk/common/asio.cpp
        asio_sdk/common/combase.cpp
        asio_sdk/common/debugmessage.cpp
        asio_sdk/common/dllentry.cpp
        asio_sdk/common/register.cpp

        asio_sdk/host/ASIOConvertSamples.cpp
        asio_sdk/host/asiodrivers.cpp

        asio_sdk/host/pc/asiolist.cpp
)

#====================================================================================

add_library(easy_asio_lib)
target_include_directories(easy_asio_lib PUBLIC asio)
target_sources(easy_asio_lib PRIVATE
        asio/AsioChannel.cpp
        asio/AsioCommon.cpp
        asio/AsioDriver.cpp
        asio/AsioDriverManager.cpp

        asio/SampleFormatConverter.cpp
)
target_link_libraries(easy_asio_lib PUBLIC asio_sdk_lib log_lib)

#====================================================================================

add_library(log_lib)
target_include_directories(log_lib PUBLIC log)
target_sources(log_lib PRIVATE log/log.cpp)

#====================================================================================

add_executable(example_app)
target_sources(example_app PRIVATE example_application/main.cpp)
target_link_libraries(example_app PUBLIC easy_asio_lib)