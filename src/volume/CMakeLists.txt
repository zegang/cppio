# Copyright 2025 volume authors. All rights reserved.
# See the AUTHORS file for names of contributors.

file(GLOB_RECURSE VOLUME_SOURCES *.cc)

add_library(volume ${VOLUME_SOURCES})

target_compile_definitions(volume PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

target_include_directories(volume PRIVATE ${CPPIO_INCLUDE_DIR})

# Link Boost/spdlog/RocksDB libraries
target_link_libraries(volume PRIVATE ${Boost_LIBRARIES})
target_link_libraries(volume PRIVATE spdlog::spdlog_header_only)
target_link_libraries(volume PUBLIC RocksDB::rocksdb)
target_link_libraries(volume PRIVATE topology_grpc_proto)
target_link_libraries(volume PRIVATE storage_grpc_proto)
# target_link_libraries(volume PRIVATE miniocpp::miniocpp)
target_link_libraries(volume PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)