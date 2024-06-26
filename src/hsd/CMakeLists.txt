# Copyright 2025 cppio authors. All rights reserved.
# See the AUTHORS file for names of contributors.

file(GLOB HSD_SOURCES *.cc ../protobuf/*.cc)

add_executable(hsd ${HSD_SOURCES})

target_compile_definitions(hsd PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

message(STATUS "CPPIO_INCLUDE_DIR: ${CPPIO_INCLUDE_DIR}")
target_include_directories(hsd PRIVATE ${CPPIO_INCLUDE_DIR})

target_link_libraries(hsd PRIVATE topology_grpc_proto)
target_link_libraries(hsd PRIVATE storage_grpc_proto)
target_link_libraries(hsd PRIVATE storage)
target_link_libraries(hsd PRIVATE service)

# Link Boost/spdlog/RocksDB libraries
target_link_libraries(hsd PRIVATE ${Boost_LIBRARIES})
target_link_libraries(hsd PRIVATE spdlog::spdlog_header_only)
target_link_libraries(hsd PUBLIC RocksDB::rocksdb)
# target_link_libraries(hsd PRIVATE miniocpp::miniocpp)
target_link_libraries(hsd PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)