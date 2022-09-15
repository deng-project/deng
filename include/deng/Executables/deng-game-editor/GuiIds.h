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
			ID_NEW_PROJECT,
			ID_OPEN_PROJECT,

			// Website stuff ids
			ID_DENG_WEBSITE,
			ID_DENG_FORUM,
			ID_GIT,

			// New project wizard
			ID_NEW_PROJECT_WIZARD,
			ID_PATH,
			ID_CHOOSE_DIRECTORY,
			ID_PROJECT_NAME,
			ID_DIRECTORY_NAME,
			ID_BACKEND,
			ID_CREATE_NEW_PROJECT,
			ID_CANCEL,
			/** End of project management **/

			/** Menubar elements **/
			// file
			ID_NEW,
			ID_OPEN,
			ID_SAVE,
			ID_IMPORT,
			ID_DAS,
			ID_GLTF,
			ID_OBJ,
			ID_STL,
			ID_EXPORT,

			// add
			ID_NODE,
			ID_MESH,
			ID_SPHERE,
			ID_CUBE,
			ID_CONE,
			ID_LIGHT,
			ID_DIRECTIONAL_LIGHT,
			ID_POINT_LIGHT,
			ID_AMBIENT_LIGHT,
			ID_CAMERA,
			ID_FIRST_PERSON_CAMERA,
			ID_THIRD_PERSON_CAMERA,
			ID_EDITOR_CAMERA,
			/** End of menubar elements **/

			/** Hierarchy panel **/
			ID_HIERARCHY_PANEL,
			ID_ANIMATION_PANEL_TITLE,
			ID_BIND_ANIMATION,
			ID_REPEAT_ANIMATION,
			ID_ANIMATE_TOGGLE,
			/** End of hierarchy panel **/

			/** Toolbar stuff **/
			ID_TOOLBAR_RUN,
		};
	}
}

#endif