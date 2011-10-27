add_library(vnet_filters
			SHARED
            vnet_randomfilter.cpp
            vnet_filters.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB INCLUDE_FILES *.h)
install(FILES ${INCLUDE_FILES} DESTINATION include/vnet/filters)
install(TARGETS vnet_filters DESTINATION lib)
