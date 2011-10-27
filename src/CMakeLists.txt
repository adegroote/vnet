set(CMAKE_CXX_FLAGS "-g -Wall -O2")

find_package(Boost 1.43 COMPONENTS thread program_options REQUIRED) # 1.43 because of boost::factory
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(vnet_common
			SHARED
            vnet_client.cpp
            vnet_core.cpp  
            vnet_message.cpp
            vnet_stage.cpp
)

target_link_libraries(vnet_common ${Boost_LIBRARIES})

set(COMMON_LIBRARIES
    vnet_common
    vnet_filters
    vnet_transports
    ${Boost_LIBRARIES}
)

file(GLOB INCLUDE_FILES *.h)
install(FILES ${INCLUDE_FILES} DESTINATION include/vnet)
install(TARGETS vnet_common DESTINATION lib)

add_subdirectory(filters)
add_subdirectory(transports)
add_subdirectory(examples)
