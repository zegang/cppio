# Copyright 2025 service authors. All rights reserved.
# See the AUTHORS file for names of contributors.

file(GLOB service_SOURCES *.cc)

add_library(service ${service_SOURCES})

target_compile_definitions(service PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

target_include_directories(service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(service PRIVATE ${CPPIO_INCLUDE_DIR})

target_link_libraries(service PRIVATE topology_grpc_proto)
target_link_libraries(service PRIVATE storage_grpc_proto)
target_link_libraries(service PRIVATE storage)
target_link_libraries(service PRIVATE volume)

# Link Boost/spdlog/RocksDB libraries
target_link_libraries(service PRIVATE ${Boost_LIBRARIES})
target_link_libraries(service PRIVATE spdlog::spdlog_header_only)
target_link_libraries(service PUBLIC RocksDB::rocksdb)
# target_link_libraries(service PRIVATE miniocpp::miniocpp)
target_link_libraries(service PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)