# Cyclone DDS
# Build options for CycloneDDS - edit if a specific build is needed
set(ENABLE_SECURITY OFF)
set(ENABLE_SSL OFF)
set(ENABLE_SHM OFF)
set(BUILD_TESTING OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_IDLC OFF CACHE BOOL "" FORCE)
set(BUILD_DDSPERF OFF CACHE BOOL "" FORCE)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF CACHE BOOL "" FORCE)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Disable warning as error
if (MSVC)
    add_compile_options(/WX-)
endif()

FetchContent_Declare(
    cyclonedds
    GIT_REPOSITORY https://github.com/eclipse-cyclonedds/cyclonedds.git
    GIT_TAG 0.10.5
    GIT_PROGRESS   ON
	SYSTEM
    FIND_PACKAGE_ARGS 0.10.5 GLOBAL
)

FetchContent_MakeAvailable(cyclonedds)

FetchContent_GetProperties(cyclonedds)
if (NOT cyclonedds_POPULATED)
    message(FATAL_ERROR "cyclonedds not populated")
endif()

message(STATUS "cyclonedds source dir: ${cyclonedds_SOURCE_DIR}")
message(STATUS "cyclonedds binary dir: ${cyclonedds_BINARY_DIR}")

# Cyclone DDS - CXX
# Build options for CycloneDDS-Cxx - edit if IDL is needed (CycloneDDS also needs modifications to successfully build IDLCXX)
set(BUILD_IDLLIB OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FetchContent_Declare(
    cyclonedds_cxx
    GIT_REPOSITORY https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git
    GIT_TAG 0.10.5
    GIT_PROGRESS   ON
	SYSTEM
    FIND_PACKAGE_ARGS 0.10.5 GLOBAL
)

FetchContent_MakeAvailable(cyclonedds_cxx)

# ---- FIX: add CycloneDDS source includes to the *real* target ----
target_include_directories(ddscxx
    PRIVATE
    ${cyclonedds_SOURCE_DIR}/src/core/include
    ${cyclonedds_SOURCE_DIR}/src/core/ddsi/include
)
