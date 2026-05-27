
#include "obj_ui/bdd/bdd_edit/keys.h"
#include "gol/gol.h"
#include "time/aaa_time.h"
#include "draw/aaa_glut.h"
#include <algorithm>
#include "infrastructure/param/param_declare.h"
#include "draw/line.h"
#include "draw/shape.h"
#include "ui/keyboard.h"

#ifndef AAA_SYSTEMKEYBOARD_H
#	include "system/win32/SystemKeyboard.h"
#endif
#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

#include "obj_ui/tracker/trackers.h"

REAL	c_key::marker_size[2];
REAL	c_key::marker_size_half[2];

#define	CONTROL_KEY_HEADER  "# CONTROL_KEY "
extern	void	CONTROL_KEY_PRINT_STRING( C_PCHAR_C fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( CONTROL_KEY_HEADER, fmt, args );
	va_end(args);
}

static	FP32	color_auto_phase_blk[] =
{
	1, 0, 0, FP32(.7),
	0, 1, 0, FP32(.7),
	0, 0, 1, FP32(.7),
	1, 1, 1, FP32(.7),
};

C_PCHAR_C	str_key_type[KEY_TYPE_NB] =
{
	"Control",
	"Const",
	"Linear",
	"Sinus",
	"Catmull",
	"Catmull_test"
};

c_key::c_key()
{
	_phase = 0;
	clear_v3( _pos );
	_s_after = KEY_CONTROL;
	_b_include = true;
}

//trally omplemet thses in particular free / reuse
c_key*	c_key::get_new()				{	return new c_key;	}
void	c_key::set_free( c_key* key )	{	delete key;			}

static	CHAR str[1024];

AAA_ERR	c_key::save_v0( FILE* file )
{
	fprintf( file, "{\n" );
		sprintf( str, "\tphase = %g ;\n", _phase );									fprintf( file, str );
		sprintf( str, "\tpos_u = %g ;\n", _pos[0] );								fprintf( file, str );
		sprintf( str, "\tpos_v = %g ;\n", _pos[1] );								fprintf( file, str );
		sprintf( str, "\tpos_axe = %g ;\n", _pos[2] );								fprintf( file, str );
		sprintf( str, "\tafter = %d ;\n", _s_after );								fprintf( file, str );
		sprintf( str, "\tinclude = %s ;\n", _b_include ? "TRUE" : "FALSE" );		fprintf( file, str );
	fprintf( file, "}\n" );
	return AAA_OK;
}

AAA_ERR	c_key::load_v0( FILE* file )
{
	if ( fscanf( file, "\n{\n\tphase = %g ;\n", &_phase ) != 1 )	goto exit;
	if ( fscanf( file, "\tpos_u = %g ;\n", &_pos[0] ) != 1 )		goto exit;
	if ( fscanf( file, "\tpos_v = %g ;\n", &_pos[1] ) != 1 )		goto exit;
	if ( fscanf( file, "\tpos_axe = %g ;\n", &_pos[2] ) != 1 )		goto exit;
	if ( fscanf( file, "\tafter = %d ;\n", &_s_after ) != 1 )		goto exit;
	if ( fscanf( file, "\tinclude = %1024s ;\n}\n", str ) != 1 )	goto exit;
	_b_include = strcmp( str, "TRUE" ) == 0;
	return AAA_OK;
exit:
//	BOX_ERR( "c_key trouble to read" );
	return ERR_FILE_FORMAT;
}

c_key::~c_key()
{
}

void	c_key::param_set_pt( c_obj_ui* owner, INT32& h )
{
	owner->param_set_pt(	h, _phase		);
	owner->param_set_pt_3(	h, _pos			);
	owner->param_set_pt(	h, _s_after		);
	owner->param_set_pt(	h, _b_include	);
}
//REAL	key_size_;
//REAL	key_size_half_;
void	c_key::draw_base( INT32 type, REAL x, REAL y )
{
	if( _s_after != KEY_CONTROL )
		type = _s_after;
	switch( type )
	{
	case KEY_CONTROL:		draw_mark_unknowed(	x, y,	marker_size_half[0],	marker_size_half[1] );	break;
	case KEY_CONST:			draw_tri_line(		x, y,	marker_size_half[0],	marker_size_half[1] );	break;
	case KEY_LINEAR:		draw_plus_line(		x, y,	marker_size_half[0],	marker_size_half[1] );	break;
	case KEY_SINUS:			draw_plus_line(		x, y,	marker_size[0],			marker_size_half[1] );	break;
	case KEY_CATMULL:
	case KEY_CATMULL_BIS:	draw_mul_line(		x, y,	marker_size_half[0],	marker_size_half[1] );	break;
	}
}
FINLINE	void	c_key::draw( INT32 type, REAL phase_factor, REAL phase_offset, INT32 axe )
{
	REAL ph = _phase * phase_factor + phase_offset;
	draw_base( type, ph, _pos[axe] );
}

FINLINE	void	c_key::draw_2d( INT32 type )
{
	draw_base( type, _pos[0], _pos[1] );
}

FACTORY_CREATE_V1( c_control_key, control_key, Control Key, control_key );

//todoqq phase offset is hacked
namespace n_control_key
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 13;
	CONSTEXPR INT32	KEY_PARAM_NB	= 7;
	CONSTEXPR INT32	COLOR_PARAM_NB	= 5;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	KEY_PARAM_NB
									+	COLOR_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(				Name			)
		PARAM_DEF_BOOL_OFF(			loop			)
		PARAM_DEF_REAL_ZERO(		phase_start		)
		PARAM_DEF_REAL_ZERO(		phase_stop		)
		PARAM_DEF_REAL_ONE(			phase_factor	)
		PARAM_DEF_REAL_ZERO(		phase_offset	)
		PARAM_DEF_REAL_ZERO(		offset			)
		PARAM_DEF_REAL_ONE(			factor			)
		PARAM_DEF_POINT_XYZ(		default_value	)
//		PARAM_DEF_SYMBO_PSTR_ZERO(	control_type,	str_key_type	)
//		PARAM_DEF_SYMBO_PSTR_ONE(	control_type,	KEY_CONST, KEY_LINEAR,			str_key_type )
		PARAM_DEF_BASE_STR(			TYPE_SYMBOLIC,	control_type,		KEY_CONST, KEY_LINEAR,		KEY_CONST, PT_NB_STR(str_key_type),			str_key_type	)
		PARAM_DEF_INT32_POS_ONE(	key_nb )	//	one to avoid save for just initialized object

		PARAM_DEF_GROUP_CLOSED( Key, KEY_PARAM_NB )	
			PARAM_DEF_INT32(				key_id,		2, 1,	1, 1024	)
			// the 6 following params are saved by c_key directly so we don't need to save or load these params with the control key
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	key_phase	)
			PARAM_DEF_POINT_UVA_SAVE_NOT(	key			)
			PARAM_DEF_SYMBO_SAVE_NOT_PSTR(	key_after,	str_key_type	)
			PARAM_DEF_BOOL_ON_SAVE_NOT(		key_include )
//			PARAM_DEF_POINT_XYZ(			pos			)

		PARAM_DEF_GROUP_CLOSED( Color, COLOR_PARAM_NB )	
			PARAM_DEF_COLOR_RGBGA(	color )
	};
}

void	c_control_key::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _control_name );
	param_set_pt( h, _b_loop );
	param_set_pt( h, _phase_start );
	param_set_pt( h, _phase_stop );
	param_set_pt( h, _phase_factor );
	param_set_pt( h, _phase_offset );
	param_set_pt( h, _offset );
	param_set_pt( h, _factor );
	param_set_pt_3( h, _default_value );
	param_set_pt( h, _s_control_type );
	param_set_pt( h, _key_nb );

	++h;
		param_set_pt( h, _key_id_cur );
		 _p_key_cur->param_set_pt( this, h );

	++h;
		param_set_pt_4( h, _color_ui );
		param_set_pt( h, _color[3] );

	err_param_init_pt( h );
}

void c_control_key::init()
{
	_b_loop = false;
	_phase_start = 0.;
	_phase_stop = 0.;
	_phase_factor = 0.;
	_phase_offset = 0.;
	_offset = 0.;
	_factor = 0.;
	param_init_with( n_control_key::param, n_control_key::PARAM_NB_MAX ); // bdd_control_key_param, BDD_CONTROL_KEY_PARAM_NB_MAX);
}

void	c_control_key::key_dealloc()
{
	// dealloc
	if( !_keys.empty() )
	{
		for( auto const & elt : _keys )
			c_key::set_free( elt );
		_keys.clear();
		_key_nb = 0;
	}
}

CONSTRUCTOR_CREATE( c_control_key )
	,_key_cache(0)
{
	_p_key_cur	= push_key_new();
	_key_id_cur	= 1;
	init();
}

c_control_key::~c_control_key()
{
	// todofranz dealloc
	key_dealloc();
}

FINLINE	INT32	c_control_key::get_key_type( c_key* key )
{
	INT32 key_type = key->get_after();
	if( key_type == KEY_CONTROL )
	{
		key_type = _s_control_type;
		if( key_type == KEY_CONTROL )
		{
			DBG_PRINT_STRING( "%s() should not have KEY_CONTROL value ", __FUNCTION__ );
			key_type = KEY_LINEAR;
		}

	}
	return key_type;
}

FINLINE	REAL	c_control_key::make_phase( REAL in )	{ return ( in * _phase_factor ) + _phase_offset; }
FINLINE	REAL	c_control_key::loop_phase( REAL in )
{
	if( _b_loop )
	{
		in -= _phase_start ;
		in = FMOD( in, _phase_stop-_phase_start );
		in += _phase_start;
	}
	return in;
}
FINLINE	INT32	c_control_key::build_id( INT32 id )
{
	return IMOD( id - 1, _key_nb ) + 1;
}

c_key*	c_control_key::find_key_pt_by_id( INT32 id_in )
{
	INT32		i	=	CLAMP( INT32( id_in - 1 ), 0, INT32( _key_nb - 1 ) );
//	iter_key	it	=	_keys.begin();
//	advance( it, i );
//	return *it;
	return _keys[i];
}

class c_key_less	{	public:		bool operator() ( c_key* a, c_key* b )	{	return ( a->get_phase() < b->get_phase() );	}	};

FINLINE	void	c_control_key::sort_key()
{
	if( !_b_need_sort )
		return;
	std::sort( _keys.begin(), _keys.end(), c_key_less() );
	//	_keys.sort( c_key_less() );
	_b_need_sort = false;

}

//todo	this is ugly and costly (loop)
INT32	c_control_key::find_key_id_by_pt( c_key* pt_in )
{
	sort_key();
	iter_key	it;
	INT32		i	=	1;
	for( it = _keys.begin(); it != _keys.end(); ++it, ++i )
	{
		if( (*it) == pt_in )
			return i;
	}
	debug_break();
	return 0;
}

//old and unused
//	return id as phase <= _key[id]->phase
INT32	c_control_key::find_key_id_by_phase_low( REAL phase_in )
{
	sort_key();
#if	0
	//	linear approach
	INT32		i = 1;
	iter_key	it = _keys.begin();
	c_key*		p = *it;
	++it;
	for( ; it != _keys.end(); ++it, ++i )
	{
		if( p->_phase == phase_in || (p->_phase < phase_in && phase_in < (*it)->_phase) )
			break;
		p = *it;
	}
	return i;
#else
	INT32	min		=	0;
	INT32	max		=	_key_nb - 1;

	while( max-min > 1 )
	{	
		INT32	center = ( max + min ) >> 1;
		if( phase_in >= _keys[center]->get_phase() )
			min = center;
		else
			max = center;
	}
	return min + 1;
#endif
}
INT32	c_control_key::find_key_id_by_phase( REAL phase_in )
{
	sort_key();
	INT32	key_cache = _key_cache;
	if( INSIDE_MIN_MAX( key_cache, 1, _key_nb-1 ) )	//	beware of change in the keys
	{
		c_key*	cached = _keys[key_cache];
		if( phase_in < cached->get_phase() )
		{
			if( key_cache >= 1 )
			{
				c_key* prev = _keys[key_cache - 1];
				if( phase_in >= prev->get_phase()	 )
					return key_cache;
			}
			//	we could search on one side only now
		}
		else if( key_cache < _key_nb-1 )
		{
			c_key*	next = _keys[++key_cache];
			if( phase_in < next->get_phase() )
			{
				_key_cache =  key_cache;
				return  key_cache;
			}
			else if( key_cache < _key_nb-1 )
			{
				next = _keys[++key_cache];
				if( phase_in < next->get_phase() )
				{
					_key_cache =  key_cache;
					return  key_cache;
				}
			}
			//	we could search on one side only now
		}
		else
		{
			return key_cache;
		}
	}

	_key_cache = find_key_id_by_phase_low( phase_in );
	return _key_cache;
}

void	c_control_key::set_key_cur(		c_key*	key	)
{
	_p_key_cur	=	key;
	_key_id_cur	=	find_key_id_by_pt( key );
}
void	c_control_key::set_key_cur_by_id( INT32 id )
{
	_key_id_cur	=	build_id( id );
	_p_key_cur	=	find_key_pt_by_id( _key_id_cur );
	//	_phase_cur = _p_key_cur->_phase;
}
void	c_control_key::prev_key()			{	set_key_cur_by_id( _key_id_cur - 1 );	}
void	c_control_key::next_key()			{	set_key_cur_by_id( _key_id_cur + 1 );	}
c_key*	c_control_key::get_key_cur()
{
	return _p_key_cur;
}

//todo	we can still optimize creation reuse of key
c_key*	c_control_key::push_key_new( REAL* val )
{
	c_key*	p_key = c_key::get_new();
	cpy_v3( p_key->get_pos(), val ? val :_default_value );
	_keys.push_back( p_key );
	_b_need_sort = true;
	_key_nb = (INT32)_keys.size();
	return p_key;
}

//	INSERT
void	c_control_key::insert_key( REAL phase_in, REAL* pos )
{
	c_key*	p_key = push_key_new( pos );
	_p_key_cur->copy_no_pos_to( p_key );
	p_key->set_phase( phase_in );

	// hack, in 2d, we have to know the current phase, because we can't move it	
	if( phase_in > _phase_stop )
		_phase_stop = phase_in;
	if( phase_in < _phase_start )
		_phase_start = phase_in;

	set_key_cur( p_key );

	DBG_PRINT_STRING( "insert_key %d", find_key_id_by_pt( p_key ) );
}
void	c_control_key::insert_key( REAL phase_in, REAL x, REAL y, REAL z )
{
	REAL vec[3];
	set_v3( vec, x, y, z );
	insert_key( phase_in, vec );
}
void	c_control_key::insert_key()
{
	insert_key( _phase_cur, nullptr );
}
void	c_control_key::insert_key_interpolate()
{
	if( _p_key_cur->get_phase() == _phase_cur )
		return;	// already a key skip
	insert_key( _phase_cur, get_value( _phase_cur , 0 ), get_value( _phase_cur , 1 ), get_value( _phase_cur , 2 ) );
}

//	DELETE
void	c_control_key::delete_key_cur()
{
	if( _key_nb <= 1 )
		return;

	c_key::set_free( _p_key_cur );
	//_keys.remove( _p_key_cur );	//for list
	_keys.erase( remove(_keys.begin(), _keys.end(), _p_key_cur ), _keys.end() );
	_key_nb = (INT32)_keys.size();
	_b_need_sort = false;

//	if( _key_id_cur != 1 )
//		--_key_id_cur;
	set_key_cur_by_id( _key_id_cur );
	if( _key_id_cur == _key_nb )
	{
		// last key deleted
		_phase_stop = _p_key_cur->get_phase();
	}
	if( _key_id_cur == 1 )
	{
		// first key
		_phase_start = _p_key_cur->get_phase();
	}
}


REAL	c_control_key::get_value_inter( INT32 key_type, c_key* p, c_key* n, REAL inter, INT32 axe )
{
	if( axe < 0 || axe > 2 )
	{
		debug_break();
		return 0;
	}
	switch( key_type )
	{
	case KEY_CONST:
		return p->get_pos( axe );
	case KEY_LINEAR:
		return interpolate( p->get_pos(axe), n->get_pos(axe), inter );
	case KEY_SINUS:
		{
			FP32 val;
			val = COS_TURN( inter * .5 );
			val = ( FP32(1) - val ) * FP32(.5);
			return interpolate( p->get_pos(axe), n->get_pos(axe), val );
		}
	case KEY_CATMULL:
	default:
		debug_break();
		return 0;
	}
}

void	c_control_key::get_value_inter_v2r( REAL* dst, INT32 key_type, c_key* p, c_key* n, REAL inter )
{
	switch( key_type )
	{
	case KEY_CONST:
		cpy_v2( dst, p->get_pos() );
		break;
	case KEY_LINEAR:
		interpolate_v2( dst, p->get_pos(), n->get_pos(), inter );
		break;
	case KEY_SINUS:
		{
			FP32 val;
			val = COS_TURN( inter * .5 );
			val = ( FP32(1) - val ) * FP32(.5);
			interpolate_v2( dst, p->get_pos(), n->get_pos(), val );
		}
		break;
	case KEY_CATMULL:	//todo
	default:
		debug_break();
		break;
	}
}

void	c_control_key::get_value_inter_v3r( REAL* dst, INT32 key_type, c_key* p, c_key* n, REAL inter )
{
	switch( key_type )
	{
	case KEY_CONST:
		cpy_v3( dst, p->get_pos() );
		break;
	case KEY_LINEAR:
		interpolate_v3( dst, p->get_pos(), n->get_pos(), inter );
		break;
	case KEY_SINUS:
		{
			REAL val;
			val = COS_TURN( inter * .5 );
			val = ( FP32(1) - val ) * FP32(.5);
			interpolate_v3( dst, p->get_pos(), n->get_pos(), val );
		}
		break;
	case KEY_CATMULL:	//todo
	default:
		debug_break();
		break;
	}
}

REAL	c_control_key::get_value_low( REAL phase_in, INT32 axe )
{
	INT32	id = find_key_id_by_phase( phase_in );
	c_key*	prev	=	_keys[id-1];
	c_key*	next	=	_keys[id];

	INT32 key_type = get_key_type(prev);

	if( key_type == KEY_CONST )
		return prev->get_pos(axe);

	REAL	inter;
	if( prev->get_phase() == next->get_phase() )
		inter = 0.;
	else
		inter = ( phase_in - prev->get_phase() ) / ( next->get_phase() - prev->get_phase() );
	switch( key_type )
	{		
	case KEY_LINEAR:
	case KEY_SINUS:
		return get_value_inter( key_type, prev, next, inter, axe ) ;
	case KEY_CATMULL:
	case KEY_CATMULL_BIS:
		{
			c_key*		nn;
			if( ++id == _key_nb )
				nn = _keys[--id];
			else
				nn = _keys[id--];

			c_key*		pp;
			--id;
			if( id != 0 )
				--id;
			pp = _keys[id];

			return catmull_rom_1r( pp->get_pos(axe), prev->get_pos(axe), next->get_pos(axe), nn->get_pos(axe), inter ) ;
		}
	default:
		debug_break();
		return 0;
	}
}

void	c_control_key::get_value_v2r_low( REAL* dst, REAL phase_in )
{
	INT32	id = find_key_id_by_phase( phase_in );
	c_key*	prev	=	_keys[id-1];
	c_key*	next	=	_keys[id];

	INT32 key_type = get_key_type(prev);

	if( key_type == KEY_CONST )
	{
		cpy_v2( dst, prev->get_pos() );
		return;
	}
	REAL	inter;
	if( prev->get_phase() == next->get_phase() )
		inter = 0.;
	else
		inter = ( phase_in - prev->get_phase() ) / ( next->get_phase() - prev->get_phase() );
	switch( key_type )
	{		
	case KEY_LINEAR:
	case KEY_SINUS:
		get_value_inter_v2r( dst, key_type, prev, next, inter ) ;
		return;
	case KEY_CATMULL:
	case KEY_CATMULL_BIS:
		{
			c_key*		nn;
			if( ++id == _key_nb )
				nn = _keys[--id];
			else
				nn = _keys[id--];

			c_key*		pp;
			--id;
			if( id != 0 )
				--id;
			pp = _keys[id];

			catmull_rom_2( dst, pp->get_pos(), prev->get_pos(), next->get_pos(), nn->get_pos(), inter ) ;
			return;
		}
	default:
		debug_break();
		clear_v2( dst );
		return;
	}
}

bool	c_control_key::is_value( REAL phase_in )
{
	phase_in -= -_phase_offset;
	if( _phase_factor != 0. )
		phase_in /= _phase_factor;

	if( _keys.empty() || phase_in < _keys[0]->get_phase() || phase_in > _keys[_keys.size()-1]->get_phase() )
		return false;	
	return true;
}

REAL	c_control_key::get_value( REAL phase_in, INT32 axe )
{
	CLAMP_REF( axe, 0, 2 );
	//todo bad case is a quick hack just to avoid crash
	if( is_value( phase_in ) && _keys.size() > 1 )
	{
		phase_in -= -_phase_offset;
		if( _phase_factor != 0. )
			phase_in /= _phase_factor;
		phase_in = loop_phase( phase_in );

		return get_value_low( phase_in, axe ) * _factor + _offset;
	}
	return _keys[0]->get_pos( axe );
}
void	c_control_key::get_value_v2r( REAL* dst, REAL phase_in )
{
	phase_in -= -_phase_offset;
	if( _phase_factor != 0. )
		phase_in /= _phase_factor;
	phase_in = loop_phase( phase_in );

	return get_value_v2r_low( dst, phase_in );
	//deal with  * _factor + _offset;
}

bool	c_control_key::add_phase( REAL& phase_in, REAL& dt, bool b_stop )
{
	iter_key	it = _keys.begin();
	c_key*		p = *it;
	//to really bad research in O(n) time
	for( ; it != _keys.end(); ++it )
	{
		c_key*	n = *it;
		if( INSIDE_MIN_MAX( phase_in, p->get_phase(), n->get_phase()) )	//	is the phase in this interval ?
		{
			if( ( dt >= 0 && phase_in!=n->get_phase() ) || ( dt < 0 && phase_in!=p->get_phase() ) )
			{
				DOUBLE	diff = n->get_phase() - p->get_phase();
				if( diff != 0. )	// avoid NULL interval
				{
					DOUBLE	f = ABS( n->get_pos(0) );	// avoid negative factor
					if( f != 0. )
					{
						f = diff / f;
						DOUBLE dtf = dt * _factor;
						if( dt >= 0 )
						{
							diff = n->get_phase() - phase_in;	//phase left
							diff /= f;						//time_left
							if( diff <= dtf )
							{
								phase_in = n->get_phase();
								return true;
							}
							else
							{
								phase_in += REAL(dtf * f);
								return false;
							}
						}
						else
						{
							diff = p->get_phase() - phase_in;	//phase left
							diff /= f;						//time_left
							if( dtf <= diff )
							{
								phase_in = p->get_phase();
								return true;
							}
							else
							{
								phase_in += REAL(dtf * f);
								return false;
							}
						}

					}
					else
					{
						phase_in = ( dt > 0 ) ? n->get_phase() : p->get_phase();
						return true;
					}
				}
				else
				{
					phase_in = ( dt > 0 ) ? n->get_phase() : p->get_phase();
					return true;
				}
			}
		}
		p = n;
	}
	return true;
}


REAL	c_control_key::get_phase_next( REAL phase_in )
{
	if( _key_nb <= 1 )
		return _keys[0]->get_phase();

	INT32	id = find_key_id_by_phase( phase_in );
	c_key*	next	=	_keys[id];
	if( phase_in < next->get_phase() || id>=_key_nb-1 )
		return next->get_phase();
	return	_keys[id+1]->get_phase();
/*
	c_key*	prev	=	_keys[id-1];
	c_key*	next	=	_keys[id];

	if( prev->_phase <= phase_in && phase_in < next->_phase )	//	the phase is in this blk
		return next->_phase;
*/	
}

REAL	c_control_key::get_phase_prev( REAL phase_in )
{
	if( _key_nb <= 1 )
		return _keys[0]->get_phase();

	INT32	id = find_key_id_by_phase( phase_in );
	c_key*	prev	=	_keys[id-1];
	if( prev->get_phase() < phase_in || id<=1 )
		return prev->get_phase();
	return	_keys[id-2]->get_phase();
/*
	iter_key	it = _keys.begin();
	c_key*		p = *it;
	for( ; it != _keys.end(); ++it )
	{
		if( p->_phase < phase_in && phase_in <= (*it)->_phase )	//the phase is in this blk
		{
			phase_in = p->_phase;
			return true;
		}
		p = *it;
	}
	return true;
*/
}

void	c_control_key::set_key_phase(	REAL phase_in )
{
	_p_key_cur->set_phase(phase_in);
	_b_need_sort = true;
	set_key_cur( _p_key_cur );	//	make sure we don't switch to another when we cross phase values of others
}
REAL	c_control_key::get_key_phase()
{
	return _p_key_cur->get_phase();
}
REAL	c_control_key::get_key_phase(	INT32 id_in )
{
	c_key* p = find_key_pt_by_id( id_in );
	return p->get_phase();
}

void	c_control_key::set_key_value(	REAL val_in,	INT32 axe	)
{
	_p_key_cur->set_pos( val_in, axe );
}
REAL	c_control_key::get_key_value(	INT32 axe	)
{
	return _p_key_cur->get_pos(axe);
}

void	c_control_key::update()
{
	scale_v3( _color, _color_ui, _color_ui[3] );
	sort_key();
	_key_nb		=	(INT32)_keys.size();	//	we need this because it is used to load and we don't want user to change it directly
	set_key_cur_by_id( _key_id_cur );
}

AAA_ERR	save_to_file_v1( FILE*	file )		{	return ERR_ANY;		}
AAA_ERR	load_from_file_v1( FILE* file )		{	return ERR_ANY;		}

static	CHAR	key_data_ext[] = "control_key_data";
AAA_ERR	c_control_key::load_do_after( o_str CONST & filename )
{
	INT32 nb = _key_nb;
	AAA_ERR retcode = AAA_OK; 
	if( nb > 0 )
	{
		//prepare name
		CHAR	name[AAA_PATH_MAX()];
		fname::cpy_and_replace_ext( name, filename.get(), key_data_ext );
		c_file::push_vfile();
			FILE* file = c_file::FOPEN( name, "rt" );
			if( file )
			{
				INT32	i;
				c_key*	p_key;

				if ( fscanf( file, "key version(%d)", &i ) != 1 )
					goto exit;
				if( i != 0 )
				{
					box_err( "c_control_key keys not version 0" );
				}

				key_dealloc();
				for( ; nb > 0; --nb )
				{
					p_key = push_key_new();
					retcode = p_key->load_v0(file);
					if( ERR(retcode) )
						goto exit;
				}
				set_key_cur_by_id( _key_id_cur );
exit:		
				c_file::FCLOSE( file );
			}
			else
				retcode = ERR_FILE_NO; 
		c_file::pop_vfile();
		if( ERR(retcode) )
			box_err( "%s()\nerror reading \"%s\"", __FUNCTION__, name );
	}
	return retcode;
}

AAA_ERR	c_control_key::save_do_after( o_str CONST & filename )
{
	CHAR	name[AAA_PATH_MAX()];
	fname::cpy_and_replace_ext( name, filename.get(), key_data_ext );
	c_file::push_vfile();
		FILE*	file = c_file::FOPEN( name, "wt");
		if( file )
		{
			fprintf( file, "key version(0)\n" );
			for( iter_key it = _keys.begin(); it != _keys.end(); ++it )
				(*it)->save_v0( file );
			c_file::FCLOSE( file );
		}
	c_file::pop_vfile();
	return AAA_OK;
}

//
//	DRAW
//
void	c_control_key::draw_ui_2d( bool b_sel, FP32 CONST * color_in )	
{
	for( iter_key it = _keys.begin(); it != _keys.end(); ++it )
	{
		c_key*	key = *it;
		if( b_sel )
		{
			FP32 a = FP32(SIN_TURN( aaa::time::get() * 1. ) * .25 + .75);
			GOL::set_line_width( 3. );
			if( key == _p_key_cur )	//todo should have a generic color ui fn
				GOL::color4( 1,0,0, a );
			else
				GOL::color4( 0,1,0, a );
		}
		else
			GOL::color4v( color_in ? color_in : _color );
		key->draw_2d( _s_control_type );
	}
}
void	c_control_key::draw_ui( bool b_sel, FP32 CONST * color_in, INT32 axe, REAL phase_min, REAL phase_max )	
{
	INT32 i_end	= find_key_id_by_phase( phase_max );
	if( _key_nb > 1 )
		++i_end;
	INT32 i =	find_key_id_by_phase( phase_min );
	for( ; i <= i_end; ++i )
	{
		c_key*		key = _keys[i-1];
		if( b_sel )
		{
			REAL a = FP32(SIN_TURN( aaa::time::get() * 1. ) * .25 + .75);
			GOL::set_line_width( 3.);
			if( key == _p_key_cur )	//todo should have a generic color ui fn
				GOL::color4( 1,0,0, a );
			else
				GOL::color4( 0,1,0, a );
		}
		else
			GOL::color4v( color_in ? color_in : _color );
		key->draw( _s_control_type, _phase_factor, _phase_offset, axe );
	}
}

void	c_control_key::draw_phase_2d()
{
	// Draw all keys phase on a line
	//INT32		i;
	REAL y = -10000000.;
	REAL phase_max = -1000000.;
	REAL phase_min = 10000000.;
	for( iter_key it = _keys.begin(); it != _keys.end(); ++it )
	{
//		y = MAX( y, (*it)->get_pos(1) );
		phase_max = MAX( phase_max, (*it)->get_phase() );
		phase_min = MIN( phase_min, (*it)->get_phase() );
	}

	y = REAL(0.6);

	REAL x_offset = ( phase_max + phase_min ) * REAL(.5);
	REAL x_factor = ( phase_max - phase_min ) * REAL(.25);
	GOL::push_att();
		GOL::push_line_width( 5. );
		GOL::push_point_size( 8. );
		GOL::push_color();

		REAL x1;
		bool b_first = true;
		for( iter_key it = _keys.begin(); it != _keys.end(); ++it )
		{
			// Draw point
			REAL x = (*it)->get_phase();
			GOL::color_red();
			GOL::begin( GL_POINTS );
				GOL::vertex2( ( x - x_offset ) / x_factor, y );
			GOL::end();

			// Draw line
			if( b_first )
			{
				x1 = x;
				b_first = false;
			}
			else
			{
	//			GOL::color_red3();
				GOL::color4( 1,0,0, 0.25 );
				draw_line( ( x - x_offset ) / x_factor, y, ( x1 - x_offset ) / x_factor, y );
				x1 = x;
			}
		}
		// Draw phase_cur
		GOL::set_point_size( 5. );
		GOL::color_white();
		GOL::begin( GL_POINTS );
			GOL::vertex2( ( _phase_cur - x_offset ) / x_factor , y );
		GOL::end();
	GOL::pop_att();
}
void	c_control_key::draw_phase( c_key* p, c_key* n )
{
	REAL l = make_phase( p->get_phase() );
	REAL r = make_phase( n->get_phase() );
	REAL top = n->get_pos(0);
	GOL::begin( GL_TRIANGLES );
		GOL::vertex2( l, 0. );
		GOL::vertex2( r, 0. );
		GOL::vertex2( r, top );
	GOL::end();
}
void	c_control_key::draw_phase( REAL phase_min, REAL phase_max )	
{
	if( _key_nb <= 1 )
		return;

	INT32	i_end	=	find_key_id_by_phase( phase_max );
	INT32	i		=	find_key_id_by_phase( phase_min );

	c_key*		prev = _keys[i-1];
	c_key*		next = nullptr;

	INT32		color_index = i % 4;
	for( ; i <= i_end; ++i )
	{
		GOL::color4v( &color_auto_phase_blk[color_index * 4] );
		if( ++color_index >= 4 )
			color_index = 0;
		next = _keys[i];
		draw_phase( prev, next );
		prev = next;
	}
}

void	c_control_key::draw_curve_2d( c_key* p, c_key* n )
{
	INT32 key_type = get_key_type( p );

	REAL vec[2];
	switch( key_type )
	{
	case KEY_CONST:
		draw_line_2r( p->get_pos(), n->get_pos() );
		break;
	case KEY_LINEAR:
		GOL::begin( GL_LINE_STRIP );
			get_value_v2r_low( vec, p->get_phase() );
			GOL::vertex2v( vec );
			get_value_v2r_low( vec, n->get_phase() );
			GOL::vertex2v( vec );
		GOL::end();
		break;
	case KEY_CATMULL:
	case KEY_CATMULL_BIS:
	case KEY_SINUS:

		CONST	INT32	step = 10;
		
		REAL	ph = p->get_phase();
		REAL	dph = ( n->get_phase() - ph ) / REAL( step );

		GOL::begin( GL_LINE_STRIP );
		for( INT32 i = 0; i <= step; ++i )
		{
			get_value_v2r_low( vec, ph );	//todo we can opt this
			GOL::vertex2v( vec );
			ph += dph;
		}
		GOL::end();
		break;
	}
}
void	c_control_key::draw_curve_2d( FP32 CONST * color_in )	
{
	//	draw_curve_check();
	if( color_in )
		GOL::color4v( color_in );
	else
		GOL::color4v( _color );

	iter_key	it = _keys.begin();
	c_key*		prev = *it;
	c_key*		next;

	//	GOL::begin( GL_LINES );
	//		GOL::vertex2( prev->_pos[0], prev->_pos[1] );
	//		GOL::vertex2( make_phase( prev->_phase ), prev->_pos[axe] );
	//	GOL::end();

	++it;
	for( ; it != _keys.end(); ++it )
	{
		next = *it;
		draw_curve_2d( prev, next );
		prev = next;
	}

	//	GOL::begin( GL_LINES );
	//		GOL::vertex2( make_phase( next->_phase ), next->_pos[axe] );
	//		GOL::vertex2( _phase_stop, next->_pos[axe] );
	//	GOL::end();
}

void	c_control_key::draw_curve( c_key* p, c_key* n, INT32 axe )
{
	INT32 key_type = get_key_type( p );
	switch( key_type )
	{
	case KEY_CONST:
		GOL::begin( GL_LINES );
			GOL::vertex2( make_phase( p->get_phase() ), p->get_pos(axe) );
			GOL::vertex2( make_phase( n->get_phase() ), p->get_pos(axe) );
		GOL::end();
		break;
	case KEY_LINEAR:
		GOL::begin( GL_LINES );
			GOL::vertex2( make_phase( p->get_phase() ), p->get_pos(axe) );
			GOL::vertex2( make_phase( n->get_phase() ), n->get_pos(axe) );
		GOL::end();
		break;
	case KEY_SINUS:
		{
			REAL	ph = p->get_phase();
			REAL	dph = ( n->get_phase() - ph ) * REAL(.1);

			GOL::begin( GL_LINE_STRIP );
			for( INT32 i = 0; i <= 10; ++i )
			{
				GOL::vertex2( make_phase( ph ), get_value_inter( key_type, p, n, REAL(.1) * REAL( i ), axe ) );
				ph += dph;
			}
			GOL::end();
		}
		break;
	default:
	case KEY_CATMULL:
	case KEY_CATMULL_BIS:
		{
			CONST		INT32	step = 10;

			REAL	ph = p->get_phase();
			REAL	dph = (n->get_phase() - ph ) / REAL( step );

			GOL::begin( GL_LINE_STRIP );
			for( INT32 i = 0; i <= step; ++i )
			{
				GOL::vertex2( make_phase( ph ), get_value_low( ph, axe ) );
				ph += dph;
			}
			GOL::end();
		}
		break;
	}
}
void	c_control_key::draw_curve_check()	
{
	REAL	p = _phase_start;
	REAL	l = _phase_stop-_phase_start;
	REAL	dp = l / REAL(256.);

	p -= l;

	GOL::begin( GL_LINE_STRIP );
	for( ; p <= _phase_stop + l; p += dp )
		GOL::vertex2( p, get_value( p ) );
	GOL::end();
}
void	c_control_key::draw_curve( FP32 CONST * color_in, INT32 axe, REAL phase_min, REAL phase_max )	
{
	if( _key_nb <= 1 )
		return;

	//	draw_curve_check();
	GOL::color4v( color_in ? color_in : _color );

	INT32	i_end	=	find_key_id_by_phase( phase_max );
	INT32	i		=	find_key_id_by_phase( phase_min );

	c_key*		prev = _keys[i-1];
	c_key*		next = nullptr;

//todo
/*	GOL::begin( GL_LINES );
		GOL::vertex2( _phase_start, prev->_pos[axe] );
		GOL::vertex2( make_phase( prev->_phase ), prev->_pos[axe] );
	GOL::end();
*/
	for( ; i <= i_end; ++i )
	{
		next = _keys[i];
		draw_curve( prev, next, axe );
		prev = next;
	}

//todo
/*	if( next )
	{
		GOL::begin( GL_LINES );
			GOL::vertex2( make_phase( next->_phase ), next->_pos[axe] );
			GOL::vertex2( _phase_stop, next->_pos[axe] );
		GOL::end();
	}
*/
}


void	c_control_key::set_key_cur_by_phase( REAL phase_in )
{
	INT32 id = find_key_id_by_phase( phase_in );
	c_key*	close	=	_keys[id-1];
	if( _key_nb > 1 )
	{
		c_key*	next	=	_keys[id];
		// find nearest key
		if( ABS( phase_in - next->get_phase() ) < ABS( close->get_phase() - phase_in ) )
			close = next;
	}
	set_key_cur( close );
}

//
//	UI
//
bool	c_control_key::do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	bool	b_return = false;
	if( modifier::is_none( *modifiers ) )
	{
		b_return = true;
		if( b_special )
		{
			switch( c )
			{
			case keyboard::RIGHT:	next_key();			break;
			case keyboard::LEFT:	prev_key();			break;
			case keyboard::INSERT:	insert_key();		break;
			default:	b_return = false;				break;
			}
		}
		else
		{
			switch( c )
			{
			case keyboard::KEY_DELETE:
						delete_key_cur();				break;	//del
			default:	b_return = false;				break;
			}
		}
	}
	return b_return;
}

void	c_control_key::mouse_get_start_uv( FP32& u_start, FP32& v_start )
{
	u_start = _p_key_cur->get_pos(0);
	v_start = _p_key_cur->get_pos(1);
}

void	c_control_key::mouse_up()
{
	//mouse_move( u_in, v_in );
	//todo
	set_key_cur( _p_key_cur );
}
