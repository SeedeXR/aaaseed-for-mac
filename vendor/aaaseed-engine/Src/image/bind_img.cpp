#include "bind_img.h"
#include "bind_img_1d.h"
#include "bind_img_2d.h"
#include "bind_img_3d.h"
#include "draw/map.h"
#include "draw/seeddraw.h"


INT32	bind_ui_get()
{
	if( c_layer::get_ui() )
	{
		switch( c_map::get_ui()->get_tex_use_ui() )
		{
		default:
		case 0:		return -1;
		case 1:		return c_layer::get_ui()->get_bind_1d();
		case 2:		return c_layer::get_ui()->get_bind_2d();
		case 3:		return c_layer::get_ui()->get_bind_3d();
		}
	}
	else
	{
		return -1;
	}
}

/*! \todo bind_cur have to change to go with tex_bind_2d ? */
void	bind_ui_set( INT32 value )
{
	INT32	bind;
	c_layer* layer_ui = c_layer::get_ui();
	switch( c_map::get_ui()->get_tex_use_ui() )
	{
	case 3:
		if( layer_ui )
		{
			layer_ui->set_bind_3d_ui( g_bind_img_3d->get_index_valid( value ) );
			bind = layer_ui->get_bind_3d();
		}
		else
			bind = 0;
		if( draw::is_rendering() )
		{
#if	AAA_DEBUG()
			SWITCH_PRINT_STRING( "3D texture bind", "%d", bind );
#endif
			g_bind_img_3d->get_bind()->set_index_cur( bind );
		}
		tex_3d_bind( bind );
		break;

	case 2:
		if( layer_ui )
		{
			layer_ui->set_bind_2d_ui(  g_bind_img_2d->get_index_valid( value ) );
			bind = layer_ui->get_bind_2d();
		}
		else
			bind = 0;
		if( draw::is_rendering() )
		{
#if	AAA_DEBUG()
			SWITCH_PRINT_STRING( "2D texture bind", "%d", bind );
#endif
			g_bind_img_2d->get_bind()->set_index_cur( bind );
		}
		tex_2d_bind( bind );
		break;

	case 1:
		if( layer_ui )
		{
			layer_ui->set_bind_1d_ui( IMOD( value, BIND_1D_MAX_NB ) );
			bind = layer_ui->get_bind_1d();
		}
		else
			bind = 0;
		if( draw::is_rendering() )
		{
#if	AAA_DEBUG()

			SWITCH_PRINT_STRING( "1D texture bind", "%d", bind );
#endif
			//todo we should have this for symmetry
			//g_bind_img_1d->get_bind()->set_index_cur( loc_bind );
		}
		tex_1d_bind( bind );
		break;
	
	default:
		break;
	}
}

template < class TEXND, class BIND_ING, bool B_LOAD >
FINLINE void	tex_bind( TEXND& tex, BIND_ING* g_bind, INT32 index )
{
	//todoq	this line should is a security
	index = g_bind->get_index_valid( index );
	tex.bind( index );
	if( B_LOAD )
		g_bind->load_data_and_move_to_gpu( index );
}

void	tex_2d_bind( INT32 index )
{
	tex_bind< c_tex2d, c_bind_img_2d, true  >( tex2d, g_bind_img_2d, index );
//#if AAA_DEBUG()
//	DBG_PRINT_STRING( "Texture 2d should be now %d", index  );
//#endif
}

void	tex_2d_bind_no_gpu_move( INT32 index )
{
	tex_bind< c_tex2d, c_bind_img_2d, false >( tex2d, g_bind_img_2d, index );
//#if AAA_DEBUG()
//	DBG_PRINT_STRING( "Texture 2d should be now %d", index  );
//#endif
}

//hack we have to deal with 3d 
void	tex_3d_bind( INT32 index )
{
	tex_bind< c_tex3d, c_bind_img_3d, true  >( tex3d, g_bind_img_3d, index );
}
void	tex_3d_bind_no_gpu_move( INT32 index )
{
	tex_bind< c_tex3d, c_bind_img_3d, false >( tex3d, g_bind_img_3d, index );
}

INT32	tex_2d_get_bind_cur()	{	return	c_layer::get_cur() ? c_layer::get_cur()->get_bind_2d() : 0;	}
INT32	tex_2d_get_bind_ui ()	{	return	c_layer::get_ui()  ? c_layer::get_ui() ->get_bind_2d()  : 0;	}

INT32	tex_3d_get_bind_cur()	{	return	c_layer::get_cur() ? c_layer::get_cur()->get_bind_3d() : 0; }
INT32	tex_3d_get_bind_ui()	{	return	c_layer::get_ui()  ? c_layer::get_ui() ->get_bind_3d() : 0; }

void	bind_img_init()
{
	bind_img_1d_init();
	bind_img_2d_init();
	bind_img_3d_init();
}

void	bind_img_deinit()
{
	bind_img_1d_deinit();
	bind_img_2d_deinit();
	bind_img_3d_deinit();
}