#include "FTInstance.h"

#if AAA_TRUETYPE1_USE()

#include "AAA_FTFace.h"
#include "FTEngine.h"

#include "freetype1/freetype.h"


FTInstance::FTInstance( AAA_FTFace * face )
{
	_face		=	face;
	_instance	=	nullptr;
}

FTInstance::~FTInstance()
{
	if( _instance )
	{
		TT_Done_Instance(*_instance);

		delete _instance;
		_instance = nullptr;
	}
	_face = nullptr;
}

bool FTInstance::create()
{
	if( _instance )
		return true;

	if( !_face )
		return false;
	if( _face->getFace() == 0 )
		return false;

	_instance = new TT_Instance;

	if( TT_New_Instance( *_face->getFace(), _instance ) )
	{
		delete _instance;
		_instance = nullptr;
		return false;
	}

	return true;
}

bool FTInstance::setResolutions( int x_resolution, int y_resolution )
{
	if( _instance == 0 )
		return false;
	TT_Error err= TT_Set_Instance_Resolutions( *_instance, x_resolution, y_resolution );
	return err == 0;
}

bool FTInstance::set_point_size( int point_size )
{
	if( !_instance )
		return false;
	TT_Error err= TT_Set_Instance_PointSize( *_instance, point_size );
	return err == 0;
}

int FTInstance::get_sy() const
{
	if( !_instance )
		return 0;

	TT_Instance_Metrics imetrics;
	TT_Error err= TT_Get_Instance_Metrics( *_instance, &imetrics );
	if( err )
		return 0;
	return (imetrics.pointSize * imetrics.y_resolution) / (72*64);
}

// Contributed by Karl Anders Oygard <Karl.Oygard@fou.telenor.no>
int FTInstance::getDescender() const
{
	if( !_instance || !_face )
		return 0;

	TT_Instance_Metrics	imetrics;
	TT_Face_Properties	properties;
	TT_Error			err;

	err = TT_Get_Face_Properties( *_face->getFace(), &properties );
	if( err )
		return 0;

	err = TT_Get_Instance_Metrics( *_instance, &imetrics );
	if( err )
		return 0;

	return (properties.horizontal->Descender * imetrics.y_ppem) / properties.header->Units_Per_EM;
}

#endif //#if AAA_TRUETYPE1_USE()
