cmake_minimum_required(VERSION 3.16)
project(spo_Compiler)
set(CMAKE_CXX_STANDARD 20)
IF (WIN32)
    set(Boost_INCLUDE_DIR c:/MinGWBoost/include/)
    set(Boost_LIBRARY_DIR c:/MinGWBoost/lib/)
ENDIF ()
find_package(Boost COMPONENTS system filesystem regex REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF ()
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif ()
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(lib/googletest)
