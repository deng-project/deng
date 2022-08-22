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
			ID_NEW,
			ID_OPEN,
			ID_SAVE,
			ID_IMPORT,
				ID_DAS,
				ID_GLTF,
				ID_OBJ,
				ID_STL,
			ID_EXPORT
		};
	}
}

#endif