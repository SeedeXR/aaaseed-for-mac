#include "draw/fog.h"
#include "ui/strsymbo.h"
#include "infrastructure/param/param_declare.h"
#include "math/v.h"


FACTORY_CREATE_V1( c_fog, fog, Fog, fog );

c_fog*	c_fog::cur	= nullptr;
c_fog*	c_fog::ui	= nullptr;

static	C_PCHAR_C	fog_mode_str[3*2] =	// *2 because of SYNO
{
	"LINEAR",
	"EXP",
	"EXP2",

	"GL_LINEAR",
	"GL_EXP",
	"GL_EXP2",
};

namespace	n_fog
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	11;
	CONSTEXPR INT32 GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			active )

		PARAM_DEF_SYMBO_SYNO_PSTR_ZERO(	mode,		fog_mode_str )
		PARAM_DEF_FP32(					density,	1.,.1,	0.,100.	)
		PARAM_DEF_SYMBO_PSTR_ZERO(		hint,		gstr::hint )

		PARAM_DEF_REAL_POS_ZERO(		red )
		PARAM_DEF_REAL_POS_ZERO(		green )
		PARAM_DEF_REAL_POS_ZERO(		blue )
		PARAM_DEF_REAL_POS_ONE(			grey )
		PARAM_DEF_REAL_POS_ONE(			alpha )

		//no alpha in gl fog PARAM_DEF_REAL_ZERO( alpha )
		PARAM_DEF_REAL_INF(				start,		0, -5	)
		PARAM_DEF_REAL_INF(				end,		0, -5	)
	};
}

void	c_fog::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );

	param_set_pt( h, _s_mode_ui		);
	param_set_pt( h, _density_ui	);
	param_set_pt( h, _s_hint_ui		);

	param_set_pt_3( h, _color_ui		);
	param_set_pt( h, _color_ui[4]		);
	param_set_pt( h, _color_ui[3]		);

	param_set_pt( h, _start_ui		);
	param_set_pt( h, _end_ui		);

	err_param_init_pt(h);
}


CONSTRUCTOR_CREATE(c_fog)
,_b_verbose	{true}
{
	param_init_with( n_fog::param, n_fog::PARAM_NB_MAX ); // fog_param, FOG_PARAM_NB_MAX);
}

c_fog::~c_fog()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;
}


static	INT16 gl_fog_mode[3] = { GL_LINEAR, GL_EXP, GL_EXP2 };

//todo deal with camera change
void c_fog::update()
{
	if( GOL::is_fog_allow() )
	{
		if( is_active() )//&& cur != this)
		{
			REAL	c[4];
			GOL::fogi( GL_FOG_MODE, gl_fog_mode[_s_mode_ui] );
			GOL::fogf( GL_FOG_DENSITY, _density_ui );
			GOL::hint( GL_FOG_HINT, _s_hint_ui );
			
			scale_v3( c, _color_ui, _color_ui[4] );
			c[3] = _color_ui[3] ;
			GOL::fogv( GL_FOG_COLOR, c );

			GOL::fogf( GL_FOG_START, _start_ui );
			GOL::fogf( GL_FOG_END, _end_ui );

			cur = this;
		}
	}
}

void c_fog::enable()
{
	if( is_active() )
		GOL::enable_fog();
}

void c_fog::disable()
{
	if( is_active() )
		GOL::disable_fog();
}

void	c_fog::print()
{
	DBG_PRINT_STRING("c_fog::print Not Implemented Yet");
}

//	VERBOSE
void	c_fog::print_verbose()
{
	if( _b_verbose )
		SWITCH_PRINT_STATE( "fog Object have Verbose", _b_verbose );
}

void	c_fog::set_verbose( bool CONST b )
{
	_b_verbose = b;
	print_verbose();
}

void	c_fog::flip_verbose()
{
	_b_verbose = !_b_verbose;
}

//	MODE
void	c_fog::print_mode()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING( "fog mode", _s_mode_ui==0?"Linear":(_s_mode_ui==1?"Exp":"Exp2") );
}

void	c_fog::set_mode( INT32 CONST i )
{
	_s_mode_ui = i%3;
	print_mode();
}

INT32	c_fog::flip_mode()
{
	set_mode(_s_mode_ui+1);
	return _s_mode_ui;
}

// DENSITY
void	c_fog::print_density()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING( "de", "%f", _density_ui );
}

void	c_fog::set_density( REAL CONST density_in )
{
	_density_ui = density_in;
	print_density();
}

//	HINT
void	c_fog::print_hint()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING( "fog hint", _s_hint_ui==0?"dont care":(_s_hint_ui==1?"Fastest":"nicest") );
}

void	c_fog::set_hint( INT32 i )
{
	_s_hint_ui = GOL::HINT(i%3);
	print_hint();
}

INT32	c_fog::flip_hint()
{
	set_hint( _s_hint_ui+1 );
	return _s_hint_ui;
}

// COLOR
void	c_fog::print_color()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING("fog color", "%f %f %f %f", _color_ui[0], _color_ui[1], _color_ui[2], _color_ui[3] );
}

void	c_fog::set_color( REAL CONST red, REAL CONST green, REAL CONST blue, REAL CONST alpha )
{
	_color_ui[0] = red;
	_color_ui[1] = green;
	_color_ui[2] = blue;
	_color_ui[3] = alpha;
	_color_ui[4] = 1.;
	print_color();
}

//	START END
void	c_fog::print_start_end()
{
	if( _b_verbose )
		SWITCH_PRINT_STRING("fog", "start %f end %f", _start_ui, _end_ui );
}

void	c_fog::set_start( REAL CONST start_in )
{
	_start_ui = start_in;
	print_start_end();
}

void	c_fog::set_end( REAL CONST end_in )
{
	_end_ui = end_in;
	print_start_end();
}


