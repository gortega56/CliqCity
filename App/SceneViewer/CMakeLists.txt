cmake_minimum_required(VERSION 3.15.2)

project(SceneViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SCENEVIEWER_DIRECTORY ${APPLICATION_DIRECTORY}/SceneViewer/src)

add_compile_definitions(SCENEVIEWER_SHADER_DIRECTORY="${SCENEVIEWER_DIRECTORY}/Shaders/")

set(SCENEVIEWER_H ${SCENEVIEWER_DIRECTORY}/SceneViewer.h
				  ${SCENEVIEWER_DIRECTORY}/SceneResource.h)

set(SCENEVIEWER_CPP ${SCENEVIEWER_DIRECTORY}/SceneViewer.cpp
					${SCENEVIEWER_DIRECTORY}/main.cpp)

set(SCENEVIEWER_HLSL ${SCENEVIEWER_DIRECTORY}/Shaders/brdf_integration_ps.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/cube_map_ps.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/cube_map_vs.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/FS_Tri_PS.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/FS_Tri_VS.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/irradiance_map_ps.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/radiance_map_ps.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/scene_model_ps.hlsl
					 ${SCENEVIEWER_DIRECTORY}/Shaders/scene_model_vs.hlsl)

set(SCENEVIEWER_VS ${SCENEVIEWER_DIRECTORY}/Shaders/cube_map_vs.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/FS_Tri_VS.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/scene_model_vs.hlsl)

set(SCENEVIEWER_PS ${SCENEVIEWER_DIRECTORY}/Shaders/brdf_integration_ps.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/cube_map_ps.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/FS_Tri_PS.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/irradiance_map_ps.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/radiance_map_ps.hlsl
				   ${SCENEVIEWER_DIRECTORY}/Shaders/scene_model_ps.hlsl)

set_source_files_properties(${SCENEVIEWER_VS} PROPERTIES VS_SHADER_TYPE Vertex
														 VS_SHADER_MODEL 5.1
														 VS_SHADER_ENTRYPOINT main
														 VS_SHADER_ENABLE_DEBUG $<$<CONFIG:DEBUG>:TRUE>
														 VS_SHADER_DISABLE_OPTIMIZATIONS $<$<CONFIG:DEBUG>:TRUE>)
														 
set_source_files_properties(${SCENEVIEWER_PS} PROPERTIES VS_SHADER_TYPE Pixel
														 VS_SHADER_MODEL 5.1
														 VS_SHADER_ENTRYPOINT main
														 VS_SHADER_ENABLE_DEBUG $<$<CONFIG:DEBUG>:TRUE>
														 VS_SHADER_DISABLE_OPTIMIZATIONS $<$<CONFIG:DEBUG>:TRUE>
														 VS_SHADER_FLAGS "/enable_unbounded_descriptor_tables /I \"${LUZ_SHADERS}\"")

add_executable(SceneViewer 	${SCENEVIEWER_H} ${SCENEVIEWER_CPP} ${SCENEVIEWER_HLSL})

source_group("Shaders" FILES ${SCENEVIEWER_HLSL})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
		
target_include_directories(${PROJECT_NAME} INTERFACE ${LIBRARY_DIRECTORY}/lina/src)

target_link_directories(SceneViewer PUBLIC ${ENGINE_DIRECTORY})
target_link_directories(SceneViewer PUBLIC ${LIBRARY_DIRECTORY}/imgui)

target_link_libraries(SceneViewer PUBLIC Luz)
target_link_libraries(SceneViewer PUBLIC imgui)
target_link_libraries(SceneViewer INTERFACE lina)

add_dependencies(SceneViewer imgui)
add_dependencies(SceneViewer Luz)
add_dependencies(SceneViewer lina)

#target_link_directories(SceneViewer PUBLIC ${LIBRARY_DIRECTORY}/lina)
#target_link_libraries(SceneViewer PUBLIC lina)
