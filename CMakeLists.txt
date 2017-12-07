cmake_minimum_required(VERSION 3.8)

project(md2017)

find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_CXX_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS})

include_directories(${CMAKE_SOURCE_DIR}/include ${MPI_CXX_INCLUDE_PATH})

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(lib_2d ${SOURCES})
add_library(lib_3d ${SOURCES})
set_target_properties(lib_3d PROPERTIES COMPILE_FLAGS "-DMD_HAVE_3D")

file(GLOB MAINS "${CMAKE_SOURCE_DIR}/*.cpp")

foreach(SOURCE ${MAINS})
    get_filename_component(NAME ${SOURCE} NAME_WE)
    add_executable(${NAME}_2d ${SOURCE})
    target_link_libraries(${NAME}_2d lib_2d ${MPI_CXX_LIBRARIES})

    add_executable(${NAME}_3d ${SOURCE})
    target_compile_definitions(${NAME}_3d PUBLIC -DMD_HAVE_3D)
    target_link_libraries(${NAME}_3d lib_3d ${MPI_CXX_LIBRARIES})
endforeach()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif(DOXYGEN_FOUND)