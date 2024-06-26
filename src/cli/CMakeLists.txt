# Copyright 2025 cppio authors. All rights reserved.
# See the AUTHORS file for names of contributors.

file(GLOB cli_SOURCES *.cc ../protobuf/*.cc)

add_executable(cli ${cli_SOURCES})

target_compile_definitions(cli PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

message(STATUS "CPPIO_INCLUDE_DIR: ${CPPIO_INCLUDE_DIR}")
target_include_directories(cli PRIVATE ${CPPIO_INCLUDE_DIR})

target_link_libraries(cli PRIVATE topology_grpc_proto)
target_link_libraries(cli PRIVATE storage_grpc_proto)
# target_link_libraries(cli PRIVATE storage)
# target_link_libraries(cli PRIVATE service)

# Link Boost/spdlog/RocksDB libraries
target_link_libraries(cli PRIVATE ${Boost_LIBRARIES})
target_link_libraries(cli PRIVATE spdlog::spdlog_header_only)
# target_link_libraries(cli PUBLIC RocksDB::rocksdb)
# target_link_libraries(cli PRIVATE miniocpp::miniocpp)
# target_link_libraries(cli PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)