# Copyright 2025 storage authors. All rights reserved.
# See the AUTHORS file for names of contributors.

file(GLOB_RECURSE STORAGE_SOURCES *.cc)

add_library(storage ${STORAGE_SOURCES})

target_compile_definitions(storage PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

target_include_directories(storage PRIVATE ${CPPIO_INCLUDE_DIR})
target_include_directories(storage PUBLIC ../volume)
target_include_directories(storage PUBLIC include)

target_link_libraries(storage PRIVATE topology_grpc_proto)
target_link_libraries(storage PRIVATE storage_grpc_proto)

# Link Boost/spdlog/RocksDB libraries
target_link_libraries(storage PRIVATE ${Boost_LIBRARIES})
target_link_libraries(storage PRIVATE spdlog::spdlog_header_only)
target_link_libraries(storage PUBLIC RocksDB::rocksdb)
# target_link_libraries(storage PRIVATE miniocpp::miniocpp)
target_link_libraries(storage PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)