// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Components.h - header for all possible component classes
// author: Karl-Mihkel Ott

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <array>
#include <string>
#include <variant>
#include <vector>
#include <entt/entt.hpp>

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"
#include "trs/Quaternion.h"

#include "deng/Api.h"
#include "deng/SID.h"
#include "deng/MathConstants.h"
#include "deng/Event.h"
#include "deng/ErrorDefinitions.h"

namespace DENG {
	class Scene;
	typedef entt::entity Entity;

	enum ComponentType_T : uint16_t {
		ComponentType_None = 0,
		ComponentType_Transform = (1 << 0),
		ComponentType_Mesh = (1 << 1),
		ComponentType_Shader = (1 << 2),
		ComponentType_Material = (1 << 3),
		ComponentType_Renderable = 14,
		ComponentType_Script = (1 << 4),
		ComponentType_PointLight = (1 << 5),
		ComponentType_DirectionalLight = (1 << 6),
		ComponentType_SpotLight = (1 << 7),
		ComponentType_Light = 224,
		ComponentType_Camera = (1 << 8),
		ComponentType_Skybox = (1 << 9)
	};

	typedef uint16_t ComponentType;

	struct DrawDescriptorIndices {
		int32_t iTransformIndex = -1;
		int32_t iMaterialIndex = -1;
		int32_t iPadding0 = 0;
		int32_t iPadding1 = 0;

		DrawDescriptorIndices() = default;
		DrawDescriptorIndices(int32_t _iTransformIndex, int32_t _iMaterialIndex) :
			iTransformIndex(_iTransformIndex),
			iMaterialIndex(_iMaterialIndex) {}
	};

	struct InstanceInfo {
		hash_t hshMesh = 0;
		hash_t hshShader = 0;
		hash_t hshMaterial = 0;
		uint32_t uInstanceCount = 1;
	};

	template<typename T>
	class Ref {
		private:
			Entity m_owner;
			Scene& m_scene;

		public:
			Ref(Entity _owner, Scene& _scene) noexcept :
				m_owner(_owner),
				m_scene(_scene) {}
			Ref(Ref<T>&& _other) noexcept :
				m_owner(_other.m_owner),
				m_scene(_other.m_scene) {}
			~Ref() = default;

			inline T& Get() { return m_scene.GetComponent<T>(m_owner); }
			inline const T& Get() const { return m_scene.GetComponent<T>(m_owner); }
			inline Entity GetOwner() const { return m_owner; }

			// implicit cast
			inline operator T& () { return Get(); }
			inline operator const T& () const { return Get(); }
	};


	struct SkyboxComponent {
		SkyboxComponent() = default;
		SkyboxComponent(const SkyboxComponent&) = default;
		SkyboxComponent(const TRS::Vector4<float>& _vScale, hash_t _hshMesh, hash_t _hshShader) :
			vScale(_vScale),
			hshMesh(_hshMesh),
			hshShader(_hshShader) {}

		TRS::Vector4<float> vScale;
		hash_t hshMesh;
		hash_t hshShader;
	};


	struct TransformComponent {
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TRS::Matrix4<float> mCustomTransform;
		TRS::Matrix4<float> mNormal;
		TRS::Vector4<float> vTranslation = { 0.f, 0.f, 0.f, 1.f };
		TRS::Vector4<float> vScale = { 1.f, 1.f, 1.f, 0.f };
		TRS::Vector4<float> vRotation = { 0.f, 0.f, 0.f, 0.f }; // in radians

		void CalculateNormalMatrix() {
			TRS::Quaternion qX = { std::sinf(vRotation.first / 2.f), 0.f, 0.f, std::cosf(vRotation.first / 2.f) };
			TRS::Quaternion qY = { 0.f, std::sinf(vRotation.second / 2.f), 0.f, std::cosf(vRotation.second / 2.f) };
			TRS::Quaternion qZ = { 0.f, 0.f, std::sinf(vRotation.third / 2.f), std::cosf(vRotation.third / 2.f) };
			mNormal = (qX * qY * qZ).ExpandToMatrix4().Inverse();
		}
	};


	struct MeshComponent {
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(hash_t _hshMesh) :
			hshMesh(_hshMesh) {}

		hash_t hshMesh = 0;

		static constexpr ComponentType GetComponentType() {
			return ComponentType_Mesh;
		}

		inline bool operator==(const MeshComponent& _mesh) const {
			return hshMesh == _mesh.hshMesh;
		}
	};


	struct ShaderComponent {
		ShaderComponent() = default;
		ShaderComponent(const ShaderComponent&) = default;
		ShaderComponent(hash_t _hshShader) :
			hshShader(_hshShader) {}

		hash_t hshShader = 0;

		static constexpr ComponentType GetComponentType() {
			return ComponentType_Shader;
		}

		inline bool operator==(const ShaderComponent& _shader) const {
			return hshShader == _shader.hshShader;
		}
	};


	struct MaterialComponent {
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(hash_t _hshMaterial) :
			hshMaterial(_hshMaterial) {}

		hash_t hshMaterial = 0;

		static constexpr ComponentType GetComponentType() {
			return ComponentType_Material;
		}

		inline bool operator==(const MaterialComponent& _material) const {
			return hshMaterial == _material.hshMaterial;
		}
	};


	struct PointLightComponent {
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;

		using Vec4 = TRS::Vector4<float>;
		using Vec3 = TRS::Vector3<float>;
		PointLightComponent(const Vec4& _vPosition, const Vec4& _vColor) :
			vPosition(_vPosition),
			vColor(_vColor) {}
		
		Vec4 vPosition = { 0.f, 0.f, 0.f, 1.f };
		Vec4 vColor = { 1.f, 1.f, 1.f, 1.f };


		static constexpr ComponentType GetComponentType() {
			return ComponentType_PointLight;
		}
	};


	struct DirectionalLightComponent {
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		
		using Vec4 = TRS::Vector4<float>;
		DirectionalLightComponent(const Vec4& _vDirection, const Vec4& _vColor) :
			vDirection(_vDirection),
			vColor(_vColor) {}

		Vec4 vDirection = { 1.f, 0.f, 0.f, 0.f };
		Vec4 vColor;

		static constexpr ComponentType GetComponentType() {
			return ComponentType_DirectionalLight;
		}
	};


	struct SpotlightComponent {
		SpotlightComponent() = default;
		SpotlightComponent(const SpotlightComponent&) = default;

		using Vec4 = TRS::Vector4<float>;
		SpotlightComponent(const Vec4& _vPosition, const Vec4& _vDirection, const Vec4& _vColor, float _fInnerCutoff, float _fOuterCutoff) :
			vPosition(_vPosition),
			vDirection(_vDirection),
			vColor(_vColor),
			fInnerCutoff(_fInnerCutoff),
			fOuterCutoff(_fOuterCutoff) {}

		Vec4 vPosition;
		Vec4 vDirection = { 1.f, 0.f, 0.f, 0.f };
		Vec4 vColor = { 1.f, 0.f, 0.f, 1.f };
		float fInnerCutoff = MF_SQRT3 / 2.f; // 30 degrees
		float fOuterCutoff = 0.819152f;		 // 35 degrees
		float _pad[2] = {};

		static constexpr ComponentType GetComponentType() {
			return ComponentType_SpotLight;
		}
	};


#define SCRIPT_DEFINE_CONSTRUCTOR(script) script::script(DENG::Entity _idEntity, DENG::Scene& _scene) :\
											 ScriptBehaviour(_idEntity, _scene, #script) {}
	class DENG_API ScriptBehaviour {
		protected:
			const Entity m_idEntity = entt::null;
			Scene& m_scene;
			const std::string m_sClassName;

		public:
			ScriptBehaviour(Entity _idEntity, Scene& _scene, const std::string& _sClassName = "MyClass") :
				m_idEntity(_idEntity),
				m_scene(_scene),
				m_sClassName(_sClassName) {}
	};

	class ScriptComponent;
	typedef void(*PFN_OnAttach)(ScriptComponent& _scriptComponent);
	typedef void(*PFN_OnUpdate)(ScriptComponent& _scriptComponent, float _fTimestamp);
	typedef void(*PFN_OnDestroy)(ScriptComponent& _scriptComponent);

	class ScriptComponent {
		private:
			ScriptBehaviour* m_pScriptBehaviour = nullptr;

			template<typename T>
			struct _ScriptBehaviourTest {
				template <typename U> static char TestOnAttach(decltype(&U::OnAttach));
				template <typename U> static short TestOnAttach(...);

				template <typename U> static char TestOnUpdate(decltype(&U::OnUpdate));
				template <typename U> static short TestOnUpdate(...);

				template <typename U> static char TestOnDestroy(decltype(&U::OnDestroy));
				template <typename U> static short TestOnDestroy(...);

				enum {
					HAS_ON_ATTACH = (sizeof(TestOnAttach<T>(0)) == sizeof(char) ? 1 : -1),
					HAS_ON_UPDATE = (sizeof(TestOnUpdate<T>(0)) == sizeof(char) ? 2 : -2),
					HAS_ON_DESTROY = (sizeof(TestOnDestroy<T>(0)) == sizeof(char) ? 3 : -3)
				};
			};

		public:
			PFN_OnAttach OnAttach = nullptr;
			PFN_OnUpdate OnUpdate = nullptr;
			PFN_OnDestroy OnDestroy = nullptr;

			static constexpr ComponentType GetComponentType() {
				return ComponentType_Script;
			}

			template<typename T, typename... Args>
			inline T& BindScript(Entity _idEntity, Scene& _scene, Args... args) {
				m_pScriptBehaviour = new T(_idEntity, _scene, std::forward<Args>(args)...);

				if constexpr (_ScriptBehaviourTest<T>::HAS_ON_ATTACH > 0) {
					OnAttach = [](ScriptComponent& _scriptComponent) {
						_scriptComponent.GetScriptBehaviour<T>()->OnAttach();
					};
				}

				if constexpr (_ScriptBehaviourTest<T>::HAS_ON_UPDATE > 0) {
					OnUpdate = [](ScriptComponent& _scriptComponent, float _fTimestep) {
						_scriptComponent.GetScriptBehaviour<T>()->OnUpdate(_fTimestep);
					};
				}
				
				if constexpr (_ScriptBehaviourTest<T>::HAS_ON_DESTROY > 0) {
					OnDestroy = [](ScriptComponent& _scriptComponent) {
						_scriptComponent.GetScriptBehaviour<T>()->OnDestroy();
					};
				}

				return *static_cast<T*>(m_pScriptBehaviour);
			}


			template<typename T>
			T* GetScriptBehaviour() {
				return static_cast<T*>(m_pScriptBehaviour);
			}
	};


	struct CameraComponent {
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		TRS::Matrix4<float> mProjection;
		TRS::Vector4<float> vCameraRight;
		TRS::Vector4<float> vCameraUp;
		TRS::Vector4<float> vCameraDirection;
		TRS::Vector4<float> vPosition;

		static constexpr ComponentType GetComponentType() {
			return ComponentType_Camera;
		}
	};


	// for das models
	struct ModelComponent {
		ModelComponent() = default;
		ModelComponent(const ModelComponent&) = default;
		ModelComponent(ModelComponent&& _model) noexcept :
			sFilename(std::move(_model.sFilename)),
			uVertexBufferOffset(_model.uVertexBufferOffset),
			uVertexBufferChunkSize(_model.uVertexBufferChunkSize),
			uUniformBufferOffset(_model.uUniformBufferOffset),
			uUniformBufferChunkSize(_model.uUniformBufferChunkSize) {}
		
		std::string sFilename;
		uint32_t uVertexBufferOffset = 0;
		uint32_t uVertexBufferChunkSize = 0;
		uint32_t uUniformBufferOffset = 0;
		uint32_t uUniformBufferChunkSize = 0;
	};

	
	struct SkeletalJointComponent {
		SkeletalJointComponent() = default;
		SkeletalJointComponent(const SkeletalJointComponent&) = default;
		SkeletalJointComponent(SkeletalJointComponent&& _skeletalJoint) noexcept :
			mInverseBindPosition(_skeletalJoint.mInverseBindPosition),
			sName(std::move(_skeletalJoint.sName)),
			mCustomTransformation(_skeletalJoint.mCustomTransformation),
			translation(_skeletalJoint.translation),
			rotation(_skeletalJoint.rotation),
			scale(_skeletalJoint.scale) {}

		TRS::Matrix4<float> mInverseBindPosition;
		std::string sName = "SkeletalJointComponent";
		TRS::Matrix4<float> mCustomTransformation;
		TRS::Point3D<float> translation;
		TRS::Point3D<float> rotation;
		TRS::Point3D<float> scale = { 1.f, 1.f, 1.f };
	};


	struct AnimationComponent {
		AnimationComponent() = default;
		AnimationComponent(const AnimationComponent&) = default;
		AnimationComponent(AnimationComponent&& _animation) noexcept :
			sName(_animation.sName),
			fDuration(_animation.fDuration),
			fCurrentTimestamp(_animation.fCurrentTimestamp),
			bIsActivated(_animation.bIsActivated),
			bIsRepeat(_animation.bIsRepeat) {}

		std::string sName = "AnimationComponent";
		float fDuration = 0.f;
		float fCurrentTimestamp = 0.f;
		bool bIsActivated = false;
		bool bIsRepeat = false;
	};

	enum AnimationTarget { WEIGHTS, TRANSLATION, ROTATION, SCALE };
	enum AnimationInterpolation { LINEAR, STEP, CUBICSPLINE };

	struct AnimationSamplerComponent {
		uint32_t animatedEntity = 0;
		AnimationTarget target;
		AnimationInterpolation interpolation;
		std::vector<float> keyframes;
		std::vector<std::variant<std::array<float, 2>, std::array<TRS::Vector3<float>, 2>, std::array<TRS::Vector4<float>, 2>>> tangents;
		std::vector<std::variant<float, TRS::Vector3<float>, TRS::Vector4<float>>> targetValues;
	};
}

#endif
