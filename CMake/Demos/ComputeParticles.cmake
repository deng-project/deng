# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ComputeParticles.cmake - Compute shader calculated particle demo
# author: Karl-Mihkel Ott

set(COMPUTE_PARTICLE_TARGET ComputeParticles)
set(COMPUTE_PARTICLE_HEADERS 
	Demos/ComputeParticles/ComputeParticlesApp.h
	Demos/ComputeParticles/ComputeParticlesLayer.h
	Demos/Utils/CameraScript.h)
set(COMPUTE_PARTICLE_SOURCES
	Demos/ComputeParticles/ComputeParticlesApp.cpp
	Demos/ComputeParticles/ComputeParticlesLayer.cpp
	Demos/Utils/CameraScript.cpp)

add_executable(${COMPUTE_PARTICLE_TARGET}
	${COMPUTE_PARTICLE_HEADERS}
	${COMPUTE_PARTICLE_SOURCES})

add_dependencies(${COMPUTE_PARTICLE_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${COMPUTE_PARTICLE_TARGET}
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${COMPUTE_PARTICLE_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})