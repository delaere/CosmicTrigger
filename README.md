<table style="width:100%" align="center">
 <tr><td>
  <img src="https://github.com/delaere/VeheMencE/blob/master/images/Logo.png" width="250"></td><td>
  <h2>A simple library for VME access</h2></td>
 </tr>
</table>

## What is this?

This is a library developped in order to simplify the control of a VME system used in CP3 at UCLouvain. It is developped in C++ but comes with Python bindings.

This project is mostly a rewrite of the code developped in 2014 for the test of CMS tracker modules for the HL-LHC upgrade.

Status of the supported boards:

| Board     |  Purpose           | Implementaton   |  Tested       |  C++  | Python |
|-----------|--------------------|-----------------|---------------|-------|--------|
| V1718     | USB VME bridge     | Full            | yes           | yes   | yes    |
| SY527     | CAEN Power system  | Full            | yes           | yes   | yes    |
| N470      | CAEN NIM HV module | Full            | yes           | yes   | yes    |
| V288      | CAENET bridge      | Full            | yes           | yes   | yes    |
| V812      | COINC / DISCRI     | Full            | yes           | yes   | yes    |
| V1190     | TDC                | Full            | yes           | yes   | yes    |
| 1151N     | Lecroy Scaler      | Full            | yes           | yes   | yes    |
| TTCvi     | TTC controler      | Full            | yes           | yes   | yes    |

## Installation

### Requirements

- CMake >= 3.11
- C++17-compatible compiler
- python >= 2.7
- boost >= 1.54, components: python, stacktrace, thread, log, log
- CAENVMELib: can be downloaded from [here](https://www.caen.it/products/caenvmelib-library/), requires creating a CAEN account (free)

### Build

The project is built through cmake:
```
cmake -S . -B build
cmake --build build -j 4
[cmake --build build --target install]
```

### Notes for CentOS7 and similar dinosaurs

- Install modern CMake from EPEL (activate EPEL: `yum -y install epel-release`); `yum -y install cmake3.x86_64`. Note that you'll need to run with `cmake3` instead of `cmake`.
- Install boost-1.69: `yum install -y boost169-python2-devel.x86_64 boost169-stacktrace.x86_64 boost169-thread.x86_64 boost169-log.x86_64`
- Install [devtoolset](https://www.softwarecollections.org/en/scls/rhscl/devtoolset-8/) to have a newer compiler (tested with version 8); enable it using `scl enable devtoolset-8 $SHELL`
- Configure the build using `cmake3 -S . -B build -DBOOST_LIBRARYDIR=/usr/lib64/boost169 -DBOOST_INCLUDEDIR=/usr/include/boost169`

## Usage

Examples of the use of the library in C++ are provided in the example directory. The typical usage consists in:
 - the instantiation of a VmeUsbBridge
 - the instantiation of the class corrresponding to the desired module, passing a pointer to the VME controller.
 - in the case of CAEN HV modules, instantiate first the CAENET bridge, and then the HV module class, either directly (SY527PowerSystem or N470HVModule) or through HVModule::HVModuleFactory.
 
 Python bindings are also provided so all the steps above can be done either interactively or through a script in Python.
 
 In C++, a logging infrastructure is provided to help debugging the code. It uses the BOOST logging infrastructure and can be configured through a text file. ExampleFull.cpp shows how to configure the logger module. The following macros are provided:
 
 | macro | debug level | purpose |
 |-------|-------------|---------|
 |LOG_TRACE(ARG) | TRACE | code |
 |LOG_DEBUG(ARG) | DEBUG | code | 
 |LOG_INFO(ARG)  | INFO  | code |
 |LOG_WARN(ARG)  | WARNING | code |
 |LOG_ERROR(ARG) | ERROR   | code |
 |LOG_FATAL(ARG) | FATAL ERROR | code |
 |LOG_DATA_TRACE(ARG) | TRACE | user data |
 |LOG_DATA_DEBUG(ARG) | DEBUG | user data |
 |LOG_DATA_INFO(ARG)  | INFO  | user data |
 |LOG_DATA_WARN(ARG)  | WARNING | user data |
 |LOG_DATA_ERROR(ARG) | ERROR   | user data |
 |LOG_DATA_FATAL(ARG) | FATAL ERROR | user data |

## Contributing

You are welcome to provide patches and fixes through github. New modules implementation is especially welcome, but we require it to be tested with hardware first.
