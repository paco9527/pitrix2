add_subdirectory(script_handler)

# set(INCLUDE_DIR ${INCLUDE_DIR} CACHE INTERNAL "${INCLUDE_DIR}")
set(INCLUDE_DIR "${INCLUDE_DIR}" PARENT_SCOPE)

include_directories(${INCLUDE_DIR})
add_library(sys STATIC ${SOURCES})
