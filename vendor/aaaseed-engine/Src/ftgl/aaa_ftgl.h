
#ifdef AAA_AAA_FTGL_H
#error "AAA_FTGL_H included more than once."
#endif
#define AAA_AAA_FTGL_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

#ifndef __ftgl__
#	include "FTGL/ftgl.h"
#endif


class	c_ftfont
{
public:
	enum	FTGL_TYPE : INT32
	{
		FTGL_TEXTURE = 0,
		FTGL_BUFFER,
		FTGL_OUTLINE,
		FTGL_POLYGON,
		FTGL_EXTRUDE,
		FTGL_BITMAP,
		FTGL_PIXMAP,
		FTGL_TYPE_NB
	};
	static	o_str ftgl_version;

	static	void c_init();
	static	void c_deinit();
	//static	FTFont*		get_font_by_type_and_name( CHAR* font_name, UINT32 type );
	static	c_ftfont*	get(			UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi = 72 );
	static	void		erase(			UINT32 CONST s_font );
	static	UINT32		get_unique_id(	UINT32 CONST s_font );
//	static	bool		is_state_unique(	UINT32 CONST s_font, UINT32 CONST id );

private:
	static	FTFont*	alloc_font( C_PCHAR_C font_name, UINT32 CONST type );
	
	FTFont*			_ft_font;
	FTSimpleLayout	_ft_layout;
	bool			_b_need_glyph_build;
	bool			_b_use_display_list;

//	FP32			_face_size_over_one;
	FP32			_line_height_over_one;
	FP32			_ascender_over_one;

	void			dealloc();
	void			alloc( UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi );

	INT32			_s_font;
	o_str			_name;
	UINT32			_face_size;
	UINT32			_face_dpi;
	UINT32			_type;

	FP32			_depth;
	FP32			_outset;


//	INT32			cur;

private:


public:
	c_ftfont( UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi );
	~c_ftfont();

	FINLINE	FTFont*			get_font()					CONST	{	return _ft_font;				}
	FINLINE	FTSimpleLayout*	get_layout()						{	return &_ft_layout;				}
//	FINLINE	REAL			get_face_size_over_one()	CONST	{	return _face_size_over_one;		}
	FINLINE	FP32			get_line_height_over_one()	CONST	{	return _line_height_over_one;	}
	FINLINE	FP32			get_ascender_over_one()		CONST	{	return _ascender_over_one;		}

	//void	set_face_size(			INT32 face_size );

	void	set_depth( FP32 CONST depth );
	void	set_outset( FP32 CONST outset );

	void	set_use_display_list( bool b_use_display_list )
	{
		if( _b_use_display_list != b_use_display_list )
		{
			_b_use_display_list = b_use_display_list;
			_ft_font->UseDisplayList( _b_use_display_list );
		}
	}
	void	update();
};



