#ifndef AAA_FFGLPluginInstance_H
#	include "FFGLPluginInstance.h"
#endif

#if AAA_USE_FFGL()

#include <stdio.h>
#include "err.h"


//Free Frame PlugIn

void FFDebugMessage( const char* _msg )
{
	//char msg[2048];
	//_snprintf(msg,sizeof(msg),"%s\n",_msg);

	//OutputDebugString(msg);
	ERR_PRINT_STRING( "FreeFrame : %s", _msg );
}

FFGLPluginInstance:: FFGLPluginInstance()
:m_ffPluginMain(nullptr),
 m_ffInstanceID(INVALIDINSTANCE),
 m_numParameters(0)
{
	for( int i=0; i<MAX_PARAMETERS; ++i )
		m_paramNames[i] = nullptr;
}

FFGLPluginInstance::~FFGLPluginInstance()
{
	if( m_ffPluginMain || m_ffInstanceID!=INVALIDINSTANCE )
	{
		FFDebugMessage("FFGLPluginInstance deleted without calling Unload()");
	}
}

void FFGLPluginInstance::SetParameterName(int paramNum, const char *name)
{
	if( paramNum<0 || paramNum>=MAX_PARAMETERS )
	  return;

	int i = paramNum;

	if( m_paramNames[i] )
	  delete m_paramNames[i];

	m_paramNames[i] = new char[strlen(name)+1];
	strcpy(m_paramNames[i], name);
}

const char *FFGLPluginInstance::GetParameterName(int paramNum)
{
	if( paramNum<0 || paramNum>=MAX_PARAMETERS )
		return "";

	if( m_paramNames[paramNum] )
		return m_paramNames[paramNum];

	return "";
}

void FFGLPluginInstance::SetFloatParameter(int paramNum, float value)
{
	if( paramNum<0 || paramNum>=m_numParameters || m_ffInstanceID==INVALIDINSTANCE || !m_ffPluginMain )
	{
		//the parameter or the plugin doesn't exist
		return;
	}

	//make sure its a float parameter type
	DWORD ffParameterType = m_ffPluginMain(FF_GETPARAMETERTYPE,(DWORD)paramNum,0).ivalue;
	if( ffParameterType!=FF_TYPE_TEXT )
	{
		SetParameterStruct ArgStruct;
		ArgStruct.ParameterNumber = paramNum;

		//be careful with this cast.. ArgStruct.NewParameterValue is DWORD
		//for this to compile correctly, sizeof(DWORD) must == sizeof(float)
		//*((float *)(unsigned)&ArgStruct.NewParameterValue) = value;
//maa did this instead
		static_assert( sizeof(DWORD) == sizeof(float), "size of DWORD and float need to be the same " );
		*((float *)&ArgStruct.NewParameterValue) = value;

//maa 2024 July this look wrong for x64
		m_ffPluginMain( FF_SETPARAMETER, (DWORD)(&ArgStruct), m_ffInstanceID );
	}
}

void FFGLPluginInstance::SetTime(double curTime)
{
	if( m_ffInstanceID==INVALIDINSTANCE || !m_ffPluginMain )
	{
		FFDebugMessage("Invalid SetTime call");
		return;
	}
//maa 2024 July this look wrong for x64
	m_ffPluginMain(FF_SETTIME, (DWORD)(&curTime), m_ffInstanceID);
}

float FFGLPluginInstance::GetFloatParameter(int paramNum)
{
	if( paramNum<0 || paramNum>=m_numParameters || m_ffInstanceID==INVALIDINSTANCE || !m_ffPluginMain )
	{
		FFDebugMessage("Invalid GetFloatParameter call");
		return 0.f;
	}

	//make sure its a float parameter type
	DWORD ffParameterType = m_ffPluginMain(FF_GETPARAMETERTYPE,(DWORD)paramNum,0).ivalue;
	if( ffParameterType!=FF_TYPE_TEXT )
	{
		plugMainUnion result = m_ffPluginMain(FF_GETPARAMETER,(DWORD)paramNum, m_ffInstanceID);

		//make sure the call to get the parameter succeeded before
		//reading the float value
		if( result.ivalue!=FF_FAIL )
		{
			return result.fvalue;
		}
	}

	return 0.f;
}

DWORD FFGLPluginInstance::CallProcessOpenGL(ProcessOpenGLStructTag &t)
{
	//make sure we have code to call otherwise return the unprocessed input
	if( !m_ffPluginMain || m_ffInstanceID==INVALIDINSTANCE )
	{
		FFDebugMessage("Invalid CallProcessOpenGL call");
		return FF_FAIL;
	}

	DWORD retVal = FF_FAIL;

	try
	{
//maa 2024 July this look wrong for x64
		retVal = m_ffPluginMain(FF_PROCESSOPENGL, (DWORD)&t, m_ffInstanceID).ivalue;
	}
	catch (...)
	{
		FFDebugMessage("Error on call to FreeFrame::ProcessOpenGL");
		retVal = FF_FAIL;
	}

	return retVal;
}

DWORD FFGLPluginInstance::CallProcess( LPVOID Frame )
{
	//make sure we have code to call otherwise return the unprocessed input
	if( !m_ffPluginMain || m_ffInstanceID==INVALIDINSTANCE )
	{
		FFDebugMessage("Invalid CallProcessOpenGL call");
		return FF_FAIL;
	}

	DWORD retVal = FF_FAIL;

	try
	{
//maa 2024 July this look wrong for x64
		retVal = m_ffPluginMain( FF_PROCESSFRAME, (DWORD)Frame, m_ffInstanceID ).ivalue;
	}
	catch (...)
	{
		FFDebugMessage("Error on call to FreeFrame::ProcessFrame");
		retVal = FF_FAIL;
	}
	return retVal;
}

DWORD FFGLPluginInstance::CallProcessCopy( const ProcessFrameCopyStruct& pfcs )
{
	//make sure we have code to call otherwise return the unprocessed input
	if( !m_ffPluginMain || m_ffInstanceID == INVALIDINSTANCE )
	{
		FFDebugMessage( "Invalid CallProcessCopy call" );
		return FF_FAIL;
	}

	DWORD retVal = FF_FAIL;

	try
	{
//maa 2024 July this look wrong for x64
		retVal = m_ffPluginMain( FF_PROCESSFRAMECOPY, DWORD(&pfcs), m_ffInstanceID ).ivalue;
	}
	catch (...)
	{
		FFDebugMessage("Error on call to FreeFrame::ProcessFrameCopy");
		retVal = FF_FAIL;
	}

	return retVal;
}

void FFGLPluginInstance::ReleaseParamNames()
{
	for( int i=0; i<MAX_PARAMETERS; ++i )
	{
		if( m_paramNames[i] )
		{
			SAFE_DELETE_ARRAY( m_paramNames[i] );
		}
	}

	m_numParameters = 0;
}

DWORD FFGLPluginInstance::InitPluginLibrary()
{
	DWORD rval = FF_FAIL;

	if( !m_ffPluginMain )
		return rval;

	//initialize the plugin
	rval = m_ffPluginMain(FF_INITIALISE,0,0).ivalue;
	if( rval!=FF_SUCCESS )
		return rval;

	get_info();
	// get capabilities
	GetCapsPlugin();

	//get the parameter names
	m_numParameters = (int)m_ffPluginMain(FF_GETNUMPARAMETERS, 0, 0).ivalue;

	for( int i=0; i<m_numParameters; ++i )
	{
		plugMainUnion result = m_ffPluginMain(FF_GETPARAMETERNAME,(DWORD)i,0);

		if( result.ivalue!=FF_FAIL && result.svalue )
		{
			  //create a temporary copy as a cstring w/null termination
			char newParamName[32];

			const char *c = result.svalue;
			char *t = newParamName;

			//FreeFrame spec defines parameter names to be 16 characters long MAX
			int numChars = 0;
			while( *c && numChars<16 )
			{
				*t = *c;

				t++;
				c++;

				numChars++;
			}

			//make sure there's a null at the end
			*t = 0;

			SetParameterName(i, newParamName);
		}
		else
		{
			SetParameterName(i, "Untitled");
		}
	}

	return FF_SUCCESS;
}

DWORD FFGLPluginInstance::InstantiateGL(const FFGLViewportStruct *viewport)
{
	if( m_ffInstanceID != INVALIDINSTANCE || !_b_opengl )
	{
		//already instantiated
		return FF_SUCCESS;
	}

	//instantiate 1 of the plugins
//maa 2024 July this look wrong for x64
	m_ffInstanceID = m_ffPluginMain( FF_INSTANTIATEGL, (DWORD)viewport, 0).ivalue;

	//if it instantiated ok, return success
	if( m_ffInstanceID==INVALIDINSTANCE )
		return FF_FAIL;

	//make default param assignments
	for( int i = 0; i < MAX_PARAMETERS && i < m_numParameters; ++i )
	{
		plugMainUnion result = m_ffPluginMain(FF_GETPARAMETERDEFAULT,(DWORD)i,0);
		if( result.ivalue!=FF_FAIL )
		{
			SetFloatParameter(i,result.fvalue);
		}
	}

	return FF_SUCCESS;
}

DWORD FFGLPluginInstance::Instantiate( VideoInfoStructTag	&video_info )
{
	if( m_ffInstanceID != INVALIDINSTANCE )
	{
		//already instantiated
		return FF_SUCCESS;
	}

	//instantiate 1 of the plugins
//maa 2024 July this look wrong for x64
	m_ffInstanceID = m_ffPluginMain( FF_INSTANTIATE, (DWORD)(&video_info), 0).ivalue;

	//if it instantiated ok, return success
	if( m_ffInstanceID==INVALIDINSTANCE )
		return FF_FAIL;

	//make default param assignments
	for( int i=0; i < MAX_PARAMETERS && i < m_numParameters; ++i )
	{
		plugMainUnion result = m_ffPluginMain( FF_GETPARAMETERDEFAULT, (DWORD)i, 0 );
		if( result.ivalue != FF_FAIL )
		{
			SetFloatParameter( i, result.fvalue );
		}
	}
	return FF_SUCCESS;
}

DWORD FFGLPluginInstance::DeInstantiate()
{
	if( m_ffInstanceID == INVALIDINSTANCE )
	{
		//already deleted
		return FF_SUCCESS;
	}

	if( !m_ffPluginMain )
	{
		//no main function available to call deinstantiate.. failure
		m_ffInstanceID = INVALIDINSTANCE;
		return FF_FAIL;
	}

	DWORD rval = FF_FAIL;

	try
	{
		rval = m_ffPluginMain( _b_opengl ? FF_DEINSTANTIATEGL : FF_DEINSTANTIATE, 0, (DWORD)m_ffInstanceID ).ivalue;
	}
	catch (...)
	{
		FFDebugMessage("FreeFrame Exception on DEINSTANTIATE");
	}

	m_ffInstanceID = INVALIDINSTANCE;
	return rval;
}

DWORD FFGLPluginInstance::DeinitPluginLibrary()
{
	if( m_ffInstanceID != INVALIDINSTANCE )
	{
		//we can't call DeInstantiate because we must guarantee an active OpenGL context
		FFDebugMessage( "Failed to call DeInstantiateGL() before calling DeinitPluginLibrary()" );
		return FF_FAIL;
	}

	ReleaseParamNames();

	DWORD rval = FF_FAIL;

	if( m_ffPluginMain )
	{
		rval = m_ffPluginMain( FF_DEINITIALISE, 0, 0 ).ivalue;
		if( rval != FF_SUCCESS )
		{
			ERR_PRINT_STRING( "FFGL DeInit failed" );
		}
		m_ffPluginMain = nullptr;
	}

	return rval;
}

void	FFGLPluginInstance::get_info()
{
	if( m_ffPluginMain )
	{
		//DWORD rval = FF_FAIL;
		//PluginInfoStruct	info;
		try
		{
			plugMainUnion result = m_ffPluginMain( FF_GETINFO, 0, 0 );
			if( result.ivalue != FF_FAIL )
			{
				GOOD_PRINT_STRING( "whoa" );
				_plugin_name.set( (C_PCHAR)(&result.PISvalue->PluginName[0]) );
				_plugin_type = result.PISvalue->PluginType;
				_plugin_version.add( UINT32(result.PISvalue->APIMajorVersion) );
				_plugin_version.add_char( '.' );
				_plugin_version.add( UINT32(result.PISvalue->APIMinorVersion) );
			}
		}
		catch (...)
		{
			ERR_PRINT_STRING( "FFGL : error getting plugin info" );
		}
	}
}

bool FFGLPluginInstance::GetCaps( int cap )
{
	if( m_ffPluginMain )
	{
		DWORD rval = FF_FAIL;
		rval = m_ffPluginMain( FF_GETPLUGINCAPS, cap, 0 ).ivalue;
		if( rval == FF_SUPPORTED )
			return true;
	}
	return false;
}

INT32 FFGLPluginInstance::GetCapsInt( int cap )
{
	if( m_ffPluginMain )
	{
		DWORD rval = FF_FAIL;
		rval = m_ffPluginMain( FF_GETPLUGINCAPS, cap, 0 ).ivalue;
		if( rval != FF_FAIL )
		{
			return rval;
		}
	}
	return 0;
}

void FFGLPluginInstance::GetCapsPlugin()
{
	_b_opengl = false;
	_b_set_time = false;
	_b_framecopy = false;

	if( GetCaps( FF_CAP_16BITVIDEO ) )
		GOOD_PRINT_STRING( "FFGL supports 16 bit video" );
	if( GetCaps( FF_CAP_24BITVIDEO ) )
		GOOD_PRINT_STRING( "FFGL supports 24 bit video" );
	if( GetCaps( FF_CAP_32BITVIDEO ) )
		GOOD_PRINT_STRING( "FFGL supports 32 bit video" );

	_b_framecopy	= GetCaps( FF_CAP_PROCESSFRAMECOPY );
	if( _b_framecopy )
		GOOD_PRINT_STRING( "FFGL supports processFrameCopy" );

	_b_opengl		= GetCaps( FF_CAP_PROCESSOPENGL );
	if( _b_opengl )
		GOOD_PRINT_STRING( "FFGL supports processOpengl" );

	_b_set_time		= GetCaps( FF_CAP_SETTIME );
	if( _b_set_time )
		GOOD_PRINT_STRING( "FFGL supports setTime" );

	if( _b_opengl || _b_framecopy )
	{
		_input_min = GetCapsInt( FF_CAP_MINIMUMINPUTFRAMES );
		_input_max = GetCapsInt( FF_CAP_MAXIMUMINPUTFRAMES );
	}
	
	_plugin_copy.erase();
	if( _b_framecopy )
	{
		//Calls to this caps index are only meaningful if the plugin has reported that it supports processFrameCopy:
		INT32	copy = GetCapsInt( FF_CAP_COPYORINPLACE );
		switch ( copy )
		{
		case FF_CAP_PREFER_NONE:	_plugin_copy.set( "none" );		break;
		case FF_CAP_PREFER_INPLACE:	_plugin_copy.set( "in place" );	break;
		case FF_CAP_PREFER_COPY:	_plugin_copy.set( "copy" );		break;
		case FF_CAP_PREFER_BOTH:	_plugin_copy.set( "both" );		break;
		default:					_plugin_copy.set( "unknown" );	break;
		}
		GOOD_PRINT_STRING( "FFGL supports %s", _plugin_copy.get() );
	}
}

#endif	//#if AAA_USE_FFGL()