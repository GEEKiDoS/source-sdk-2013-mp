# Name of the project
project( raytrace )

# Sources and headers files in this project
set( RAYTRACE_SOURCES
	raytrace.cpp
	trace2.cpp
	trace3.cpp
	)

# Add a static library
add_library( raytrace STATIC ${RAYTRACE_SOURCES} )

# Pre-processor definitions
target_compile_definitions( raytrace PRIVATE ${SHARED_LIB_COMPILE_DEFS} )

# Linker definitions
set_target_properties( raytrace PROPERTIES
	COMPILE_FLAGS "${SHARED_LIB_LINKER_DEFS}"
	)

# Include directories
target_include_directories( raytrace PRIVATE
	${SHARED_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/utils/common"
	)

# No prefixes
set_target_properties( raytrace PROPERTIES PREFIX "" )
