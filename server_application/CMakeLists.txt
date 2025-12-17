# ==== SERVER APPLICATION ===
# Instruct the build system to make a new program ("EXE" file)
add_executable(server_example main.cpp)

# Connect the "EXE" file with libraries it uses
target_link_libraries(server_example
    PRIVATE
    daq::opendaq
)
# Leftover from opendaq example_module
add_dependencies(
    unitree_module
    daq::native_stream_srv_module
)
# ===== ENDOF SERVER APPLICATION ===

# ===== CYCLONE EXAMPLE APP =======
# Use Cyclone IDL compiler to generate source code
idlcxx_generate(TARGET helloworlddata FILES HelloWorldData.idl WARNINGS no-implicit-extensibility)

# Instruct the build system to make a new program ("EXE" file)
add_executable(cyclone_example cyclone_main.cpp)

# Connect the "EXE" file with libraries it uses
target_link_libraries(cyclone_example
    PRIVATE
    daq::opendaq
    CycloneDDS-CXX::ddscxx
    helloworlddata
)
# ==== ENDOF CYCLONE EXAMPLE APP ===