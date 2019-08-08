cmake_minimum_required (VERSION 2.6)
project (VeheMencE)

# Don't put the lib prefix in front of the livrary, so that it nicely loads in python
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Control the verbosity of the build
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Python version
set(PYTHON_MIN_VERSION "2.7" CACHE STRING "Minimum Python version to look for")
set(Boost_PYTHON_VERSION_TAG "python27" CACHE STRING "Suffix for boost::python shared library (X in libboost_pythonX.py)")

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Python headers and configure the build
find_package(PythonLibs ${PYTHON_MIN_VERSION} REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
message("-- Using python version: ${PYTHONLIBS_VERSION_STRING}")

# Find boost headers and configure the build
set(Boost_NO_BOOST_CMAKE ON)
set(_Boost_STACKTRACE_BASIC_HEADERS     "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_BACKTRACE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_ADDR2LINE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_NOOP_HEADERS      "boost/stacktrace.hpp")
find_package(Boost 1.54 REQUIRED COMPONENTS ${Boost_PYTHON_VERSION_TAG} stacktrace_basic stacktrace_noop thread log log_setup)
include_directories(${Boost_INCLUDE_DIRS})
message("-- Using Boost version: ${Boost_VERSION}" )
add_definitions(-DBOOST_STACKTRACE_LINK -DBOOST_LOG_DYN_LINK)

# Find CAENLIB library and headers
set(CAEN_LIBRARY "CAENVME")
find_library(CAENLIB ${CAEN_LIBRARY})
find_file(CAENLIBHEADERS CAENVMElib.h)
if(NOT (CAENLIB AND CAENLIBHEADERS))
message(FATAL_ERROR "CAEN library not found")
endif()
get_filename_component(CAENLIB_INCLUDE_DIR ${CAENLIBHEADERS} DIRECTORY)
get_filename_component(CAENLIB_LIBRARY_PATH ${CAENLIB} DIRECTORY)
message("-- Using CAEN library.")
include_directories(${CAENLIB_INCLUDE_DIR})
link_directories(${CAENLIB_LIBRARY_PATH})
add_definitions(-DLINUX)

# Specify the sources
set(VEHEMENCE_SOURCES 
     "src/CaenetBridge.cpp"
     "src/Discri.cpp"
     "src/logger.cpp"
     "src/Scaler.cpp"
     "src/TTCvi.cpp"
     "src/VmeController.cpp"
     "src/CommonDef.cpp"
     "src/HVmodule.cpp"
     "src/N470HVmodule.cpp"
     "src/SY527PowerSystem.cpp"
     "src/PythonModule.cpp"
     "src/V1190Event.cpp"
     "src/TDC.cpp"
     "src/VmeBoard.cpp"
     "src/VmeUsbBridge.cpp"
   )

# Build and link the shared library
add_library(VeheMencE SHARED ${VEHEMENCE_SOURCES})
target_include_directories(VeheMencE PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(VeheMencE ${CAEN_LIBRARY} ${Boost_LIBRARIES})

# Examples
add_subdirectory (examples)

#installation
install (TARGETS VeheMencE DESTINATION lib)
install (TARGETS VeheMencE DESTINATION lib/python${PYTHON_MIN_VERSION}/site-packages)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include/vehemence FILES_MATCHING PATTERN "*.h")
