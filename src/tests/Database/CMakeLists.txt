file(GLOB_RECURSE EXECUTABLE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

include_directories(
        ${CMAKE_SOURCE_DIR}/src/framework
        ${CMAKE_SOURCE_DIR}/src/game
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/src/framework
        ${MYSQL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
)

add_executable(dbtest ${EXECUTABLE_SRCS})

target_link_libraries(dbtest framework ${Boost_LIBRARIES})

if(WIN32)
    target_link_libraries(dbtest
            optimized ${MYSQL_LIBRARY}
            debug ${MYSQL_DEBUG_LIBRARY}
            )
    if(PLATFORM MATCHES X86)
        target_link_libraries(dbtest)
    endif()
endif()

if(UNIX)
    target_link_libraries(dbtest
            ${MYSQL_LIBRARY}
            )
endif()

configure_file(${CMAKE_SOURCE_DIR}/src/mangosd/mangosd.conf.dist.in ${CMAKE_CURRENT_BINARY_DIR}/mangosd.conf.dist)