# Name of the project
project( tier1 )

# Sources and headers files in this project
set( TIER1_SOURCES
	bitbuf.cpp
	newbitbuf.cpp
	byteswap.cpp
	characterset.cpp
	checksum_crc.cpp
	checksum_md5.cpp
	checksum_sha1.cpp
	commandbuffer.cpp
	convar.cpp
	datamanager.cpp
	diff.cpp
	generichash.cpp
	ilocalize.cpp
	interface.cpp
	KeyValues.cpp
	kvpacker.cpp
	lzmaDecoder.cpp
	mempool.cpp
	memstack.cpp
	NetAdr.cpp
	splitstring.cpp
	rangecheckedvar.cpp
	reliabletimer.cpp
	stringpool.cpp
	strtools.cpp
	strtools_unicode.cpp
	tier1.cpp
	tokenreader.cpp
	sparsematrix.cpp
	uniqueid.cpp
	utlbuffer.cpp
	utlbufferutil.cpp
	utlstring.cpp
	utlsymbol.cpp
	utlbinaryblock.cpp
	snappy.cpp
	snappy-internal.h
	snappy-sinksource.cpp
	snappy-stubs-internal.cpp
	snappy-stubs-internal.h
	"${CMAKE_SOURCE_DIR}/public/datamap.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/bitbuf.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/byteswap.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/callqueue.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/characterset.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/checksum_crc.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/checksum_md5.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/checksum_sha1.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/CommandBuffer.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/convar.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/datamanager.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/delegates.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/diff.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/fmtstr.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/functors.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/generichash.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/iconvar.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/ilocalize.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/interface.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/KeyValues.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/kvpacker.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/lzmaDecoder.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/lzss.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/mempool.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/memstack.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/netadr.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/processor_detect.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/rangecheckedvar.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/refcount.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/smartptr.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/snappy.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/snappy-sinksource.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/stringpool.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/strtools.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/tier1.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/tokenreader.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlbidirectionalset.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlblockmemory.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlbuffer.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlbufferutil.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlcommon.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utldict.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlenvelope.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlfixedmemory.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlhandletable.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlhash.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlhashtable.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utllinkedlist.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlmap.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlmemory.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlmultilist.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlpriorityqueue.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlqueue.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlrbtree.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/UtlSortVector.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlstack.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlstring.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/UtlStringMap.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlsymbol.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlsymbollarge.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlvector.h"
	"${CMAKE_SOURCE_DIR}/public/tier1/utlbinaryblock.h"
	"${CMAKE_SOURCE_DIR}/utils/lzma/C/LzmaDec.h"
	"${CMAKE_SOURCE_DIR}/utils/lzma/C/LzmaDec.c"
	"${CMAKE_SOURCE_DIR}/utils/lzma/C/7zTypes.h"
	)

# Platform specific files
if( WIN32 )
	set( TIER1_SOURCES
		${TIER1_SOURCES}
		processor_detect.cpp
		"${CMAKE_SOURCE_DIR}/common/xbox/xboxstubs.h"
		"${CMAKE_SOURCE_DIR}/public/tier1/uniqueid.h"
		)

	set_source_files_properties( processor_detect.cpp PROPERTIES
		COMPILE_FLAGS /EHsc
	)
else()
	set( TIER1_SOURCES
		${TIER1_SOURCES}
		processor_detect_linux.cpp
		)

	if( NOT APPLE )
		set( TIER1_SOURCES
			${TIER1_SOURCES}
			qsort_s.cpp
			pathmatch.cpp
		)
	endif()
endif()

# Add a static library
add_library( tier1 STATIC ${TIER1_SOURCES} )

# Pre-processor definitions
target_compile_definitions( tier1 PRIVATE
	${SHARED_LIB_COMPILE_DEFS}
	TIER1_STATIC_LIB
	)

# Linker definitions
set_target_properties( tier1 PROPERTIES
	COMPILE_FLAGS "${SHARED_LIB_LINKER_DEFS}"
	)

# Include directories
target_include_directories( tier1 PRIVATE ${SHARED_INCLUDE_DIRS} )

# Windows specific library
if( WIN32 )
	target_link_libraries( tier1 rpcrt4.lib )
endif()

# No prefixes
set_target_properties( tier1 PROPERTIES PREFIX "" )
