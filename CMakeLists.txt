cmake_minimum_required(VERSION 3.20)

project(
	CaptureWarmer
	VERSION 1.2.0
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(XSEPlugin)

target_link_libraries(
	${PROJECT_NAME} 
	PRIVATE 
		d3dcompiler.lib
)