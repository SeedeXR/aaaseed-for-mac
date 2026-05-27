
#include "bdd_ffgl.h"

#if AAA_USE_FFGL()

#include "draw/tex.h"
#include "infrastructure/viewport.h"
#include "image/img.h"
#include "image/bind_img_2d.h"
#include "infrastructure/layer/layers.h"
#include "fbo/fbo.h"


#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

FACTORY_CREATE_PROP_V1( c_bdd_ffgl, bdd_ffgl, FreeFrame Plugin, FreeFrame, sub_menu="Special"; );

namespace n_bdd_ffgl
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PLUGIN_PARAM_NB	= 16;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;	
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	PLUGIN_PARAM_NB * 2
							//	+	INT_PARAM_NB
							//	+	REAL_PARAM_NB
								+	GROUP_PARAM_NB;
#define	PARAM_DEF_PLUGIN( nb )\
	PARAM_DEF_STR_LOCKED( plugin_param_name_##nb## )\
	PARAM_DEF_REAL_ZERO_ONE( plugin_param_value_##nb## )
			
	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_REF(				plugin_filename )
		PARAM_DEF_STR_LOCKED(		plugin_name )
		PARAM_DEF_STR_LOCKED(		plugin_version )
		PARAM_DEF_STR_LOCKED(		plugin_type )
		PARAM_DEF_STR_LOCKED(		plugin_cap )
		PARAM_DEF_INT32_LOCKED(		plugin_input_min )
		PARAM_DEF_INT32_LOCKED(		plugin_input_max )
		PARAM_DEF_BOOL_LOCKED(		plugin_set_time )
		PARAM_DEF_BIND_2D_ALONE(	frame_dst )

		PARAM_DEF_GROUP_CLOSED( Plugin_param, PLUGIN_PARAM_NB * 2 )
			PARAM_DEF_PLUGIN( 01 )
			PARAM_DEF_PLUGIN( 02 )
			PARAM_DEF_PLUGIN( 03 )
			PARAM_DEF_PLUGIN( 04 )
			PARAM_DEF_PLUGIN( 05 )
			PARAM_DEF_PLUGIN( 06 )
			PARAM_DEF_PLUGIN( 07 )
			PARAM_DEF_PLUGIN( 08 )
			PARAM_DEF_PLUGIN( 09 )
			PARAM_DEF_PLUGIN( 10 )
			PARAM_DEF_PLUGIN( 11 )
			PARAM_DEF_PLUGIN( 12 )
			PARAM_DEF_PLUGIN( 13 )
			PARAM_DEF_PLUGIN( 14 )
			PARAM_DEF_PLUGIN( 15 )
			PARAM_DEF_PLUGIN( 16 )
	};
}


void	c_bdd_ffgl::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _o_plugin_filename );
	param_set_pt( h, _o_plugin_name );
	param_set_pt( h, _o_plugin_version );
	param_set_pt( h, _o_plugin_type );
	param_set_pt( h, _o_plugin_cap );
	param_set_pt( h, _input_min );
	param_set_pt( h, _input_max );
	param_set_pt( h, _b_set_time );
	param_set_pt( h, _frame_dst );

	++h;
		for( INT32 i = 0; i < 16; ++i )
		{
			param_set_pt( h, _o_param_name[ i ] );
			param_set_pt( h, _param_value_ui[ i ] );
		}

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_bdd_ffgl)
	,_input_min(0)
	,_input_max(0)
{
	_plugin = nullptr;
	_p_input_frame = nullptr;
	_b_loaded = false;
	_b_init = false;
	param_init_with( n_bdd_ffgl::param, n_bdd_ffgl::PARAM_NB );
}

c_bdd_ffgl::~c_bdd_ffgl()
{
	if( _b_loaded )
	{
		unload_plugin();
		// call deinit function in dll
	//	pf_dealloc();
	}

}

void	c_bdd_ffgl::unload_plugin()
{
	if ( _plugin )
	{
		_plugin->DeInstantiate();
		_plugin->Unload();
		SAFE_DELETE( _plugin );
	}
}

void	c_bdd_ffgl::populate_params()
{
	// get param name from plugin
	if ( _plugin )
	{
		for( INT32 i = 0; i < 16; ++i )
		{
			_o_param_name[ i ] = _plugin->GetParameterName( i );
			_param_value[ i ] = _plugin->GetFloatParameter( i );
			_param_value_ui[ i ] = _param_value[ i ];
		}
	}

}
bool	c_bdd_ffgl::load_plugin()
{
	// empty string don't bother
	if( _o_plugin_filename.get_len() == 0 )	
		return false;

	//load first plugin dll (*DOES NOT INSTANTIATE*)
	_plugin = FFGLPluginInstance::New();
	if( _plugin->Load( _o_plugin_filename.get() )==FF_FAIL )
	{
		ERR_PRINT_STRING( "FFGL : could not load plugin %s",  _o_plugin_filename.get() );
		return false;
	}
//	_plugin->GetCapsPlugin();

	populate_params();
	_o_plugin_name = _plugin->get_name();
	_o_plugin_version = _plugin->get_version();
	switch ( _plugin->get_type() )
	{
	case 0 :	_o_plugin_type.set( "Effect" );		break;
	case 1 :	_o_plugin_type.set( "Source" );		break;
	default:	_o_plugin_type.set( "Unknown" );	break;
	}
	if ( _plugin->is_opengl() )
		_o_plugin_cap.set( "GPU" );
	else
		_o_plugin_cap.set( "CPU" );
	_b_set_time = _plugin->is_set_time();
	_input_min = _plugin->get_input_min();
	_input_max = _plugin->get_input_max();

	//if (  !_plugin->is_opengl() )
	//{
	//	_plugin->Instantiate();
	//}
		
	return true;
}

void	c_bdd_ffgl::update()
{
	if( is_active() )
	{
		if( !_b_loaded )
			_b_loaded = load_plugin();
		if( _b_loaded )
		{
			if( _plugin )
			{
				bool _b_param_changed = false;
				for( INT32 i = 0; i < 16; ++i )
				{
					if( _param_value[ i ] != _param_value_ui[ i ] )
					{
						_plugin->SetFloatParameter( i, _param_value_ui[ i ] );
						_param_value[ i ] = _param_value_ui[ i ];
						_b_param_changed = true;
					}
				}

				if( _plugin->is_opengl() )
				{
					//instantiate the first plugin with the FBO viewport
					FFGLViewportStruct plugin_viewport;
					c_viewport*	viewport	=	c_viewport::get_cur();
					plugin_viewport.x		=	viewport->get_x();
					plugin_viewport.y		=	viewport->get_y();
					plugin_viewport.width	=	viewport->get_sx();
					plugin_viewport.height	=	viewport->get_sy();

					
					if( _plugin->InstantiateGL(&plugin_viewport) != FF_SUCCESS )
						ERR_PRINT_STRING( "FFGL plugin instantiate failed" );

				}
				else
				{
					if( _plugin->is_frame_copy() )
					{
						//// plugin supports frame copy
						//for (int i = 0; i < NumInputs; ++i ) {	// for each input
						//	int	InpIdx = plug.GetInput(i);
						//	if (InpIdx >= 0)	// if non-default input, use its frame
						//		m_InpFrame[i] = frm->GetPlugin(InpIdx).GetFrame();
						//	else {	// use default input: copy of current frame
						//		memcpy(m_DefInpFrame, sd.lpSurface, m_PlugFrameBytes);
						//		m_InpFrame[i] = m_DefInpFrame;
						//	}
						//}
						c_img_2d*	src_img = g_bind_img_2d->get_cur();
						bool	b_ok = false;
						if( src_img )
						{
							if( src_img->is_changed() || _b_param_changed )
							{
								UINT8*	p_data = src_img->get_data_uint8();
								UINT64	size = src_img->compute_data_size();
								if( _p_input_frame )
									SAFE_DELETE_ARRAY( _p_input_frame );

								_p_input_frame = new UINT8[ size ];
								if( p_data )
								{
									MEMCPY( _p_input_frame, p_data, size, __FUNCTION__ );	// too much depending on alignment
									m_InpFrame[ 0 ] = _p_input_frame;
									b_ok = true;
								}
							}
						}
						if( b_ok )
						{
							c_img_2d*	target_img = nullptr;
							target_img = g_bind_img_2d->get_always( _frame_dst );
							target_img->img_init_from_img( src_img, __FUNCTION__ );
							if( _p_input_frame )
							{

								VideoInfoStructTag	video_info;
								video_info.BitDepth = 1;
								video_info.FrameHeight = src_img->get_size_y();
								video_info.FrameWidth = src_img->get_size_x();
								video_info.Orientation = 0;

								_plugin->Instantiate( video_info );

								ProcessFrameCopyStruct	pfcs;
								pfcs.ppInputFrames = m_InpFrame;
								pfcs.numInputFrames = 1;
								pfcs.pOutputFrame = target_img->get_data_uint8();
								if( pfcs.pOutputFrame )
									_plugin->CallProcessCopy( pfcs );

								target_img->erase_filename();
								target_img->set_changed();	
							}
						}
					}
					else
					{
						//plugin does not support frame copy

					}
				}
			}
		}
	}
}


void	c_bdd_ffgl::draw_low_gl()
{
	//prepare the structure used to call
	//the plugin's ProcessOpenGL method
	ProcessOpenGLStructTag processStruct;

	//create the array of OpenGLTextureStruct * to be passed to the plugin
	FFGLTextureStruct *inputTextures[1];
	//	inputTextures[0] = &aviTexture;

	//fill the OpenGLTextureStruct
	FFGLTextureStruct t;
	c_img_2d*	src_img = nullptr;
	src_img =	g_bind_img_2d->get_cur();

	if( src_img )
	{
		t.Handle = tex2d.get_name_gl( g_bind_img_2d->get_ui_index() );

		t.Width = src_img->get_size_x();
		t.Height = src_img->get_size_y();

		t.HardwareWidth = src_img->get_size_x();
		t.HardwareHeight = src_img->get_size_y();
	}
	else
	{
		t.Handle = 0;

		t.Width = 0;
		t.Height = 0;

		t.HardwareWidth = 0;
		t.HardwareHeight = 0;
	}

	inputTextures[0] = &t;

	if( _plugin->get_input_min() > 0 )
	{
		//provide our 1 input texture
		processStruct.numInputTextures = 1;
		processStruct.inputTextures = inputTextures;
	}
	else
	{
		processStruct.numInputTextures = 0;
	}

	processStruct.HostFBO = 0;
	if( c_layers::get_cur()->get_fbo() != nullptr )
	{
		c_fbo*	fbo = c_layers::get_cur()->get_fbo();
		if( fbo )
		{
			processStruct.HostFBO = fbo->get_id_used();
		}
	}
	_plugin->CallProcessOpenGL( processStruct );
}

void	c_bdd_ffgl::draw()
{
	if( is_active() && _plugin )
	{
		if ( _plugin->is_opengl() )
		{
			draw_low_gl();
		}
	}
}


FFGLPluginInstance *FFGLPluginInstance::New()
{
	return new c_aaa_plugin_instance();
}

c_aaa_plugin_instance::c_aaa_plugin_instance()
:m_ffModule(nullptr)
{}

DWORD c_aaa_plugin_instance::Load( CONST char *fname )
{
	//warning_printf("FreeFrame Plugin Load Failed: %s", fname);		
	if( !fname || fname[0] == 0 )	{ return FF_FAIL; }

	Unload();

	wchar_t * wch = sysutils::utf8_to_unicode( fname );
	m_ffModule = LoadLibraryW( wch );
	sysutils::free_str_tmp( wch );

	if( !m_ffModule )				{ return FF_FAIL; }

	FF_Main_FuncPtr pFreeFrameMain = (FF_Main_FuncPtr)GetProcAddress( m_ffModule, "plugMain" );

	if( !pFreeFrameMain )
	{
		FreeLibrary( m_ffModule );
		m_ffModule = nullptr;
		return FF_FAIL;
	}

	m_ffPluginMain = pFreeFrameMain;
	return  InitPluginLibrary();
}

DWORD c_aaa_plugin_instance::Unload()
{
	if ( m_ffInstanceID != INVALIDINSTANCE )
	{
		//we can't call DeInstantiate because we must
		//guarantee an active OpenGL context
		//DeInstantiateGL();
		ERR_PRINT_STRING( "Failed to call DeInstantiateGL() before calling Unload()" );
		return FF_FAIL;
	}

	DeinitPluginLibrary();

	if( m_ffModule )
	{
		FreeLibrary( m_ffModule );
		m_ffModule = nullptr;
	}

	return FF_SUCCESS;
}

c_aaa_plugin_instance::~c_aaa_plugin_instance()
{
	if( m_ffModule )
	{
		ERR_PRINT_STRING( "FFGL plugin deleted without calling Unload()" );
	}
}

#endif	//#if AAA_USE_FFGL()