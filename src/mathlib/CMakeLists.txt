# Name of the project
project( mathlib )

# Sources and headers files in this project
set( MATHLIB_SOURCES
	almostequal.cpp
	anorms.cpp
	bumpvects.cpp
	color_conversion.cpp
	halton.cpp
	IceKey.cpp
	imagequant.cpp
	lightdesc.cpp
	mathlib_base.cpp
	noisedata.h
	polyhedron.cpp
	powsse.cpp
	quantize.cpp
	randsse.cpp
	simdvectormatrix.cpp
	sparse_convolution_noise.cpp
	spherical.cpp
	sse.cpp
	sse.h
	sseconst.cpp
	ssenoise.cpp
	vector.cpp
	vmatrix.cpp
	"${CMAKE_SOURCE_DIR}/public/mathlib/anorms.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/bumpvects.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/compressed_3d_unitvec.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/compressed_light_cube.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/compressed_vector.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/halton.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/IceKey.H"
	"${CMAKE_SOURCE_DIR}/public/mathlib/lightdesc.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/math_pfns.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/mathlib.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/noise.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/polyhedron.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/quantize.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/simdvectormatrix.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/spherical_geometry.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/ssemath.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/ssequaternion.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/vector.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/vector2d.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/vector4d.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/vmatrix.h"
	"${CMAKE_SOURCE_DIR}/public/mathlib/vplane.h"
	)

# Mac don't need these files
if( NOT APPLE )
	set( MATHLIB_SOURCES
		${MATHLIB_SOURCES}
		3dnow.cpp
		3dnow.h
		"${CMAKE_SOURCE_DIR}/public/mathlib/amd3dx.h"
	)
endif()

# Add a static library
add_library( mathlib STATIC ${MATHLIB_SOURCES} )

# Pre-processor definitions
target_compile_definitions( mathlib PRIVATE
	${SHARED_LIB_COMPILE_DEFS}
	MATHLIB_LIB
	)

# Linker definitions
set_target_properties( mathlib PROPERTIES
	COMPILE_FLAGS "${SHARED_LIB_LINKER_DEFS}"
	)

# Include directories
target_include_directories( mathlib PRIVATE
	${SHARED_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/public/mathlib"
	)

# No prefixes
set_target_properties( mathlib PROPERTIES PREFIX "" )
