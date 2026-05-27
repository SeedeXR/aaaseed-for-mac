#include "alphabet.h"
#include "alphabet_maa.h"
#include "alphabet_roman.h"
#include "gol/gol_matrix.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"
#include "gol/gol_list.h"
#include "time/aaa_time.h"
#include "draw/stereo.h"


namespace aaa::alphabet
{
	static	REAL	str_tra[3] = { 0, 0, 0 };
	static	REAL	str_sx = 1.;
	static	REAL	str_sy = 1.;

	typedef	void	fn_transfo_type();
	static	void	set_transform()
	{
		GOL::matrix::translate3v( str_tra );
		GOL::matrix::scale( str_sx, str_sy );
	}

	static	fn_transfo_type*	fn_fransfo = nullptr;

	static	REAL	move_up = 0;
	CONST	REAL	stereo_up = .25;

	static	INT32	loc_font;
	static	INT32	nb_cr;

	static	INT32	color = -1;
	static	bool	b_active_on = false;
	static	bool	b_focus_on = false;

	static	FP32	color_change[3];

	static	REAL	alphabet_alpha = 1.;
	//static	REAL	color_slider[4] = {1,1,1,1};
			FP32	color_char[4];
#if 1

#ifdef _MSC_VER
#	pragma warning( push )  
#	pragma warning (4 : 4305)	// disable truncation warnings
#endif	//_MSC_VER
	FP32 CONST alphabet_color[COLOR_TYPE_MAX_NB][4] =
			{
				{	.5,		.8,		1.,		alphabet_alpha },	//COLOR_ITEM
				{	.7,		.5,		.5,		alphabet_alpha },	//COLOR_ITEM_UNUSED
				{	1.,		1.,		1.,		alphabet_alpha },	//COLOR_TITLE
				{	.7,		.5,		.5,		alphabet_alpha },	//COLOR_TITLE_UNUSED
				{	.6,		.6,		.6,		alphabet_alpha },	//COLOR_TITLE_NEXT
				{	1.,		.5,		.2,		alphabet_alpha },	//COLOR_INFO
				{	1.,		.0,		.0,		alphabet_alpha },	//COLOR_ERROR
				{	.25,	.5,		.25,	alphabet_alpha },	//COLOR_OFF
				{	0.,		1.,		.0,		alphabet_alpha },	//COLOR_ON
				{	1.,		1.,		1.,		alphabet_alpha },	//COLOR_NODE
				{	.9,		.9,		.9,		alphabet_alpha },	//COLOR_NODE_1
				{	.8,		.8,		.8,		alphabet_alpha },	//COLOR_NODE_2
				{	.7,		.7,		.7,		alphabet_alpha },	//COLOR_NODE_3
				{	.6,		.6,		.6,		alphabet_alpha },	//COLOR_NODE_4
			};
#ifdef _MSC_VER
#	pragma warning( pop )  
#endif	//_MSC_VER

#else
	//Miguel
	FP32 CONST alphabet_color[COLOR_TYPE_MAX_NB][4] =
			{
				{	0,	1,	0,	alphabet_alpha	},
				{	0,	1,	0,	alphabet_alpha	},
				{	0,	1,	0,	alphabet_alpha	},
				{	.8, .2, .2,	alphabet_alpha	}
				{	0,	1,	0,	alphabet_alpha	},
			};
#endif
	REAL CONST change_freq = 2.;	//todo param on it
//	used for the slider 

	extern	maa::p_fn_draw	maa::str_draw_array[];

	typedef	void (*getfn)( REAL* dst );
	namespace{
		void move_no( REAL* dst )	{}
		void move_cr( REAL* dst )	{ dst[1] += loc_font == 1 ? roman::LINE_SIZE : maa::LINE_SIZE; }
	}

	CONSTEXPR getfn	move_array[32] =
	{	//0-15
		move_no,		move_no,		move_no,		move_no,
		move_no,		move_no,		move_no,		move_no,
		move_no,		move_no,		move_cr,		move_no,
		move_no,		move_cr,		move_no,		move_no,
		//16-31
		move_no,		move_no,		move_no,		move_no,
		move_no,		move_no,		move_no,		move_no,
		move_no,		move_no,		move_no,		move_no,
		move_no,		move_no,		move_no,		move_no,
	};


	FINLINE	void translate_line_nb( INT32 nb )
	{
		if( nb != 0 )
		{
			if( loc_font == 1 )
				GOL::matrix::translate( 0., -roman::LINE_SIZE * nb, 0. );
			else
				GOL::matrix::translate( 0., -maa::LINE_SIZE * nb, 0. );
		}
	}

	void draw_cr_ui()
	{
	/*
		GOL::begin(GL_LINE_STRIP);
			GOL::vertex2( 30+nb_cr/2, 0);
			GOL::vertex2( 40, 0);
			GOL::vertex2( 40, .5);
		GOL::end();
	*/
		//go back to beginning of line
		GOL::matrix::pop_push();

		translate_line_nb( ++nb_cr );
	}

	void draw_cr()
	{
		//go back to beginning of line
		GOL::matrix::pop_push();

		//todo refine this in a unified mecanism
		//	work in bdd
		if( fn_fransfo )
			(*fn_fransfo)();
		translate_line_nb( ++nb_cr );
	}



	//todo this init should be done for each window (each GL context in fact)
	INT32 CONSTEXPR FONT_NB = 2;
	INT32 list_char[FONT_NB] = { 0, 0 };

	//todo add deinit and call it when closing the app
	void draw_str_init()
	{
		list_char[0] = maa::compile_lists();
		list_char[1] = roman::prepare_char();
		maa::build_vbo();
	}

	void update()
	{
		REAL ph = FMOD( REAL(aaa::time::get()) * change_freq );
		color_change[0] = SIN_TURN( ph )		* FP32(.5);
		color_change[1] = SIN_TURN( ph + .33)	* FP32(.5);
		color_change[2] = SIN_TURN( ph + .66)	* FP32(.5);
	}

	void draw_str_set_color( INT32 CONST color_in, bool CONST b_active, bool CONST b_focus )
	{
		if( color_in != color || b_active_on != b_active || b_focus_on != b_focus )
		{
			REAL	tmp;
			color = color_in;
			b_active_on = b_active;
			b_focus_on = b_focus;
	//		cpy_v3r( color_slider, alphabet_color[color_in]);
			cpy_v4( color_char, alphabet_color[color_in] );

			if( b_focus )
			{
				tmp = color_char[0] + REAL(.5);
				if( tmp > 1.)
				{
					tmp = tmp - REAL(1);
					color_char[0] = 1.;
					color_char[1] -= tmp;
					color_char[2] -= tmp;
				}
				else
					color_char[0] = tmp;
			}
			if( b_active )
			{
				add_v3( color_char, color_change );

	/*			r += r_change;
				g += g_change;
				b += b_change;
				if( g > 1.)
					{
					tmp = g - 1.;
					r -= tmp;
					g = 1.;
					b -= tmp;
					}
	*/
			}

			GOL::color4v( color_char );

			if( g_stereo->is_active() )
			{
				if( b_active )
					move_up = stereo_up;
				else
				{
					move_up = 0;
					switch( color_in )
					{
					case COLOR_ON:	move_up = stereo_up;	break;
					case COLOR_OFF:	move_up = -stereo_up;	break;
					}

				}
			}
			else
				move_up = 0;
		}
	}

	#if	DRAW_WITH_GLUT
	void bitmap_output( int x, int y, char *string, void *font )
	{
		int len;

		GOL::raster_pos_2f( x,y );
		len = (int) strlen( string );
		for( INT32 i = 0; i < len; ++i )
		{
			glutBitmapCharacter( font, string[i] );
		}
	}
	/*
		bitmap_output(40, 35, "This is written in a GLUT bitmap font.",
						GLUT_BITMAP_TIMES_ROMAN_24);
		bitmap_output(30, 210, "More bitmap text is a fixed 9 by 15 font.",
						GLUT_BITMAP_9_BY_15);
		bitmap_output(70, 240, "                Helvetica is yet another bitmap font.",
						GLUT_BITMAP_HELVETICA_18);
	*/
	#endif



	namespace roman {
		//todo extend to 3d ?
		void get_str_translate( REAL* CONST dst, UINT8 CONST * str )
		{
			if( str )
			{
				cpy_v3( dst, unit_y_v4fp32 );	// we start with 1 line
				CONST UINT8* pt = str - 1;
				while( *++pt )
				{
					if( *pt < 32 )
					{
						INT32 len = INT32(pt - str);
						if( len > 0 )
							*dst = ::MAX( *dst, get_char_translate( str, len ) );
						str = pt + 1;
						(*move_array[*pt])( dst );
					}
				}
				INT32 len = INT32(pt - str);
				if( len > 0 )
					*dst = ::MAX( *dst, get_char_translate( str, len ) );
			}
			else
				clear_v3( dst );
		}
	}	//	namespace roman 

	namespace maa {
		//todo extend to 3d ?
		FINLINE REAL get_char_translate( UINT8 CONST * str, INT32 len )
		{
			return (X_SIZE + X_SPACE ) * len - X_SPACE;
		}
		void get_str_translate( REAL* CONST dst, UINT8 CONST * str )
		{
			if( str )
			{
				cpy_v3( dst, unit_y_v4fp32 );
				CONST UINT8* pt = str - 1;
				while( *++pt )
				{
					if( *pt < 32 )
					{
						INT32 len = INT32(pt - str);
						if( len > 0 )
							*dst = ::MAX( *dst, get_char_translate( str, len ) );
						str = pt + 1;
						(*move_array[*pt])( dst );
					}
				}
				INT32 len = INT32(pt - str);
				if( len > 0 )
					*dst = ::MAX( *dst, get_char_translate( str, len ) );
			}
			else
				clear_v3( dst );
		}

		FINLINE void translate_char( REAL CONST nb )
		{
			if( nb != 0 )
				GOL::matrix::translate( nb * ( X_SIZE + X_SPACE ), 0., 0. );
		}
		FINLINE	REAL compute_str_move( UINT8 CONST * str, INT32 CONST nb )
		{
			--str;
			REAL tot = 0.;
			for( INT32 i=nb; i>0; --i )
			{
				INT32	c = *++str;
				tot += maa::str_draw_translate_x[c]; 
			}
			return tot;
		}
		void move_str( UINT8 CONST * str, INT32 CONST nb )
		{
			translate_char( compute_str_move( str, nb ) );
		}
	}	//	namespace maa 

	void	get_str_translate( REAL* CONST dst,	C_PCHAR str, INT32 CONST font	)
	{
		if( font == 1 )
			roman::get_str_translate( dst, (UINT8 CONST *) str );	//todo should we have CHAR be a unsigned int 
		else
			maa::get_str_translate( dst, (UINT8 CONST *) str );
	}

	FINLINE	void	stereo_push_up()
	{
		if( move_up!=0. )
			GOL::matrix::translate(  g_stereo->is_right_eye() ? -move_up : move_up, .0, .0 );
	}
	FINLINE	void	stereo_pop_up()
	{
		if( move_up!=0. )
			GOL::matrix::translate( g_stereo->is_right_eye() ? move_up : -move_up, .0, .0 );
	}

	//todo	this don't really work in particular draw_cr and draw_tab
	//		the special character need to be treated apart and not in a list
	//	bitmap_output( 0, 0, (char * ) str,  GLUT_BITMAP_8_BY_13 );
	FINLINE	void draw_str( CONST UINT8* str )
	{
		if( !str || *str==0 )
			return;
		stereo_push_up();

		bool b_list_use = GOL::b_list_allow;
		//	printf( "%s\n", str);

		INT32 len;
		CONST UINT8* pt = str - 1;

		if( b_list_use )
		{
			GOL::set_list_base( list_char[loc_font] );
			while( *++pt )
			{
				if( *pt < 32 )
				{
					len = INT32(pt - str);
					if( len > 0 )
						GOL::call_lists( len, GL_UNSIGNED_BYTE, str );
					str = pt + 1;
					maa::str_draw_array[*pt]();
				}
			}
			len = INT32(pt - str);
			if( len > 0 )
				GOL::call_lists( len, GL_UNSIGNED_BYTE, str );
		}
		else
		{	// VBO + glMultiDrawArrays(for roman) path (no display lists)
			while( *++pt )
			{
				if( *pt < 32 )
				{
					len = INT32(pt - str);
					if( len > 0 )
					{
						if( loc_font==1 )
							roman::stroke_char( str, len );
						else
							maa::stroke_char( str, len );
					}
					str = pt + 1;
					maa::str_draw_array[*pt]();
				}
			}
			len = INT32(pt - str);
			if( len > 0 )
			{
				if( loc_font==1 )
					roman::stroke_char( str, len );
				else
					maa::stroke_char( str, len );
			}
		}
		stereo_pop_up();
	}

	void draw_str_begin( INT32 CONST font )
	{
		loc_font = font;
		nb_cr = 0;
		color = COLOR_TYPE_MAX_NB;

		GOL::matrix::push();
		fn_fransfo = nullptr;
	}

	void draw_str_end()
	{
		GOL::matrix::pop();
	}


	FINLINE	void	draw_str_xyz_low( C_PCHAR str, REAL CONST sx, REAL CONST sy, INT32 CONST font )
	{
		loc_font = font;
		nb_cr = 0;
		str_sx = (font == 1) ? sx : sx * REAL(.5);
		str_sy = sy;
		fn_fransfo = set_transform;
		GOL::matrix::push();
			set_transform();
			draw_str( str );
		GOL::matrix::pop();
	}

	void	draw_str_v3( C_PCHAR str, REAL CONST * CONST pos, REAL CONST sx, REAL CONST sy, INT32 CONST font )
	{
		if( !str || *str==0 )
			return;
		cpy_v3( str_tra, pos );
		draw_str_xyz_low( str, sx, sy, font );
	}
	void	draw_str_xyz( C_PCHAR str, REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST sx, REAL CONST sy, INT32 CONST font )
	{
		if( !str || *str==0 )
			return;
		set_v3( str_tra, x, y, z );
		draw_str_xyz_low( str, sx, sy, font );
	}
	void	draw_str_xy( C_PCHAR str, REAL CONST x, REAL CONST y, REAL CONST sx, REAL CONST sy, INT32 CONST font )
	{
		if( !str || *str==0 )
			return;
		set_v3( str_tra, x, y, 0. );
		draw_str_xyz_low( str, sx, sy, font );
	}

	void	draw_str_ui( CHAR* str )
	{
		if( !str || *str==0 )
			return;
		CHAR* pt = str - 1;
		while( *++pt )
		{
			if( *pt==10 || *pt==13 )
			{
				*pt = 0;
				draw_str( str );
				draw_cr_ui();
				str = pt + 1;
			}
		}
		if( *str )
			draw_str( str );
	}
/*
	void	n_alphabet::draw_str_roman( CHAR* str )
	{
		if( !str || *str==0 )
			return;
		CHAR* pt = str - 1;
		while( *++pt )
		{
			if( *pt==10 || *pt==13 )
			{
				*pt = 0;
				draw_str( str, 1 );
				draw_cr_ui();
				str = pt + 1;
			}
		}
		if( *str )
			draw_str( str, 1 );
	}
*/

	void	draw_str_with_color_begin_end( CHAR* str, INT32 CONST i )
	{
		if( !str || *str==0 )
			return;
		draw_str_begin( 0 );
		draw_str_with_color( str, i );
		draw_str_end();
	}

	void	draw_str_begin_end( CHAR* str )
	{
		if( !str || *str==0 )
			return;
		draw_str_begin( 0 );
		draw_str( str );
		draw_str_end();
	}

	//todo	multitask ?
	static	CHAR	str_tmp[256];
	void	draw_int32( INT32 CONST i )
	{
		sprintf( str_tmp, "%-255d", i );
		draw_str( str_tmp );
	}
	void	draw_int32_xyz(		INT32 i,	REAL CONST x, REAL CONST y, REAL CONST z,		REAL CONST sx, REAL CONST sy	)
	{
		sprintf( str_tmp, "%-255d", i );
		draw_str_xyz( str_tmp, x, y, z, sx, sy, 1 );	//hack 1 is a hack

	}
	void	draw_real(  REAL CONST r, INT32 CONST decimal_nb )
	{
		sprintf( str_tmp, "%.*f", decimal_nb, r );
		draw_str( str_tmp );
	}

}	//namespace aaa::alphabet
