#include <iostream>
#include <string>
#include <thread>

#include "deng/Api.h"
#include "deng/SDLWindowContext.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/VulkanRenderer.h"

using namespace DENG;

#define WIDTH	1280
#define HEIGHT	720

static float s_TriangleVertices[] = {
	-0.5, -0.5, 0.f,
	1.0f, 0.f, 0.f, 1.f,
	0.f, 0.5f, 0.f,
	0.f, 1.f, 0.f, 1.f,
	0.5f, -0.5f, 0.f,
	0.f, 0.f, 1.f, 1.f
};

int main(void) {
	std::cin.tie(0);
	std::ios_base::sync_with_stdio(false);

	SDLWindowContext windowContext;
	windowContext.SetHints(WindowHints::SHOWN | WindowHints::VULKAN);
	
	try {
		windowContext.Create("SDLTriangle example", WIDTH, HEIGHT);
	}
	catch (const WindowContextException& e) {
		DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL);
	}

	IRenderer* pRenderer = new VulkanRenderer;
	IFramebuffer* pFramebuffer = nullptr;

	try {
		pFramebuffer = pRenderer->CreateContext(&windowContext);
	}
	catch (const HardwareException& e) {
		DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
	}
	catch (const RendererException& e) {
		DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
	}

	PipelineModule trianglePipelineModule;
	trianglePipelineModule.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
	trianglePipelineModule.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
	trianglePipelineModule.attributeStrides.push_back(sizeof(TRS::Vector3<float>) + sizeof(TRS::Vector4<float>));
	trianglePipelineModule.attributeStrides.push_back(sizeof(TRS::Vector3<float>) + sizeof(TRS::Vector4<float>));
	trianglePipelineModule.bEnableIndexing = false;
	trianglePipelineModule.pShader = new Shader("Triangle", "Triangle");

	size_t uOffset = pRenderer->AllocateMemory(sizeof(s_TriangleVertices), BufferDataType::VERTEX);
	std::list<PipelineModule>::iterator refPipeline;

	try {
		pRenderer->UpdateBuffer(s_TriangleVertices, sizeof(s_TriangleVertices), uOffset);
		refPipeline = pRenderer->CreatePipeline(trianglePipelineModule);
	}
	catch (const RendererException& e) {
		DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
	}
	catch (const ShaderException& e) {
		DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::CRITICAL);
	}

	MeshComponent mesh;
	mesh.itShaderModule = refPipeline;
	mesh.drawCommands.emplace_back();
	mesh.drawCommands.back().attributeOffsets.push_back(uOffset);
	mesh.drawCommands.back().attributeOffsets.push_back(uOffset + sizeof(TRS::Vector3<float>));
	mesh.drawCommands.back().uDrawCount = 3;

	while (windowContext.IsAlive()) {
		try {
			pFramebuffer->BeginCommandBufferRecording({ 0.f, 0.f, 0.f, 1.f });
			pRenderer->DrawMesh(mesh, 1, pFramebuffer);
			pFramebuffer->EndCommandBufferRecording();
			pFramebuffer->RenderToFramebuffer();
		}
		catch (const RendererException& e) {
			DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
		}
		catch (const ShaderException& e) {
			DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::CRITICAL);
		}

		windowContext.Update();

		auto& events = windowContext.GetEventQueue();
		while (!events.empty()) {
			Event event = events.front();
			events.pop();

			switch (event.eType) {
				case EventType::WINDOW:
					if (event.uDescription == (uint32_t)WindowEvent::RESIZED)
						std::cout << "Window resized " << event.x << 'x' << event.y << '\n';
					break;

				case EventType::KEY_DOWN:
				case EventType::MOUSE_BUTTON_DOWN:
					std::cout << "Pressed key: " << event.uDescription << '\n';
					break;

				case EventType::KEY_UP:
				case EventType::MOUSE_BUTTON_UP:
					std::cout << "Released key: " << event.uDescription << '\n';
					break;

				default:
					break;
			}
		}
	}

	delete pRenderer;
	delete trianglePipelineModule.pShader;
	return 0;
}