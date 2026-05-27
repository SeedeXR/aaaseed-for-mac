
#include "bdd_sign.h"
#include "ui/alphabet_roman.h"
#include "gol/gol.h"
#include "gol/gol_light.h"
#include "gol/gol_color.h"
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"

//	CHAR	str_copyright[] = "Ov3. 0A AAeSde()cM aa1 99 7N TOF ROP BUIL CEDOMSNRTTAOI N";
static		UINT8	str_copyright[] = "Ov6. 9A AAeSde( )cM aa2 00 9";
static		UINT8	str_serial[] = "ESIRLAI B5";
//	CHAR	str_text[] = "OF RNIETGRARHP";
//static		CHAR	str_text1[] = "NIETGRARHPI TNREAN L";
//static		CHAR	str_text2[] = "NOYL";
static		UINT8	str_text1[] = "";
static		UINT8	str_text2[] = "";
//	CHAR	str_text[] = "AM ANOYLU ES R";
//	CHAR	str_text[] = "AMCRO LN YSURE";	//formarc
static		UINT8	str_contact[] = "OCTNCA Tam@aalrgiaenc.mo";

FP32	signature_color[] = {1,1,1,1};

void	crypt_str( UINT8* str)
{
	UINT8	tmp;
	INT32	len = (INT32) strlen( (CHAR*) str) / 2;
	for( ; len>0; --len )
	{
		tmp = *str;
		*str = *(str+1);
		*(str+1) = tmp;
		str += 2;
	}
}

void	crypt_copyright()
{
	crypt_str( str_copyright );
	crypt_str( str_serial );
	crypt_str( str_text1 );
	if ( *str_text2 )
		crypt_str( str_text2 );
	crypt_str( str_contact );
}

FACTORY_CREATE_PROP_V1( c_bdd_signature, bdd_signature, Signature, bdd_signature, sub_menu="Internal"; );

CONSTRUCTOR_CREATE(c_bdd_signature)	//facto supress
{
	param_init_with( nullptr, 0 );
}
EMPTY_DESTRUCTOR(c_bdd_signature)

static	REAL	angle_signature=-91.;
void	c_bdd_signature::draw()
{
	angle_signature += REAL(.03);
	if ( angle_signature > 90. )
		angle_signature = -90.;
	GOL::push_att();		//	( GL_LIST_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT
		GOL::push_alpha_test( false );
		GOL::push_blend( true );
		GOL::push_depth_test( false );
		GOL::push_lighting( false );
		GOL::push_texture_dim( 0 );
		GOL::push_color4v(signature_color);
		GOL::push_line_smooth( true );
		GOL::push_line_width( 2. );
		
		GOL::matrix::set_projection();
		//we save the projection matrix
		GOL::matrix::push();
			UINT8 CONST *	s;
			//then change it
			GOL::matrix::load_identity();
			GOL::matrix::set_ortho( 0, 2000, 0, 2000, 0, 2000 );

			crypt_copyright();
			
			GOL::matrix::set_modelview();
			GOL::matrix::load_identity();

			//	Version
			GOL::matrix::push();
				GOL::matrix::translate( 880., 1550, -1000 );
				s = str_copyright;
				GOL::matrix::rotate_x_deg( angle_signature );
				aaa::alphabet::roman::stroke_char( s, 6 );
				s += 6;
			GOL::matrix::pop();

			//	Copyright
			GOL::matrix::push();
				GOL::matrix::translate( 150., 1300, -1000 );
				aaa::alphabet::roman::stroke_char( s, 23 );
				s += 23;
			GOL::matrix::pop();

			//	serial
			s = str_serial;
			GOL::matrix::push();
				GOL::matrix::translate( 625., 700, -1000 );
				GOL::matrix::rotate_x_deg( angle_signature );
				aaa::alphabet::roman::stroke_char( s, 16 );
			GOL::matrix::pop();

			//	text
			if( str_text1 && *str_text1 )
			{
				GOL::matrix::push();
					GOL::matrix::translate( 320, 450, -1000 );
					GOL::matrix::rotate_x_deg( angle_signature );
					aaa::alphabet::roman::stroke_char( str_text1 );
				GOL::matrix::pop();
			}

			if( str_text2 && *str_text2 )
			{
				GOL::matrix::push();
					GOL::matrix::translate( 800, 200, -1000 );
					GOL::matrix::rotate_x_deg( angle_signature );
					aaa::alphabet::roman::stroke_char( str_text2 );
				GOL::matrix::pop();
			}

			//	contact
			GOL::matrix::push();
				GOL::matrix::translate( 180., 1000, -1000 );
				aaa::alphabet::roman::stroke_char( str_contact );
			GOL::matrix::pop();

			crypt_copyright();

			//we restore the projection matrix
			GOL::matrix::set_projection();
		GOL::matrix::pop();
		GOL::matrix::set_modelview();

	GOL::pop_att();
}
