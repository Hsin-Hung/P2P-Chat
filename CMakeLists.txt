cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(P2P_Chat)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "system: ${CMAKE_SYSTEM}")
message(STATUS "build_type: ${CMAKE_BUILD_TYPE}")
message(STATUS "paths:")
message(STATUS " - ${PROJECT_SOURCE_DIR}")
message(STATUS " - ${CMAKE_INSTALL_PREFIX}")
message(STATUS " - ${CMAKE_SOURCE_DIR}")
message(STATUS " - ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS)
message(STATUS "compiler:")
message(STATUS " - ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - ${CMAKE_CXX_COMPILER}")
message(STATUS " - ${CMAKE_CXX_FLAGS}")
message(STATUS)
message(STATUS "cmake:")
message(STATUS " - ${CMAKE_VERSION}")
message(STATUS " - ${CMAKE_COMMAND}")
message(STATUS " - ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS " - ${CMAKE_GENERATOR}")
message(STATUS " - ${CMAKE_BUILD_TOOL}")
message(STATUS)

find_package(Boost 1.71.0)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SERVER_SRC_FILES ${PROJECT_SOURCE_DIR}/src/server/*.cpp)
file(GLOB PEER_SRC_FILES ${PROJECT_SOURCE_DIR}/src/peer/*.cpp)

add_executable(Server ${SERVER_SRC_FILES})
add_executable(Peer ${PEER_SRC_FILES})


target_include_directories(Server PRIVATE ${PROJECT_SOURCE_DIR}/src/server ${PROJECT_SOURCE_DIR}/include)
target_include_directories(Peer PRIVATE ${PROJECT_SOURCE_DIR}/src/peer ${PROJECT_SOURCE_DIR}/include)
target_compile_options(Peer PRIVATE -fcoroutines-ts)

 
