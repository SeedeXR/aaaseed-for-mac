#ifdef AAA_GOL_COLOR_H
	#error "GOL_COLOR_H included more than once."
#endif
#define AAA_GOL_COLOR_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef AAA_GOL_ATTRIB_H
#	include "gol_attrib.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

namespace GOL	{

enum class aaa_color : UINT32
{
	GREY = 0,
	WHITE,
	RED,
	GREEN,
	BLUE,
	CYAN,
	MAGENTA,
	YELLOW,
	BLACK,
	ALICE_BLUE,
	ANTIQUE_WHITE,
	AQUA,
	AQUA_MARINE,
	AZURE,
	BEIGE,
	BISQUE,
	BLANCHED_ALMOND,
	BLUE_VIOLET,
	BROWN,
	BURLY_WOOD,
	CADET_BLUE,
	CHARTREUSE,
	CHOCOLATE,
	CORAL,
	CORNFLOWER_BLUE,
	CORN_SILK,
	CRIMSON,
	DARK_BLUE,
	DARK_CYAN,
	DARK_GOLDEN_ROD,
	DARK_GREY,
	DARK_GREEN,
	DARK_KHAKI,
	DARK_MAGENTA,
	DARK_OLIVE_GREEN,
	DARK_ORANGE,
	DARK_ORCHID,
	DARK_RED,
	DARK_SALMON,
	DARK_SEA_GREEN,
	DARK_SLATE_BLUE,
	DARK_SLATE_GREY,
	DARK_TURQUOISE,
	DARK_VIOLET,
	DEEP_PINK,
	DEEP_SKY_BLUE,
	DIM_GREY,
	DODGER_BLUE,
	FIRE_BRICK,
	FLORAL_WHITE,
	FOREST_GREEN,
	FUCHSIA,
	GAINSBORO,
	GHOST_WHITE,
	GOLD,
	GOLDEN_ROD,
	GREEN_YELLOW,
	HONEY_DEW,
	HOT_PINK,
	INDIAN_RED,
	INDIGO,
	IVORY,
	KHAKI,
	LAVENDER,
	LAVENDER_BLUSH,
	LAWN_GREEN,
	LEMON_CHIFFON,
	LIGHT_BLUE,
	LIGHT_CORAL,
	LIGHT_CYAN,
	LIGHT_GOLDEN_ROD_YELLOW,
	LIGHT_GREY,
	LIGHT_GREEN,
	LIGHT_PINK,
	LIGHT_SALMON,
	LIGHT_SEA_GREEN,
	LIGHT_SKY_BLUE,
	LIGHT_SLATE_GREY,
	LIGHT_STEEL_BLUE,
	LIGHT_YELLOW,
	LIME,
	LIME_GREEN,
	LINEN,
	MAROON,
	MEDIUM_AQUA_MARINE,
	MEDIUM_BLUE,
	MEDIUM_ORCHID,
	MEDIUM_PURPLE,
	MEDIUM_SEA_GREEN,
	MEDIUM_SLATE_BLUE,
	MEDIUM_SPRING_GREEN,
	MEDIUM_TURQUOISE,
	MEDIUM_VIOLET_RED,
	MIDNIGHT_BLUE,
	MINT_CREAM,
	MISTY_ROSE,
	MOCCASIN,
	NAVAJO_WHITE,
	NAVY,
	OLD_LACE,
	OLIVE,
	OLIVE_DRAB,
	ORANGE,
	ORANGE_RED,
	ORCHID,
	PALE_GOLDEN_ROD,
	PALE_GREEN,
	PALE_TURQUOISE,
	PALE_VIOLET_RED,
	PAPAYA_WHIP,
	PEACH_PUFF,
	PERU,
	PINK,
	PLUM,
	POWDER_BLUE,
	PURPLE,
	ROSY_BROWN,
	ROYAL_BLUE,
	SADDLE_BROWN,
	SALMON,
	SANDY_BROWN,
	SEA_GREEN,
	SEA_SHELL,
	SIENNA,
	SILVER,
	SKY_BLUE,
	SLATE_BLUE,
	SLATE_GREY,
	SNOW,
	SPRING_GREEN,
	STEEL_BLUE,
	TAN,
	TEAL,
	THISTLE,
	TOMATO,
	TURQUOISE,
	VIOLET,
	WHEAT,
	WHITE_SMOKE,
	YELLOW_GREEN,
	MAX_NB
};

extern FP32 CONST *	CONST	get_color_value( aaa_color CONST color );
extern C_PCHAR_C			get_color_name(	 aaa_color CONST color );

extern	FP32	color_cur[4];
FINLINE	FP32 CONST * CONST	get_color()
							{
								return color_cur;
							}

extern void	reset_scale_bias();

FINLINE	void	color3v_low(	FP32 CONST * CONST col )
				{
					cpy_v3( color_cur, col );
					color_cur[3] = FP32(1);
					glColor3fv( col );
				}
FINLINE	void	color4v_low(	FP32 CONST * CONST col )
				{
					cpy_v4( color_cur, col );
					glColor4fv( col );
				}

FINLINE	void	color3v(		FP32 CONST * CONST col )
				{
					if( is_state_cache_no() || is_diff_v3( color_cur, col ) || color_cur[3] != FP32(1) )
						color3v_low( col );
				}
FINLINE	void	color3(			FP32 CONST r, FP32 CONST g, FP32 CONST b )
				{
					FP32 col[3] = { r,g,b };
					color3v( col );
				}
		
FINLINE	void	color4v(		FP32 CONST * CONST col )
				{
					if( is_state_cache_no() || is_diff_v4( color_cur, col ) )
						color4v_low( col );
				}
FINLINE	void	color4(			FP32 CONST r, FP32 CONST g, FP32 CONST b, FP32 CONST a )
				{
					if( a == FP32(1) )
					{
						FP32 col[3] = { r,g,b };
						color3v( col );
					}
					else
					{
						FP32 col[4] = { r,g,b, a };
						color4v( col );
					}
				}

FINLINE	void	color4(			aaa_color CONST color, FP32 CONST a = 1. )
				{
					if( a == 1. )
						color3v( get_color_value(color) );
					else
					{
						FP32 rgba[4];
						cpy_v3( rgba, get_color_value( color ) );
						rgba[3] = a;
						color4v( rgba );
					}
				}

CONSTEXPR	FP32	fp32_white[3]	= { 1., 1., 1. };
CONSTEXPR	FP32	fp32_black[3]	= { 0., 0., 0. };

CONSTEXPR	FP32	fp32_red[3]		= { 1., 0., 0. };
CONSTEXPR	FP32	fp32_green[3]	= { 0., 1., 0. };
CONSTEXPR	FP32	fp32_blue[3]	= { 0., 0., 1. };

CONSTEXPR	FP32	fp32_yellow[3]	= { 1., 1., 0. };
CONSTEXPR	FP32	fp32_cyan[3]	= { 0., 1., 1. };
CONSTEXPR	FP32	fp32_magenta[3]	= { 1., 0., 1. };

FINLINE	void	color_white()	{ color3v( fp32_white	); }
FINLINE	void	color_black()	{ color3v( fp32_black	); }
							  
FINLINE	void	color_red()		{ color3v( fp32_red		); }
FINLINE	void	color_green()	{ color3v( fp32_green	); }
FINLINE	void	color_blue()	{ color3v( fp32_blue	); }
									  
FINLINE	void	color_yellow()	{ color3v( fp32_yellow	); }
FINLINE	void	color_cyan()	{ color3v( fp32_cyan	); }
FINLINE	void	color_magenta()	{ color3v( fp32_magenta	); }

FINLINE	void	reset_color()
				{
					color_white();
				}
FINLINE	void	pop_color()
				{
					FP32 col[4];
					c_poper*	poper = get_att_poper();
					poper->get_value4<FP32>( col );
					color4v( col );
				}
FINLINE	void	push_color()
				{
					c_poper*	poper = get_att_poper();
					poper->add_value4<FP32>( color_cur );	//potential state sync problem
					poper->add_fn( pop_color );
				}
FINLINE	void	push_color4v(	FP32 CONST * CONST col )
				{
					if( is_state_cache_no() || is_diff_v4( color_cur, col ) )
					{
						push_color();
						color4v_low( col );
					}
				}
FINLINE	void	push_color4(	FP32 CONST r, FP32 CONST g, FP32 CONST b, FP32 CONST a = 1. )
				{
					FP32 col[4] = { r,g,b, a };
					push_color4v( col );
				}
FINLINE	void	push_color3v(	FP32 CONST * CONST col )
				{
					if( is_state_cache_no() || is_diff_v3( color_cur, col ) )
					{
						push_color();
						color3v_low( col );
					}
				}
FINLINE	void	push_color3(	FP32 CONST r, FP32 CONST g, FP32 CONST b )
				{
					FP32 col[3] = { r,g,b };
					push_color3v( col );
				}

FINLINE	void	texenvv( GLenum CONST target, GLenum CONST pname, FP32 CONST * CONST params )
{
	glTexEnvfv( target, pname, params );
}

}	//namespace GOL
