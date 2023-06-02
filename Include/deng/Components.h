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
#include "deng/MathConstants.h"
#include "deng/Event.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderComponent.h"

namespace DENG {

	typedef entt::entity Entity;

	struct TransformComponent {
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TRS::Matrix4<float> mCustomTransform;
		TRS::Vector4<float> vTranslation = { 0.f, 0.f, 0.f, 1.f };
		TRS::Vector3<float> vScale = { 1.f, 1.f, 1.f };
		float _pad;
		TRS::Vector3<float> vRotation = { 0.f, 0.f, 0.f }; // in radians
		float _pad1;
	};


	struct MaterialComponent {
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;

		TRS::Vector4<float> vAmbient = { 1.f, 1.f, 1.f, 0.f };
		TRS::Vector4<float> vDiffuse;
		TRS::Vector4<float> vSpecular;
		// first: diffuse map id, second: specular map ids, third and fourth: padding
		TRS::Vector4<uint32_t> vMaps;
		float fShininess = 0.3f;
	};


	struct PointLightComponent {
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;

		using Vec4 = TRS::Vector4<float>;
		using Vec3 = TRS::Vector3<float>;
		PointLightComponent(const Vec4& _vPosition, const Vec4& _vDiffuse, const Vec4& _vSpecular, const Vec3& _vK) :
			vPosition(_vPosition),
			vDiffuse(_vDiffuse),
			vSpecular(_vSpecular),
			vK(_vK) {}
		
		Vec4 vPosition = { 0.f, 0.f, 0.f, 1.f };
		Vec4 vDiffuse;
		Vec4 vSpecular;

		// first: constant, second: linear, third: quadric
		Vec3 vK;
		float _pad = 0.f;
	};


	struct DirectionalLightComponent {
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		
		using Vec4 = TRS::Vector4<float>;
		DirectionalLightComponent(const Vec4& _vDirection, const Vec4& _vDiffuse, const Vec4& _vSpecular) :
			vDirection(_vDirection),
			vDiffuse(_vDiffuse),
			vSpecular(_vSpecular) {}

		Vec4 vDirection = { 1.f, 0.f, 0.f, 0.f };
		Vec4 vDiffuse;
		Vec4 vSpecular;
	};


	struct SpotlightComponent {
		SpotlightComponent() = default;
		SpotlightComponent(const SpotlightComponent&) = default;

		using Vec4 = TRS::Vector4<float>;
		SpotlightComponent(const Vec4& _vPosition, const Vec4& _vDirection, float _fInnerCutoff, float _fOuterCutoff) :
			vPosition(_vPosition),
			vDirection(_vDirection),
			fInnerCutoff(_fInnerCutoff),
			fOuterCutoff(_fOuterCutoff) {}

		Vec4 vPosition;
		Vec4 vDirection = { 1.f, 0.f, 0.f, 0.f };
		Vec4 vDiffuse;
		Vec4 vSpecular;
		float fInnerCutoff = MF_SQRT3 / 2.f; // 30 degrees
		float fOuterCutoff = 0.819152f;		 // 35 degrees
		float _pad[2] = {};
	};


#define SCRIPT_DEFINE_CONSTRUCTOR(script) script::script(DENG::Entity _idEntity, DENG::EventManager& _eventManager, DENG::Scene& _scene) :\
											 ScriptBehaviour(_idEntity, _eventManager, _scene, #script) {}
	class Scene;
	class DENG_API ScriptBehaviour {
		protected:
			const Entity m_idEntity = entt::null;
			EventManager& m_eventManager;
			Scene& m_scene;
			const std::string m_sClassName;

		public:
			ScriptBehaviour(Entity _idEntity, EventManager& _eventManager, Scene& _scene, const std::string& _sClassName = "MyClass") :
				m_idEntity(_idEntity),
				m_eventManager(_eventManager),
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

			template<typename T, typename... Args>
			inline void BindScript(Entity _idEntity, EventManager& _eventManager, Scene& _scene, Args... args) {
				m_pScriptBehaviour = new T(_idEntity, _eventManager, _scene, std::forward<Args>(args)...);

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


	struct DrawCommand {
		uint32_t uIndicesOffset = 0;
		uint32_t uDrawCount = 0; // if used with indices, this property specifies index count, otherwise it describes attribute count
		std::vector<std::size_t> attributeOffsets;
		bool bWireframe = false;

		struct {
			TRS::Point2D<int32_t> offset = { INT32_MAX, INT32_MAX };
			TRS::Point2D<uint32_t> extent = { INT32_MAX, INT32_MAX };
			bool bEnabled = false;
		} scissor;
	};


	struct MeshComponent {
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(MeshComponent&& _mesh) noexcept :
			sName(std::move(_mesh.sName)),
			drawCommands(std::move(_mesh.drawCommands)) {}

		MeshComponent(const std::string& _sName, const std::vector<DrawCommand>& _drawCommands) :
			sName(_sName),
			drawCommands(_drawCommands) {}

		std::string sName = "MeshComponent";
		std::vector<DrawCommand> drawCommands;
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
