cmake_minimum_required(VERSION 3.15.2)

project(Luz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LUZ_DIRECTORY ${ENGINE_DIRECTORY}/src)
set(LUZ_SHADERS ${LUZ_DIRECTORY}/Graphics/DirectX12/Shaders PARENT_SCOPE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(FBX REQUIRED)
add_definitions(-DFBX_VERSION_MAJOR=${FBX_VERSION_MAJOR})

add_compile_definitions(LUZ_SHADER_DIRECTORY="${LUZ_DIRECTORY}/Graphics/DirectX12/Shaders/")

file(GLOB COMMON "${LUZ_DIRECTORY}/*.cpp"
				 "${LUZ_DIRECTORY}/*.h")

file(GLOB GRAPHICS 	"${LUZ_DIRECTORY}/Graphics/*.cpp"
					"${LUZ_DIRECTORY}/Graphics/*.h")

file(GLOB GRAPHICS_D3D12 "${LUZ_DIRECTORY}/Graphics/DirectX12/*.cpp"
						 "${LUZ_DIRECTORY}/Graphics/DirectX12/*.h")

file(GLOB PLATFORM 	"${LUZ_DIRECTORY}/Platform/*.cpp"
					"${LUZ_DIRECTORY}/Platform/*.h")
		
file(GLOB PLATFORM_WIN64 "${LUZ_DIRECTORY}/Platform/Win64/*.cpp"
						 "${LUZ_DIRECTORY}/Platform/Win64/*.h")
		
file(GLOB RESOURCE 	"${LUZ_DIRECTORY}/Resource/*.cpp"
					"${LUZ_DIRECTORY}/Resource/*.h")

file(GLOB SHADERS "${LUZ_DIRECTORY}/Graphics/DirectX12/Shaders/*.hlsli")

add_library(${PROJECT_NAME} SHARED ${COMMON}
								   ${GRAPHICS}
								   ${GRAPHICS_D3D12}
								   ${PLATFORM}
								   ${PLATFORM_WIN64}
								   ${RESOURCE}
								   ${SHADERS})
			
target_include_directories(${PROJECT_NAME} PUBLIC ${LUZ_DIRECTORY}
									  ${LUZ_DIRECTORY}/Graphics
									  ${LUZ_DIRECTORY}/Platform
									  ${LUZ_DIRECTORY}/Resource)

target_include_directories(${PROJECT_NAME} PRIVATE ${LUZ_DIRECTORY}
									   ${LUZ_DIRECTORY}/Graphics/DirectX12
									   ${LUZ_DIRECTORY}/Platform/Win64)

source_group("" FILES ${COMMON})
source_group("Graphics" FILES ${GRAPHICS})
source_group("Graphics\\DirectX12" FILES ${GRAPHICS_D3D12})
source_group("Platform" FILES ${PLATFORM})
source_group("Platform\\Win64" FILES ${PLATFORM_WIN64})
source_group("Resource" FILES ${RESOURCE})
source_group("Graphics\\DirectX12\\Shaders" FILES ${Shaders})

# Dependencies
add_subdirectory(${LIBRARY_DIRECTORY}/DirectXTex ${CMAKE_CURRENT_BINARY_DIR}/DirectXTex)
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBRARY_DIRECTORY}/DirectXTex)
target_link_libraries(${PROJECT_NAME} PRIVATE directxtex)
add_dependencies(${PROJECT_NAME} directxtex)

add_subdirectory(${LIBRARY_DIRECTORY}/DirectXTK12 ${CMAKE_CURRENT_BINARY_DIR}/DirectXTK12)
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBRARY_DIRECTORY}/DirectXTK12)
target_link_libraries(${PROJECT_NAME} PRIVATE directxtk12)
add_dependencies(${PROJECT_NAME} directxtk12)

add_subdirectory(${LIBRARY_DIRECTORY}/imgui ${CMAKE_CURRENT_BINARY_DIR}/imgui)
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBRARY_DIRECTORY}/imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
add_dependencies(${PROJECT_NAME} imgui)

add_subdirectory(${LIBRARY_DIRECTORY}/lina ${CMAKE_CURRENT_BINARY_DIR}/lina)
target_link_directories(${PROJECT_NAME} INTERFACE ${LIBRARY_DIRECTORY}/lina)
target_link_libraries(${PROJECT_NAME} INTERFACE lina)
add_dependencies(${PROJECT_NAME} lina)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBRARY_DIRECTORY}/lina/src)

# Libraries
target_include_directories(${PROJECT_NAME} PUBLIC "C://Program Files (x86)//Windows Kits//10//Include//${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}//shared"
												   "C://Program Files (x86)//Windows Kits//10//Include//${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}//um")
target_link_directories(${PROJECT_NAME} PUBLIC "C://Program Files (x86)//Windows Kits//10//Lib//${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}//um//x64")

target_link_libraries(${PROJECT_NAME} PRIVATE "XInput.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "d3d12.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "dxgi.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE "d3dcompiler.lib")

target_include_directories(${PROJECT_NAME} PUBLIC SYSTEM ${FBX_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${FBX_LIBRARIES})

 #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FBX_SDK_DLL} $<TARGET_FILE_DIR:Luz>)
