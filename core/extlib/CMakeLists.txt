add_custom_target(
    extlib
    ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/build.sh 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_C_COMPILER}
    ${CMAKE_CXX_COMPILER}
)