#include "obj_ui/bdd/bdd_spe/bdd_chat.h"
#include "infrastructure/data/datacube.h"
#include "infrastructure/data/dataline.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif
#include "draw/model.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "obj_ui/bdd/bdd_geo/bddtex2d.h"
#include "file/asc_parser.h"

c_chat_mess::c_chat_mess()
{
	_line_nb = 0;
	_pseudo = nullptr;
}

c_chat_mess::~c_chat_mess()
{
	obj_delete( _pseudo);
	while( !_lines.empty() )
	{
		obj_delete( *_lines.begin() );
		_lines.pop_front();
	}
}

c_bdd_tex2d*	c_chat_mess::get_line( INT32 index )
{
	if( index >= INT32(_lines.size()) )		//todostl check better
	{
		c_bdd_tex2d*	p_tex = nullptr;
		obj_get( p_tex );	//todo should be this ? use obj_ui/root ?
		_lines.push_back(p_tex);
	}
	return _lines[index];
}

FACTORY_CREATE_PROP_V1( c_bdd_chat, bdd_chat, Chat, bdd_chat, sub_menu="Old"; );

void	c_bdd_chat::update_chat_mess( c_chat_mess* pcm, C_PCHAR_C pseudo_str, C_PCHAR_C mess_str )
{
	c_bdd_tex2d*	p_tex;
	INT32			line_index;	
	REAL			pos;
	REAL			new_pos;
	REAL			len;
	REAL			blank_len = font_get_width( " ", _s_font, false )*_font_size_over_one;	//todo deal with outline
	c_asc_parser	parser;
	bool			b_start_line;

	if( _b_debug )
		DBG_PRINT_STRING( "UPDATE MESS" );

	obj_get( pcm->_pseudo );	//todo should be this ?

	pos = _pos_pseudo;

	p_tex = pcm->_pseudo;
	p_tex->_s_axe = _s_axe;
	p_tex->_s_type = _s_type;
	p_tex->_s_font = _s_font_pseudo;
	p_tex->_align_hori = 0;
	cpy_v3( p_tex->_origin, _origin );	//because of rotation in bdd_tex ?
//	if( b_debug)
//		DBG_PRINT_STRING( "UPDATE MESS 0" );
	p_tex->_origin[ _i_u ] += pos;
	p_tex->set_text( pseudo_str );
	p_tex->update();
	pos += font_get_width( pseudo_str, _s_font_pseudo, false ) * _font_pseudo_size_over_one;	//todo deal with outline
	pos += _pos_pseudo_after;

	_ostr.set( mess_str );
	parser.set_start( (CHAR*) _ostr.get() );	//	parser change content sometimes
	line_index = 0;	

//	if( b_debug)
//		DBG_PRINT_STRING( "UPDATE MESS 1" );
	p_tex = pcm->get_line( line_index );
	p_tex->_s_axe = _s_axe;
	p_tex->_s_type = _s_type;
	p_tex->_s_font = _s_font;
	p_tex->_align_hori = 0;
	cpy_v3( p_tex->_origin, _origin );
	p_tex->_origin[ _i_u ] += pos;
	p_tex->_text.erase();

//	if( b_debug)
//		DBG_PRINT_STRING( "UPDATE MESS 2" );
	b_start_line = true;
	while( parser.get_next_word() )
	{
//		if( b_debug)
//			DBG_PRINT_STRING( "UPDATE MESS a" );
		len = font_get_width( parser.get_word(), _s_font, false )*_font_size_over_one;	//todo deal with outline
//		if( b_debug)
//			DBG_PRINT_STRING( "UPDATE MESS b" );
		new_pos = pos + len;
		if( !b_start_line )
			new_pos += blank_len;
		if( new_pos > _pos_right )
		{
			p_tex->update();
//		if( b_debug)
//			DBG_PRINT_STRING( "UPDATE MESS c" );

			++line_index;
			p_tex = pcm->get_line( line_index );
			p_tex->_s_axe = _s_axe;
			p_tex->_s_type = _s_type;
			p_tex->_s_font = _s_font;
			p_tex->_align_hori = 0;
			cpy_v3( p_tex->_origin, _origin);
			p_tex->_origin[ _i_u ] += _pos_left;
			p_tex->_text.erase();
//		if( b_debug)
//			DBG_PRINT_STRING( "UPDATE MESS d" );

			b_start_line = true;
			pos = _pos_left + len;
			p_tex->_text.add( parser.get_word());
//			if( b_debug)
//				DBG_PRINT_STRING( "UPDATE MESS : %s", parser.get_word() );
		}
		else
		{
//		if( b_debug)
//			DBG_PRINT_STRING( "UPDATE MESS d bis" );
			p_tex->_text.add_space();
//			if( b_debug)
//				DBG_PRINT_STRING( "UPDATE MESS e bis: %s", parser.get_word() );
			p_tex->_text.add( parser.get_word());
//			if( b_debug)
//				DBG_PRINT_STRING( "UPDATE MESS : %s", parser.get_word() );
			pos += len + blank_len;
			b_start_line = false;
		}
	}
	p_tex->update();

	pcm->_line_nb = line_index + 1;

}

void	c_bdd_chat::draw_chat_mess( c_chat_mess* pcm)
{
	c_bdd_tex2d*	p_tex;
	obj_get( pcm->_pseudo );	//todo should be this ?

	p_tex = pcm->_pseudo;
	p_tex->draw();

	p_tex = pcm->get_line( 0 );
	p_tex->draw();
}


/*
void	c_chat_mess::set_text( o_str* in_pseudo, o_str* in)
{
}
void	c_chat_mess::set_font( INT32 font_pseudo, INT32 font )
{
}
*/

//static	INT32 CONST	BDD_CHAT_PARAM_NB_MAX = 28
//											+ BDD_BASE_PARAM_NB;

//careful same than 
static	INT32 CONST TYPE_MAX_NB = 4;	
static	C_PCHAR_C	type_str[TYPE_MAX_NB] =
{
	"GLUT_LINE",
	"MAA_LINE",
	"FONT_OUTLINE",
	"FONT_FILL",
};

static	CONST	INT32	COL_MAX = 128;

namespace n_bdd_chat
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	28 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32(			datagrid_id,		2, 1,	0, DATAGRID_NB	)
		PARAM_DEF_INT32(			pseudo_id,			2, 1,	0, COL_MAX	)
		PARAM_DEF_INT32(			message_id,			1, 2,	0, COL_MAX	)

		PARAM_DEF_INT32(			line_min,			1, 16,	1, PARAM_MAX_INT32	)
		PARAM_DEF_INT32(			line_speed_max,		1, 16,	1, PARAM_MAX_INT32	)
		PARAM_DEF_INT32(			line_nb,			1, 16,	1, PARAM_MAX_INT32	)
		PARAM_DEF_REAL_ZERO_SAVE_NOT( line_offset )	//todo 	we don't save it to avoid to save empty bdd_chat 
													//		if used again should be refine with a second parameter
		PARAM_DEF_REAL_POS_ZERO(	speed_min			)
		PARAM_DEF_REAL_POS_ONE(		speed_max			)

		PARAM_DEF_REAL_ZERO(		pos_pseudo			)
		PARAM_DEF_REAL_ZERO(		pos_pseudo_after	)
		PARAM_DEF_REAL_ZERO(		pos_left			)
		PARAM_DEF_REAL_ONE(			pos_right			)

		PARAM_DEF_POINT_XYZ(		origin				)
		PARAM_DEF_AXE_X(			axe					)

		PARAM_DEF_SCALE_UVAF(		font_size			)

		PARAM_DEF_SYMBO_PSTR(		type,				0, 3,	type_str )
		PARAM_DEF_INT32(			font_pseudo,		1, 0,	0, FONT_MAX_NB-1	)
		PARAM_DEF_INT32(			font,				1, 0,	0, FONT_MAX_NB-1	)

		{	nullptr,	PARAM_STR,		"pseudo_prefix",	1, 0,	0, 0,	nullptr, nullptr },
		{	nullptr,	PARAM_STR,		"pseudo_postfix",	1, 0,	0, 0,	nullptr, nullptr },

		PARAM_DEF_BOOL_OFF(			debug )
		PARAM_DEF_INT32_LOCKED(		line_ready )

	//	{	nullptr,	PARAM_REAL,	"align_horizontal",	0., 1.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//	{	nullptr,	PARAM_REAL,	"align_vertical",	0., 1.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	};
}

void	c_bdd_chat::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _datagrid_id );
	param_set_pt( h, _pseudo_id );
	param_set_pt( h, message_id );

	param_set_pt( h, _line_min );
	param_set_pt( h, _line_speed_max );
	param_set_pt( h, _line_nb );
	param_set_pt( h, _line_offset );

	param_set_pt( h, _speed_min );
	param_set_pt( h, _speed_max );

	param_set_pt( h, _pos_pseudo_ui );
	param_set_pt( h, _pos_pseudo_after_ui );
	param_set_pt( h, _pos_left_ui );
	param_set_pt( h, _pos_right_ui );

	param_set_pt_3( h, _origin_ui );
	param_set_pt( h, _s_axe );

	param_set_pt_4( h, _font_size_ui );

	param_set_pt( h, _s_type );
	param_set_pt( h, _s_font_pseudo );
	param_set_pt( h, _s_font );

	param_set_pt( h, _pseudo_prefix );
	param_set_pt( h, _pseudo_postfix );

	param_set_pt( h, _b_debug );
	param_set_pt( h, _line_ready );

	err_param_init_pt(h);
}

void c_bdd_chat::init()
{
	_s_type_last = -1;
	_s_font_last = -1;
	_s_font_pseudo_last = -1;
	_mess_nb	 = 0;
	_mess_index = 0; 

	_b_first = true;

	_line_ready = 0;
}

CONSTRUCTOR_CREATE( c_bdd_chat )
{
	param_init_with( n_bdd_chat::param, n_bdd_chat::PARAM_NB_MAX ); // bdd_chat_param, BDD_CHAT_PARAM_NB_MAX);

	init();
}

c_bdd_chat::~c_bdd_chat()
{
	deinit();
}

void	c_bdd_chat::deinit()
{
	if( _messages.size() > 0 )
		_messages.clear();
}

void c_bdd_chat::mess_pop()
{
	if( _mess_index > 0 )
	{	//todo jump several
		c_chat_mess*	p_chat_mess = _messages[0];
		_messages.pop_front();
		_messages.push_back( p_chat_mess );
		--_mess_index;
		g_datacube->pop_row( _datagrid_id );
		_line_offset -= p_chat_mess->_line_nb;
		_line_ready -= p_chat_mess->_line_nb;
	}
}

void c_bdd_chat::mess_restart()
{
	_mess_index = 0;
	_line_ready = 0;
}

void c_bdd_chat::mess_alloc( INT32 mess_nb_in )
{
	if( _mess_nb <= mess_nb_in )
	{
		while( _mess_nb <= mess_nb_in )
		{
			// todofranz dealloc
			c_chat_mess* p_chat_mess = new c_chat_mess;
			_messages.push_back( p_chat_mess );
			_mess_nb = (INT32)_messages.size();	//todostl this should be optimized
		}
		mess_restart();
		mess_update();
	}
}

void c_bdd_chat::mess_update()
{
	INT32	row_nb = g_datacube->get_row_nb( _datagrid_id );		
	
	while( 	_mess_index < _mess_nb && _mess_index < row_nb )
	{
/*
		CHAR*	pseudo;
		pseudo =	g_datacube->get_str( datagrid_id, mess_index+1, pseudo_id);
*/
		o_str				pseudo;

		pseudo.set( _pseudo_prefix.get() );
		pseudo.add( g_datacube->get_str( _datagrid_id, _mess_index+1, _pseudo_id) );
		pseudo.add( _pseudo_postfix.get() );

		C_PCHAR_C mess = g_datacube->get_str( _datagrid_id, _mess_index+1, message_id);

		if( _b_debug)
			DBG_PRINT_STRING( "BUILD MESS : %s %s", pseudo.get(), mess );
		update_chat_mess( _messages[_mess_index], pseudo.get(), mess );
		_line_ready += _messages[_mess_index]->_line_nb;
		if( _b_debug)
			DBG_PRINT_STRING( "BUILD MESS DONE" );
		++_mess_index;
	}
}

//todo add verbose
void c_bdd_chat::update()
{
	bool	b_recompute = false;

	if( _b_first )
	{
		INT32	layers_index = c_layers::get_cur()->get_index();
		//todo is this clean ?
		_layers_pseudo = c_module::get_cur()->layers_get_from_index( layers_index-2 );
		_layers_text = c_module::get_cur()->layers_get_from_index( layers_index-1 );

		_b_first = false;
	}

	if( _delta_t.update() )
	{
		_line_offset = 0;
		mess_restart();
		b_recompute = true;
	}

	axe_build_index( _i_u, _i_v, _s_axe );

//	if( s_type >= 2)
//	{
		c_model::cur->get_size_v3(_size);
		if( is_diff_v3( _size_last, _size) )
		{
			cpy_v3( _size_last, _size );
			b_recompute = true;
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause size" );
		}

		scale_v3( _font_size, _font_size_ui, _font_size_ui[3] );
		if( is_diff_v3( _font_size_last, _font_size) )
		{
			cpy_v3( _font_size_last, _font_size );
			b_recompute = true;
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause font_size" );
		}

		_font_size[1] /= _size[1];
		_origin[0] = _origin_ui[0]/_font_size[_i_u];
		_origin[1] = _origin_ui[1]/_font_size[_i_v];
		_origin[2] = _origin_ui[2]/_font_size[_s_axe];

		_pos_pseudo = _pos_pseudo_ui * _size[0] / _font_size[0];
		if( _pos_pseudo_last != _pos_pseudo)
		{
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause pos_pseudo %f %f", _pos_pseudo_last, _pos_pseudo);
			_pos_pseudo_last = _pos_pseudo;
			b_recompute = true;
		}
		_pos_pseudo_after = _pos_pseudo_after_ui * _size[0] / _font_size[0];
		if( _pos_pseudo_after_last != _pos_pseudo_after)
		{
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause pos_pseudo %f %f", _pos_pseudo_after_last, _pos_pseudo_after);
			_pos_pseudo_after_last = _pos_pseudo_after;
			b_recompute = true;
		}
		_pos_left = _pos_left_ui * _size[0] / _font_size[0];
		if( _pos_left_last != _pos_left )
		{
			if( _b_debug )
				DBG_PRINT_STRING( "RECOMPUTE cause pos_left %f %f", _pos_left_last, _pos_left );
			_pos_left_last = _pos_left;
			b_recompute = true;
		}
		_pos_right = _pos_right_ui * _size[0] / _font_size[0];
		if( _pos_right_last != _pos_right)
		{
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause pos_right %f %f", _pos_right_last, _pos_right);
			_pos_right_last = _pos_right;
			b_recompute = true;
		}


		if( _s_font_last != _s_font || _s_type_last != _s_type )
		{
			_s_font_last = _s_font;
			_s_type_last = _s_type;
			_font_size_over_one = 1./(font_get_size( _s_font, _s_type==2 ));// * font_size[0]);
			b_recompute = true;
			if( _b_debug )
				DBG_PRINT_STRING( "RECOMPUTE cause s_font" );
		}
		if( _s_font_pseudo_last != _s_font_pseudo || _s_type_last != _s_type )
		{
			_s_font_pseudo_last = _s_font_pseudo;
			_s_type_last = _s_type;
			_font_pseudo_size_over_one = 1./(font_get_size( _s_font_pseudo, _s_type==2 ));// * font_size[0]);
			b_recompute = true;
			if( _b_debug)
				DBG_PRINT_STRING( "RECOMPUTE cause s_font_pseudo" );
		}
//		scale_v3( size, font_size_over_one );
//		}
	if( _line_nb != 1)
		_line_inc = _size[1] / (REAL(_line_nb-1) * _font_size[1]);
	else
		_line_inc = _size[1] / _font_size[1];

	if( _line_ready > _line_min ) 
	{
		REAL	speed;
		if( _line_ready > _line_speed_max )
			speed = _speed_max;
		else if( _line_speed_max != _line_min )
			speed = _speed_max*(_line_ready-_line_min)/(_line_speed_max-_line_min);
		else
			speed = _speed_max;
		_line_offset += speed*_delta_t.get_dt();
	}
	mess_alloc( _line_nb );
	if( b_recompute )
		mess_restart();
	mess_update();
		
}

void c_bdd_chat::draw_one_line( INT32 i )
{
}

void c_bdd_chat::draw_multiple()
{
REAL			pos;
INT32			i;
c_chat_mess*	mess_cur;
INT32			mess_line_cur;
INT32			mess_index_cur;

	if( _line_offset > _messages[0]->_line_nb )
		mess_pop();

//	cpy_v3( vec, zero_v4fp32);
	pos = _origin[_i_v] + FMOD( _line_offset, 1. ) * _line_inc;

	c_multiple::cur->set_nb( _line_nb);

	mess_line_cur = I_FLOOR( _line_offset );
	mess_index_cur = 0;
	mess_cur = _messages[0];

	for( i=0; i<_line_nb; ++i )
		{
		if( mess_line_cur <= mess_cur->_line_nb && _layers_text )
			{
			mess_cur->get_line( mess_line_cur )->_origin[ _i_v ] = pos;
			_layers_text->layer_get_always( 0 )->set_bdd( mess_cur->get_line( mess_line_cur ) );
			c_multiple::cur->set_multiple_layers_cur( _layers_text->get_index() );
			c_multiple::cur->align_then_draw( zero_v4fp32, _font_size );
			}
		if( mess_line_cur==0)
			{
			if( _layers_pseudo)
				{
				if( mess_cur->_pseudo )
					{
					mess_cur->_pseudo->_origin[ _i_v ] = pos;
					_layers_pseudo->layer_get_always( 0)->set_bdd( mess_cur->_pseudo );
					c_multiple::cur->set_multiple_layers_cur( _layers_pseudo->get_index() );
					c_multiple::cur->align_then_draw( zero_v4fp32, _font_size );
					}
				}
			}
		c_multiple::cur->next_index();
		pos -= _line_inc;
		++mess_line_cur;
		if( mess_line_cur >= mess_cur->_line_nb )
			{
			++mess_index_cur;
			if( mess_index_cur >= _mess_index )
				break;
			mess_cur = _messages[mess_index_cur];
			mess_line_cur = 0;
			}
		}
	_line_drawn = i;
}


void c_bdd_chat::draw_single()
{
	draw_chat_mess( _messages[0]);
}
