add_executable(server_example main.cpp)

target_link_libraries(server_example PRIVATE daq::opendaq CycloneDDS-CXX::ddscxx)

add_dependencies(
    unitree_module
    daq::native_stream_srv_module
)