#include <iostream>
#include <string>

#include "deng/Api.h"
#include "deng/App.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"

#define WIDTH	1280
#define HEIGHT	720

class TriangleLayer : public DENG::ILayer {
	private:
		const float m_cTriangleVertices[21] = {
			-0.5, -0.5, 0.f,
			1.0f, 0.f, 0.f, 1.f,
			0.f, 0.5f, 0.f,
			0.f, 1.f, 0.f, 1.f,
			0.5f, -0.5f, 0.f,
			0.f, 0.f, 1.f, 1.f
		};
		DENG::MeshComponent m_meshComponent;
		DENG::ShaderComponent m_shaderComponent;

	public:
		virtual void Attach(DENG::IRenderer* _pRenderer, DENG::IWindowContext* _pWindowContext) override {
			m_shaderComponent.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			m_shaderComponent.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC4_FLOAT);
			m_shaderComponent.attributeStrides.push_back(sizeof(TRS::Vector3<float>) + sizeof(TRS::Vector4<float>));
			m_shaderComponent.attributeStrides.push_back(sizeof(TRS::Vector3<float>) + sizeof(TRS::Vector4<float>));
			m_shaderComponent.bEnableIndexing = false;
			m_shaderComponent.pShader = new DENG::Shader("Triangle", "Triangle");

			size_t uOffset = _pRenderer->AllocateMemory(sizeof(m_cTriangleVertices), DENG::BufferDataType::VERTEX);
			
			try {
				_pRenderer->UpdateBuffer(m_cTriangleVertices, sizeof(m_cTriangleVertices), uOffset);
			}
			catch (const DENG::RendererException& e) {
				DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
			}
			catch (const DENG::ShaderException& e) {
				DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::CRITICAL);
			}

			m_meshComponent.drawCommands.emplace_back();
			m_meshComponent.drawCommands.back().attributeOffsets.push_back(uOffset);
			m_meshComponent.drawCommands.back().attributeOffsets.push_back(uOffset + sizeof(TRS::Vector3<float>));
			m_meshComponent.drawCommands.back().uDrawCount = 3;
		
			m_pRenderer = _pRenderer;
			m_pWindowContext = _pWindowContext;
		}

		virtual void Update(DENG::IFramebuffer* _pFramebuffer) override {
			m_pRenderer->DrawMesh(m_meshComponent, m_shaderComponent, _pFramebuffer);
		}
};

class SDLTriangleApp : public DENG::App {
	public:
		SDLTriangleApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext);
			DENG::IRenderer* pVulkanRenderer = SetRenderer(new DENG::VulkanRenderer);
		
			pWindowContext->SetHints(DENG::WindowHints::SHOWN | DENG::WindowHints::VULKAN);
			try {
				pWindowContext->Create("SDLTriangle example", WIDTH, HEIGHT);
			}
			catch (const DENG::WindowContextException& e) {
				DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL);
			}

			DENG::IFramebuffer* pFramebuffer = nullptr;
			try {
				pFramebuffer = SetMainFramebuffer(pVulkanRenderer->CreateContext(pWindowContext));
			}
			catch (const DENG::HardwareException& e) {
				DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
			}
			catch (const DENG::RendererException& e) {
				DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
			}

			PushLayer<TriangleLayer>();
			AttachLayers();
		}

		~SDLTriangleApp() {
			std::cout << "Explosion..." << std::endl;
		}
};

DENG_MAIN_DECLARATION(SDLTriangleApp);