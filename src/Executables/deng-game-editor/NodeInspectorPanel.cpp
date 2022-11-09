// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NodeInspectorPanel.cpp - Node inspector panel class implementation
// author: Karl-Mihkel Ott

#define NODE_INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/NodeInspectorPanel.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(NodeInspectorPanel, wxWindow)
			// slider scroll events
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_TRANSLATION_X, NodeInspectorPanel::_OnTranslationXSlide)
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_TRANSLATION_Y, NodeInspectorPanel::_OnTranslationYSlide)
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_TRANSLATION_Z, NodeInspectorPanel::_OnTranslationZSlide)
			
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_ROTATION_X, NodeInspectorPanel::_OnRotationXSlide)
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_ROTATION_Y, NodeInspectorPanel::_OnRotationYSlide)
			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_ROTATION_Z, NodeInspectorPanel::_OnRotationZSlide)

			EVT_COMMAND_SCROLL(ID_EDITOR_NODE_PANEL_SCALE, NodeInspectorPanel::_OnScaleSlide)
		
			// text entry events
			EVT_TEXT(ID_EDITOR_NODE_PANEL_TRANSLATION_X_CTRL, NodeInspectorPanel::_OnTranslationXValueChange)
			EVT_TEXT(ID_EDITOR_NODE_PANEL_TRANSLATION_Y_CTRL, NodeInspectorPanel::_OnTranslationYValueChange)
			EVT_TEXT(ID_EDITOR_NODE_PANEL_TRANSLATION_Z_CTRL, NodeInspectorPanel::_OnTranslationZValueChange)

			EVT_TEXT(ID_EDITOR_NODE_PANEL_ROTATION_X_CTRL, NodeInspectorPanel::_OnRotationXValueChange)
			EVT_TEXT(ID_EDITOR_NODE_PANEL_ROTATION_Y_CTRL, NodeInspectorPanel::_OnRotationYValueChange)
			EVT_TEXT(ID_EDITOR_NODE_PANEL_ROTATION_Z_CTRL, NodeInspectorPanel::_OnRotationZValueChange)

			EVT_TEXT(ID_EDITOR_NODE_PANEL_SCALE_CTRL, NodeInspectorPanel::_OnScaleValueChange)
		wxEND_EVENT_TABLE()

		NodeInspectorPanel::NodeInspectorPanel(wxWindow* _parent) :
			wxWindow(_parent, wxID_ANY, wxDefaultPosition, wxSize(200, 800)),
			m_translation_x_validator(2, &m_translation.first),
			m_translation_y_validator(2, &m_translation.second),
			m_translation_z_validator(2, &m_translation.third),

			m_rotation_x_validator(2, &m_rotation.x),
			m_rotation_y_validator(2, &m_rotation.y),
			m_rotation_z_validator(2, &m_rotation.z),
			m_scale_validator(2, &m_scale)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			
			// title property
			m_title = new wxStaticText(this, ID_EDITOR_NODE_PANEL_TITLE, wxT("(null)"));
			m_sizer->Add(m_title, 0, wxALIGN_CENTER);

			// translation properties
			// NOTE: (-100, 100) is just some arbitrary value
			m_translation_title = new wxStaticText(this, wxID_ANY, wxT("Translation:"));
			m_translation_x_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_TRANSLATION_X_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_translation_x_validator);
			m_translation_x_slider = new wxSlider(this, ID_EDITOR_NODE_PANEL_TRANSLATION_X, 0, -100 * FLOAT_PRECISION, 100 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_translation_y_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_TRANSLATION_Y_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_translation_y_validator);
			m_translation_y_slider = new wxSlider(this, ID_EDITOR_NODE_PANEL_TRANSLATION_Y, 0, -100 * FLOAT_PRECISION, 100 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_translation_z_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_TRANSLATION_Z_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_translation_z_validator);
			m_translation_z_slider = new wxSlider(this, ID_EDITOR_NODE_PANEL_TRANSLATION_Z, 0, -100 * FLOAT_PRECISION, 100 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_sizer->Add(m_translation_title, 0, wxALIGN_LEFT);
			m_sizer->Add(m_translation_x_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_translation_x_slider, 0, wxALIGN_LEFT);
			m_sizer->Add(m_translation_y_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_translation_y_slider, 0, wxALIGN_LEFT);
			m_sizer->Add(m_translation_z_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_translation_z_slider, 0, wxALIGN_LEFT);

			// rotation properties
			m_rotation_title = new wxStaticText(this, wxID_ANY, wxT("Rotation (degrees):"));
			m_rotation_x_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_ROTATION_X_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_rotation_x_validator);
			m_rotation_x = new wxSlider(this, ID_EDITOR_NODE_PANEL_ROTATION_X, 0, -180 * FLOAT_PRECISION, 180 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_rotation_y_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_ROTATION_Y_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_rotation_y_validator);
			m_rotation_y = new wxSlider(this, ID_EDITOR_NODE_PANEL_ROTATION_Y, 0, -180 * FLOAT_PRECISION, 180 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_rotation_z_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_ROTATION_Z_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_rotation_z_validator);
			m_rotation_z = new wxSlider(this, ID_EDITOR_NODE_PANEL_ROTATION_Z, 0, -180 * FLOAT_PRECISION, 180 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_sizer->Add(m_rotation_title, 0, wxALIGN_LEFT);
			m_sizer->Add(m_rotation_x_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_rotation_x, 0, wxALIGN_LEFT);
			m_sizer->Add(m_rotation_y_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_rotation_y, 0, wxALIGN_LEFT);
			m_sizer->Add(m_rotation_z_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_rotation_z, 0, wxALIGN_LEFT);

			// scale properties
			m_scale_title = new wxStaticText(this, wxID_ANY, wxT("Scale"));
			
			m_scale_ctrl = new wxTextCtrl(this, ID_EDITOR_NODE_PANEL_SCALE_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 20), 0L, m_scale_validator);
			m_scale_slider = new wxSlider(this, ID_EDITOR_NODE_PANEL_SCALE, 1 * FLOAT_PRECISION, 0, 100 * FLOAT_PRECISION, wxDefaultPosition, wxSize(200, 20));
			m_sizer->Add(m_scale_title, 0, wxALIGN_LEFT);
			m_sizer->Add(m_scale_ctrl, 0, wxALIGN_CENTER);
			m_sizer->Add(m_scale_slider, 0, wxALIGN_LEFT);

			SetSizer(m_sizer);
		}


		void NodeInspectorPanel::View(NodeLoader* _node) {
			m_node = _node;
			m_title->SetLabelText(_node->GetName());

			m_translation = m_node->GetCustomTranslation();
			m_translation_x_slider->SetValue(static_cast<int>(m_translation.first * FLOAT_PRECISION));
			m_translation_y_slider->SetValue(static_cast<int>(m_translation.second * FLOAT_PRECISION));
			m_translation_z_slider->SetValue(static_cast<int>(m_translation.third * FLOAT_PRECISION));
			
			m_rotation = _QuaternionToDegrees(m_node->GetCustomRotation());
			m_rotation_x->SetValue(static_cast<int>(m_rotation.x * FLOAT_PRECISION));
			m_rotation_y->SetValue(static_cast<int>(m_rotation.y * FLOAT_PRECISION));
			m_rotation_z->SetValue(static_cast<int>(m_rotation.z * FLOAT_PRECISION));

			m_scale = m_node->GetCustomScale();
			m_scale_slider->SetValue(static_cast<int>(m_scale * FLOAT_PRECISION));
			
			Show();
			TransferDataToWindow();
		}


		TRS::Point3D<float> NodeInspectorPanel::_QuaternionToDegrees(TRS::Quaternion& _q) {
			float half_phi = 0.f;
			TRS::Point3D<float> rot = { 0.f, 0.f, 0.f };
			if (_q.w < 1.0f) {
				half_phi = acosf(_q.w);
				rot.x = (_q.x / sinf(half_phi)) * 180.f / PI;
				rot.y = (_q.y / sinf(half_phi)) * 180.f / PI;
				rot.z = (_q.z / sinf(half_phi)) * 180.f / PI;
			}

			return rot;
		}

		TRS::Quaternion NodeInspectorPanel::_DegreesToQuaternion(TRS::Point3D<float>& _rot) {
			TRS::Quaternion x = { sinf(_rot.x * PI / 360.f), 0.0f, 0.0f, cosf(_rot.x * PI / 360.f) };
			TRS::Quaternion y = { 0.f, sinf(_rot.y * PI / 360.f), 0.f, cosf(_rot.y * PI / 360.f) };
			TRS::Quaternion z = { 0.f, 0.f, sinf(_rot.z * PI / 360.f), cosf(_rot.z * PI / 360.f) };

			return x * y * z;
		}

		void NodeInspectorPanel::_OnTranslationXSlide(wxScrollEvent& _evt) {
			float val = static_cast<float>(_evt.GetPosition());
			m_translation = m_node->GetCustomTranslation();
			m_translation.first = val / FLOAT_PRECISION;
			m_node->SetCustomTranslation(m_translation);
			
			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_translation.first;
			m_translation_x_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnTranslationYSlide(wxScrollEvent& _evt) {
			float val = static_cast<float>(_evt.GetPosition());
			m_translation = m_node->GetCustomTranslation();
			m_translation.second = val / FLOAT_PRECISION;
			m_node->SetCustomTranslation(m_translation);

			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_translation.second;
			m_translation_y_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnTranslationZSlide(wxScrollEvent& _evt) {
			float val = static_cast<float>(_evt.GetPosition());
			m_translation = m_node->GetCustomTranslation();
			m_translation.third = val / FLOAT_PRECISION;
			m_node->SetCustomTranslation(m_translation);

			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_translation.third;
			m_translation_z_ctrl->SetValue(s.str());

			_evt.Skip();
		}


		void NodeInspectorPanel::_OnRotationXSlide(wxScrollEvent& _evt) {
			m_rotation = _QuaternionToDegrees(m_node->GetCustomRotation());
			m_rotation.x = static_cast<float>(_evt.GetPosition()) / FLOAT_PRECISION;
			m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));

			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_rotation.x;
			m_rotation_x_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnRotationYSlide(wxScrollEvent& _evt) {
			m_rotation = _QuaternionToDegrees(m_node->GetCustomRotation());
			m_rotation.y = static_cast<float>(_evt.GetPosition()) / FLOAT_PRECISION;
			m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));
		
			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_rotation.y;
			m_rotation_y_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnRotationZSlide(wxScrollEvent& _evt) {
			m_rotation = _QuaternionToDegrees(m_node->GetCustomRotation());
			m_rotation.z = static_cast<float>(_evt.GetPosition()) / FLOAT_PRECISION;
			m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));
			
			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_rotation.z;
			m_rotation_z_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnScaleSlide(wxScrollEvent& _evt) {
			m_scale = static_cast<float>(_evt.GetPosition()) / FLOAT_PRECISION;
			m_node->SetCustomScale(m_scale);
		
			std::stringstream s;
			s << std::fixed << std::setprecision(2) << m_scale;
			m_scale_ctrl->SetValue(s.str());

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnTranslationXValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_translation_x_slider->SetValue(static_cast<int>(m_translation.first * FLOAT_PRECISION));
				m_node->SetCustomTranslation(m_translation);
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnTranslationYValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_translation_y_slider->SetValue(static_cast<int>(m_translation.second * FLOAT_PRECISION));
				m_node->SetCustomTranslation(m_translation);
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnTranslationZValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_translation_z_slider->SetValue(static_cast<int>(m_translation.third * FLOAT_PRECISION));
				m_node->SetCustomTranslation(m_translation);
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnRotationXValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_rotation_x->SetValue(static_cast<int>(m_rotation.x * FLOAT_PRECISION));
				m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnRotationYValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_rotation_y->SetValue(static_cast<int>(m_rotation.y * FLOAT_PRECISION));
				m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnRotationZValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_rotation_z->SetValue(static_cast<int>(m_rotation.z * FLOAT_PRECISION));
				m_node->SetCustomRotation(_DegreesToQuaternion(m_rotation));
			}

			_evt.Skip();
		}

		void NodeInspectorPanel::_OnScaleValueChange(wxCommandEvent& _evt) {
			if (Validate() && TransferDataFromWindow()) {
				m_scale_slider->SetValue(static_cast<int>(m_scale * FLOAT_PRECISION));
				m_node->SetCustomScale(m_scale);
			}

			_evt.Skip();
		}
	}
}