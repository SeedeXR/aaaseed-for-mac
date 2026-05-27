
#ifdef AAA_FFGLPluginInstance_H
#error "FFGLPluginInstance_H included more than once."
#endif
#define AAA_FFGLPluginInstance_H 1


//todo 2025 November FFGL version too old (32 bits only) need to be updated
#define AAA_USE_FFGL() 0

#if AAA_USE_FFGL()

#ifndef	AAA_AAA_GL_H
#	include "draw/aaa_gl.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef __FFGL_H__
#	include <FFGL/FFGL.h>
#endif

class FFGLPluginInstance
{
protected:
	FF_Main_FuncPtr m_ffPluginMain;

	//many plugins will return 0x00000000 as the first valid instance,
	//so we use 0xFFFFFFFF to represent an uninitialized/invalid instance
	enum { INVALIDINSTANCE=0xFFFFFFFF };
	DWORD	m_ffInstanceID;

	enum { MAX_PARAMETERS = 64 };
	int		m_numParameters;  
	char	*m_paramNames[ MAX_PARAMETERS ];

	bool	_b_opengl;
	bool	_b_set_time;
	bool	_b_framecopy;
	o_str	_plugin_name;
	o_str	_plugin_version;
	INT32	_plugin_type;
	o_str	_plugin_copy;

	INT32	_input_min;
	INT32	_input_max;


	//helper methods calls plugMain(FF_INITIALISE) and gets the parameter names
	DWORD	InitPluginLibrary();

	//calls DeletePluginInstance if needed, calls ReleaseParamNames, then calls plugMain(FF_DEINITIALISE)
	DWORD	DeinitPluginLibrary();

	void	SetParameterName( int paramNum, const char *srcString );
	void	ReleaseParamNames();
	bool	GetCaps( int cap );
	INT32	GetCapsInt( int cap );
	void	get_info();

public:
	//each platform implements this and returns a class that derives from FFGLPluginInstance
	//(that class implements the real Load and Unload methods which by default return FF_FAIL below)
	static FFGLPluginInstance *New();

	FFGLPluginInstance();

	//these methods are virtual because each platform implements dynamic libraries differently
	virtual DWORD	Load( const char *filename )	{ return FF_FAIL; }

	//calls plugMain(FF_INSTANTIATEGL) and assigns each parameter its default value
			DWORD	InstantiateGL( const FFGLViewportStruct *vp );

	//calls plugMain(FF_DEINSTANTIATEGL)
			DWORD	DeInstantiate();
			DWORD	Instantiate();
			DWORD	Instantiate( VideoInfoStructTag	&video_info );


			void	GetCapsPlugin();

	virtual DWORD	Unload()		{ return FF_FAIL; }

	//these methods are shared by the platform-specific implementations
	const	char*	GetParameterName( int paramNum );
			float	GetFloatParameter( int paramNum );
			void	SetFloatParameter( int paramNum, float value );
			void	SetTime( double curTime );

			DWORD	CallProcessOpenGL( ProcessOpenGLStructTag &t );
			DWORD	CallProcessCopy( const ProcessFrameCopyStruct& pfcs );
			DWORD	CallProcess( LPVOID Frame );

			bool	is_opengl()		{ return _b_opengl; }
			bool	is_set_time()	{ return _b_set_time; }
			o_str	get_name()		{ return _plugin_name; }
			INT32	get_type()		{ return _plugin_type; }
			o_str	get_version()	{ return _plugin_version; }
			INT32	get_input_min()	{ return _input_min; }
			INT32	get_input_max()	{ return _input_max; }
			bool	is_frame_copy()	{ return _b_framecopy; }

	virtual ~FFGLPluginInstance();

};

#endif //#if AAA_USE_FFGL()