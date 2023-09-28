// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: BulletPhysicsDemo.cpp - implementation of layer to demonstrate the power of bullet3 engine :)
// author: Karl-Mihkel Ott

#include <string>
#include <iostream>
#include <trs/Iterators.h>
#include <trs/Vector.h>
#include <trs/Matrix.h>
#include <trs/Quaternion.h>

#include <deng/MathConstants.h>
#include <deng/Layers/BulletPhysicsDemo.h>
#include <deng/Layers/CubeVertices.h>
#include <deng/RenderResources.h>
#include <deng/FileSystemShader.h>
#include <deng/ResourceIdTable.h>
#include <deng/Layers/CameraScript.h>


static const float g_arrPlaneVertices[] = {
	// positions			// normals
	-25.f, 0.f, -25.f,	0.f, 1.f, 0.f,
	25.f, 0.f, -25.f,	0.f, 1.f, 0.f,
	25.f, 0.f, 25.f,	0.f, 1.f, 0.f,

	-25.f, 0.f, -25.f,	0.f, 1.f, 0.f,
	25.f, 0.f, 25.f,	0.f, 1.f, 0.f,
	-25.f, 0.f, 25.f,	0.f, 1.f, 0.f
};

static const float g_arrCubeVertices[] = {
	// positions			// normals
	-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,
};

namespace DENG {

	dDECLARE_RESOURCE_ID_TABLE(BulletTable)
		dRESOURCE_ID_ENTRY("BulletShader"),
		dRESOURCE_ID_ENTRY("PlaneMesh"),
		dRESOURCE_ID_ENTRY("DirectionalLight"),
		dRESOURCE_ID_ENTRY("CubeMesh"),
		dRESOURCE_ID_ENTRY("PlaneMaterial"),
		dRESOURCE_ID_ENTRY("CubeMaterial")
	dEND_RESOURCE_ID_TABLE(BulletTable)


	class BulletShaderBuilder {
		public:
			BulletShaderBuilder() = default;
			IShader* Get() {
				FileSystemShader* pShader = new FileSystemShader("Bullet", "Bullet");
				pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
				pShader->PushAttributeType(VertexAttributeType::Vec3_Float);

				pShader->PushAttributeStride(2 * sizeof(TRS::Vector3<float>));
				pShader->PushAttributeStride(2 * sizeof(TRS::Vector3<float>));
				
				pShader->SetProperty(ShaderPropertyBit_EnableDepthTesting |
									 ShaderPropertyBit_EnableBlend |
									 ShaderPropertyBit_EnablePushConstants |
									 ShaderPropertyBit_Enable2DTextures);

				pShader->SetPushConstant(0, ShaderStageBit_Vertex | ShaderStageBit_Fragment, nullptr);
				pShader->SetPipelineCullMode(PipelineCullMode::None);
				pShader->SetMaterialSamplerCount(MAX_PBR_SAMPLERS);

				// [DrawDescriptorIndices]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex | ShaderStageBit_Fragment, 0);
				// [TransformComponent]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex, 1);
				// [PointLightComponent]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 2);
				// [DirectionalLightComponent]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 3);
				// [SpotlightComponent]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 4);
				// [Material]
				pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 5);
				

				return pShader;
			}
	};

	class BulletPlaneMeshBuilder {
		private:
			size_t m_uVertexOffset = SIZE_MAX;
			IRenderer* m_pRenderer = nullptr;

		public:
			BulletPlaneMeshBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) 
			{
			}

			MeshCommands Get() {
				if (m_uVertexOffset == SIZE_MAX) {
					m_uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_arrPlaneVertices), BufferDataType::Vertex);
					m_pRenderer->UpdateBuffer(g_arrPlaneVertices, sizeof(g_arrPlaneVertices), m_uVertexOffset);
				}

				MeshCommands meshCommands;
				meshCommands.drawCommands.emplace_back();
				meshCommands.drawCommands.back().uDrawCount = 6;
				meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset);
				meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + sizeof(TRS::Vector3<float>));

				return meshCommands;
			}
	};

	
	class BulletCubeMeshBuilder {
		private:
			size_t m_uVertexOffset = SIZE_MAX;
			IRenderer* m_pRenderer = nullptr;

		public:
			BulletCubeMeshBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}

			MeshCommands Get() {
				if (m_uVertexOffset == SIZE_MAX) {
					m_uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_arrCubeVertices), BufferDataType::Vertex);
					m_pRenderer->UpdateBuffer(g_arrCubeVertices, sizeof(g_arrCubeVertices), m_uVertexOffset);
				}

				MeshCommands meshCommands;
				meshCommands.drawCommands.emplace_back();
				meshCommands.drawCommands.back().uDrawCount = 36;
				meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset);
				meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + 3ull * sizeof(float));

				return meshCommands;
			}
	};



	class BulletPlaneMaterialBuilder {
		public:
			BulletPlaneMaterialBuilder() = default;
			Material<MaterialPBR, MAX_PBR_SAMPLERS> Get() {
				Material<MaterialPBR, MAX_PBR_SAMPLERS> material;
				material.material.fMetallic = 0.4f;
				material.material.vAlbedoFactor = { 0.988f, 0.98f, 0.455f, 1.0f };
				material.material.fRoughness = 0.6f;
				material.material.fAmbientOcclusion = 1.f;

				for (auto it = material.textures.begin(); it != material.textures.end(); it++) {
					*it = SID("__MissingTexture2D__");
				}
				return material;
			}
	};


	class BulletCubeMaterialBuilder {
		public:
			BulletCubeMaterialBuilder() = default;
			Material<MaterialPBR, MAX_PBR_SAMPLERS> Get() {
				Material<MaterialPBR, MAX_PBR_SAMPLERS> material;
				material.material.fMetallic = 0.9f;
				material.material.vAlbedoFactor = { 0.25f, 0.25f, 0.25f };
				material.material.fRoughness = 0.1f;
				material.material.fAmbientOcclusion = 0.95f;

				for (auto it = material.textures.begin(); it != material.textures.end(); it++) {
					*it = SID("__MissingTexture2D__");
				}

				return material;
			}
	};


	BulletFallingCubeLayer::BulletFallingCubeLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_scene(_pRenderer, _pFramebuffer)
	{
		m_pRenderer = _pRenderer;
	}


	BulletFallingCubeLayer::~BulletFallingCubeLayer() {
		// remove the rigidbodies from the dynamics world and delete them
		for (int i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
			btCollisionObject* pObj = m_pDynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* pBody = btRigidBody::upcast(pObj);

			if (pBody && pBody->getMotionState()) {
				delete pBody->getMotionState();
			}

			m_pDynamicsWorld->removeCollisionObject(pObj);
			delete pObj;
		}

		// delete collision shapes
		for (int i = 0; i < m_collisionShapes.size(); i++) {
			btCollisionShape* pShape = m_collisionShapes[i];
			m_collisionShapes[i] = nullptr;
			delete pShape;
		}

		// delete dynamics world
		delete m_pDynamicsWorld;

		// delete solver
		delete m_pSolver;

		// delete broadphase
		delete m_pOverlappingPairCache;

		// delete dispatcher
		delete m_pDispatcher;

		delete m_pCollisionConfiguration;
	}


	void BulletFallingCubeLayer::_CreateRigidBodies() {
		// create a static rigid body
		{
			btCollisionShape* pGroundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
			m_collisionShapes.push_back(pGroundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();

			btDefaultMotionState* pMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, pMotionState, pGroundShape);
			btRigidBody* body = new btRigidBody(rbInfo);
			body->setUserIndex(static_cast<int>(m_entPlane));

			// add the body to the dynamics world
			m_pDynamicsWorld->addRigidBody(body);
		}

		// create a dynamic rigid body
		{
			btCollisionShape* pShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
			m_collisionShapes.push_back(pShape);

			btTransform startTransform;
			startTransform.setIdentity();
			startTransform.setOrigin(btVector3(0, 20, 0));

			btScalar mass = 1.f;
			btVector3 localInertia(0, 0.f, 0);
			pShape->calculateLocalInertia(mass, localInertia);

			btDefaultMotionState* pMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, pMotionState, pShape, localInertia);
			btRigidBody* pBody = new btRigidBody(rbInfo);
			pBody->setUserIndex(static_cast<int>(m_entCube));

			m_pDynamicsWorld->addRigidBody(pBody);
		}
	}

	void BulletFallingCubeLayer::Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) {
		m_pRenderer = _pRenderer;
		m_pWindowContext = _pWindowContext;

		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<BulletFallingCubeLayer, WindowResizedEvent>(&BulletFallingCubeLayer::OnWindowResizedEvent, this);

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMesh<BulletPlaneMeshBuilder>(dRO_SID("PlaneMesh", BulletTable), m_pRenderer);
		resourceManager.AddMaterialPBR<BulletPlaneMaterialBuilder>(dRO_SID("PlaneMaterial", BulletTable));
		resourceManager.AddShader<BulletShaderBuilder>(dRO_SID("BulletShader", BulletTable));

		resourceManager.AddMesh<BulletCubeMeshBuilder>(dRO_SID("CubeMesh", BulletTable), m_pRenderer);
		resourceManager.AddMaterialPBR<BulletCubeMaterialBuilder>(dRO_SID("CubeMaterial", BulletTable));

		// create camera entity
		m_entCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(m_entCamera);
		m_scene.EmplaceComponent<ScriptComponent>(m_entCamera)
			.BindScript<CameraScript>(m_entCamera, m_scene)
			.InitializePosition({ 0.f, 1.f, -1.5f });
		m_scene.SetMainCamera(m_entCamera);

		// create directional light entity
		m_entDirectionalLight = m_scene.CreateEntity();
		m_scene.EmplaceComponent<DirectionalLightComponent>(m_entDirectionalLight);
		auto& dirLight = m_scene.GetComponent<DirectionalLightComponent>(m_entDirectionalLight);
		dirLight.vColor = { 1.f, 1.f, 1.f, 1.f };

		// rotation in radians
		const float fRotation = MF_PI_4;
		dirLight.vDirection = TRS::Quaternion(std::sinf(fRotation / 2.f), 0.f, 0.f, std::cosf(fRotation / 2.f)) * TRS::Vector4<float>(0.f, 1.f, 0.f, 0.f);

		// create cube entity
		m_entCube = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_entCube, dRO_SID("CubeMesh", BulletTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_entCube, dRO_SID("BulletShader", BulletTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_entCube, dRO_SID("CubeMaterial", BulletTable));
		m_scene.EmplaceComponent<TransformComponent>(m_entCube)
			.vTranslation = { 0.f, 20.f, 0.f };

		// create plane entity
		m_entPlane = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_entPlane, dRO_SID("PlaneMesh", BulletTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_entPlane , dRO_SID("BulletShader", BulletTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_entPlane, dRO_SID("PlaneMaterial", BulletTable));
		m_scene.EmplaceComponent<TransformComponent>(m_entPlane);

		m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
		m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
		m_pOverlappingPairCache = new btDbvtBroadphase();
		m_pSolver = new btSequentialImpulseConstraintSolver;

		m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);
		m_pDynamicsWorld->setGravity(btVector3(0.f, -9.81f, 0.f));

		// create rigid bodies
		_CreateRigidBodies();

		eventManager.Dispatch<ComponentAddedEvent>(m_entCube, ComponentType_Mesh | ComponentType_Shader | ComponentType_Transform | ComponentType_Material);
		eventManager.Dispatch<ComponentAddedEvent>(m_entPlane, ComponentType_Mesh | ComponentType_Shader | ComponentType_Transform | ComponentType_Material);

		m_scene.AttachComponents();
	}

	void BulletFallingCubeLayer::Update(IFramebuffer* _pFramebuffer) {

		m_endTimePoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> frametime = m_endTimePoint - m_beginTimePoint;
		float fDeltaTime = frametime.count() / 1000.f;
		if (fDeltaTime < 0.f)
			fDeltaTime = 0.f;
		m_beginTimePoint = std::chrono::high_resolution_clock::now();

		m_pDynamicsWorld->stepSimulation(fDeltaTime, 10000);
		
		// iterate through objects
		for (int i = m_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
			btCollisionObject* pObj = m_pDynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* pBody = btRigidBody::upcast(pObj);
			btTransform trans;
			if (pBody && pBody->getMotionState()) {
				pBody->getMotionState()->getWorldTransform(trans);
			}
			else continue;

			trans = pObj->getWorldTransform();
			Entity ent = static_cast<Entity>(pObj->getUserIndex());
			auto& entTransform = m_scene.GetComponent<TransformComponent>(ent);
			
			btVector3& origin = trans.getOrigin();
			btQuaternion& rotation = trans.getRotation();

			entTransform.vTranslation = { origin.getX(), origin.getY(), origin.getZ(), 1.f };
			TRS::Quaternion qRot = { rotation.getX(), rotation.getY(), rotation.getZ(), rotation.getZ() };

			entTransform.mCustomTransform = qRot.ExpandToMatrix4();

			EventManager& eventManager = EventManager::GetInstance();
			eventManager.Dispatch<ComponentModifiedEvent>(ent, ComponentType_Transform);
		}

		m_scene.RenderScene();
	}


	bool BulletFallingCubeLayer::OnWindowResizedEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return true;
	}
}