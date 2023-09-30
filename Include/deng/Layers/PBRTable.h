#ifndef PBR_TABLE_H
#define PBR_TABLE_H

#include <cvar/SID.h>
#include "deng/ResourceIdTable.h"

namespace DENG {
	dDECLARE_RESOURCE_ID_TABLE(PBRTable)
		dRESOURCE_ID_ENTRY("RedMaterial"),
		dRESOURCE_ID_ENTRY("SphereMesh"),
		dRESOURCE_ID_ENTRY("PBRShader"),
		dRESOURCE_ID_ENTRY("RustAlbedo"),
		dRESOURCE_ID_ENTRY("RustMetallic"),
		//dRESOURCE_ID_ENTRY("RustNormal"),
		dRESOURCE_ID_ENTRY("RustRoughness")
	dEND_RESOURCE_ID_TABLE(PBRTable)
}

#endif
