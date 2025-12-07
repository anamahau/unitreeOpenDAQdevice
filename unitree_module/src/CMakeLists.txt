get_current_folder_name(TARGET_FOLDER_NAME)
set(LIB_NAME unitree_module)
set(MODULE_HEADERS_DIR ../include/${TARGET_FOLDER_NAME})

project(unitree_module VERSION 1.0.0 LANGUAGES CXX)

find_package(CycloneDDS)
find_package(CycloneDDS-CXX)
find_package(helloworld_unitree)

set(SRC_Include common.h
                module_dll.h
                unitree_module.h
                unitree_device.h
                unitree_channel.h
)

set(SRC_Srcs module_dll.cpp
             unitree_module.cpp
             unitree_device.cpp
             unitree_channel.cpp
)

prepend_include(${TARGET_FOLDER_NAME} SRC_Include)

source_group("module" FILES ${MODULE_HEADERS_DIR}/unitree_module.h
                            ${MODULE_HEADERS_DIR}/unitree_channel.h
                            ${MODULE_HEADERS_DIR}/unitree_device.h
                            ${MODULE_HEADERS_DIR}/module_dll.h
                            module_dll.cpp
                            unitree_module.cpp
                            unitree_device.cpp
                            unitree_channel.cpp
)


add_library(${LIB_NAME} SHARED ${SRC_Include}
                               ${SRC_Srcs}
)

add_library(${SDK_TARGET_NAMESPACE}::${LIB_NAME} ALIAS ${LIB_NAME})

if (MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /bigobj)
endif()

target_link_libraries(${LIB_NAME} PUBLIC daq::opendaq
)

target_include_directories(${LIB_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                                              $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                                              $<INSTALL_INTERFACE:include>
)

opendaq_set_module_properties(${LIB_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${LIB_NAME})



find_package(helloworld_unitree)
if(helloworld_unitree_FOUND)
    message(STATUS "helloworld_unitree was found!")
else()
    message(WARNING "helloworld_unitree not found!")
endif()
find_package(helloworld)
if(helloworld_FOUND)
    message(STATUS "helloworld was found!")
else()
    message(WARNING "helloworld not found!")
endif()
if(CycloneDDS-CXX_FOUND)
    message(STATUS "CycloneDDS-CXX was found!")
else()
    message(WARNING "CycloneDDS-CXX not found!")
endif()
if(CycloneDDS_FOUND)
    message(STATUS "CycloneDDS was found!")
else()
    message(WARNING "CycloneDDS not found!")
endif()
find_package(ddscxxHelloworldSubscriber)
if(ddscxxHelloworldSubscriber_FOUND)
    message(STATUS "ddscxxHelloworldSubscriber was found!")
else()
    message(WARNING "ddscxxHelloworldSubscriber not found!")
endif()

# set(CycloneDDS_DIR "D:/Razno/unitree_go2/cyclonedds-master/cyclonedds-master/install/lib/cmake/CycloneDDS")
# find_package(CycloneDDS REQUIRED)
# set(CycloneDDS_CXX_DIR "D:/Razno/unitree_go2/cyclonedds-master/cyclonedds-master/install/lib/cmake/CycloneDDS-CXX")
# find_package(CycloneDDS-CXX REQUIRED)
# 
# target_include_directories(${LIB_NAME}
#     PRIVATE
#         "${CycloneDDS_DIR}/../../.."
# )
# 
# target_link_libraries(${LIB_NAME}
#     PRIVATE
#         # "${Cyclone_DDS_DIR}/../../../lib/ddsc.lib"
#         # "${Cyclone_DDS_DIR}/../../../lib/ddscxx.lib"
#         "D:/Razno/unitree_go2/cyclonedds-master/cyclonedds-master/install/lib/ddsc.lib"
#         "D:/Razno/unitree_go2/cyclonedds-master/cyclonedds-master/install/lib/ddscxx.lib"
# )
# 
# target_link_libraries(unitree_module
#     PRIVATE
#     CycloneDDS::ddsc
#     CycloneDDS::cxx
# )
