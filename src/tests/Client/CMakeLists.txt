file(GLOB_RECURSE EXECUTABLE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/framework
        ${CMAKE_SOURCE_DIR}/src/game
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/src/framework
)

add_executable(clitest ${EXECUTABLE_SRCS})

target_link_libraries(clitest framework game ${Boost_LIBRARIES})

configure_file(${CMAKE_SOURCE_DIR}/src/mangosd/mangosd.conf.dist.in ${CMAKE_CURRENT_BINARY_DIR}/mangosd.conf.dist)