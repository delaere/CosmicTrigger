cmake_minimum_required (VERSION 2.6)
project (VeheMencE)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(PYTHON_MIN_VERSION "2.7" CACHE STRING "Minimum Python version to look for")
set(Boost_PYTHON_VERSION_TAG "python27" CACHE STRING "Suffix for boost::python shared library (X in libboost_pythonX.py)")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DLINUX -DBOOST_STACKTRACE_LINK -DBOOST_LOG_DYN_LINK)

set(Boost_NO_BOOST_CMAKE ON)

# Python headers
find_package(PythonLibs ${PYTHON_MIN_VERSION} REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# Find boost headers
set(_Boost_STACKTRACE_BASIC_HEADERS     "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_BACKTRACE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_ADDR2LINE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_NOOP_HEADERS      "boost/stacktrace.hpp")
find_package(Boost 1.54 REQUIRED)
find_package(Boost 1.54 COMPONENTS ${Boost_PYTHON_VERSION_TAG} stacktrace_basic stacktrace_noop thread log log_setup)
include_directories(${Boost_INCLUDE_DIRS})

set(VEHEMENCE_SOURCES 
     "src/CaenetBridge.cpp"
     "src/Discri.cpp"
     "src/logger.cpp"
     "src/Scaler.cpp"
     "src/TTCvi.cpp"
     "src/VmeController.cpp"
     "src/CommonDef.cpp"
     "src/HVmodule.cpp"
     "src/PythonModule.cpp"
     "src/TDC.cpp"
     "src/VmeBoard.cpp"
     "src/VmeUsbBridge.cpp"
   )

add_library(VMEPythonModule SHARED ${VEHEMENCE_SOURCES})

target_include_directories(VMEPythonModule PRIVATE ${CMAKE_SOURCE_DIR} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(VMEPythonModule CAENVME ${Boost_LIBRARIES})

add_subdirectory (examples)
