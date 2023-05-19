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

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"


namespace DENG {

	struct TransformComponent {
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TRS::Matrix4<float> mCustomTransform;
		TRS::Point3D<float> translation;
		TRS::Point3D<float> rotation;
		TRS::Point3D<float> scale = { 1.f, 1.f, 1.f };
	};


	struct LightComponent {
		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(TRS::Vector4<float> _vColor, float _fIntensity) :
			vColor(_vColor),
			fIntensity(_fIntensity) {}
		
		TRS::Vector4<float> vColor = { 1.f, 1.f, 1.f, 1.f };
		float fIntensity = 1.f;
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


	// to be implemented in more depth
	struct MaterialComponent {
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(MaterialComponent&& _material) noexcept :
			textureIds(std::move(_material.textureIds)),
			fReflectivity(_material.fReflectivity) {}

		std::vector<uint32_t> textureIds;
		float fReflectivity = 1.f;
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
			itShaderModule(_mesh.itShaderModule),
			uSupportedTextureCount(_mesh.uSupportedTextureCount),
			drawCommands(std::move(_mesh.drawCommands)),
			uniformDataLayouts(std::move(_mesh.uniformDataLayouts)) {}

		std::string sName = "MeshComponent";
		std::list<PipelineModule>::iterator itShaderModule = {};
		uint32_t uSupportedTextureCount = 0;
		std::vector<DrawCommand> drawCommands;
		std::vector<UniformDataLayout> uniformDataLayouts;
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
