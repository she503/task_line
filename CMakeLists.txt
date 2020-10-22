project(task_refline_editor)
cmake_minimum_required(VERSION 2.8)

set(TERGEO_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${TERGEO_ROOT_PATH})

include(${TERGEO_ROOT_PATH}/cmake/3rdparty.cmake)
include(${TERGEO_ROOT_PATH}/cmake/options.cmake)
include(${TERGEO_ROOT_PATH}/cmake/find_qt.cmake)

include(${TERGEO_ROOT_PATH}/cmake/find_common2.cmake)
include(${TERGEO_ROOT_PATH}/cmake/find_framework2.cmake)
include(${TERGEO_ROOT_PATH}/cmake/find_calib2.cmake)
include(${TERGEO_ROOT_PATH}/cmake/find_vehicle2.cmake)
include(${TERGEO_ROOT_PATH}/cmake/find_hdmap2.cmake)

# include_directories(./proto)

# aux_source_directory(./proto PROTO_SRC)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
file(GLOB RESOURCES "*.qrc")

ADD_EXECUTABLE(task_refline_editor 
    ${SOURCES} ${HEADERS} ${RESOURCES} ${PROTO_SRC} ${MODULE_CONTROL_SRC})
target_link_libraries(task_refline_editor 
        ${QT5_LIBRARIES} ${TERGEO_COMMON_LIB}
        ${TERGEO_FRAMEWORK_LIB} ${TERGEO_HDMAP_LIB}
        ${TERGEO_TASK_LIB} ${TERGEO_VEHICLE_LIB}
        ${catkin_LIBRARIES} glog gflags protobuf)



