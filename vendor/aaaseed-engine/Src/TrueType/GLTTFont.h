
#ifdef AAA_GLTTFONT_H
#error "GLTTFONT_H included more than once."
#endif
#define AAA_GLTTFONT_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

class AAA_FTFace;
class FTInstance;
class AAA_font_tmp;


class GLTTFont
{
private:
	AAA_FTFace*		_face;
	FTInstance*		_instance;
	AAA_font_tmp*	_font;
	bool*			_b_loaded;
	INT32			_list_base;

	DOUBLE			_precision;

public:
	GLTTFont( AAA_FTFace* _face );
	virtual ~GLTTFont();

	void				destroy();
	void				set_precision( DOUBLE precision );
	bool				create( int point_size );

	bool				load_glyph( int i );
	void				load( INT32 from = 0, INT32 to = 255 );
	void				load( C_PCHAR text );

	AAA_font_tmp*		get_font() CONST
	{
		return _font;
	}

	void				output( C_PCHAR text );
	void				output( CONST CHAR ch );

	INT32				get_width( C_PCHAR text );
	INT32				get_height() CONST;
	INT32				get_descender() CONST;

	void				get_bbox( C_PCHAR text, INT32& llx, INT32& lly, INT32& urx, INT32& ury ) CONST;
};

#endif	//#if AAA_TRUETYPE1_USE()

