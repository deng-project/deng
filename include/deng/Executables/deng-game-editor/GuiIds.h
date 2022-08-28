// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GuiIds.h - Gui element ID definitions header
// author: Karl-Mihkel Ott

#ifndef GUI_IDS_H
#define GUI_IDS_H

namespace DENG {
	namespace Editor {

		enum ProjectManagerIDs {
			// Project management ids
			ID_NEW_PROJECT = 1,
			ID_OPEN_PROJECT = 2,

			// Website stuff ids
			ID_DENG_WEBSITE = 3,
			ID_DENG_FORUM = 4,
			ID_GIT = 5,

			// New project wizard
			ID_PATH = 6,
			ID_CHOOSE_DIRECTORY = 7,
			ID_PROJECT_NAME = 8,
			ID_DIRECTORY_NAME = 9,
			ID_BACKEND = 10,
			ID_CREATE_NEW_PROJECT = 11,
			ID_CANCEL = 12
		};


		enum GameEditorIDs {
			/** Menubar elements **/
			// file
			ID_NEW = 13,
			ID_OPEN	= 14,
			ID_SAVE	= 15,
			ID_IMPORT = 16,
				ID_DAS = 17,
				ID_GLTF	= 18,
				ID_OBJ = 19,
				ID_STL = 20,
			ID_EXPORT = 21,

			// add
			ID_NODE	= 22,
			ID_MESH	= 23,
				ID_SPHERE = 24,
				ID_CUBE	= 25,
				ID_CONE = 26,
			ID_LIGHT = 27,
				ID_DIRECTIONAL_LIGHT = 28,
				ID_POINT_LIGHT = 29,
				ID_AMBIENT_LIGHT = 30,
			ID_CAMERA = 31,
				ID_FIRST_PERSON_CAMERA = 32,
				ID_THIRD_PERSON_CAMERA = 33,
				ID_EDITOR_CAMERA = 34,
			/** End of menubar elements **/
		};
	}
}

#endif