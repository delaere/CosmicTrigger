# test executables

link_libraries(VeheMencE)

include_directories(${CMAKE_SOURCE_DIR} PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(exampleFull exampleFull.cpp)
add_executable(exampleTDC exampleTDC.cpp)
add_executable(normalOp normalOp.cpp)


