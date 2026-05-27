
#ifdef AAA_ALPHABET_H
#error "ALPHABET_H included more than once."
#endif
#define AAA_ALPHABET_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

enum COLOR_TYPE : INT32
		{
		COLOR_ITEM,
		COLOR_ITEM_UNUSED,
		COLOR_TITLE,
		COLOR_TITLE_UNUSED,
		COLOR_TITLE_NEXT,
		COLOR_INFO,
		COLOR_ERROR,
		COLOR_OFF,
		COLOR_ON,
		COLOR_NODE,
		COLOR_NODE_1,
		COLOR_NODE_2,
		COLOR_NODE_3,
		COLOR_NODE_4,
		COLOR_TYPE_MAX_NB
		};

namespace aaa::alphabet
{
	CONSTEXPR REAL Y_SIZE		=	1.;
	
	namespace maa {
		CONSTEXPR REAL X_SIZE		=	REAL(.8);
		CONSTEXPR REAL X_SPACE		=	REAL(.4);
		CONSTEXPR REAL Y_SPACE		=	.0;
		CONSTEXPR REAL LINE_SIZE	=	Y_SIZE + Y_SPACE;

		CONSTEXPR REAL	get_size_line()		{	return LINE_SIZE;		}
		CONSTEXPR REAL	get_char_size_x()	{	return X_SIZE+X_SPACE;	}

	}
	namespace roman {
		CONSTEXPR REAL Y_SPACE		=	REAL(.2);	//todo good for GaBU, bad for symmetry Maa vs Roman ( we should adjust interline )		
		CONSTEXPR REAL LINE_SIZE	=	Y_SIZE + Y_SPACE ;
	}

			void	draw_cr();

	extern	FP32	color_char[4];
	extern	FP32	CONST alphabet_color[COLOR_TYPE_MAX_NB][4];

	extern	void	update();

	extern	void	draw_str_init();

	extern	void	draw_str_begin( INT32 CONST font = 0 );
	extern	void	draw_str_end();

	extern	void	draw_str( CONST UINT8* str );
	FINLINE	void	draw_str( C_PCHAR str ) { draw_str( (CONST UINT8*) str ); }
	FINLINE	void	draw_str( C_PCHAR str, INT32 CONST font )
	{
		if( !str || *str==0 )
			return;
		draw_str_begin( font );
		draw_str( str );
		draw_str_end();
	}


	namespace maa
	{
		extern	void	translate_char(		REAL CONST nb );
		extern	REAL	compute_str_move(	UINT8 CONST * str, INT32 CONST nb );
		extern	void	move_str(			UINT8 CONST * str, INT32 CONST nb );
	}


	//	done in 1d for now and only for roman stroke
	//todo extend in 2d (height and carriage return should be used)
	extern	void	get_str_translate(	REAL* CONST dst,	C_PCHAR str,	INT32 CONST font	);	

	extern	void	draw_str_ui(	CHAR* str );
//	extern	void	draw_str_roman(	CHAR* str );
	extern	void	draw_cr_ui();

	extern	void	draw_str_v3(	C_PCHAR str,	REAL CONST * CONST pos,						REAL CONST sx, REAL CONST sy, INT32 CONST font );
	extern	void	draw_str_xy(	C_PCHAR str,	REAL CONST x, REAL CONST y,					REAL CONST sx, REAL CONST sy, INT32 CONST font );
	extern	void	draw_str_xyz(	C_PCHAR str,	REAL CONST x, REAL CONST y, REAL CONST z,	REAL CONST sx, REAL CONST sy, INT32 CONST font );
	FINLINE	void	draw_str_xyzv(	C_PCHAR str,	REAL* pt,									REAL CONST sx, REAL CONST sy, INT32 CONST font )
					{	draw_str_xyz( str, *pt, *(pt+1), *(pt+2), sx, sy, font );	}



	//todo the color cooking should be one layer on top
	//				and done with object
	extern	void	draw_str_set_color( INT32 CONST i, bool CONST b_active=false, bool CONST b_focus=false );
	FINLINE	void	draw_str_with_color( CHAR* str, INT32 CONST i )
					{
						draw_str_set_color( i );
						draw_str( str );
					}
	extern	void	draw_str_with_color_begin_end( CHAR* str, INT32 CONST i );
	extern	void	draw_str_begin_end( CHAR* str );
	extern	void	draw_int32( INT32 CONST i );
	FINLINE	void	draw_int32( INT32 CONST i, INT32 CONST font )
					{
						draw_str_begin( font );
						draw_int32( i );
						draw_str_end();
					}

	extern	void	draw_int32_xyz(		INT32 CONST i,	REAL CONST x, REAL CONST y, REAL CONST z,		REAL CONST sx, REAL CONST sy	);	//	, INT32 font );
	FINLINE	void	draw_int32_xyzv(	INT32 CONST i,	REAL* pt,										REAL CONST sx, REAL CONST sy	)	{	draw_int32_xyz( i, *pt, *(pt+1), *(pt+2), sx, sy );	}
	FINLINE	void	draw_int32_xy(		INT32 CONST i,	REAL CONST x, REAL CONST y,						REAL CONST sx, REAL CONST sy	)	{	draw_int32_xyz( i, x, y, 0, sx, sy );				}

	extern	void	draw_real( REAL CONST r, INT32 CONST decimal_nb = 2 );

	extern	REAL	get_char_size_x();

//	FINLINE	void	stereo_push_up();
//	FINLINE	void	stereo_pop_up();

}	//namespace aaa::alphabet

