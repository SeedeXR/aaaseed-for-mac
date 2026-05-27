
#ifdef AAA_AAA_FONT_TMP_H
#error "AAA_FONT_TMP_H included more than once."
#endif
#define AAA_AAA_FONT_TMP_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

class FTInstance;
class AAA_FTGlyph;

class AAA_font_tmp
{
protected:
	FTInstance*		_instance;
	AAA_FTGlyph**	_glyphs;

public:
	AAA_font_tmp( FTInstance* instance );
	virtual ~AAA_font_tmp();

	void			destroy();
	virtual bool	create();
	AAA_FTGlyph*	get_glyph( INT32 i ) CONST
	{
		if( _glyphs == 0 )
			return 0;
		if( i < 0 || i > 255 )
			return 0;
		return _glyphs[i];
	}

	INT32			get_height() CONST;
	INT32			get_descender() CONST;
	INT32			get_width( C_PCHAR text );
	void			get_bbox( C_PCHAR text, INT32& llx, INT32& lly, INT32& urx, INT32& ury ) CONST;
};

#endif	//#if AAA_TRUETYPE1_USE()
/////////////////////////////////////////////////////////////////////////////


