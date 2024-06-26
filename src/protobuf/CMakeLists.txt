# Copyright 2015 cppio authors. All rights reserved.

find_package(absl REQUIRED PATHS "${THIRD_PARTY_ROOT}/grpc/lib/cmake")
find_package(utf8_range REQUIRED PATHS "${THIRD_PARTY_ROOT}/grpc/lib/cmake")
find_package(re2 REQUIRED PATHS "${THIRD_PARTY_ROOT}/grpc/lib/cmake")
find_package(c-ares REQUIRED PATHS "${THIRD_PARTY_ROOT}/grpc/lib/cmake")
find_package(Protobuf CONFIG REQUIRED PATHS "${THIRD_PARTY_ROOT}/grpc/lib/cmake")

message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
find_program(_PROTOBUF_PROTOC protoc PATHS "${THIRD_PARTY_ROOT}/grpc/bin")
message(STATUS "Using _PROTOBUF_PROTOC ${_PROTOBUF_PROTOC}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin PATHS "${THIRD_PARTY_ROOT}/grpc/bin")
message(STATUS "Using _GRPC_CPP_PLUGIN_EXECUTABLE ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

message(STATUS "Using CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")

# Topology
get_filename_component(topology_proto "topology.proto" ABSOLUTE)
get_filename_component(topology_proto_path "${topology_proto}" PATH)
set(TOPOLOGY_BUILDOUT_DIR "${topology_proto_path}")

set(topology_proto_srcs "${TOPOLOGY_BUILDOUT_DIR}/topology.pb.cc")
set(topology_proto_hdrs "${TOPOLOGY_BUILDOUT_DIR}/topology.pb.h")
set(topology_grpc_srcs "${TOPOLOGY_BUILDOUT_DIR}/topology.grpc.pb.cc")
set(topology_grpc_hdrs "${TOPOLOGY_BUILDOUT_DIR}/topology.grpc.pb.h")
add_custom_command(
      OUTPUT "${topology_proto_srcs}" "${topology_proto_hdrs}" "${topology_grpc_srcs}" "${topology_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${TOPOLOGY_BUILDOUT_DIR}"
        --cpp_out "${TOPOLOGY_BUILDOUT_DIR}"
        -I "${CMAKE_CURRENT_BINARY_DIR}" -I "${topology_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${topology_proto}"
      DEPENDS "${topology_proto}")

# Include generated *.pb.h files
include_directories("${TOPOLOGY_BUILDOUT_DIR}")

# topology_grpc_proto
add_library(topology_grpc_proto
  ${topology_grpc_srcs}
  ${topology_grpc_hdrs}
  ${topology_proto_srcs}
  ${topology_proto_hdrs})
target_link_libraries(topology_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Storage
get_filename_component(storage_proto "storage.proto" ABSOLUTE)
get_filename_component(storage_proto_path "${storage_proto}" PATH)
set(STORAGE_BUILDOUT_DIR "${storage_proto_path}")

set(storage_proto_srcs "${STORAGE_BUILDOUT_DIR}/storage.pb.cc")
set(storage_proto_hdrs "${STORAGE_BUILDOUT_DIR}/storage.pb.h")
set(storage_grpc_srcs "${STORAGE_BUILDOUT_DIR}/storage.grpc.pb.cc")
set(storage_grpc_hdrs "${STORAGE_BUILDOUT_DIR}/storage.grpc.pb.h")
add_custom_command(
      OUTPUT "${storage_proto_srcs}" "${storage_proto_hdrs}" "${storage_grpc_srcs}" "${storage_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${STORAGE_BUILDOUT_DIR}"
        --cpp_out "${STORAGE_BUILDOUT_DIR}"
        -I "${CMAKE_CURRENT_BINARY_DIR}" -I "${storage_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${storage_proto}"
      DEPENDS "${storage_proto}")

# Include generated *.pb.h files
include_directories("${STORAGE_BUILDOUT_DIR}")

# storage_grpc_proto
add_library(storage_grpc_proto
  ${storage_grpc_srcs}
  ${storage_grpc_hdrs}
  ${storage_proto_srcs}
  ${storage_proto_hdrs})
target_link_libraries(storage_grpc_proto
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})