#include "draw/tex.h"
#include "image/bind_img_2d.h"
#include "image/bind_img.h"
#include "fbo/fbo.h"
#include "bdd_spout2.h"
#include "SpoutSDK/SpoutGL/SpoutUtils.h"

#if defined(WIN32)
//#	include <VersionHelpers.h>
#	include <d3dcommon.h>
#	include <d3d11.h>
#	include <dxgi1_2.h>
//#	include <DXGIDebug.h>		// to use D3D11 debug functions
#endif // WIN32

#ifndef AAA_LIB_USE_H
#	include <lib_use.h>
#endif
AAA_LIB_USE_MESSAGE( "dxgi" )

namespace{

	//IDXGIAdapter* get_d3d11_adapter( INT32 index )
	//{
	//	// Enum Adapters first : multiple video cards
	//	IDXGIFactory1*	_dxgi_factory1;
	//	//	if ( FAILED( CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), (void**)&dxgi_device_ptr ) ) )
	//	if ( FAILED( CreateDXGIFactory1( __uuidof(IDXGIFactory1), (void**)&_dxgi_factory1 ) ) )
	//	{
	//		err_print( "Could not create CreateDXGIFactory1" );
	//	}
	//
	//	UINT32	i = 0;
	//	IDXGIAdapter*	adapter1_ptr = nullptr;
	//	for ( UINT32 i = 0; _dxgi_factory1->EnumAdapters( i, &adapter1_ptr ) != DXGI_ERROR_NOT_FOUND; i++ )
	//	{
	//		GOOD_PRINT_STRING( "bdd_spout : D3D11 Adapter %d found", i );
	//
	//		DXGI_ADAPTER_DESC	desc;
	//		adapter1_ptr->GetDesc( &desc );
	//		GOOD_PRINT_STRING( "Adapter : %S", desc.Description );
	//		GOOD_PRINT_STRING( "  Vendor Id : %d", desc.VendorId );
	//		GOOD_PRINT_STRING( "  Dedicated System Memory : %.0f MiB", (REAL)desc.DedicatedSystemMemory / (1024.f * 1024.f) );
	//		GOOD_PRINT_STRING( "  Dedicated Video Memory : %.0f MiB", (REAL)desc.DedicatedVideoMemory / (1024.f * 1024.f) );
	//		GOOD_PRINT_STRING( "  Shared System Memory : %.0f MiB", (REAL)desc.SharedSystemMemory / (1024.f * 1024.f) );
	//		CComPtr<IDXGIOutput>	p_output;
	//	//	IDXGIOutput*	p_output;
	//		for ( UINT32 j = 0; adapter1_ptr->EnumOutputs( j, &p_output ) != DXGI_ERROR_NOT_FOUND; j++ )
	//		{
	//			DXGI_OUTPUT_DESC	desc_out;
	//			p_output->GetDesc( &desc_out );
	//			GOOD_PRINT_STRING( "  Output : %d", j );
	//			GOOD_PRINT_STRING( "    Name %S", desc_out.DeviceName );
	//			GOOD_PRINT_STRING( "    Attached to desktop : %s", desc_out.AttachedToDesktop ? "yes" : "no" );
	//			//GOOD_PRINT_STRING( "    Rotation", desc_out.DeviceName );
	//			GOOD_PRINT_STRING( "    Left : %d", desc_out.DesktopCoordinates.left );
	//			GOOD_PRINT_STRING( "    Top : %d", desc_out.DesktopCoordinates.top );
	//			GOOD_PRINT_STRING( "    Right : %d", desc_out.DesktopCoordinates.right );
	//			GOOD_PRINT_STRING( "    Bottom : %d", desc_out.DesktopCoordinates.bottom );
	//			p_output.Release();
	//		}
	//		if( index == i )
	//		{
	//			return adapter1_ptr;
	//		}
	//		adapter1_ptr->Release();
	//		//	++i;
	//	}
	//	return nullptr;
	//}
	// 
	// 
	// these fns are done just for this spout2 class
	aaa::PIXEL_FORMAT convert_DX_format_to_aaa( DXGI_FORMAT dx_texture_format )
	{
		aaa::PIXEL_FORMAT pixel_format;
		switch( dx_texture_format )
		{
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				pixel_format = aaa::PIXEL_FORMAT::BGRA_8;
				break;
			//case DXGI_FORMAT_R10G10B10A2_UNORM:
			//	glformat = GL_RGB10_A2;
			//	break;
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
				pixel_format = aaa::PIXEL_FORMAT::RGBA_16;
				break;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				pixel_format = aaa::PIXEL_FORMAT::RGBA_16FP;
				break;
			case DXGI_FORMAT_R32_FLOAT:				pixel_format = aaa::PIXEL_FORMAT::R_32FP;		break;
			case DXGI_FORMAT_R32G32_FLOAT:			pixel_format = aaa::PIXEL_FORMAT::RG_32FP;		break;
			case DXGI_FORMAT_R32G32B32_FLOAT:		pixel_format = aaa::PIXEL_FORMAT::RGB_32FP;		break;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:	pixel_format = aaa::PIXEL_FORMAT::RGBA_32FP;	break;
			// DirectX 9
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
			// DirectX 11
			case DXGI_FORMAT_B8G8R8X8_UNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			default:
				pixel_format = aaa::PIXEL_FORMAT::RGBA_8;
				break;
		}
		return pixel_format;
	}

	DXGI_FORMAT convert_GL_format_to_DX( GLenum gl_format )
	{
		DXGI_FORMAT dx_format = DXGI_FORMAT_UNKNOWN;
		switch( gl_format )
		{
		//case GL_RGB10_A2:
		//	dx_format = DXGI_FORMAT_R10G10B10A2_UNORM;
		//	break;
		case GL_R16:
		case GL_RG16:
		case GL_RGB16:
		case GL_RGBA16:		dx_format = DXGI_FORMAT_R16G16B16A16_UNORM;	break;	// DXGI_FORMAT_R16G16B16A16_SNORM
					
		case GL_R16F:
		case GL_RG16F:
		case GL_RGB16F:
		case GL_RGBA16F:	dx_format = DXGI_FORMAT_R16G16B16A16_FLOAT;	break;

		case GL_R32F:		//dx_format = DXGI_FORMAT_R32_FLOAT;		break;
		case GL_RG32F:		//dx_format = DXGI_FORMAT_R32G32_FLOAT;		break;
		case GL_RGB32F:		//dx_format = DXGI_FORMAT_R32G32B32_FLOAT;	break;
		case GL_RGBA32F:	dx_format = DXGI_FORMAT_R32G32B32A32_FLOAT;	break;
			//// DirectX 9
			//case D3DFMT_A8R8G8B8:
			//case D3DFMT_X8R8G8B8:
			//// DirectX 11
			//case DXGI_FORMAT_B8G8R8X8_UNORM:
			//case DXGI_FORMAT_B8G8R8A8_UNORM:
		case GL_R8:
		case GL_RG8:
		case GL_RGB8:
		case GL_RGBA8:
		default:
			dx_format = DXGI_FORMAT_B8G8R8A8_UNORM;
			break;
		}
		return dx_format;
	}
}

namespace spoututils
{
extern bool gb_spoutsdk_verbose; // added to the spout sdk to see the messages
}

FACTORY_CREATE_PROP_V1( c_bdd_spout2, bdd_spout2, Spout2, spout2, sub_menu="Special"; );

namespace n_bdd_spout2
{
	CONSTEXPR UINT32 BASE_PARAM_NB				= 4 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR UINT32 RECEIVE_SENDERS_PARAM_NB	= 2 + c_bdd_spout2::SRC_UI_NB;
	CONSTEXPR UINT32 TEK_PARAM_NB				= 10;
	CONSTEXPR UINT32 SEND_PARAM_NB				= 6;
	CONSTEXPR UINT32 RECEIVE_PARAM_NB			= 7;
	CONSTEXPR UINT32 GROUP_NB					= 4;
	CONSTEXPR UINT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TEK_PARAM_NB
									+	RECEIVE_PARAM_NB
									+	RECEIVE_SENDERS_PARAM_NB
									+	SEND_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(		enabled	)

		//PARAM_DEF_GROUP(			Tek, TEK_PARAM_NB )
		PARAM_DEF_INT32_POS_ZERO(	gpu_adapter				)

		PARAM_DEF_GROUP_CLOSED(		TEK, TEK_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	version_sdk				)
			PARAM_DEF_STR_LOCKED(   version_spout			)
			PARAM_DEF_BOOL_OFF(		verbose					)
			PARAM_DEF_BOOL_OFF(		verbose_sdk				)
			PARAM_DEF_BOOL_OFF(		receiver_dx9_asked		)
			PARAM_DEF_BOOL_LOCKED(	receiver_dx9_use		)
			PARAM_DEF_BOOL_LOCKED(	receiver_memory_share	)
			PARAM_DEF_BOOL_OFF(		sender_dx9_asked		)
			PARAM_DEF_BOOL_LOCKED(	sender_dx9_used			)
			PARAM_DEF_BOOL_LOCKED(	sender_memory_share		)

		PARAM_DEF_GROUP(		In, RECEIVE_PARAM_NB + RECEIVE_SENDERS_PARAM_NB + 1 )
			PARAM_DEF_GROUP_CLOSED(	Receiver senders , RECEIVE_SENDERS_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		receiver_sender_enum_trig	)
				PARAM_DEF_INT32_LOCKED(	receiver_sender_count		)
				PARAM_DEF_0_15(			src,						PARAM_DEF_STR_LOCKED	)
			PARAM_DEF_BOOL_ON(				receive					)
			PARAM_DEF_UINT32_ZERO(			receiver_index			)
			PARAM_DEF_STR_LOCKED(			receiver_name			)
			PARAM_DEF_BOOL_OFF(				receiver_vertical_flip	)
			PARAM_DEF_BOOL_OFF(				receive_image			)
			PARAM_DEF_BOOL_LOCKED(			receiving				)
			PARAM_DEF_INT32_ZERO_SAVE_NOT(	receiving_count			)

		PARAM_DEF_GROUP(		Out, SEND_PARAM_NB )
			PARAM_DEF_BOOL_ON(				send					)
			PARAM_DEF_REF(					sender_name				)
			PARAM_DEF_BOOL_OFF(				sender_vertical_flip	)
			PARAM_DEF_BOOL_OFF(				send_image				)
			PARAM_DEF_BOOL_LOCKED(			sending					)
			PARAM_DEF_INT32_ZERO_SAVE_NOT(	sending_count			)

		PARAM_DEF_INT32_LOCKED_XY(	size	)
	};
}

//todo make param comment so object is readable even uis closed
void	c_bdd_spout2::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt(	h,	_b_enable_ui	);
	param_set_pt(	h,	_gpu_adapter_ui	);
	++h;
		param_set_pt(	h,	_o_version_sdk				);
		param_set_pt(	h,	_o_version_spout			);
		param_set_pt(	h,	_b_verbose_ui				);
		param_set_pt(	h,	gb_spoutsdk_verbose			);
		param_set_pt(	h,	_b_receiver_dx9_asked_ui	);
		param_set_pt(	h,	_b_receiver_dx9_used		);
		param_set_pt(	h,	_b_receiver_memory_share	);
		param_set_pt(	h,	_b_sender_dx9_asked_ui		);
		param_set_pt(	h,	_b_sender_dx9_used			);
		param_set_pt(	h,	_b_sender_memory_share		);
	++h;
		++h;
			param_set_pt(	h,	_b_receiver_sender_enum_trig	);
			param_set_pt(	h,	_receiver_sender_count			);
			for( INT32 i=0; i<SRC_UI_NB; ++i )
				param_set_pt(	h,	&_src_name[i]		);
		param_set_pt(	h,	_b_receive_ui					);
		param_set_pt(	h,	_receiver_index_ui				);
		param_set_pt(	h,	_receiver_name_ui				);
		param_set_pt(	h,	_b_receiver_flip_vertical_ui	);
		param_set_pt(	h,	_b_receive_img_ui				);
		param_set_pt(	h,	_b_receiving					);
		param_set_pt(	h,	_receiving_count_out			);
	++h;
		param_set_pt(	h,	_b_send_ui					);
		param_set_pt(	h,	_sender_name_ui				);
		param_set_pt(	h,	_b_sender_flip_vertical_ui	);
		param_set_pt(	h,	_b_send_img_ui				);
		param_set_pt(	h,	_b_sending					);
		param_set_pt(	h,	_sending_count_out			);
	param_set_pt(	h,	_size_x	);
	param_set_pt(	h,	_size_y	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_spout2)
	,_size_x						(	0		)
	,_size_y						(	0		)
	,_receiver						(	nullptr )
	,_b_receive						(	false	)
	,_b_receiving					(	false	)
	,_receiving_count_out			(	0		)
	,_b_receiver_dx9_used			(	false	)
	,_b_receiver_memory_share		(	false	)
	,_b_receiver_init				(	false	)

	,_receiver_index				(	-1		)

	,_sender						(	nullptr )
	,_b_send						(	false	)
	,_b_sending						(	false	)
	,_sending_count_out				(	0		)
	,_b_sender_dx9_used				(	false	)
	,_b_sender_memory_share			(	false	)
	,_b_sender_init					(	false	)

	,_receiver_sender_count			(	0		)
	,_receiver_sender_count_done	(	0		)
	//,_dx11_adapter				(	nullptr	)
{
	_o_version_sdk.set( spoututils::GetSDKversion().c_str() );
	_o_version_spout.set( spoututils::GetSpoutVersion().c_str() );

	param_init_with( n_bdd_spout2::param, n_bdd_spout2::PARAM_NB_MAX );
}

c_bdd_spout2::~c_bdd_spout2()
{
	close_receiver();	
	close_sender();
	
	SAFE_DELETE( _receiver );
	SAFE_DELETE( _sender );
}

void	c_bdd_spout2::check( C_PCHAR_C str )
{
	mem::check(str);
}

#if 1
#	define CHECK( str )
#else
#	define CHECK( str ) check(str);
#endif

void	c_bdd_spout2::update_sender()
{
	INT32	bind = g_bind_img_2d->build_index_from_param(-1);
	INT32	size_x;
	INT32	size_y;
	tex2d.get_size( bind, size_x,size_y );
	GLenum gl_format = tex2d.get_internal_format( bind );

	if( size_x<=0 || size_y<=0 )
	{	//	texture is not defined yet
		if( _b_verbose_ui )
			err_print( "%s() size <=0 : texture is not defined yet ", __FUNCTION__ );
		return;
	}

	if( IS_NULL( _sender ) )
	{
		init_sender();
		if( IS_NULL( _sender ) )
			return;
	}
	else
	{
		if( size_x != _size_x || size_y != _size_y || gl_format != _sender_gl_format )
			close_sender();
	}

	if( !_b_sender_init )
	{
		DXGI_FORMAT CONST dx_format = convert_GL_format_to_DX(gl_format);
		_b_sender_init = open_sender( size_x, size_y, dx_format );

		if( _b_sender_init )
		{
			_size_x	= size_x;
			_size_y	= size_y;
			_sender_gl_format = gl_format;
			_pixel_format		= convert_DX_format_to_aaa( dx_format );
			_sender_gl_format	= gl_format;		
		}
		else
			return;
	}

	
	// Spout needed the fbo to eventually (if needed) restore it after
	GLuint	fbo_name_gl	= c_fbo::get_cur_name_gl();
	bool	b_ret = false;
	if( _b_send_img_ui )
	{
		c_img_2d*	img = g_bind_img_2d->get_always( bind );
		if( img->is_valid() )
		{
 			GLenum	format = img->get_gl_format();
			b_ret = _sender->SendImage( (unsigned char*)img->get_data(), img->get_size_x(), img->get_size_y(), format, !_b_sender_flip_vertical_ui, fbo_name_gl );
		}
		else
		{
			//if( _b_verbose_ui )
			err_print( "SpoutSender image bind %d is not valid", bind );
		}
	}
	else
	{
		GLuint gl_name = tex2d.get_name_gl( bind );
		b_ret = _sender->SendTexture( gl_name, GL_TEXTURE_2D, _size_x, _size_y, !_b_sender_flip_vertical_ui, fbo_name_gl );
	}

	if( b_ret )
	{
		++_sending_count_out;
		_b_sending = true;
	}
	else
	{
		_b_sending = false;
		if( _b_verbose_ui )
			err_print( "SpoutSender could not send texture" );
	}
}

bool	c_bdd_spout2::open_sender( UINT32 CONST size_x, UINT32 CONST size_y, DWORD CONST dwFormat )
{
	bool b_ret  = false;
	if( _sender )
	{
		_sender->SetDX9( _b_sender_dx9_asked_ui );
		C_PCHAR name = _sender_name_ui.is_empty() ? ( _b_sender_dx9_asked_ui ? "AAASeed dx9" : "AAASeed Dx11" ) : _sender_name_ui.get();
		//if( _gpu_adapter_ui > 0 )
		//{
		//	_dx11_adapter = get_d3d11_adapter( _gpu_adapter_ui );
		//}
		_sender->SetPreferredAdapter( _gpu_adapter_ui );	
		b_ret = _sender->CreateSender( name, MAX1( size_x ), MAX1( size_y ), dwFormat );
		if( b_ret )
		{
			// Detect texture share compatibility (optional)
			_b_sender_memory_share		= _sender->GetMemoryShareMode();
			_b_sender_dx9_used			= _sender->GetDX9();
		}
		else
			err_print( "%s() Spout not CreateSender", __FUNCTION__ );
	}
	return b_ret;
}

void	c_bdd_spout2::close_sender()
{
	if( _sender )
	{
		// Release the sender - this can be used for repeated
		// Create and Release of a sender within the program
		_sender->ReleaseSender();	
		//SAFE_DELETE( _sender );
	}
	_b_sender_init			= false;
	_b_sender_memory_share	= false;
	_b_send					= false;
	_b_sending				= false;
}

void	c_bdd_spout2::init_sender()
{
	_sender = new Spout;			// Create a new Spout sender
	if( IS_NULL( _sender) )
		err_print( "Could not create SpoutSender" );
}

void	c_bdd_spout2::update_receiver()
{
	CHECK( "update_receiver() Begin" );
	bool b_ret;
	if( IS_NULL( _receiver ) )
	{
		_receiver = new Spout;			// Create a new Spout receiver
		if( IS_NULL( _receiver) )
		{
			err_print( "Could not create SpoutReceiver" );
			return;
		}
		CHECK( "update_receiver() After new SpoutReceiver" );
		b_ret = _receiver->SetPreferredAdapter( _gpu_adapter_ui );
		if( !b_ret )
			err_print( "%s() : Failed to Set preferred adapter to %d", __FUNCTION__, _gpu_adapter_ui );
		_b_receiver_sender_enum_trig = true;
	}

	_receiver_sender_count = _receiver->GetSenderCount();
	if( _receiver_sender_count != _receiver_sender_count_done  )
		_b_receiver_sender_enum_trig = true;

	if( _b_receiver_sender_enum_trig )
	{
		enum_sender();
		CHECK( "update_receiver() After enum_sender()" );
		_b_receiver_sender_enum_trig = false;
	}

	bool b_close_need = false;
	if( _receiver_index != _receiver_index_ui ) //|| _b_receive_img != _b_receive_img_ui )
		b_close_need = true;
	else
	{	
		//todo deal with format change too
		// DWORD	format;
		UINT32	size_x;
		UINT32  size_y;
		bool	b_connected;
		if( _receiver->CheckReceiver( _name, size_x,size_y, b_connected ) )
		{
			if( b_connected )
			{
				//	_spout_receiver->GetSenderInfo( sender_name, size_x,size_y, dx_handle, format );
				//	CHECK( "update_receiver() After GetSenderInfo()" );
				//	// extend to format
				if( size_x != _size_x || size_y != _size_y )
				{
					GOOD_PRINT_STRING( "\tSpoutReceiver %d-%s: size changed to %d x %d", _receiver_index_ui, _name, size_x,size_y );
					b_close_need = true;
				}
			}
			else
				b_close_need = true;
		}
		else
			b_close_need = true;
	}

	
	if( b_close_need )
	{
		if( _b_receiver_init )
			close_receiver();
		CHECK( "update_receiver() After close_receiver()" );
	}

	if( !_b_receiver_init  )
	{
//		_receiver_sender_count = _spout_receiver->GetSenderCount();
		if( _receiver_index_ui >= _receiver_sender_count )
			return;
	
		_b_receiver_init = open_receiver( _receiver_index_ui );
		if( !_b_receiver_init  )
			return;
		
		_receiver_index = _receiver_index_ui;
		CHECK( "update_receiver() After SetActiveSender()" );
	}

	strcpy( _name, _receiver_name_ui.get() );


#if 0	//we don't need this ??? 2024 May	;
	_name[0] = 0;
	//strcpy_s( SenderName, _receiver_name.get_len() +1, _receiver_name.get() );
	_receiver->GetActiveSender( _name );
#endif

	//_name[0] = 0;
	//UINT32	size_x, size_y;
	//bool	memory_mode;
	//b_ret = _spout_receiver->GetImageSize( sender_name, size_x, size_y, memory_mode );
	//if( !b_ret )
	//{
	//	if( _b_verbose_ui )
	//		err_print( "Error getting image size" );
	//	return;
	//}

	if( _size_x > 0 && _size_x > 0 )
	{
		// Spout needed the fbo to eventually (if needed) restore it after
		//c_fbo::disable();
		GLuint	fbo_name_gl	= c_fbo::get_cur_name_gl();

		//todo don't init all the time
		//todo do stuff only when needed (size bind ... )
		INT32		bind = g_bind_img_2d->build_index_from_param(-1);
		c_img_2d*	img = g_bind_img_2d->get_always( bind );
	
		//GLDXformat( DXGI_FORMAT textureformat );
		//UINT32 _gl_format
		UINT32 size_x;
		UINT32 size_y;

		if( _b_receive_img_ui )	// _b_receiving )
		{
			img->init_with_size( _size_x,_size_y, _pixel_format, "Spout2 Image" );
			CHECK( "update_receiver() After init_with_size()" );
			b_ret = _receiver->ReceiveImage(	_name,
												size_x,size_y, img->get_data_uint8(), aaa::c_pixel_format::get_gl_format_internal(_pixel_format),
												!_b_receiver_flip_vertical_ui, fbo_name_gl );
			CHECK( "update_receiver() After ReceiveImage()" );
			if( b_ret )
				img->set_changed();
		}
		else
		{
			//todo make sure the init_with_size_no_cpu_mem version function too
			//img->init_with_size( size_x, size_y, aaa::PIXEL_FORMAT::RGBA_8, "Spout2 Image" );
			img->init_with_size_no_cpu_mem( _size_x,_size_y, _pixel_format, "Spout2 Image" );
			CHECK( "update_receiver() After init_with_size_no_cpu_mem()" );
			//make sure texture is defined
			//	tex_2d_bind init the texture calling move_to_gpu which deal with Null data now

			//tex_2d_bind_no_gpu_move( bind );
			tex_2d_bind( bind );

			//make texture complete
			//not necessary infact
			//tex2d.set_wrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
			//GOL::set_tex_2d_min_mag( GL_LINEAR, GL_LINEAR );

			UINT32	receiver_texture_gl_name = tex2d.get_name_gl( bind );
			CHECK( "update_receiver() After tex_2d_bind()" );
			b_ret = _receiver->ReceiveTexture(	_name,
												size_x,size_y, receiver_texture_gl_name, GL_TEXTURE_2D,
												!_b_receiver_flip_vertical_ui, fbo_name_gl );  
			CHECK( "update_receiver() After ReceiveTexture()" );
			if( b_ret )
			{
				img->set_changed();
				img->set_gpu_in_sync();
			}
		}
		_b_receive_img = _b_receive_img_ui;

		if( b_ret )
		{
			_b_receiving = true;
			++_receiving_count_out;
			if( size_x != _size_x || size_y != _size_y ) //|| format != _format  )
			{
				GOOD_PRINT_STRING( "\tSpoutReceiver %d - %s : changed size to %d x %d from %d x %d", _receiver_index_ui, _name, size_x, size_y, _size_x, _size_y );
				//too check we don't need this
				_size_x = size_x;
				_size_y = size_y;
				//_format = format;
			}
		}
		else
		{
			if( _b_verbose_ui )
				err_print( "Error receiving texture" );
		}
	}

//	_spout_receiver->SetActiveSender( active_name );
}

void	c_bdd_spout2::enum_sender()
{
	if( IS_NULL( _receiver ) )
		return;

	_receiver_sender_count = _receiver->GetSenderCount();
	GOOD_PRINT_STRING( "%s() : Got %d SpoutSender", __FUNCTION__, _receiver_sender_count );

	UINT32 i;
	UINT32 nb = MIN( _receiver_sender_count, SRC_UI_NB );

	for( i = 0; i < nb; ++i )
	{
		UINT32	size_x, size_y;
		HANDLE	dx_handle;
		DWORD	format;

		bool b_ret = _receiver->GetSender( i, _name );
		if( !b_ret )
			err_print( "%s() : Failed to GetSender() for sender %d", __FUNCTION__, i );
		else
		{
			b_ret = _receiver->GetSenderInfo( _name, size_x, size_y, dx_handle, format );
			if( !b_ret )
				err_print( "%s() : Failed to GetSenderInfo() for sender %s", __FUNCTION__, _name );
			else
				GOOD_PRINT_STRING( "\tSpoutSender %d : %s with size %d x %d and format %d", i, _name, size_x, size_y, format );
		}
		if( b_ret )
			_src_name[i].set( _name );
		else
			_src_name[i].erase();
	}
	for( ; i < SRC_UI_NB; ++i )
		_src_name[i].erase();

	_receiver_sender_count_done = _receiver_sender_count;
}



bool	c_bdd_spout2::open_receiver( UINT32 CONST index )
{
	bool b_ret  = false;

	enum_sender();

	if( index < _receiver_sender_count )
	{
		UINT32	size_x = 0;
		UINT32	size_y = 0;

		_receiver->SetDX9( _b_receiver_dx9_asked_ui );

		b_ret = _receiver->SetPreferredAdapter( _gpu_adapter_ui );
		if( !b_ret )
			err_print( "Fail to SetAdapter to %d", _gpu_adapter_ui );
		else
		{
			b_ret = _receiver->GetSender( index, _name );
			if( !b_ret )
				err_print( "Fail to GetSende for index %d", index );
			else
			{
				_receiver_name_ui.set( _name );
				
				DWORD	dx_format;
				HANDLE	dx_handle;
				_receiver->GetSenderInfo( _name, size_x, size_y, dx_handle, dx_format );
				_receiver->SetActiveSender( _name );
				b_ret = _receiver->CreateReceiver( _name, size_x, size_y );
				if( !b_ret )
					err_print( "Can not Connect to SpoutServer %s", _name );
				else
				{
					GOOD_PRINT_STRING( "Connected to SpoutServer %s with size : %dx%d", "active", size_x, size_y );
					// Detect texture share compatibility (optional)
					_b_receiver_memory_share	= _receiver->GetMemoryShareMode();
					_b_receiver_dx9_used		= _receiver->GetDX9();
					_size_x						= size_x;
					_size_y						= size_y;
					_pixel_format				= convert_DX_format_to_aaa( DXGI_FORMAT(dx_format) );
					_sender_gl_format			= _receiver->GLDXformat( DXGI_FORMAT(dx_format) );		
				}

			}
		}
	}
	if( !b_ret )
	{
		_size_x	= 0;
		_size_y	= 0;
	}
	return b_ret;
}

void	c_bdd_spout2::close_receiver()
{
	if( _receiver )
	{
		// Release the sender - this can be used for repeated
		// Create and Release of a sender within the program
		_receiver->ReleaseReceiver();
		//SAFE_DELETE( _receiver );
		_receiver_index = -1;
	}
	_b_receiver_init			= false;
	_b_receiver_memory_share	= false;
	_b_receive					= false;
	_b_receiving				= false;

//	_size_x						= 0;
//	_size_y						= 0;
}

void	c_bdd_spout2::update()
{
//	mem::set_check_heap(   true );
//	mem::set_check_memory( true );
	CHECK( "Begin" );

	if( _b_enable_ui )
	{
		if( _b_receive != _b_receive_ui )
		{
			if( _b_receive )
				close_receiver();
		}
		if( _b_send != _b_send_ui )
		{
			if( _b_send )
				close_sender();
			else
				_b_sending = false;
		}

		if( _b_receive_ui )
			update_receiver();
		else
			_b_receiving = false;
		if( _b_send_ui )
			update_sender();
		else
			_b_sending = false;
	}
	else
	{
		_b_receiving	= false;
		_b_sending		= false;
	}
	CHECK( "End" );
}

//
//	DRAW
//
void	c_bdd_spout2::draw()
{
}
