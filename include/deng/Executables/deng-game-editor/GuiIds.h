// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GuiIds.h - Gui element ID definitions header
// author: Karl-Mihkel Ott

#ifndef GUI_IDS_H
#define GUI_IDS_H

namespace DENG {
	namespace Editor {

		enum GuiIDs {
			/** Project management **/
			// Project management ids
			ID_PROJECT_MANAGER_NEW,
			ID_PROJECT_MANAGER_OPEN,

			// Website stuff ids
			ID_PROJECT_MANAGER_DENG_WEBSITE,
			ID_PROJECT_MANAGER_DENG_FORUM,
			ID_PROJECT_MANAGER_GIT,

			// New project wizard
			ID_NEW_PROJECT_WIZARD,
			ID_NEW_PROJECT_WIZARD_PATH,
			ID_NEW_PROJECT_WIZARD_CHOOSE_DIRECTORY,
			ID_NEW_PROJECT_WIZARD_PROJECT_NAME,
			ID_NEW_PROJECT_WIZARD_DIRECTORY_NAME,
			ID_NEW_PROJECT_WIZARD_BACKEND,
			ID_NEW_PROJECT_WIZARD_CREATE_NEW_PROJECT,
			ID_NEW_PROJECT_WIZARD_CANCEL,
			/** End of project management **/

			/** Menubar elements **/
			// file
			ID_EDITOR_MENUBAR_FILE_NEW,
			ID_EDITOR_MENUBAR_FILE_OPEN,
			ID_EDITOR_MENUBAR_FILE_SAVE,
			ID_EDITOR_MENUBAR_FILE_IMPORT,
			ID_EDITOR_MENUBAR_FILE_IMPORT_DAS,
			ID_EDITOR_MENUBAR_FILE_IMPORT_GLTF,
			ID_EDITOR_MENUBAR_FILE_IMPORT_OBJ,
			ID_EDITOR_MENUBAR_FILE_IMPORT_STL,

			// add
			ID_EDITOR_MENUBAR_ADD_SKYBOX,
			ID_EDITOR_MENUBAR_ADD_NODE,
			ID_EDITOR_MENUBAR_ADD_MESH,
			ID_EDITOR_MENUBAR_ADD_MESH_SPHERE,
			ID_EDITOR_MENUBAR_ADD_MESH_CUBE,
			ID_EDITOR_MENUBAR_ADD_MESH_CONE, // temporary
			ID_EDITOR_MENUBAR_ADD_MESH_CAPSULE,
			ID_EDITOR_MENUBAR_ADD_LIGHT,
			ID_EDITOR_MENUBAR_ADD_LIGHT_DIRECTIONAL_LIGHT,
			ID_EDITOR_MENUBAR_ADD_LIGHT_POINT_LIGHT,
			ID_EDITOR_MENUBAR_ADD_LIGHT_AMBIENT_LIGHT,
			ID_EDITOR_MENUBAR_ADD_CAMERA,
			ID_EDITOR_MENUBAR_ADD_CAMERA_FIRST_PERSON_CAMERA,
			ID_EDITOR_MENUBAR_ADD_CAMERA_THIRD_PERSON_CAMERA,
			ID_EDITOR_MENUBAR_ADD_CAMERA_EDITOR_CAMERA,

			// view
			ID_EDITOR_MENUBAR_VIEW_GRID,

			// debug
#ifdef __DEBUG
			ID_EDITOR_MENUBAR_DEBUG_TRIANGLE,
#endif
			/** End of menubar elements **/

			/** Hierarchy panel **/
			ID_EDITOR_HIERARCHY_PANEL,
			/** End of hierarchy panel **/

			/** Animation inspector panel **/
			ID_EDITOR_ANIMATION_PANEL_TITLE,
			ID_EDITOR_ANIMATION_PANEL_BIND,
			ID_EDITOR_ANIMATION_PANEL_REPEAT,
			ID_EDITOR_ANIMATION_PANEL_TOGGLE,
			/** End of animation panel **/

			/** Node inspector panel **/
			ID_EDITOR_NODE_PANEL_TITLE,
			ID_EDITOR_NODE_PANEL_TRANSLATION_X_CTRL,
			ID_EDITOR_NODE_PANEL_TRANSLATION_X,
			ID_EDITOR_NODE_PANEL_TRANSLATION_Y_CTRL,
			ID_EDITOR_NODE_PANEL_TRANSLATION_Y,
			ID_EDITOR_NODE_PANEL_TRANSLATION_Z_CTRL,
			ID_EDITOR_NODE_PANEL_TRANSLATION_Z,
			
			ID_EDITOR_NODE_PANEL_ROTATION_X_CTRL,
			ID_EDITOR_NODE_PANEL_ROTATION_X,
			ID_EDITOR_NODE_PANEL_ROTATION_Y_CTRL,
			ID_EDITOR_NODE_PANEL_ROTATION_Y,
			ID_EDITOR_NODE_PANEL_ROTATION_Z_CTRL,
			ID_EDITOR_NODE_PANEL_ROTATION_Z,

			ID_EDITOR_NODE_PANEL_SCALE_CTRL,
			ID_EDITOR_NODE_PANEL_SCALE,
			/** End of node inspector panel **/

			/** Mesh inspector panel **/
			ID_EDITOR_MESH_PANEL_TITLE,
			ID_EDITOR_MESH_PANEL_USE_TEXTURES,
			ID_EDITOR_MESH_PANEL_COLOR_PICKER,
			ID_EDITOR_MESH_PANEL_PICK_TEXTURE,
			ID_EDITOR_MESH_PANEL_EDIT_VERTEX_SHADER,
			ID_EDITOR_MESH_PANEL_EDIT_FRAGMENT_SHADER,
			ID_EDITOR_MESH_PANEL_EDIT_GEOMETRY_SHADER,

			ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_USE_TEXTURE,
			ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_OK,
			/** End of mesh inspector panel **/

			/** Shader viewer dialog **/
			ID_SHADER_VIEWER_EDIT_SHADER,
			ID_SHADER_VIEWER_COMMIT_CHANGES,
			ID_SHADER_VIEWER_SHADER_SOURCE_VIEW,
			ID_SHADER_VIEWER_SAVE_CHANGES,
			/** End of shader viewer dialog **/

			/** Cube generator dialog **/
			ID_CUBE_GENERATOR_DIALOG_MIN,
			ID_CUBE_GENERATOR_DIALOG_MAX,
			ID_CUBE_GENERATOR_DIALOG_USE_NORMALS,
			ID_CUBE_GENERATOR_DIALOG_OK,
			/** End of cube generator dialog **/

			/** Sphere generator dialog **/
			ID_SPHERE_GENERATOR_DIALOG_RADIUS,
			ID_SPHERE_GENERATOR_DIALOG_SUBDIVIDE,
			ID_SPHERE_GENERATOR_DIALOG_USE_NORMALS,
			ID_SPHERE_GENERATOR_DIALOG_OK,
			/** End of sphere generator dialog **/

			/** Skybox generator dialog **/
			ID_SKYBOX_GENERATOR_TOP,
			ID_SKYBOX_GENERATOR_LEFT,
			ID_SKYBOX_GENERATOR_FRONT,
			ID_SKYBOX_GENERATOR_RIGHT,
			ID_SKYBOX_GENERATOR_BOTTOM,
			ID_SKYBOX_GENERATOR_BACK,
			ID_SKYBOX_GENERATOR_SIZE,
			ID_SKYBOX_GENERATOR_OK,
			/** End of skybox generator dialog **/

			/** Toolbar stuff **/
			ID_TOOLBAR_RUN,
		};
	}
}

#endif