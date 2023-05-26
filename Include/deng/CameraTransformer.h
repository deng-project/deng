// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraTransfomer.h - Camera transform abstraction class header
// author: Karl-Mihkel Ott

#ifndef CAMERA_TRANSFORMER_H
#define CAMERA_TRANSFORMER_H

#include <cstdint>
#include <cmath>
#include <string>
#include "trs/Vector.h"
#include "trs/Matrix.h"

#ifdef CAMERA_TRANSFORMER_CPP
	#include "deng/ErrorDefinitions.h"
#endif

#include "deng/Api.h"
#include "deng/MathConstants.h"

namespace DENG {

	class DENG_API CameraTransformer {
		private:
			float m_fFieldOfView = 65.f * (float)M_PI / 180.f;
			float m_fNearPlane = 0.0001f;
			float m_fFarPlane = 1e6f;

			float m_fPitch = 0.f;
			float m_fYaw = 0.f;
			float m_fRoll = 0.f;

			TRS::Vector3<float> m_vRightDirection = { 1.f, 0.f, 0.f };
			TRS::Vector3<float> m_vUpDirection = { 0.f, 1.f, 0.f };
			TRS::Vector3<float> m_vLookAtDirection = { 0.f, 0.f, -1.f };
			TRS::Vector3<float> m_vPosition = { 0.f, 0.f, 0.f };

		public:
			inline void SetFieldOfView(float _fFOV)		{ m_fFieldOfView = _fFOV;		}
			inline float GetFieldOfView()				{ return m_fFieldOfView;		}
			inline void SetNearPlane(float _fNearPlane) { m_fNearPlane = _fNearPlane;	}
			inline float GetNearPlane()					{ return m_fNearPlane;			}
			inline void SetFarPlane(float _fFarPlane)	{ m_fFarPlane = _fFarPlane;		}
			inline float GetFarPlane()					{ return m_fFarPlane;			}
			inline void SetPitch(float _fPitch)			{ m_fPitch = _fPitch;			}
			inline float GetPitch()						{ return m_fPitch;				}
			inline void SetYaw(float _fYaw)				{ m_fYaw = _fYaw;				}
			inline float GetYaw()						{ return m_fYaw;				}
			inline void SetRoll(float _fRoll)			{ m_fRoll = _fRoll;				}
			inline float GetRoll()						{ return m_fRoll;				}

			inline void AbsoluteMoveX(float _fDelta) { m_vPosition[0] += _fDelta;					}
			inline void AbsoluteMoveY(float _fDelta) { m_vPosition[1] += _fDelta;					}
			inline void AbsoluteMoveZ(float _fDelta) { m_vPosition[2] += _fDelta;					}
			inline void RelativeMoveX(float _fDelta) { m_vPosition += m_vRightDirection * _fDelta;	}
			inline void RelativeMoveY(float _fDelta) { m_vPosition += m_vUpDirection * _fDelta;		}
			inline void RelativeMoveZ(float _fDelta) { m_vPosition += m_vLookAtDirection * _fDelta; }

			inline void SetPosition(const TRS::Vector3<float>& _vPosition) { m_vPosition = _vPosition; }
			inline TRS::Vector3<float> GetPosition() { return m_vPosition;  }

			TRS::Matrix4<float> CalculateProjection(uint32_t _uViewportWidth, uint32_t _uViewportHeight);
			void CalculateLookAt();
			TRS::Matrix4<float> CalculateViewMatrix();
	};
}

#endif
