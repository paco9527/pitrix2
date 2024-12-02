add_subdirectory(network)
add_subdirectory(render)
add_subdirectory(log)

# set(SOURCES ${SOURCES} PARENT_SCOPE)
# message(${INCLUDE_DIR})
set(INCLUDE_DIR "${INCLUDE_DIR}" PARENT_SCOPE)

include_directories(${INCLUDE_DIR})
add_library(modules STATIC ${SOURCES})
