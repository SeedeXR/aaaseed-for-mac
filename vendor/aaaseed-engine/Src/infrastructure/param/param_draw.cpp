#include "infrastructure/param/param_draw.h"
#include "ui/alphabet_maa.h"
#include "ui/flatland.h"
#include "gol/gol.h"
#include "gol/gol_matrix.h"
#include "infrastructure/factory/factory_group.h"
#include "param_focus.h"
#include "trax.h"


using namespace aaa::param;

//todo	connect directly with alphabet

static	UINT8 CONST	SY_OUT				=	128;	// trax OUT side of a connection
static	UINT8 CONST	SY_IN				=	129;	// trax IN  side of a connection
//	SY_LOCK shares the SY_OUT glyph today : a locked param renders with the same marker as
//	a trax-OUT-driven param. The two meanings are unrelated, the visual reuse is a UI choice.
//todo split SY_LOCK into its own alphabet glyph if the visual distinction becomes useful.
static	UINT8 CONST	SY_LOCK				=	SY_OUT;	// param is_lock()

static	UINT8 CONST	SY_BOX_PLUS			=	130;	// goon and branch right
static	UINT8 CONST	SY_RIGHT			=	131;	
static	UINT8 CONST	SY_BOX_EMPTY		=	132;

static	UINT8 CONST	SY_BOX_PLUS_END		=	133;	// just branch right
static	UINT8 CONST	SY_RIGHT_END		=	134;	
static	UINT8 CONST	SY_BOX_EMPTY_END	=	135;

static	UINT8 CONST	SY_T_DOWN			=	136;	// go right with a line going down "T junction"
static	UINT8 CONST	SY_LINE_HORI		=	137;	// just go right

static	UINT8 CONST	SY_TRAX				=	138;	//  same with empty box when it is open
static	UINT8 CONST	SY_REF				=	139;
static	UINT8 CONST	SY_BLANK			=	140;
static	UINT8 CONST	SY_LINE_VERT		=	'|';

static	UINT8 CONST	SY_OBJ_PLUS_END		=	141;
static	UINT8 CONST	SY_OBJ_EMPTY_END	=	142;


static	INT32		int32_param_line_offset	= 0;
static	REAL		real_param_line_offset	= 0;

		INT32		line_drawn = 0;		//todo 
static	INT32		draw_line = 1;

static	INT32		dive_line;
static	INT32		dive_line_to_find;

static	INT32		draw_level;
static	INT32		draw_top_line		= 0;
static	INT32		draw_bottom_line	= 0;

CONSTEXPR	INT32	DRAW_RECURSION_MAX = 24;
//static	p_param	param_no_repeat[DRAW_RECURSION_MAX] = { nullptr, nullptr };
//par should be static and more elegant
static	p_param		param_no_repeat[DRAW_RECURSION_MAX] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
static	c_obj_ui*	obj_dive[DRAW_RECURSION_MAX];	

//todo unclear what this does, and it is not initialized 
static	bool		b_param_hilite_menu_focus;

static	UINT8		draw_recursion_mark;
CONSTEXPR	INT32	STR_PARAM_SIZE		=	512;
CONSTEXPR	INT32	STR_PARAM_SIZE_UI	=	256;
static	CHAR		cleaning_str[STR_PARAM_SIZE];
static	CHAR		str_param[STR_PARAM_SIZE];
static	CHAR		str_param_buf[STR_PARAM_SIZE];
CONSTEXPR	INT32	PARAM_STR_LINE_MAX = 3;

static	REAL		clean_char_tra_x;
static	INT32		clean_len;
static	INT32		clean_start;


FINLINE INT32	get_int32_param_line_offset()	{	return int32_param_line_offset;	}
FINLINE REAL	get_real_param_line_offset()	{	return real_param_line_offset;	}
FINLINE void	set_real_param_line_offset( REAL CONST in )
{
	if( in <= 0. )
		real_param_line_offset = 0;
	else
		real_param_line_offset = in ;
	int32_param_line_offset = I_FLOOR(real_param_line_offset);
	//	VERBOSE_PRINTF( "deg int32_param_line_offset %d/n", int32_param_line_offset);
}

namespace aaa::param::flat
{
	c_obj_ui*	obj_found					= nullptr;
	bool		b_char_draw_slider			= false;
	REAL		comment_trax_scale_x		= .5;
	REAL		comment_trax_scale_y		= .5;
	REAL		comment_scale_x				= .5;
	REAL		comment_scale_y				= .5;
	REAL		comment_trax_scale_x_over	= 2.;
	REAL		comment_trax_scale_y_over	= 2.;
	REAL		comment_scale_x_over		= 2.;
	REAL		comment_scale_y_over		= 2.;

	void	scale_update()
	{
		comment_scale_x_over		= OVER_ONE_AS_REAL(comment_scale_x);
		comment_scale_y_over		= OVER_ONE_AS_REAL(comment_scale_y);
		comment_trax_scale_x_over	= OVER_ONE_AS_REAL(comment_trax_scale_x);
		comment_trax_scale_y_over	= OVER_ONE_AS_REAL(comment_trax_scale_y);
	}

	FINLINE void	scale_comment_before()		{	GOL::matrix::scale( comment_scale_x,			comment_scale_y );				}
	FINLINE void	scale_comment_after()		{	GOL::matrix::scale( comment_scale_x_over,		comment_scale_y_over );			}
	FINLINE void	scale_comment_trax_before()	{	GOL::matrix::scale( comment_trax_scale_x,		comment_trax_scale_y );			}
	FINLINE void	scale_comment_trax_after()	{	GOL::matrix::scale( comment_trax_scale_x_over,	comment_trax_scale_y_over );	}
}


//	make line with slider
//	only used in flatland
bool	c_param::sprint( CHAR* str, INT32 CONST len )	//	len is done as a hack for now
{
//done in calling fn anyhow
#if	AAA_DEBUG()
	IF_THIS_NULL()
	{
		ERR_PRINT_STRING( "%s() Il y a une couille dans la puree : NULL param", __FUNCTION__ );
		return false;
	}
#endif

	bool b_ret = false;
	auto CONST type = get_type();
	auto CONST type_internal = get_type_internal();
	//	DRAW SLIDER
	if( aaa::param::flat::b_char_draw_slider && is_changeable() )
	{
		REAL	min	= get_min();
		REAL	max	= get_max();
		REAL	dif	= max - min;

		switch( type_internal )
		{
		case TYPE_INTERNAL_BOOL:
			aaa::alphabet::maa::draw_switch_line( get_bool() );
			b_ret = true;
			break;
		case TYPE_INTERNAL_INT32:
			aaa::alphabet::maa::draw_slider_line( (get_int32() - min)/dif, INT32(dif), min != PARAM_MIN_INT32, max != PARAM_MAX_INT32 );
			b_ret = false;
			break;
		case TYPE_INTERNAL_UINT32:
			aaa::alphabet::maa::draw_slider_line( (get_uint32() - min)/dif, INT32(dif), min != 0, max != PARAM_MAX_UINT32 );
			b_ret = false;
			break;
		case TYPE_INTERNAL_DOUBLE:
			{
				DOUBLE f = get_double();
				if( !_isnan( f ) )
					aaa::alphabet::maa::draw_slider_line( (REAL(f) - min)/dif, 0, min != PARAM_MIN_DOUBLE, max != PARAM_MAX_DOUBLE );
				b_ret = false;
			}
			break;
		case TYPE_INTERNAL_FP32:
			{
				DOUBLE f = get_fp32();
				if( !_isnan( f ) )
					aaa::alphabet::maa::draw_slider_line( (REAL(f) - min )/dif, 0, min != PARAM_MIN_FP32, max != PARAM_MAX_FP32 );
				b_ret = false;
			}
			break;
		default:
			print_debug( __FUNCTION__, "have no slider" );
			b_ret = false;
			break;
		}
	}
	else
	{
		switch( type )
		{
		case TYPE_BOOL:
		case TYPE_SYMBOLIC:
			b_ret = true;
			break;
		default:
			b_ret = false;
			break;
		}
	}
	auto pt = get_pt_const_void();
#if AAA_DEBUG()
	{
		if( pt==nullptr )
		{
			switch( type )
			{
			case TYPE_GROUP:
			case TYPE_GROUP_CLOSED:
			case TYPE_NONE:
				break;
			default:
				print_debug( __FUNCTION__, "pt is nullptr can be normal" );
				break;
			}
		}
		else
		{
			if( !mem::is_pointer_fucked(pt) )
			{
				INT32 i;
				INT32 max = -1;
				switch( type )
				{
				case TYPE_SYMBO_NEG:
 					i = *(INT32*)pt;
					if (i < 0)
					{
						i = -i - 1;
						max = -INT32(get_min()) - 1;
					}
					break;
				case TYPE_SYMBO_ZERO:
					i = *(INT32*)pt;
					if (i <= 0)
					{
						i = -i;
						max = -INT32(get_min());
					}
					break;
				case TYPE_SYMBOLIC:
					i = *(INT32*)pt;
					max = INT32(get_max());
					break;
				}
				if( max >= 0  )
				{
					if( OUTSIDE_MIN_MAX( i, 0, max ) )
						print_err( __FUNCTION__, "symbolic value out of limit, it should crash soon" );
				}
			}
		}
	}
#endif
	str += c_param::sprint_ui( str, len, get_name().get(), type, pt, get_symbolic_str() );
	*str = 0;
	return b_ret;	//unused for now
}

INT32	c_param::get_display_line_nb()
{
	switch( get_type() )
	{
	case TYPE_REF:
	case TYPE_STR:
		{
			auto s = get_pt_o_str();
			if( s )
				return CLAMP( s->get_line_nb(), 1, UI_STR_LINE_MAX );
		}
		return 1;
	default:
		return 1;
	}
}

namespace
{
	FINLINE	bool	is_line_before_end()	{	return draw_line < draw_bottom_line;	}
	FINLINE	bool	is_line_after_begin()	{	return draw_top_line <= draw_line;		}
	FINLINE	bool	is_line_not_clipped()	{	return is_line_after_begin();			}

	static	FINLINE	void	draw_str_ui( C_PCHAR_C str )
	{
		if( clean_len )
		{
			CHAR* pt =(CHAR*) str + clean_start; // we do tmp change
			CHAR c = *pt;
			*pt = 0;
				aaa::alphabet::draw_str_ui( (CHAR*) str );
			*pt = c;
			aaa::alphabet::maa::translate_char( aaa::alphabet::maa::compute_str_move( (UINT8*)pt, clean_len ) );
			aaa::alphabet::draw_str_ui( pt + clean_len );
			clean_len = 0;
		}
		else
			aaa::alphabet::draw_str_ui( (CHAR*) str );
	}

	static FINLINE	void	draw_str_line_clipped( C_PCHAR_C str )
	{
		if( is_line_not_clipped() )
			draw_str_ui( str );
	}

	static FINLINE bool	draw_str_spe( C_PCHAR_C str, PCHAR mark, bool b_on_focus, bool b_on_focus_menu, C_PCHAR_C pat, INT32 pat_len, INT32 color  )
	{
		if( str_is_equal_nocase( mark+1, pat, pat_len ) )
		{
			CHAR	c = *mark;
			*mark = 0;
			draw_str_ui( str );
			aaa::alphabet::draw_str_set_color(	color, b_on_focus, b_on_focus_menu );
			*mark = c;
			aaa::alphabet::draw_str_ui( mark );
			return true;
		}
		return false;
	}

	static	FINLINE	void	draw_str_2color_line_clipped( CHAR* CONST str, COLOR_TYPE CONST color, bool CONST b_on_focus, bool CONST b_on_focus_menu )
	{
		CHAR* mark = str + strlen( str );
		//todo	pass info from caller to avoid this search which infact will be wrong on GROUP for example
		while( *--mark != ' ' )
		{
			if( *mark == '\"' )
			{
				while( *--mark != '\"' )
				{
					if( mark == str )
					{
						draw_str_ui( str );
						return;
					}
				}
			}
			if( mark == str )
			{
				draw_str_ui( str );
				return;
			}
		}

		++mark;
		if( *mark == 'O' || *mark == 'o' )
		{
			if(			draw_str_spe(	str,	mark,	b_on_focus, b_on_focus_menu,	"N",	1,	COLOR_ON		)	)	// On
				return;
			else if(	draw_str_spe(	str,	mark,	b_on_focus, b_on_focus_menu,	"FF",	2,	COLOR_OFF		)	)	// Off
				return;
			else if(	draw_str_spe(	str,	mark,	b_on_focus, b_on_focus_menu,	"wner",	4,	COLOR_ON		)	)	// Owner
				return;
			else if(	draw_str_spe(	str,	mark,	b_on_focus, b_on_focus_menu,	"nce",	3,	COLOR_ON		)	)	// Once
				return;
		}
		else if( *mark == 'N' || *mark == 'n' )
		{
			if(			draw_str_spe(	str,	mark,	b_on_focus, b_on_focus_menu,	"one",	3,	COLOR_NODE_2	)	)	// None
				return;
			/*
			if( str_is_equal_nocase( mark, "one", 3 ) )
			{
				aaa::alphabet::draw_str_ui( str );
				return;
			}
			*/
		}
		else if( *mark == '\"' && *(mark+1) == '\"' )
		{
			draw_str_ui( str );
				return;
		}
		*(mark-1) = 0;
			draw_str_ui( str );
		*(mark-1) = ' ';
		aaa::alphabet::draw_str_set_color(	color, b_on_focus, b_on_focus_menu );
		aaa::alphabet::draw_str_ui( mark );

		//else if(	draw_str_spe( str, len, b_on_focus, b_on_focus_menu, "none",	4, COLOR_OFF	)	)	return;
		//else		draw_str_ui( str );
	}
}

namespace {
	bool	b_draw;
	FINLINE bool param_spe_draw_one_line( PCHAR dst, C_PCHAR_C draw, C_PCHAR_C name, INT32 type, void CONST * CONST pt, C_PCHAR_C * CONST symbolic = nullptr )
	{
		c_param::sprint_ui( dst, STR_PARAM_SIZE_UI, name, type, pt, symbolic );
		draw_str_line_clipped( draw );
		aaa::alphabet::draw_cr_ui();
		++draw_line;
		return !is_line_before_end();
	}
}


p_param	c_param::draw( CHAR sy_cont )
{
	if( !mem::is_pointer_valid(this) )
		return nullptr;
#if	AAA_DEBUG()
//	if( !pointer_is_valid(_pt)  )
//		return nullptr;
	if( !mem::is_pointer_valid(_data) )
		return nullptr;
#endif

	CHAR*		str_pt;
	CHAR*		str_start;
	c_obj_ui*	obj;
	c_obj_ui*	obj_sum_up = nullptr;
	bool		b_on_focus;
	bool		b_on_focus_menu;
	bool		b_draw_this_one;
	p_param		param_found;

	b_draw_this_one = b_draw && is_line_after_begin();

	if( obj = get_obj() )
	{
		obj->param_init_pt();
		obj->prepare_for_ui();
		obj_dive[draw_level] = obj;
		obj_sum_up = obj;
	}
	else if( draw_level > 0 )
		obj_dive[draw_level] = obj_dive[draw_level-1];
	else
		obj_dive[0] = nullptr;
	obj = obj_dive[draw_level];
	
	INT32 dbg_display_cnt = is_dbg_display() ? 11 : 0;
	if( !b_draw )
	{
		dive_line += dbg_display_cnt + get_display_line_nb();
		if ( dive_line >= dive_line_to_find )
		{
			aaa::param::flat::obj_found = obj;
			return this;
		}
	}
	
	INT32 list_nb = 0;
	bool b_param_under = false;
	bool b_obj_under = false;
	INT32 param_attached_nb = get_param_attached_nb();
	c_param* param_attached = nullptr;
	INT32 expand_cnt;
	INT32 expand_in_nb;
	INT32 expand_out_nb;
	if( draw_level < DRAW_RECURSION_MAX )
	{		
		if( is_group() )
		{
			b_param_under = true;
			list_nb = is_expand_list() ? INT32(get_def()) : 0;
			if( list_nb > 0 )	//careful with empty group
			{	
				// group is like attaching the next param
				param_attached = this;
				++param_attached;
			}
		}
		else
		{
			b_param_under = param_attached_nb > 0;
			list_nb = get_expand_list_nb();
			if( b_param_under )	//careful with empty group
			{
				param_attached = get_param_attached();
				b_obj_under = param_attached->is_obj();
			}
		}
		expand_cnt = list_nb;
		if( is_header() )
		{
			expand_in_nb = 0;
			expand_out_nb = 0;
		}
		else
		{
			expand_in_nb = get_expand_in_nb();
			expand_out_nb = get_expand_out_nb();
			expand_cnt += expand_in_nb + expand_out_nb;
		}
	}
	else
	{
		expand_cnt = 0;
		expand_in_nb = 0;
		expand_out_nb = 0;
	}

	draw_line += get_display_line_nb();
	b_on_focus = false;
	b_on_focus_menu = false;
	if( this == focus_param::get_param() && obj == focus_param::get_obj() )	//par will change
	{
		focus_param::set_line( draw_line );
		b_on_focus = true;
	}
	if( b_param_hilite_menu_focus && this == focus_param::get_menu_param() && obj == focus_param::get_menu_obj() )
		b_on_focus_menu = true;
	if( is_referenced() )
		str_param[0] = SY_REF;
	else
		str_param[0] = SY_BLANK;
	str_start = str_param + draw_level;
	*(str_start-1) = sy_cont; 
	str_pt = str_start;
	if( is_out() || is_in() || b_param_under ) //todo should be in the flags
	{
		if( is_last() || is_obj() )
		{
			if( expand_cnt )
				*str_pt++ = is_obj() ? SY_OBJ_EMPTY_END : SY_BOX_EMPTY_END;
			else
				*str_pt++ = is_obj() ? SY_OBJ_PLUS_END : SY_BOX_PLUS_END;
		}
		else
		{
			if( expand_cnt )
				*str_pt++ = SY_BOX_EMPTY;
			else
				*str_pt++ = SY_BOX_PLUS;
		}

		if( is_lock() )
			*str_pt++ = SY_LOCK;
		else if( expand_cnt )
			*str_pt++ = SY_T_DOWN;
		else
		//	*str_pt++ = draw_recursion_mark;
			*str_pt++ = b_obj_under ? SY_T_DOWN : draw_recursion_mark;
	}
	else
	{
		if( !is_last() )
			*str_pt++ = SY_RIGHT;
		else
			*str_pt++ = SY_RIGHT_END;

		if( is_lock() )
			*str_pt++ = SY_LOCK;
		else
			*str_pt++ = draw_recursion_mark;
	}
	draw_recursion_mark = SY_LINE_HORI;

	if( b_draw_this_one )
	{
		//todo we could do better for unused but this is fast and enough for now (2024 Feb)
		aaa::alphabet::draw_str_set_color(	is_unused() ?	COLOR_ITEM_UNUSED 
														:	(b_param_under ? COLOR_NODE + CLAMP( draw_level-2, 0, 4 ) : COLOR_ITEM ),
											b_on_focus,
											b_on_focus_menu ) ;
		//	get full str
		//todo	call a fn to get text on one hand and arg on the other one
		//bool b_line_2color =
		sprint( str_pt, 512 );	//hack 512

		//	indent if suitable
		if( dbg_display_cnt || ( *str_pt == '\n' ) )
		{
		}
		else
		{	//todo reenable and optimize again
			//	keep full string
			strcpy( str_param_buf, str_start );
			//	optimize str_param with blanks
			CHAR*	cur = str_start + 2;	//we keep the first char
			CHAR*	old = cleaning_str + draw_level + 2;
			if( strcmp( old, cur ) != 0 )
			{
				INT32 i = 0;

				while( *cur == *old )
				{
					//*cur++ = ' ';
					++cur;
					++old;
					++i;
				}
				if( *old != ' ' )
				{
					--old;
					while( !( *old==' ' || *old=='_' || *old==0 ) && i-- )
					{
						*--cur = *old--;
					}
				}
				if( i > 0 )
				{
					clean_len = i;
					clean_start = draw_level + 2;
				}
				//	store full string
				strcpy( cleaning_str + draw_level, str_param_buf );
			}
			else
			{
			}
		}

		//	here we really set the color and draw
		if(	obj && obj->is_class<c_trax>() )
		{
//			aaa::alphabet::draw_str_set_color(	((c_obj_active_ui*)obj)->is_active()?COLOR_ON:COLOR_OFF, b_on_focus, b_on_focus_menu );
			aaa::alphabet::draw_str_set_color(	((c_obj_ui*)obj)->is_active() ? COLOR_ON : COLOR_OFF, b_on_focus, b_on_focus_menu );
		}

		if( is_line_not_clipped() )
			draw_str_2color_line_clipped( str_param, is_unused() ? COLOR_TITLE_UNUSED : COLOR_TITLE, b_on_focus, b_on_focus_menu );

		o_str*	com;
		o_str*	ref;
		if( obj && obj->get_param_header() == this )
		{
			com = obj->get_comment();
			if( com && com->is_empty() )
				com = nullptr;
			ref = &obj->get_name_symbo();
			if( ref && ref->is_empty() )
				ref = nullptr;
		}
		else
		{
			com = nullptr;
			ref = nullptr;
		}

		if( !com && !ref && param_attached ) //	is_list avoid phantom comment when no layer
		{
			c_obj_ui* CONST obj = param_attached->get_obj();
			if( obj )
			{
				if( !com  )	
					com = obj->get_comment();
				if( com && com->is_empty() )
					com = nullptr;
				if( !ref  )	
					ref = &obj->get_name_symbo();
				if( ref && ref->is_empty() )
					ref = nullptr;
				obj_sum_up = obj;
			}
		}
		//	this draw the layer comment after the layer_? param in layers
		//	if( str_is_equal_nocase( cleaning_str+draw_level+2, "Layer_", 6) || str_is_equal_nocase( str_pt, "Layer_", 6) )
		if( com || ref )
		{
//			aaa::alphabet::draw_str_set_color(	list_nb!=0 ? COLOR_NODE+CLAMP(draw_level-2,0,4) : COLOR_ITEM,
			aaa::alphabet::draw_str_set_color(	b_param_under ? COLOR_NODE+CLAMP(draw_level-2,0,4) : COLOR_ITEM,
												b_on_focus,
												b_on_focus_menu ) ;
			CHAR*	pt = str_param_buf;
			if( ref )
			{
				*pt++ = ' ';
				strcpy( pt+1, ref->get() );
				*pt = '(';
				pt += ref->get_len();
				*++pt = ')';
				*++pt = 0;
			}
			if( com )
			{
				*pt++ = ' ';
				if( ref )
					*pt++ = ' ';
				//no control code now was : strcpy( pt, com->get() );
				C_PCHAR src = com->get();
				CHAR c;
				do 
				{
					c = *src++;
					if( OUTSIDE_MIN_MAX( c, CHAR(1), CHAR(31) ) )
						*pt++ = c;
				}
				while( c );
			}
			aaa::param::flat::scale_comment_trax_before();
				draw_str_line_clipped( str_param_buf );
			aaa::param::flat::scale_comment_trax_after();
			/*
			strcpy( &str_param[tmp], com->get() );
			str_param[ tmp+com->get_len()] = 10;
			str_param[ tmp+com->get_len()+1] = 0;
			draw_str_line_clipped( &str_param[tmp]);
			str_param[tmp] = 0;
			*/
		}
		else
		{
			//todo understand this one
			if( !obj->is_class<c_trax>() )
			{
				c_obj_ui* owner = find_obj_owner();
				if( owner && owner != obj )
				{
					aaa::alphabet::draw_str_set_color(	COLOR_NODE, b_on_focus, b_on_focus_menu ) ;
					sprintf( str_param_buf, " %.120s %.128s", owner->get_class_name(), owner->get_name_search_str() );
					//		str_param_buf[ com->get_len()] = 10;	//LF
					//		str_param_buf[ com->get_len()+1] = 0;
					aaa::param::flat::scale_comment_before();
					draw_str_line_clipped( str_param_buf );
					aaa::param::flat::scale_comment_after();
				}
			}
		}
		if( C_PCHAR_C comment = get_comment_str() )
		{
			draw_str_line_clipped( " " );
			COLOR_TYPE ct = COLOR_TYPE::COLOR_INFO;
			//if( str_start_is_equal_nocase( comment, "ON" ) == 0 )
			//	COLOR_TYPE ct = COLOR_TYPE::COLOR_ON;
			//else
			if( is_flag_error() )
				ct = COLOR_TYPE::COLOR_ERROR;
			else if( str_start_is_equal_nocase( comment, "off" ) || str_start_is_equal_nocase( comment, "invalid" ) )
				ct = COLOR_TYPE::COLOR_OFF;
			aaa::alphabet::draw_str_set_color(	ct, b_on_focus, b_on_focus_menu );
			draw_str_line_clipped( comment );
			//if( is_line_not_clipped() )
			//	aaa::alphabet::romain::draw_str( (CHAR*) comment );
		}
		if( obj_sum_up && obj_sum_up->is_draw_sum_up() )
		{
			aaa::param::flat::scale_comment_trax_before();
				//obj_sum_up->update();
				GOL::matrix::scale_x( c_flatland::get_char_ratio_y() );
					obj_sum_up->draw_sum_up();
				GOL::matrix::scale_x( c_flatland::get_ratio_char_x() );
				aaa::alphabet::draw_str_set_color( is_unused() ? COLOR_TITLE_UNUSED : COLOR_TITLE );	
			aaa::param::flat::scale_comment_trax_after();
		}
		aaa::alphabet::draw_cr_ui();

		if( !is_line_before_end() )
			return nullptr;

		//todofred put this flag at global debug level
		if( dbg_display_cnt )
		{
			//		*str_pt++ = ' ';
			--str_pt;
			*str_pt++ = '|';
			//param set these debug pref somewhere
			{
				auto CONST tmp = get_type();
				if( param_spe_draw_one_line( str_pt, str_param, "is", TYPE_SYMBOLIC, &tmp, type_str_array ) )
					return nullptr;
			}
			{	//todo we are limited to 32 bits here
				if( param_spe_draw_one_line( str_pt, str_param, "at", TYPE_BIT32, &_pt) )
					return nullptr;
			}
			{	//todo we are limited to 32 bits here
				if( param_spe_draw_one_line( str_pt, str_param, "value in hex", TYPE_BIT32, get_pt_const_void() ) )
					return nullptr;
			}
			{
				REAL tmp = get_ina();
				if( param_spe_draw_one_line( str_pt, str_param, "ina", TYPE_FP32, &tmp ) )
					return nullptr;
			}
			{
				REAL tmp = get_def();
				if( param_spe_draw_one_line( str_pt, str_param, "def", TYPE_FP32, &tmp ) )
					return nullptr;
			}
			{
				REAL tmp = get_min();
				if( param_spe_draw_one_line( str_pt, str_param, "min", TYPE_FP32, &tmp ) )
					return nullptr;
			}
			{
				REAL tmp = get_max();
				if( param_spe_draw_one_line( str_pt, str_param, "max", TYPE_FP32, &tmp ) )
					return nullptr;
			}
			{
				if( param_spe_draw_one_line( str_pt, str_param, "flags", TYPE_BIT32, &_flags ) )
					return nullptr;
			}
			{
				INT32 tmp = get_out_nb();
				if( param_spe_draw_one_line( str_pt, str_param, "out_nb", TYPE_INT32, &tmp ) )
					return nullptr;
			}
			{
				INT32 tmp = get_in_nb();
				if( param_spe_draw_one_line( str_pt, str_param, "in_nb", TYPE_INT32, &tmp ) )
					return nullptr;
			}
			{
				INT32 tmp = param_attached_nb;
				if( param_spe_draw_one_line( str_pt, str_param, "param_attached_nb", TYPE_INT32, &tmp ) )
					return nullptr;
			}
			//{
			//	param_ui_sprint( str_pt, STR_PARAM_SIZE_UI, "parent", TYPE_ADDR, &(param->parent));
			//	draw_str_line_clipped( str_param);
			//}
			//{
			//	param_ui_sprint( str_pt, STR_PARAM_SIZE_UI, "child ", TYPE_ADDR, &(param->child ));
			//	draw_str_line_clipped( str_param);
			//}
			strcpy( cleaning_str, str_param+1 );
		}
	}


	if( expand_cnt )
	{
//		*str_start = 'X';	//sy_cont;
		param_no_repeat[draw_level+2] = this;
		++draw_level;
		if( expand_out_nb ) 
		{
			for( INT32 i=0 ; i<expand_out_nb && expand_cnt>0 ; ++i )
			{
#if	AAA_DEBUG()
//todo move this to param_more ?
				if( !get_out(i) )
					debug_break();
#endif
				c_param * CONST par = get_out(i)->get_out_param();
				draw_recursion_mark = (obj && obj->is_class<c_trax>()) ? SY_TRAX : SY_OUT;
				--expand_cnt;
				if( par != param_no_repeat[draw_level] )
				{
					param_found = par->draw( expand_cnt ? SY_LINE_VERT : SY_BLANK );
					if( param_found && !b_draw )
						goto exit;
					*(cleaning_str+draw_level+2) = 0;
				}
			}
		}
		if( expand_in_nb )
		{
			for( INT32 i=0; i<expand_in_nb && expand_cnt>0; ++i )
			{
#if	AAA_DEBUG()
//todo move this to param_more ?
				if( !get_in(i) )
					debug_break();
#endif
				c_param * CONST par = get_in(i)->get_in_param();
				draw_recursion_mark = (obj && obj->is_class<c_trax>()) ? SY_TRAX : SY_IN;
//				draw_recursion_mark = SY_IN;
				--expand_cnt;
				if( par != param_no_repeat[draw_level] )
				{
//					obj = get_in(i)->get_in_obj();
//					obj->prepare_for_ui();
					param_found = par->draw( expand_cnt ? SY_LINE_VERT : SY_BLANK );
					if( param_found && !b_draw )
						goto exit;
					*(cleaning_str+draw_level+2) = 0;
				}
			}
		}
		if( INT32 i = list_nb )
		{
			//	VERBOSE_PRINTF("expand count %d %8x", i, st);
			draw_recursion_mark = SY_LINE_HORI;
			CHAR char_next = SY_LINE_VERT;
			if( is_last() )
				char_next = SY_BLANK;
			else if( is_obj() )
				char_next = SY_BLANK;
			if( b_draw_this_one )
				*str_start = char_next;	//'0' + draw_level;
//			str_param[draw_level-1] = char_next;
			cleaning_str[draw_level-1] = char_next;
			for( ; i>0 && expand_cnt>0; --i )
			{
				//	VERBOSE_PRINTF("i %d %8x %s", i, st, st->par_name.get() );
//				draw_recursion_mark = SY_LINE_HORI;
				--expand_cnt;

				param_found = param_attached->draw( char_next );	//&& get_obj()
				//param_found = par->draw( ((is_group()&&expand_cnt) || get_obj()) ? SY_LINE_VERT : SY_BLANK );
				if( param_found && !b_draw )
					goto exit;
				if( !is_line_before_end() )
					return nullptr;

				//if ( param_attached->is_group() ) )	//|| (param_attached->get_list() && ( param_attached->get_type() != TYPE_NONE )) ) ) //was a test
				//if( param_attached->is_group() || (param_attached->get_data_unsafe_maa()->get_type_maa()&aaa::param::M_MAA_FLAG) ) )
				if( param_attached->is_group() )
				{
					INT32 inc = INT32(param_attached->get_def());
					i -= inc;
					param_attached += inc;
				}
				++param_attached;
			}
		}
		--draw_level;
	}
	return  nullptr;
exit:
	--draw_level;
	return  param_found;
}

static	void	param_draw_dive_start()
{
	draw_level = 0;
	draw_line = 0;
	draw_top_line = get_int32_param_line_offset();

	if( draw_top_line > line_drawn -10 )
	{
		set_real_param_line_offset( REAL(line_drawn - 10) );
		draw_top_line = get_int32_param_line_offset();
	}
	draw_bottom_line = c_flatland::get_line_bottom();

	clean_char_tra_x	=	0.;
	clean_len			=	0;
	clean_start			=	0;
}

static	p_param param_list_dive( p_param param, INT32 line_to_find )
{
	dive_line = 0;
	dive_line_to_find = line_to_find;

	b_draw = false;
	param_draw_dive_start();		
	return param->draw( SY_LINE_VERT );
}

static	AAA_ERR	param_list_draw( c_obj_ui* obj )
{
	if( !obj )
	{
		debug_break( "%s() null obj", __FUNCTION__ );
		return ERR_ANY;
	}

	p_param param = obj->get_param_header();
	if( !param )	// can be null when obj deleted
	{
		ERR_PRINT_STRING( "%s() null param_header, deleted obj_ui ?", __FUNCTION__ );
		return ERR_ANY;
	}

	//	clear the string used
	MEMCLEAR(	cleaning_str,	sizeof(cleaning_str)	);
	MEMSET(		str_param,		32,	sizeof(str_param)	);
	str_param[sizeof(str_param)-1] = 0;

	GOL::push_att();
		GOL::push_polygon_mode( GL_FRONT, GL_FILL );

		draw_recursion_mark = obj->is_class<c_trax>() ? SY_TRAX : SY_LINE_HORI;

		b_draw = true;
		param_draw_dive_start();
		param->draw( SY_LINE_VERT );
	GOL::pop_att();
	line_drawn = draw_line;

	return	AAA_OK;
}


p_param param_get_from_line( INT32 line )
{
	if( line >= 0 )
		return param_list_dive( focus_param::get_param_list(), line );
	return nullptr;
}

/*
void	param_draw_title()
{
	since we don't use the arrows anymore
	INT32	ind;
	INT32	i;

	ind = IMOD(fifo_index_cur-1,fifo_index_max);
	for( i=3; i>0; --i )
	{
		if ( i==2 )
			draw_str_set_color( COLOR_TITLE, false, b_param_hilite_menu_focus && !focus_menu_param );
		else
			draw_str_set_color( COLOR_TITLE_NEXT, false, false);
		draw_str( fifo[ind].name );
		draw_str( " " );
		ind = IMOD( ind+1, fifo_index_max );
	}
	draw_str( "\n" );
}
*/

//todo str_start change the interface stuff
void	param_draw( c_obj_ui* obj )
{
	aaa::param::flat::scale_update();
	aaa::alphabet::draw_str_begin();	//we use font 0 (Maa) which is adapted for flatland Param

		c_flatland::draw_focus_info();

		aaa::alphabet::draw_cr_ui();
		param_list_draw( obj );
		//	draw_str("abcdefghijklm\nnopqrstuvwxyz\nABCDEFGHIJKLM\nNOPQRSTUVWXYZ\n0123456789_+-\n");

	aaa::alphabet::draw_str_end();
}
