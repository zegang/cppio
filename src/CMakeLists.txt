# Copyright 2025 cppio authors. All rights reserved.
# See the AUTHORS file for names of contributors.

# gRPC
add_subdirectory(protobuf)
include_directories(protobuf)

# include_directories(include)
set(CPPIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add_subdirectory(util)

add_subdirectory(volume)
add_subdirectory(storage)
add_subdirectory(service)
add_subdirectory(hsd)

add_subdirectory(cli)
# 
# # Add source files
# file(GLOB SOURCES *.cc)

# # Create an executable target from sources
# add_executable(cppio ${SOURCES})
# target_compile_definitions(cppio PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})
# # target_include_directories(cppio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(cppio PRIVATE cli)

# # Link Boost/spdlog/RocksDB libraries
# target_link_libraries(cppio PRIVATE ${Boost_LIBRARIES} spdlog::spdlog_header_only)
# target_link_libraries(cppio PUBLIC RocksDB::rocksdb)
# target_link_libraries(cppio PRIVATE topology_grpc_proto)
# target_link_libraries(cppio PRIVATE storage_grpc_proto)
# # target_link_libraries(cppio PRIVATE miniocpp::miniocpp)
# target_link_libraries(cppio PRIVATE aws-cpp-sdk-s3 aws-cpp-sdk-core)