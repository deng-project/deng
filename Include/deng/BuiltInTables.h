// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: BuiltInTables.h - Built in id tables
// author: Karl-Mihkel Ott

#ifndef BUILT_IN_TABLES_H
#define BUILT_IN_TABLES_H

#include "deng/ResourceIdTable.h"

namespace DENG {

	dDECLARE_RESOURCE_ID_TABLE(BuiltInEventTable)
		// WindowEvents.h
		dRESOURCE_ID_ENTRY("WindowEvent"),
		dRESOURCE_ID_ENTRY("WindowShownEvent"),
		dRESOURCE_ID_ENTRY("WindowHiddenEvent"),
		dRESOURCE_ID_ENTRY("WindowExposedEvent"),
		dRESOURCE_ID_ENTRY("WindowResizedEvent"),
		dRESOURCE_ID_ENTRY("WindowMinimizedEvent"),
		dRESOURCE_ID_ENTRY("WindowMaximizedEvent"),
		dRESOURCE_ID_ENTRY("WindowDisplayChangedEvent"),
		dRESOURCE_ID_ENTRY("WindowFocusEvent"),
		dRESOURCE_ID_ENTRY("WindowGainedMouseFocusEvent"),
		dRESOURCE_ID_ENTRY("WindowLostMouseFocusEvent"),
		dRESOURCE_ID_ENTRY("WindowGainedKeyboardFocusEvent"),
		dRESOURCE_ID_ENTRY("WindowLostKeyboardFocusEvent"),
		
		// InputEvents.h
		dRESOURCE_ID_ENTRY("InputEvent"),
		dRESOURCE_ID_ENTRY("KeyboardEvent"),
		dRESOURCE_ID_ENTRY("KeyPressedEvent"),
		dRESOURCE_ID_ENTRY("KeyReleasedEvent"),
		dRESOURCE_ID_ENTRY("KeyTypedEvent"),
		dRESOURCE_ID_ENTRY("MouseEvent"),
		dRESOURCE_ID_ENTRY("MouseMovedEvent"),
		dRESOURCE_ID_ENTRY("MouseScrolledEvent"),
		dRESOURCE_ID_ENTRY("MouseButtonEvent"),
		dRESOURCE_ID_ENTRY("MouseButtonPressedEvent"),
		dRESOURCE_ID_ENTRY("MouseButtonReleasedEvent"),
		dRESOURCE_ID_ENTRY("GamepadEvent"),
		dRESOURCE_ID_ENTRY("GamepadButtonEvent"),
		dRESOURCE_ID_ENTRY("GamepadButtonPressedEvent"),
		dRESOURCE_ID_ENTRY("GamepadButtonReleasedEvent"),
		dRESOURCE_ID_ENTRY("GamepadJoystickMovedEvent"),

		// SceneEvents.h
		dRESOURCE_ID_ENTRY("SceneEvent"),
		dRESOURCE_ID_ENTRY("RegistryStateEvent"),
		dRESOURCE_ID_ENTRY("EntityEvent"),
		dRESOURCE_ID_ENTRY("EntityAddedEvent"),
		dRESOURCE_ID_ENTRY("EntityRemoveEvent"),
		dRESOURCE_ID_ENTRY("ComponentEvent"),
		dRESOURCE_ID_ENTRY("ComponentModifiedEvent"),
		dRESOURCE_ID_ENTRY("ComponentAddedEvent"),
		dRESOURCE_ID_ENTRY("ComponentRemoveEvent"),

		// ResourceEvents.h
		dRESOURCE_ID_ENTRY("ResourceEvent"),
		dRESOURCE_ID_ENTRY("ResourceModifiedEvent"),
		dRESOURCE_ID_ENTRY("ResourceAddedEvent"),
		dRESOURCE_ID_ENTRY("ResourceRemoveEvent")
	dEND_RESOURCE_ID_TABLE(BuiltEventTable)

	dDECLARE_RESOURCE_ID_TABLE(BuiltInMeshTable)
		dRESOURCE_ID_ENTRY("_dImGuiMesh_")
	dEND_RESOURCE_ID_TABLE(BuiltInMeshTable)

	dDECLARE_RESOURCE_ID_TABLE(BuiltInShaderTable)
		dRESOURCE_ID_ENTRY("_dImGuiShader_")
	dEND_RESOURCE_ID_TABLE(BuiltInShaderTable)

	dDECLARE_RESOURCE_ID_TABLE(BuiltInTextureTable)
		dRESOURCE_ID_ENTRY("_dImGuiTextureAtlas_"),
		dRESOURCE_ID_ENTRY("_dMissingTexture2D_"),
		dRESOURCE_ID_ENTRY("_dMissingTexture3D_")
	dEND_RESOURCE_ID_TABLE(BuiltInTextureTable)

	dDECLARE_RESOURCE_ID_TABLE(BuiltInMaterialTable)
		dRESOURCE_ID_ENTRY("_dEmeraldMaterial_"),
		dRESOURCE_ID_ENTRY("_dJadeMaterial_"),
		dRESOURCE_ID_ENTRY("_dObsidianMaterial_"),
		dRESOURCE_ID_ENTRY("_dPearlMaterial_"),
		dRESOURCE_ID_ENTRY("_dRubyMaterial_"),
		dRESOURCE_ID_ENTRY("_dTurquoiseMaterial_"),
		dRESOURCE_ID_ENTRY("_dBrassMaterial_"),
		dRESOURCE_ID_ENTRY("_dBronzeMaterial_"),
		dRESOURCE_ID_ENTRY("_dChromeMaterial_"),
		dRESOURCE_ID_ENTRY("_dCopperMaterial_"),
		dRESOURCE_ID_ENTRY("_dGoldMaterial_"),
		dRESOURCE_ID_ENTRY("_dSilverMaterial_"),
		dRESOURCE_ID_ENTRY("_dBlackPlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dCyanPlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dGreenPlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dRedPlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dWhitePlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dYellowPlasticMaterial_"),
		dRESOURCE_ID_ENTRY("_dBlackRubberMaterial_"),
		dRESOURCE_ID_ENTRY("_dCyanRubberMaterial_"),
		dRESOURCE_ID_ENTRY("_dGreenRubberMaterial_"),
		dRESOURCE_ID_ENTRY("_dRedRubberMaterial_"),
		dRESOURCE_ID_ENTRY("_dWhiteRubberMaterial_"),
		dRESOURCE_ID_ENTRY("_dYellowRubberMaterial_")
	dEND_RESOURCE_ID_TABLE(BuiltInMaterialTable)
}

#endif
