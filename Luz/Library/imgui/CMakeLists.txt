cmake_minimum_required(VERSION 3.15.2)

project(imgui LANGUAGES CXX)

set(IMGUI_DIRECTORY ${LIBRARY_DIRECTORY}/imgui)

add_library(${PROJECT_NAME} SHARED ${IMGUI_DIRECTORY}/imconfig.h
								   ${IMGUI_DIRECTORY}/imgui.h
								   ${IMGUI_DIRECTORY}/imgui_internal.h
								   ${IMGUI_DIRECTORY}/imstb_rectpack.h
								   ${IMGUI_DIRECTORY}/imstb_textedit.h
								   ${IMGUI_DIRECTORY}/imstb_truetype.h
								   ${IMGUI_DIRECTORY}/imgui.cpp
								   ${IMGUI_DIRECTORY}/imgui_demo.cpp
								   ${IMGUI_DIRECTORY}/imgui_widgets.cpp
								   ${IMGUI_DIRECTORY}/imgui_draw.cpp
								   ${IMGUI_DIRECTORY}/examples/imgui_impl_win32.h
								   ${IMGUI_DIRECTORY}/examples/imgui_impl_win32.cpp
								   ${IMGUI_DIRECTORY}/examples/imgui_impl_dx12.h
								   ${IMGUI_DIRECTORY}/examples/imgui_impl_dx12.cpp)
						 
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_DIRECTORY})
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_DIRECTORY}/examples)		
						
target_link_libraries(${PROJECT_NAME} PRIVATE "d3d12.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "dxgi.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "d3dcompiler.lib")
