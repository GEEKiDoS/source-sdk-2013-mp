# Required minimum CMake version.
cmake_minimum_required( VERSION 3.6 )

# Name of the solution.
project( Source2013MP )

# Configurations.
option( DEDICATED "Optimize for dedicated servers." )
if( DEDICATED )
	set( SHARED_COMPILE_DEFS DEDICATED )
endif()

# --------------------------------------------------
# SHARED
# --------------------------------------------------

# ----------------------------------------
# PRE-PROCESSOR
# ----------------------------------------

# Pre-processor definitions common to all projects.
set( SHARED_COMPILE_DEFS
	${SHARED_COMPILE_DEFS}
	RAD_TELEMETRY_DISABLED
	VPROF_LEVEL=1
	)

# Pre-processor definitions based on compile configuration for all projects.
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	set( SHARED_COMPILE_DEFS
		${SHARED_COMPILE_DEFS}
		_DEBUG
	)
endif()

# Pre-processor definitions based on platform for all projects.
if( MSVC )
	set( SHARED_COMPILE_DEFS
		${SHARED_COMPILE_DEFS}
		_CRT_SECURE_NO_DEPRECATE
		_CRT_NONSTDC_NO_DEPRECATE
		_WIN32
		COMPILER_MSVC
	)

	if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			PLATFORM_64BITS
			_WIN64
			COMPILER_MSVC64
		)
	else()
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			COMPILER_MSVC32
		)
	endif()

	# You can force QuickTime on Windows by removing "BINK_VIDEO" and adding
	# "FORCE_QUICKTIME". This is pointless, but it's for information purposes.
	set( SHARED_COMPILE_DEFS
		${SHARED_COMPILE_DEFS}
		BINK_VIDEO
		AVI_VIDEO
		WMV_VIDEO
	)
else()
	set( SHARED_COMPILE_DEFS
		${SHARED_COMPILE_DEFS}
		GNUC
		POSIX
		_POSIX
		NO_HOOK_MALLOC
		NO_MALLOC_OVERRIDE
	)

	if( APPLE )
		# Mac uses QuickTime.
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			OSX
			_OSX
			QUICKTIME_VIDEO
			FORCE_QUICKTIME
			_DARWIN_UNLIMITED_SELECT
			FD_SETSIZE=10240
			OVERRIDE_V_DEFINES
			PNG_NO_PEDANTIC_WARNINGS
		)
	else()
		# Linux uses Bink.
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			LINUX
			_LINUX
			BINK_VIDEO
		)
	endif()

	# Use SDL2 on dedicated servers or add some OpenGL extras for clients.
	if( DEDICATED )
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			USE_SDL
		)
	else()
		set( SHARED_COMPILE_DEFS
			${SHARED_COMPILE_DEFS}
			GL_GLEXT_PROTOTYPES
			DX_TO_GL_ABSTRACTION
		)
	endif()
endif()

# ----------------------------------------
# LINKER
# ----------------------------------------

# Linker definitions based on platform for all projects.
if( MSVC )
	if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
		set( SHARED_LINKER_DEFS "${SHARED_LINKER_DEFS} /Zi" )
	else()
		if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
			set( SHARED_LINKER_DEFS "${SHARED_LINKER_DEFS} /ZI" )
		else()
			set( SHARED_LINKER_DEFS "${SHARED_LINKER_DEFS} /Z7" )
		endif()
	endif()
else()
	if( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
		set( SHARED_LINKER_DEFS "${SHARED_LINKER_DEFS} -ftree-vectorize -funswitch-loop" )
	endif()
endif()

# ----------------------------------------
# INCLUDE DIRECTORIES
# ----------------------------------------

# Include directories common to all projects
set( SHARED_INCLUDE_DIRS
	"${CMAKE_SOURCE_DIR}/common"
	"${CMAKE_SOURCE_DIR}/public"
	"${CMAKE_SOURCE_DIR}/public/tier0"
	"${CMAKE_SOURCE_DIR}/public/tier1"
)

# Unix based platforms need SDL2 includes.
if( UNIX )
	set( SHARED_INCLUDE_DIRS
		${SHARED_INCLUDE_DIRS}
		"${CMAKE_SOURCE_DIR}/thirdparty/SDL2"
	)

	if( APPLE )
		set( SHARED_INCLUDE_DIRS
			${SHARED_INCLUDE_DIRS}
			"/usr/include/malloc"
		)
	endif()
endif()

# --------------------------------------------------
# STATIC LIBRARIES
# --------------------------------------------------

# Pre-processor definitions common to all static libraries projects.
set( SHARED_LIB_COMPILE_DEFS
	${SHARED_COMPILE_DEFS}
	FRAME_POINTER_OMISSION_DISABLED
	)

# Pre-processor definitions based on platform common to all static libraries projects.
if( MSVC )
	set( SHARED_LIB_COMPILE_DEFS
		${SHARED_LIB_COMPILE_DEFS}
		_LIB
		_ALLOW_RUNTIME_LIBRARY_MISMATCH
		_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
		_ALLOW_MSC_VER_MISMATCH
	)

	set( SHARED_LIB_LINKER_DEFS "${SHARED_LINKER_DEFS} /GF /fp:fast /FC /MP /wd4316 /Gw /Zc:inline" )
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
		set( SHARED_LIB_COMPILE_DEFS
			${SHARED_LIB_COMPILE_DEFS}
			_HAS_ITERATOR_DEBUGGING=0
		)

		set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} /MTd" )
	else()
		set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} /MT /Oi /Ot /MT /Oy-" )
	endif()
endif()

if( MSVC )
	set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} /W4 /WX /arch:SSE" ) # VPC added "/Wp64" here but VS2013 bork about it.
	set( CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /ignore:4221" ) # This needs to be here for an unknown reason.
else()
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=gnu++1z -Wno-register -Wno-reorder" )
	set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} -m32 -march=pentium4 -msse2 -mtune=core2 -Wall -Wno-ignored-attributes -Wno-narrowing -Wno-unknown-pragmas -Wno-unused-local-typedefs -Wno-sign-compare -Wno-unused-function -Wno-unused-value -Wno-unused-result -Wno-unused-variable" )

	if( APPLE )
		set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} -Wno-absolute-value -Wno-inconsistent-missing-override -Wno-overloaded-virtual -Wno-undefined-bool-conversion -Wno-unused-private-field -Wno-unused-const-variable" )
	else()
		set( SHARED_LIB_LINKER_DEFS "${SHARED_LIB_LINKER_DEFS} -Wno-misleading-indentation -Wno-nonnull-compare -Wno-strict-aliasing -mfpmath=sse" )
	endif()
endif()

# Add the projects
add_subdirectory( mathlib )
add_subdirectory( raytrace )
add_subdirectory( tier1 )
