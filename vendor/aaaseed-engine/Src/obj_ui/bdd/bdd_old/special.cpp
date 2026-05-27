#include "obj_ui/bdd/bdd_old/special.h"
#include "aaa_def.h"

#include "boulogne.h"	
#include "bpi.h"
#if	APP_SPECIAL_BPI()
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#endif
#include "lcp.h"
#include "nib.h"
#include "peripherie.h"
#include "orgia.h"
#include "strato.h"
#include "upstream.h"
#include "obj_ui/bdd/bdd_old/aichi.h"

#include "infrastructure/layer/layers.h"

#ifdef _MSC_VER
#	pragma warning(disable:4065)	//switch off statement contains 'default' but no 'case' labels
									// whichhappen when no special which is the default case now
#endif

static	C_PCHAR_C	str_special[SPECIAL_NB] =
{
	"bpi",
	"nib",
	"orgia",
	"strato",
	"lcp",
	"aichi",
	"upstream",
};

INT32	s_special_type = 0;

c_obj_active_ui*	special1 = nullptr;
c_obj_active_ui*	special2 = nullptr;

#if	APP_SPECIAL_NIB()
bool	b_nib = true;
#else
bool	b_nib = false;
#endif

void	special_get_user_name( char* str)
{
#if	AAA_DEMO_INTERGRAPH()
	strcpy( str, "intergraph");
#endif
}

void	special_set_focus()
{
	if( special1 )
		special1->set_focus();
	if( special2 )
		special2->set_focus();
}

void	special_go_render()
{
}

void	special_deinit()
{
	SAFE_DELETE( special1 );
	SAFE_DELETE( special2 );
}

void	special_init()
{
	switch( s_special_type )
	{
#if	APP_SPECIAL_NIB()
	case TYPE_NIB:		special1 = nib = new c_nib;				break;
#endif
#if	APP_SPECIAL_ORGIA()
	case TYPE_ORGIA:	special1 = orgia = new c_orgia;			break;
#endif
#if	APP_SPECIAL_STRATO()
	case TYPE_STRATO:	special1 = strato = new c_strato;		break;
#endif
#if	APP_SPECIAL_LCP()
	case TYPE_LCP:		special1 = lcp = new c_lcp;				break;
#endif
#if	APP_SPECIAL_AICHI()
	case TYPE_AICHI:	special1 = aichi = new c_aichi;			break;
#endif
#if	APP_SPECIAL_UPSTREAM()
	case TYPE_UPSTREAM:	special1 = upstream = new c_upstream;	break;
#endif
#if	APP_SPECIAL_BPI()
	case TYPE_BPI:
#endif
	default:
#if	APP_SPECIAL_BPI()
						special1 = bpi = new c_bpi;
#endif
		break;

	}
#if	APP_SPECIAL_PERI()
	special1 = peri = new c_peri;
#endif
#if	APP_SPECIAL_BOUL()
	special1 = boul = new c_boul;
#endif

	special_set_focus();
	special_menu_init();
}

void	special_menu_init()
{
	switch( s_special_type )
	{
#if	APP_SPECIAL_LCP()
	case TYPE_LCP:
		if( lcp)
			lcp->menu_init();
		break;
#endif
	default:
		break;
	}
}

bool	special_menu_add()
{
	switch( s_special_type )
	{
#if	APP_SPECIAL_LCP()
	case TYPE_LCP:
		if( lcp )
			lcp->menu_add();
		return true;
		break;
#endif
	default:
		return false;
	}
}

void	special_save( o_str CONST & filename )
{
	if( special1 )
		special1->save_to_file( filename );
	if( special2 )
		special2->save_to_file( filename );
}

void	special_load( o_str CONST & filename )
{
	if( special1 )
		special1->load_from_file( filename );
	if( special2 )
		special2->load_from_file( filename );
}

INT32	special_get_bdd_tri_id( C_PCHAR_C name_in )
{
	switch( s_special_type )
	{
#if	APP_SPECIAL_LCP()
	case TYPE_LCP:	if( lcp )	return lcp->get_bdd_tri_id( name_in);	break;
#endif
	default:	break;
	}
	return 0;
}

/*
#if	AAA_DEMO_INTERGRAPH()
	INT32	stress_level_start[6] = {1,6,1,5,1,1};
	INT32	stress_level[6] = {1,6,1,5,1,1};
	void	stress_update()
	{
		INT32	which = c_layers::ui->get_layers_index();
		INT32	val = stress_level[which-1];
		g_lights_def.set_stop( 0 );
		switch( which)
		{
		case 1:	// Intro
			break;
		case 2:	// Wheel
			c_layers::ui->layer_get_always(0)->get_bdd_face()->bdd_nb[2] = val;
			break;
		case 3:	// Blob
			val = MIN( val, 8);
			g_lights_def.set_stop( val-1 );
			break;
		case 4:	// Explode
			c_layers::ui->layer_get_always(0)->get_bdd_particle()->nb_max_ui = val*200;
			break;
		case 5:	// Ping
			{
			c_layers::ui->layer_get_always(0)->get_bdd_face()->bdd_nb[2] = val;
			}
			break;
		case 6:	// Credits
			break;
		}
		stress_level[which-1] = val;
	}

	bool	b_multisample_first = true;
	bool	b_multisample_change = false;
	c_layer*	multisample_layer_off;
	c_layer*	multisample_layer_on;
	extern	bool	b_allow_multisample_ui;

	void	multisample_update()
	{
		if( b_multisample_first )
		{
			c_layers* layers = layers_get_from_name_short( "Logo" );
			c_layer*	layer_base = layers->layer_get_always(1);
			multisample_layer_off = layers->layer_get_always(2);
			multisample_layer_on = layers->layer_get_always(3);

			b_multisample_change = maaglut_multisample_is_on();

			multisample_layer_off->set_active( b_multisample_change );
			multisample_layer_on->set_active( b_multisample_change );
			layer_base->set_active( b_multisample_change );

			b_multisample_first = false;
		}
		if( b_multisample_change )
		{
			multisample_layer_off->set_active( !b_allow_multisample_ui );
			multisample_layer_on->set_active( b_allow_multisample_ui );
		}
	}

	bool	b_cycling = false;
	REAL	cycling_time;
	DOUBLE	cycling_time_change;
	void	cycling_set( INT32 value)
	{
		b_cycling = value != 0;
		cycling_time = (value*6. - 20.)/4.;
		cycling_time_change = aaa::time::get_real_time();
	}

	void	cycling_update()
	{
		DOUBLE	t = aaa::time::get_real_time();
		if( b_cycling && t > cycling_time_change )
		{
			INT32	i;
			i = layers_exclusive_get();
			cycling_time_change = aaa::time::get_real_time();
			cycling_time_change += (i<=4)?cycling_time:10.;
			layers_exclusive_next();
		}
	}

	void	special_stress_add( INT32 inc)
	{
		INT32*	pt = &stress_level[c_layers::ui->get_layers_index()-1];
		INT32	val = *pt;
		val += inc;
		if (val<1)
			val = 1;
		*pt = val;
		if ( (inc>0) && (c_layers::ui->get_layers_index() == 4) )
			time_start();
#if 1
		if (c_layers::ui->index == 2)
			{
			CONST	INT32	nb = 128;
			GLuint array[nb];
			GLclampf f[nb];
			INT32	i;
			for( i=0; i<nb; ++i )
				{
				array[i] = i;
				f[i] = 1. - GLclampf(i)/FLOAT(nb);
				}
			glPrioritizeTextures( nb, array, f);
			}
#endif	
	}

	void	special_stress_reset()
	{
		INT32	i = c_layers::ui->get_layers_index()-1;
		stress_level[i] = stress_level_start[i];
	}

	#define	RENDER_TYPE_NB	15
	INT32	render_array[RENDER_TYPE_NB][3] = 
	{
		{	1, 0, 1 },
		{	0, 0, 0 },
		{	0, 0, 1 },
		{	0, 0, 2 },
		{	1, 1, 1 },
		{	0, 1, 0 },
		{	0, 1, 1 },
		{	0, 1, 2 },
		{	1, 2, 1 },
		{	0, 2, 0 },
		{	0, 2, 1 },
		{	0, 2, 2 },
		{	2, 1, 0 },
		{	2, 1, 1 },
		{	2, 1, 2 },
	};

	INT32	rendering_type[6] = {1,1,1,1,1,1};

	void	intergraph_demo_render_set(int value)
	{
	INT32	i = c_layers::ui->get_layers_index();
		if ( i==3 || i==5 )
			{
			value = IMOD(value, RENDER_TYPE_NB);
			rendering_type[i-1] = value;
			render_ui->set_cull(render_array[value][0]);
			render_ui->set_front_mode(render_array[value][1]);
			render_ui->set_back_mode(render_array[value][2]);
			}
	}
	void	intergraph_demo_render_inc()
	{
		intergraph_demo_render_set( rendering_type[c_layers::ui->get_layers_index()-1] +1);
	}

	void	intergraph_demo_render_dec()
	{
		intergraph_demo_render_set( rendering_type[c_layers::ui->get_layers_index()-1] -1);
	}

	void	intergraph_demo_render_reset()
	{
		intergraph_demo_render_set( 0);
	}

*/

bool	special_key_special( INT32 key_code, INT32* modifiers, INT32* x, INT32* y )
{
#if	APP_SPECIAL_LCP()
	if (lcp)
		return lcp->do_key( key_code, true, modifiers, x, y );
#endif
#if	AAA_DEMO_INTERGRAPH()
	bool	b_return = true;
	switch( key_code )
	{
	case keyboard::F4:
		layers_exclusive_refresh_camera();
		break;
	case keyboard::F5:
		layers_exclusive_refresh();
		special_stress_reset();
		intergraph_demo_render_reset();
		break;
	default:
		break;
	}
	return b_return;
#else
	return false;
#endif
}

bool	special_key( INT32 key_code, INT32* modifiers, INT32* x, INT32* y )
{
#if	APP_SPECIAL_LCP()
	if (lcp)
		return lcp->do_key( c, false, modifiers, x,y );
#endif

#if	AAA_DEMO_INTERGRAPH()
	bool	b_return = true;
	switch (*c)
	{
	case 27:
		layers_exclusive_set(6);
		b_return = false;
		break;
	case '0':
	case '1':
	case '2':
		c_map::get_ui()->set_tex_use( *c-'0' );
		break;
	case keyboard::SPACE:
		if ( modifier_is_shift_on(*modifiers) )
			layers_exclusive_prev();
		else
			layers_exclusive_next();
		break;
	case 'A':
	case 'a':
		{
			c_layer* p = c_layers::ui->layer_get(0);
			if(p)
			{
				c_feedback* f = p->get_feedback();
				f->set_angle( f->get_angle() + ((*c=='a')?-.002:.002) );
			}
		}
		break;
	case 'F':
	case 'f':
		intergraph_demo_menu( 111);
		break;
	case 'Z':
	case 'z':
		{
		c_layer* p;
		p = c_layers::ui->layer_get(0);
		if( p)
			{
			c_feedback* f;
			f = p->get_feedback();
			f->set_scale_factor( f->get_scale_factor() + ((*c=='z')?-.005:.005) );
			}
		}
		break;
	case 'M':
	case 'm':
		intergraph_demo_menu( 110)	;
		break;
	case 'S':
	case 's':
		b_allow_feedback = !b_allow_feedback;
		break;
	case 't':
		bind_ui_set( IMOD(bind_ui_get()+1, 11 ) );
		break;
	case 'T':
		bind_ui_set( IMOD(bind_ui_get()-1, 11 ) );
		break;
/*		case 't':
	case 'T':
		*c -= 9;
		b_return = false;
		break;
*/	
	case '+':
	case '-':
		special_stress_add( (*c=='+')?1:-1);
		break;
	case 'r':
		intergraph_demo_render_inc();
		break;
	case 'R':
		intergraph_demo_render_dec();
		break;
	case 'g':
	case 'G':
	case 'w':
	case 'W':
//		case 'f':
//		case 'F':
//		case 'b':
//		case 'B':
//		case 'c':
//		case 'C':
		if( modifier_is_none(*modifiers ) )
			b_return = false;
		break;
	default:
		break;
	}
	return b_return;
#else
	return false;
#endif
}

void	special_build_message()
{
#if	AAA_DEMO_INTERGRAPH()
	INT32	which = c_layers::ui->get_layers_index();
	CHAR*	mess = mess_str;
	switch( which )
	{
	case 1:	// Intro
		strcpy( mess, "Press Space for Next Fx" );
		break;
	case 2:	// Wheel
		{
			INT32 i;
			REAL r;
			i = c_layers::ui->layer_get_always(0)->get_bdd_face()->bdd_nb[2];
			CLAMP_REF( i, 0, INT32(bind_2d_bank_size));
			if ( b_allow_feedback )
				++i;
			i *= 1398104;	//512x512 MipMaped
			r = i / (1024.*1024.);
			sprintf( mess, "Texture Stress: %3.1f MB of Texture Memory", r );
		}
		break;
	case 3:	// Blob
		{
			INT32 nb = g_lights_def.get_stop();
			if ( nb )
				sprintf( mess, "Lights Stress: %d lights", nb+1 );
			else
				sprintf( mess, "Lights Stress: One light");
		}
		break;
	case 4:	// Explode
		{
			INT32 i;
			i = c_layers::ui->layer_get_always(0)->get_bdd_particle()->nb_max;
			sprintf( mess, "Processor Stress: %d Particles", i );
		}
		break;
	case 5:	// Ping
		{
			INT32	i;
			INT32	nb_poly;
			INT32	nb_tri;
			c_bdd_tri* bdd_tri = c_layers::ui->layer_get_always(0)->get_bdd_tri();
			nb_poly = bdd_tri->get_nb_poly();
			nb_tri = bdd_tri->get_nb_tri();
			i = c_layers::ui->layer_get_always(0)->get_bdd_face()->bdd_nb[2];
			nb_poly *= i;
			nb_tri *= i;
			sprintf( mess, "Polygon Stress: %d Polygons/%d Triangles", nb_poly, nb_tri );
		}
		break;
	default:
		*mess = 0;
		break;
	}
#endif
}

void	special_update()
{
#if	AAA_DEMO_INTERGRAPH()
	cycling_update();
	stress_update();
	multisample_update();
#endif
	//todo call it only if active
	if( is_obj_exist_and_active(special1) )
		special1->update();
}

extern	void	special_draw_on_top()
{
#if	APP_SPECIAL_LCP()
	if (lcp)
		//hack
		lcp->draw_on_top( viewport_cur->get_sx(), viewport_cur->get_sy(), c_flatland::get_area_focus_size_x() );
#endif
}

void	special_mocap_set_seq( INT32	index)
{
//#if	BELGA
//	c_layers* p = layers_get_from_name_short( "RFoot" );
//	if( p )
//		p->layer_get_always(0)->get_transfo_trs1()->set_b_tra_ui(index < 8);
//#endif
}

void	special_mocap_end_seq( c_bdd_mocap* mocap)
{
//#if	BELGA
//	if( belga->is_active() && net && net->is_master() )
//		{
//		if ( b_mode_wait )
//			mocap->set_seq( 0 + INT32(rand_seq.get_ufloat()*7.95) );
//		else
//			mocap->set_seq( 9 + INT32(rand_seq.get_ufloat()*10.8) );
//		}
//#endif
#if	APP_SPECIAL_BPI()
/*	if( bpi->is_active() )
		{
		mocap->set_seq( bpi->get_seq() );
		}
*/
#endif
}

void	special_about()
{
#if	APP_SPECIAL_BPI()
	((c_bpi *)special1)->about();
#endif
}