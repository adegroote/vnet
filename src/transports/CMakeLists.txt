add_library(vnet_transports
			SHARED
            vnet_localtransport.cpp
            vnet_transports.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB INCLUDE_FILES *.h)
install(FILES ${INCLUDE_FILES} DESTINATION include/vnet/transports)
install(TARGETS vnet_transports DESTINATION lib)
