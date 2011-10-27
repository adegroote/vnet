project(vnet)
set(PACKAGE_VERSION 0.0.99)

cmake_minimum_required(VERSION 2.8)

add_subdirectory(src)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/vnet.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/vnet.pc
	@ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vnet.pc
		DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

