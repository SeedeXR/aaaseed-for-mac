#include "FTEngine.h"

#if AAA_TRUETYPE1_USE()

#include "freetype1/freetype.h"

// static
FTEngine* FTEngine::_static_engine= 0;

FTEngine::FTEngine()
{
	_engine = nullptr;
}


FTEngine::~FTEngine()
{
	if( _engine )
	{
		TT_Done_FreeType(*_engine);
		delete _engine;
		_engine = nullptr;
	}
}

bool FTEngine::init()
{
	if( _engine )
		return true;

	_engine = new TT_Engine;
	if( TT_Init_FreeType( _engine ) )
	{
		delete _engine;
		_engine = nullptr;
		return false;
	}
	return true;
}

// static
FTEngine* FTEngine::getStaticEngine()
{
	if( !_static_engine )
	{
		_static_engine = new FTEngine;
		if( ! _static_engine->init() )
		{
			delete _static_engine;
			_static_engine = nullptr;
		}
	}
  return _static_engine;
}

#endif	//#if AAA_TRUETYPE1_USE()
