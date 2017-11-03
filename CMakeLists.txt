cmake_minimum_required(VERSION 3.8)

project(md2017)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(lib ${SOURCES})

file(GLOB MAINS "${CMAKE_SOURCE_DIR}/*.cpp")

foreach(SOURCE ${MAINS})
    get_filename_component(NAME ${SOURCE} NAME_WE)
    add_executable(${NAME}_2d ${SOURCE})
    target_link_libraries(${NAME}_2d lib)
endforeach()

foreach(SOURCE ${MAINS})
    get_filename_component(NAME ${SOURCE} NAME_WE)
    add_executable(${NAME}_3d ${SOURCE})
    target_compile_definitions(${NAME}_3d PUBLIC -DMD_HAVE_3D)
    target_link_libraries(${NAME}_3d lib)
endforeach()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif(DOXYGEN_FOUND)