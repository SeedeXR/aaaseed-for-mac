
WRAP_LOADER( Dense3DGetErrorCode,	Dense3DErrorCode,	(void) )
WRAP_LOADER( Dense3DGetLibVersion,	char*,				(void) )
WRAP_LOADER( Dense3DOpen,			bool,				(Dense3DInstance *dense3D) )
WRAP_LOADER( Dense3DClose,			bool,				(Dense3DInstance dense3D) )
WRAP_LOADER( Dense3DGetDepth,		bool,				(Dense3DInstance dense3D, uint8_t *leftImage, uint8_t *rightImage, float *disparityData, PDense3DDepth depthData) )
WRAP_LOADER( Dense3DGetRectified,	bool,				(Dense3DInstance dense3D, uint8_t **leftImage, uint8_t **rightImage) )
WRAP_LOADER( Dense3DSavePLY,		bool,				(Dense3DInstance dense3D, char *plyFile) )
WRAP_LOADER( __Dense3DParamSet__,	bool,				(Dense3DInstance dense3D, Dense3DParameter param, ...) )
WRAP_LOADER( __Dense3DParamGet__,	bool,				(Dense3DInstance dense3D, Dense3DParameter param, ...) )

