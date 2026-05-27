
#ifdef AAA_COLOR_H
#error "COLOR_H included more than once."
#endif
#define AAA_COLOR_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif
#ifndef AAA_GOL_COLOR_H
#	include "gol/gol_color.h"
#endif

class	c_color_mod final : public c_obj_ui
{
	FACTORY_DECLARE( c_color_mod, c_obj_ui );
private:
	FP32	_bias_ui[4];
	FP32	_bias[4];
	FP32	_scale_ui[4];
	FP32	_scale[4];
	bool	_b_need_process;

protected:
public:
	virtual	void	param_init_pt();

/*	maa 2015 : do the FP32 version if neeeded
	FINLINE	void	process_3v( FP32* dst, FP32 CONST* src )
					{
						if( _b_need_process )	{	mul_add_v3r( dst, src, _scale, _bias );	}
						else					{	cpy_v3r( dst, src );					}
					}
	FINLINE	void	process_3v( FP32* vec )
					{
						if( _b_need_process )	{	mul_add_v3r( vec, _scale, _bias );		}
					}
*/
	FINLINE	void	process_rgbfa( FP32* dst, FP32 CONST* src ) CONST
					{
						scale_v3_cpy_v4( dst, src );
						if( _b_need_process )
							mul_add_v4( dst, _scale, _bias );
					}
	FINLINE	void	process_4v( FP32* dst, FP32 CONST* src ) CONST
					{
						if( _b_need_process )
							mul_add_v4( dst, src, _scale, _bias );
						else
							cpy_v4( dst, src );
					}
	FINLINE	void	process_4v( FP32* vec ) CONST
					{
						if( _b_need_process )
							mul_add_v4( vec, _scale, _bias );
					}
	FINLINE	void	update()
					{
						_bias[0] = _bias_ui[3] + _bias_ui[0];
						_bias[1] = _bias_ui[3] + _bias_ui[1];
						_bias[2] = _bias_ui[3] + _bias_ui[2];

						scale_v3( _scale, _scale_ui, _scale_ui[3]);
						//todoopt do it composante by composante
						_b_need_process = !is_all_one_v4(_scale ) || is_not_null_v4(_bias );
					}
	FINLINE	void	draw_color( FP32 CONST* v4 ) CONST
					{
						if( _b_need_process )
						{
							FP32	col[4];
							mul_add_v4( col, v4, _scale, _bias );
							GOL::color4v( col );
						}
						else
						{
							GOL::color4v( v4 );
						}
	}
	FINLINE	void	draw_color( FP32 CONST r, FP32 CONST g, FP32 CONST b, FP32 CONST a ) CONST
					{
						if( _b_need_process )
						{
							GOL::color4(	_scale[0] * r + _bias[0],
											_scale[1] * g + _bias[1],
											_scale[2] * b + _bias[2],
											_scale[3] * a + _bias[3]
							);
						}
						else
						{
							GOL::color4(	r,g,b, a );
						}
					}
	template< INT32 R, INT32 G, INT32 B >
	FINLINE	void	draw_with_alpha( FP32 CONST alpha_in ) CONST
					{
						if( _b_need_process )
						{
							GOL::color4(	R ? _scale[0] * R + _bias[0] : _bias[0],
											G ? _scale[1] * G + _bias[1] : _bias[1],
											B ? _scale[2] * B + _bias[2] : _bias[2],
											_scale[3] * alpha_in + _bias[3]
										);
						}
						else
						{
							GOL::color4(	R,G,B, alpha_in );
						}
					}
	FINLINE	void	draw_white(		FP32 CONST alpha_in ) CONST	{	draw_with_alpha < 1, 1, 1 >	(	alpha_in );	}
	FINLINE	void	draw_red(		FP32 CONST alpha_in ) CONST	{	draw_with_alpha < 1, 0, 0 >	(	alpha_in );	}
	FINLINE	void	draw_green(		FP32 CONST alpha_in ) CONST	{	draw_with_alpha < 0, 1, 0 >	(	alpha_in );	}
	FINLINE	void	draw_blue(		FP32 CONST alpha_in ) CONST	{	draw_with_alpha < 0, 0, 1 >	(	alpha_in );	}
};

class	c_color final : public c_obj_ui
{
	FACTORY_DECLARE(c_color, c_obj_ui);
private:
	static	c_color* cur;
public:
	static	c_color* def;
	static	c_color* ui;
	static	c_color_mod* mod;
private:
	FP32	_rgba[4];		//	color to draw
	FP32	_rgbaf_ui[5];	//	param to define color in rgb mode, infact in memory r g b a then f a factor for rgb (grey)
	bool	_b_use_hsv;		//	determine the source information rgb or hsv
	bool	_b_convert;
	FP32	_hsv[3];		//	param to define color in hsv mode
protected:
public:
	FINLINE	static	void		set_cur(c_color* color)	{ cur = color;		}
	FINLINE	static	void		set_cur_null()			{ set_cur(nullptr); }
	FINLINE	static	c_color*	get_cur()				{ return cur;		}

	static	void	c_init();
	static	void	c_deinit();

	virtual	bool	is_draw_sum_up() { return true; }
	virtual	void	draw_sum_up();

	virtual	void	param_init_pt();

	virtual	void	update();
	//todo change name/change philosophy ?
	virtual	void	draw();
	FINLINE	void	draw_no_alpha()									CONST		{	GOL::color3v(	_rgba );						}
	FINLINE	void	draw_with_alpha_scaled( REAL CONST val )		CONST		{	GOL::color4(	_rgba[0],	_rgba[1],	_rgba[2],	_rgba[3] * val );	}
	FINLINE	void	draw_with_color_scaled( REAL CONST r, REAL CONST g, REAL CONST b, REAL CONST a )
																	CONST		{	GOL::color4(	_rgba[0]*r,	_rgba[1]*g, _rgba[2]*b, _rgba[3] * a );		}
	FINLINE	void	draw_with_color_scaled( REAL CONST* in )		CONST		{	draw_with_color_scaled( in[0], in[1], in[2], in[3] );					}


	/*
	void	pixel_transfert_reset();
	void	pixel_transfert_update();

	void	transfert_update();
	void	set_1f( REAL value );
	void	set_3f( REAL red, REAL green, REAL blue );
	void	set_4f( REAL red, REAL green, REAL blue, REAL alpha );
	*/
//	void	set_rgb_v3r( REAL CONST* in );

	FINLINE	FP32 CONST *	get_color_pt()							CONST	{ return _rgba; };
	FINLINE	FP32			get_color_compo( INT32 CONST index )	CONST	{ return _rgba[index]; };
	FINLINE	FP32			get_color_alpha()						CONST	{ return get_color_compo(3); };
};

