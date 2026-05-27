
#ifdef AAA_GLTTOUTLINEFONT_H
#error "GLTTOUTLINEFONT_H included more than once."
#endif
#define AAA_GLTTOUTLINEFONT_H 1



#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class AAA_FTFace;
class FTInstance;
class AAA_font_tmp;

class GLTTOutlineFont
{
protected:
	AAA_FTFace*		_face;
	FTInstance*		_instance;
	AAA_font_tmp*	_font;
	bool*			_loaded;
	INT32			_list_base;
	DOUBLE			_precision;

public:
	GLTTOutlineFont( AAA_FTFace* face );
	virtual ~GLTTOutlineFont();

	void	destroy();
	void	set_precision( DOUBLE precision );
	bool	create( INT32 point_size );
	bool	load_glyph( INT32 i );
	void	load( INT32 from = 0, INT32 to = 255 );
	void	load( C_PCHAR text );

	AAA_font_tmp* getFont() CONST
	{
		return _font;
	}

	void output( C_PCHAR text );
	void output( const CHAR ch );

	int get_height() CONST;
	int get_descender() CONST;
	int get_width( C_PCHAR text );
};

#endif //#if AAA_TRUETYPE1_USE()
