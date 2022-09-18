// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NodeInspectorPanel.h - Node inspector panel class header
// author: Karl-Mihkel Ott

#ifndef NODE_INSPECTOR_PANEL_H
#define NODE_INSPECTOR_PANEL_H

#ifdef NODE_INSPECTOR_PANEL_CPP
	#include <sstream>
	#include <iomanip>
	
	#include <wx/wx.h>
	#include <wx/valnum.h>
	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"

	#define FLOAT_PRECISION 100
#endif

namespace DENG {
	namespace Editor {

		class NodeInspectorPanel : public wxWindow {
			private:
				NodeLoader* m_node = nullptr;
				TRS::Vector3<float> m_translation;
				TRS::Point3D<float> m_rotation;
				float m_scale = 1.0f;
				wxBoxSizer* m_sizer = nullptr;
				
				wxStaticText* m_title = nullptr;
				
				wxStaticText* m_translation_title = nullptr;
				wxFloatingPointValidator<float> m_translation_x_validator;
				wxTextCtrl* m_translation_x_ctrl = nullptr;
				wxSlider* m_translation_x_slider = nullptr;
				wxFloatingPointValidator<float> m_translation_y_validator;
				wxTextCtrl* m_translation_y_ctrl = nullptr;
				wxSlider* m_translation_y_slider = nullptr;
				wxFloatingPointValidator<float> m_translation_z_validator;
				wxTextCtrl* m_translation_z_ctrl = nullptr;
				wxSlider* m_translation_z_slider = nullptr;
				
				wxStaticText* m_rotation_title = nullptr;
				wxFloatingPointValidator<float> m_rotation_x_validator;
				wxTextCtrl* m_rotation_x_ctrl = nullptr;
				wxSlider* m_rotation_x = nullptr;
				wxFloatingPointValidator<float> m_rotation_y_validator;
				wxTextCtrl* m_rotation_y_ctrl = nullptr;
				wxSlider* m_rotation_y = nullptr;
				wxFloatingPointValidator<float> m_rotation_z_validator;
				wxTextCtrl* m_rotation_z_ctrl = nullptr;
				wxSlider* m_rotation_z = nullptr;

				wxStaticText* m_scale_title = nullptr;
				wxFloatingPointValidator<float> m_scale_validator;
				wxTextCtrl* m_scale_ctrl = nullptr;
				wxSlider* m_scale_slider = nullptr;
		
			private:
				// utility methods
				TRS::Point3D<float> _QuaternionToDegrees(TRS::Quaternion& _q);
				TRS::Quaternion _DegreesToQuaternion(TRS::Point3D<float>& _rot);

				// slider callbacks
				void _OnTranslationXSlide(wxScrollEvent& _evt);
				void _OnTranslationYSlide(wxScrollEvent& _evt);
				void _OnTranslationZSlide(wxScrollEvent& _evt);

				void _OnRotationXSlide(wxScrollEvent& _evt);
				void _OnRotationYSlide(wxScrollEvent& _evt);
				void _OnRotationZSlide(wxScrollEvent& _evt);

				void _OnScaleSlide(wxScrollEvent& _evt);

				// manual text entry enter callback
				void _OnTranslationXValueChange(wxCommandEvent& _evt);
				void _OnTranslationYValueChange(wxCommandEvent& _evt);
				void _OnTranslationZValueChange(wxCommandEvent& _evt);

				void _OnRotationXValueChange(wxCommandEvent& _evt);
				void _OnRotationYValueChange(wxCommandEvent& _evt);
				void _OnRotationZValueChange(wxCommandEvent& _evt);
				
				void _OnScaleValueChange(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				NodeInspectorPanel(wxWindow* _parent);
				void View(NodeLoader* _node);
		};
	}
}

#endif