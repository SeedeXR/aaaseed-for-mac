#include "alphabet_maa.h"
#include "gol/gol_matrix.h"
#include "gol/gol_draw.h"
#include "gol/gol_list.h"
#include "gol/gol_color.h"
#ifndef _VECTOR_
#	include <vector>
#endif

namespace aaa::alphabet::maa
{
	//todo something stink here
	#define	PT_X(x)	GLfloat( (REAL) x * X_SIZE / 8. )
	#define	PT_Y(y)	GLfloat( (REAL) y * Y_SIZE / 24. - Y_SPACE )
	#define	PT(y,x)	{ PT_X(x), PT_Y(y) }

	typedef	CONST GLfloat	pt2[2];
	
	static	pt2	P00		=	PT(	24	,	0	)	;
	static	pt2	P01		=	PT(	24	,	1	)	;
	static	pt2	P02		=	PT(	24	,	2	)	;
	static	pt2	P03		=	PT(	23	,	3	)	;
	static	pt2	P04		=	PT(	25	,	4	)	;
	static	pt2	P05L	=	PT(	23	,	4	)	;
	static	pt2	P05		=	PT(	23	,	5	)	;
	static	pt2	P06		=	PT(	24	,	6	)	;
	static	pt2	P07		=	PT(	24	,	7	)	;
	static	pt2	P08		=	PT(	24	,	8	)	;

	static	pt2	P06L	=	PT(	24	,	5	)	;

	static	pt2	P10LL	=	PT(	20	,	-2	)	;
	static	pt2	P10L	=	PT(	20	,	-1	)	;
	static	pt2	P10		=	PT(	20	,	0	)	;
	static	pt2	P11		=	PT(	20	,	1	)	;
	static	pt2	P12		=	PT(	20	,	2	)	;
	static	pt2	P13		=	PT(	20	,	3	)	;
	static	pt2	P14		=	PT(	20	,	4	)	;
	static	pt2	P15		=	PT(	20	,	5	)	;
	static	pt2	P16		=	PT(	20	,	6	)	;
	static	pt2	P17		=	PT(	20	,	7	)	;
	static	pt2	P18		=	PT(	20	,	8	)	;
	//	these two for the dot of the i

	static	pt2	P10D	=	PT(	19	,	0	)	;
	static	pt2	P12D	=	PT(	19	,	2	)	;
	static	pt2	DOTI1	=	PT(	18	,	0	)	;
	static	pt2	DOTI2	=	PT(	18	,	2	)	;
	static	pt2	DOTI3	=	PT(	19	,	1	)	;

	static	pt2	I14		=	PT(	19	,	4	)	;

	static	pt2	P20L	=	PT(	16	,	-1	)	;
	static	pt2	P20		=	PT(	16	,	0	)	;
	static	pt2	P21		=	PT(	16	,	1	)	;
	static	pt2	P22		=	PT(	16	,	2	)	;
	static	pt2	P23MX5	=	PT(	15	,	-2	)	;
	static	pt2	P23LLL	=	PT(	15	,	0	)	;
	static	pt2	P23LL	=	PT(	15	,	1	)	;
	static	pt2	P23L	=	PT(	15	,	2	)	;
	static	pt2	P23		=	PT(	15	,	3	)	;
	static	pt2	P24		=	PT(	15	,	4	)	;
	static	pt2	P24D	=	PT(	14	,	4	)	;
	static	pt2	P25		=	PT(	15	,	5	)	;
	static	pt2	P26L	=	PT(	16	,	5	)	;
	static	pt2	P26		=	PT(	16	,	6	)	;
	static	pt2	P27		=	PT(	16	,	7	)	;
	static	pt2	P28		=	PT(	16	,	8	)	;
	static	pt2	P216	=	PT(	16	,	16	)	;

	static	pt2	P30		=	PT(	10	,	0	)	;
	static	pt2	P31		=	PT(	13	,	1	)	;
	static	pt2	P32		=	PT(	13	,	2	)	;
	static	pt2	P33		=	PT(	11	,	3	)	;
	static	pt2	P34MX5	=	PT(	12	,	-1	)	;
	static	pt2	P34MX4	=	PT(	12	,	0	)	;
	static	pt2	P34MX3	=	PT(	12	,	1	)	;
	static	pt2	P34LL	=	PT(	12	,	2	)	;
	static	pt2	P34		=	PT(	12	,	4	)	;
	static	pt2	P35		=	PT(	11	,	5	)	;
	static	pt2	P36		=	PT(	12	,	6	)	;
	static	pt2	P37		=	PT(	11	,	7	)	;
	static	pt2	P310	=	PT(	12	,	10	)	;
	static	pt2	P312	=	PT(	12	,	12	)	;
	static	pt2	P314	=	PT(	12	,	14	)	;

	static	pt2	P34L	=	PT(	12	,	3	)	;

	static	pt2	P40M	=	PT(	8	,	-1	)	;
	static	pt2	P40		=	PT(	8	,	0	)	;
	static	pt2	P41		=	PT(	8	,	1	)	;
	static	pt2	P42		=	PT(	8	,	2	)	;
	static	pt2	P43MX5	=	PT(	9	,	-2	)	;
	static	pt2	P43MX3	=	PT(	9	,	0	)	;
	static	pt2	P43LL	=	PT(	9	,	1	)	;
	static	pt2	P43L	=	PT(	9	,	2	)	;
	static	pt2	P43		=	PT(	9	,	3	)	;
	static	pt2	P44		=	PT(	9	,	4	)	;
	static	pt2	P44U	=	PT(	10	,	4	)	;
	static	pt2	P45		=	PT(	9	,	5	)	;
	static	pt2	P46LL	=	PT(	8	,	4	)	;
	static	pt2	P46L	=	PT(	8	,	5	)	;
	static	pt2	P46		=	PT(	8	,	6	)	;
	static	pt2	P47		=	PT(	8	,	7	)	;
	static	pt2	P48		=	PT(	8	,	8	)	;
	static	pt2	P49		=	PT(	7	,	9	)	;
	static	pt2	P410	=	PT(	8	,	10	)	;
	static	pt2	P411	=	PT(	8	,	11	)	;
	static	pt2	P416	=	PT(	8	,	16	)	;

	static	pt2	P50		=	PT(	4	,	0	)	;
	static	pt2	P54		=	PT(	4	,	4	)	;

	static	pt2	P60		=	PT(	0	,	0	)	;
	static	pt2	P63		=	PT(	1	,	3	)	;
	static	pt2	P68		=	PT(	0	,	8	)	;

	static	pt2	C0		=	PT(	20	,	0	)	;
	static	pt2	C1		=	PT(	23	,	1	)	;
	static	pt2	C2		=	PT(	24	,	4	)	;
	static	pt2	C3		=	PT(	23	,	7	)	;
	static	pt2	C4		=	PT(	20	,	8	)	;
	static	pt2	C5		=	PT(	17	,	7	)	;
	static	pt2	C6		=	PT(	16	,	4	)	;
	static	pt2	C7		=	PT(	17	,	1	)	;

	static	pt2	C2L		=	PT(	24	,	3	)	;
	static	pt2	C2LL	=	PT(	24	,	2	)	;
	static	pt2	C2MX3	=	PT(	24	,	1	)	;
	static	pt2	C2MX5	=	PT(	24	,	-1	)	;

	static	pt2	RLL0	=	PT(	12	,	-2	)	;
	static	pt2	RLL1	=	PT(	15	,	-1	)	;
	static	pt2	RLl2	=	PT(	16	,	2	)	;
	static	pt2	RLL3	=	PT(	15	,	5	)	;
	static	pt2	RLL4	=	PT(	12	,	6	)	;
	static	pt2	RLL5	=	PT(	9	,	5	)	;
	static	pt2	RLL6	=	PT(	8	,	2	)	;
	static	pt2	RLL7	=	PT(	9	,	-1	)	;

	static	pt2	RL0		=	PT(	12	,	-1	)	;
	static	pt2	RL1		=	PT(	15	,	0	)	;
	static	pt2	RL2		=	PT(	16	,	3	)	;
	static	pt2	RL3		=	PT(	15	,	6	)	;
	static	pt2	RL4		=	PT(	12	,	7	)	;
	static	pt2	RL5		=	PT(	9	,	6	)	;
	static	pt2	RL6		=	PT(	8	,	3	)	;
	static	pt2	RL7		=	PT(	9	,	0	)	;

	static	pt2	R0		=	PT(	12	,	0	)	;
	static	pt2	R1		=	PT(	15	,	1	)	;
	static	pt2	R2		=	PT(	16	,	4	)	;
	static	pt2	R3		=	PT(	15	,	7	)	;
	static	pt2	R4		=	PT(	12	,	8	)	;
	static	pt2	R5		=	PT(	9	,	7	)	;
	static	pt2	R6		=	PT(	8	,	4	)	;
	static	pt2	R7		=	PT(	9	,	1	)	;

	//	second cercle on self of first one

	static	pt2	RB0		=	PT(	12	,	6	)	;
	static	pt2	PIEDBL	=	PT(	8	,	7	)	;
	static	pt2	RB1		=	PT(	15	,	7	)	;
	static	pt2	RB2		=	PT(	16	,	10	)	;
	static	pt2	RB3		=	PT(	15	,	13	)	;
	static	pt2	RB4		=	PT(	12	,	14	)	;
	static	pt2	PIEDBR	=	PT(	8	,	14	)	;
	static	pt2	RB5		=	PT(	9	,	13	)	;
	static	pt2	RB6		=	PT(	8	,	12	)	;
	static	pt2	RB7		=	PT(	9	,	7	)	;


	static	pt2	RC0		=	PT(	12	,	8	)	;
	static	pt2	RC1		=	PT(	15	,	9	)	;
	static	pt2	RC2		=	PT(	16	,	12	)	;
	static	pt2	RC3		=	PT(	15	,	15	)	;
	static	pt2	RC4		=	PT(	12	,	16	)	;
	static	pt2	RC5		=	PT(	9	,	15	)	;
	static	pt2	RC6		=	PT(	8	,	12	)	;
	static	pt2	RC7		=	PT(	9	,	9	)	;

	//left of R
	static	pt2	R0L		=	PT(	12	,	-1	)	;
	static	pt2	R1L		=	PT(	15	,	0	)	;
	static	pt2	R2L		=	PT(	16	,	3	)	;
	static	pt2	R3L		=	PT(	15	,	6	)	;
	static	pt2	R4L		=	PT(	12	,	7	)	;
	static	pt2	R5L		=	PT(	9	,	6	)	;
	static	pt2	R6L		=	PT(	8	,	3	)	;
	static	pt2	R7L		=	PT(	9	,	0	)	;

	static	pt2	R0LL	=	PT(	12	,	-2	)	;
	static	pt2	R1LL	=	PT(	15	,	-1	)	;
	static	pt2	R2LLL	=	PT(	16	,	1	)	;
	static	pt2	R2LL	=	PT(	16	,	2	)	;
	static	pt2	R3LL	=	PT(	15	,	5	)	;
	static	pt2	R4LL	=	PT(	12	,	6	)	;
	static	pt2	R5LL	=	PT(	9	,	5	)	;
	static	pt2	R6MX5	=	PT(	8	,	-1	)	;
	static	pt2	R6MX3	=	PT(	8	,	1	)	;
	static	pt2	R6LL	=	PT(	8	,	2	)	;
	static	pt2	R7LL	=	PT(	9	,	-1	)	;

	static	pt2	B0		=	PT(	4	,	0	)	;
	static	pt2	B1		=	PT(	7	,	1	)	;
	static	pt2	B2		=	PT(	8	,	4	)	;
	static	pt2	B3		=	PT(	7	,	7	)	;
	static	pt2	B4		=	PT(	4	,	8	)	;
	static	pt2	B5		=	PT(	1	,	7	)	;
	static	pt2	B6		=	PT(	0	,	4	)	;
	static	pt2	B7		=	PT(	1	,	1	)	;

	static	pt2	I0		=	PT(	18	,	1	)	;
	static	pt2	I1		=	PT(	18	,	5	)	;
	static	pt2	I2		=	PT(	18	,	7	)	;
	static	pt2	I3		=	PT(	18	,	9	)	;

	static	pt2	I0L		=	PT(	18	,	0	)	;
	static	pt2	I1L		=	PT(	18	,	4	)	;
	static	pt2	I2L		=	PT(	18	,	6	)	;
	static	pt2	I3L		=	PT(	18	,	8	)	;

	static	pt2	I0LL	=	PT(	18	,	-1	)	;
	static	pt2	I1LL	=	PT(	18	,	3	)	;
	static	pt2	I2LL	=	PT(	18	,	5	)	;
	static	pt2	I3LL	=	PT(	18	,	7	)	;

	static	pt2	S1		=	PT(	6	,	-2	)	;

	static	pt2	P0		=	PT(	12	,	1	)	;
	static	pt2	P1		=	PT(	12	,	7	)	;

	// --- VBO recording infrastructure ---
	namespace {
		struct maa_stroke_t { GLenum type; GLint first; GLsizei count; };
		struct maa_char_t   { GLint first_stroke; GLsizei stroke_nb; };

		std::vector<GLfloat>      s_verts;
		std::vector<maa_stroke_t> s_strokes;
		maa_char_t                s_char_data[256];
		GLuint                    s_vbo      = 0;
		GLenum                    s_cur_type = 0;
		GLint                     s_cur_first = 0;
	}

	typedef void (*fn_gbegin_t) ( GLenum );
	typedef void (*fn_gend_t)   ();
	typedef void (*fn_gadd2v_t) ( GLfloat CONST* );

	static void gl_begin  ( GLenum prim )        { GOL::begin( prim );       }
	static void gl_end    ()                     { GOL::end();               }
	static void gl_add2v  ( GLfloat CONST* pt )  { GOL::vertex2fv( pt );     }

	static void rec_begin ( GLenum prim )
	{
		s_cur_type  = prim;
		s_cur_first = (GLint)( s_verts.size() / 2 );
	}
	static void rec_end   ()
	{
		GLsizei cnt = (GLsizei)( s_verts.size() / 2 ) - s_cur_first;
		if( cnt > 0 )
			s_strokes.push_back({ s_cur_type, s_cur_first, cnt });
	}
	static void rec_add2v ( GLfloat CONST* pt )
	{
		s_verts.push_back( pt[0] );
		s_verts.push_back( pt[1] );
	}

	static fn_gbegin_t  gfn_begin = gl_begin;
	static fn_gend_t    gfn_end   = gl_end;
	static fn_gadd2v_t  gfn_add2v = gl_add2v;
	// --- end recording infrastructure ---

#define BEGIN_STROKE( what )	gfn_begin( what )
#define END_STROKE()			gfn_end()
#define ADD2V( pt )				gfn_add2v( pt )

	FINLINE	static	void	DRAW_CERCLE()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
	}

	static void draw_arebase()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P36 );
			ADD2V( P25 );
			ADD2V( P23 );
			ADD2V( P32 );
			ADD2V( P33 );
			ADD2V( P35 );
			ADD2V( R4 );
			ADD2V( P28 );
			ADD2V( I1 );
			ADD2V( C7 );
			ADD2V( R0 );
			ADD2V( R7 );
			ADD2V( R6 );
			ADD2V( R5 );
		END_STROKE();
	}

	static void draw_a()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P28 );
			ADD2V( P48 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_b()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P40 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_c()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
		END_STROKE();
	}

	static void draw_d()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P08 );
			ADD2V( P48 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_e()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R0 );
		END_STROKE();
	}

	static void draw_f_fix()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( I0 );
			ADD2V( I2 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R6 );
			ADD2V( P14 );
			ADD2V( P05 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_f()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( I0L );
			ADD2V( I2L );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R6L );
			ADD2V( P13 );
			ADD2V( P05L );
			ADD2V( P07 );
		END_STROKE();
	}

	static void draw_g()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P28 );
			ADD2V( B4 );
			ADD2V( B5 );
			ADD2V( B6 );
			ADD2V( B7 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_h()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_i_fix()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P13 );
			ADD2V( P15 );
	//		ADD2V( P22 );
	//		ADD2V( P26 );
			ADD2V( R2 );
			ADD2V( R6 );
	//		ADD2V( P42 );
	//		ADD2V( P46 );
		END_STROKE();
	}

	static void draw_i()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( DOTI1 );
			ADD2V( DOTI2 );
			ADD2V( DOTI3 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( R2LLL );
			ADD2V( R6MX3 );
		END_STROKE();
	}

	static void draw_j()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P13 );
			ADD2V( P15 );
			ADD2V( P22 );
			ADD2V( P26 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R2 );
			ADD2V( P54 );
			ADD2V( P63 );
			ADD2V( P60 );
		END_STROKE();
	}

	static void draw_k()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( R0 );
			ADD2V( P28 );
			ADD2V( P32 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_l_fix()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( C2 );
			ADD2V( R6 );
		END_STROKE();
	}

	static void draw_l()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P40 );
			ADD2V( P00 );
		END_STROKE();
	}


	static void draw_n()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( PIEDBL );
			//ADD2V( R4 );
			//ADD2V( P48 );
		END_STROKE();
	}

	static void draw_m_fix()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R6 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P34 );
			ADD2V( P25 );
			ADD2V( P28 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_m_big()
	{
		draw_n();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( RC0 );
			ADD2V( RC1 );
			ADD2V( RC2 );
			ADD2V( RC3 );
			ADD2V( RC4 );
			ADD2V( P416 );
		END_STROKE();
	}

	static void draw_m()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( PIEDBL );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( RB1 );
			ADD2V( RB2 );
			ADD2V( RB3 );
			ADD2V( RB4 );
			ADD2V( PIEDBR );
		END_STROKE();
	}

	static void draw_o()
	{
		DRAW_CERCLE();
	}

	static void draw_p()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P60 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_q()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P28 );
			ADD2V( P68 );
		END_STROKE();
		DRAW_CERCLE();
	}

	static void draw_r()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
		END_STROKE();
	}

	static void draw_s()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R3 );
			ADD2V( R2 );
			ADD2V( R1 );
	//		ADD2V( R0 );
	//		ADD2V( R4 );
			ADD2V( P31 );
			ADD2V( P37 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
	}

	static void draw_t_fix()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( R6 );
			ADD2V( C2 );
			ADD2V( I0 );
			ADD2V( I2 );
		END_STROKE();
	}

	static void draw_t()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( R6LL );
			ADD2V( C2LL );
			ADD2V( P10 );
			ADD2V( P16 );
		END_STROKE();
	}

	static void draw_u()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P26 );
			ADD2V( P46 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20L );
			ADD2V( RLL7 );
			ADD2V( RLL6 );
			ADD2V( RLL5 );
			ADD2V( RLL4 );
		END_STROKE();
	}

	static void draw_v()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( R6 );
			ADD2V( P28);
		END_STROKE();
	}

	static void draw_w_fix()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( R0 );
			ADD2V( R7 );
			ADD2V( R6 );
			ADD2V( R2 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P34 );
			ADD2V( P45 );
			ADD2V( P48 );
			ADD2V( P28 );
		END_STROKE();
	}

	static void draw_w()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( R6 );
			ADD2V( P28);
			ADD2V( RB6 );
			ADD2V( P216 );
		END_STROKE();
	}

	static void draw_x()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P48 );
			ADD2V( P40 );
			ADD2V( P28 );
		END_STROKE();
	}

	static void draw_y()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( P44 );
			ADD2V( P60 );
			ADD2V( P28 );
		END_STROKE();
	}

	static void draw_z()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( P28 );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_A()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P22 );
			ADD2V( P26 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( C2 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_B()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( C6 );
			ADD2V( C5 );
			ADD2V( C4 );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( R6 );
			ADD2V( R5 );
			ADD2V( R4 );
			ADD2V( R3 );
			ADD2V( R2 );
		END_STROKE();
	}

	static void draw_C()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( C1 );
			ADD2V( C0 );
			ADD2V( R0 );
			ADD2V( R7 );
			ADD2V( R6 );
			ADD2V( R5 );
		END_STROKE();
	}

	static void draw_D()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( P00 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( P40 );
		END_STROKE();
	}

	static void draw_E()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( R2 );
		END_STROKE();
	}

	static void draw_F()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( P00 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20 );
			ADD2V( R2 );
		END_STROKE();
	}

	static void draw_G()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R2 );
			ADD2V( P28 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
		END_STROKE();
	}

	static void draw_H()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( P08 );
			ADD2V( P48 );
			ADD2V( P20 );
			ADD2V( P28 );
		END_STROKE();
	}

	static void draw_I_fix()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( C2 );
			ADD2V( R6 );
		END_STROKE();
	}

	static void draw_I()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( C2LL );
			ADD2V( R6LL );
		END_STROKE();
	}

	static void draw_J()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
		END_STROKE();
	}

	static void draw_K()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( P20 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_L()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_M()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( P00 );
			ADD2V( R2 );
			ADD2V( P08 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_N()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( P00 );
			ADD2V( P48 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_O()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
		END_STROKE();
	}

	static void draw_P()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( P00 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( C5 );
			ADD2V( C6 );
			ADD2V( P20 );
		END_STROKE();
	}

	static void draw_Q()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P35 );
			ADD2V( P49 );
		END_STROKE();
	}

	static void draw_R()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( P00 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( C5 );
			ADD2V( C6 );
			ADD2V( P20 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_S()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( C1 );
			ADD2V( C0 );
			ADD2V( C7 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
	}

	static void draw_T()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P08 );
			ADD2V( C2 );
			ADD2V( R6 );
		END_STROKE();
	}

	static void draw_U()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( R0 );
			ADD2V( R7 );
			ADD2V( R6 );
			ADD2V( R5 );
			ADD2V( R4 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_V()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( R6 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_W()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P40 );
			ADD2V( P34 );
			ADD2V( P48 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_X()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P48 );
			ADD2V( P40 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_Y()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( R2 );
			ADD2V( P40 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_Z()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P08 );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_0()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( C3 );
		END_STROKE();
	}

	static void draw_1()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C0 );
			ADD2V( C2 );
			ADD2V( R6 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_2()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( C5 );
			ADD2V( P40 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_3()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P08 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
	}

	static void draw_4()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C2 );
			ADD2V( R0 );
			ADD2V( R4 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( R2 );
			ADD2V( R6 );
		END_STROKE();
	}

	static void draw_5()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( P00 );
			ADD2V( P20 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
	}

	static void draw_6()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( R1 );
			ADD2V( R2 );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_7()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P08 );
			ADD2V( P40 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P22 );
			ADD2V( P26 );
		END_STROKE();
	}

	static void draw_8()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( R3 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( C5 );
			ADD2V( C4 );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( C1 );
			ADD2V( C0 );
			ADD2V( C7 );
		END_STROKE();
	}

	static void draw_9()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( C5 );
			ADD2V( C4 );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( C1 );
			ADD2V( C0 );
			ADD2V( C7 );
			ADD2V( C6 );
			ADD2V( C5 );
		END_STROKE();
	}

	static void draw_doublequote()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P12  );
			ADD2V( P22 );
			ADD2V( P16 );
			ADD2V( P26 );
		END_STROKE();
	}

	static void draw_dieze()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P12 );
			ADD2V( P42 );
			ADD2V( P16 );
			ADD2V( P46 );
			ADD2V( P20 );
			ADD2V( P28 );
			ADD2V( R0 );
			ADD2V( R4 );
		END_STROKE();
	}

	static void draw_dollar()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C3 );
			ADD2V( C2 );
			ADD2V( C1 );
			ADD2V( C7 );
			ADD2V( R3 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P04 );
			ADD2V( P54 );
		END_STROKE();
	}

	static void draw_percent()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P20 );

			ADD2V( P40 );
			ADD2V( P08 );

			ADD2V( P28 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_and()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P28 );
			ADD2V( R4 );
			ADD2V( R5 );
			ADD2V( R6 );
			ADD2V( R7 );
			ADD2V( R0 );
			ADD2V( R1 );
			ADD2V( C6 );
			ADD2V( P16 );
			ADD2V( P05 );
			ADD2V( P03 );
			ADD2V( P12 );
			ADD2V( P49 );
		END_STROKE();
	}

	static void draw_quote()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P14  );
			ADD2V( P24 );
		END_STROKE();
	}

	static void draw_apostrophe()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P12 );
			ADD2V( P24 );
		END_STROKE();
	}

	static void draw_parenthesis_open_fix()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P06 );
			ADD2V( P14 );
			ADD2V( P34 );
			ADD2V( P46 );
		END_STROKE();
	}

	static void draw_parenthesis_open()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P06L );
			ADD2V( P13 );
			ADD2V( P34L );
			ADD2V( P46L );
		END_STROKE();
	}

	static void draw_parenthesis_close_fix()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P02 );
			ADD2V( P14 );
			ADD2V( P34 );
			ADD2V( P42 );
		END_STROKE();
	}

	static void draw_parenthesis_close()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P01 );
			ADD2V( P13 );
			ADD2V( P34L );
			ADD2V( P41 );
		END_STROKE();
	}

	static void draw_bracket_open()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( C2 );
			ADD2V( B2 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_bracket_close()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( C2 );
			ADD2V( B2 );
			ADD2V( P40 );
		END_STROKE();
	}

	static void draw_acolade_open()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08 );
			ADD2V( P05 );
			ADD2V( P15 );
			ADD2V( R2 );
			ADD2V( P22 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P48 );
			ADD2V( P45 );
			ADD2V( P35 );
			ADD2V( R2 );
		END_STROKE();
	}

	static void draw_acolade_close()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00 );
			ADD2V( P03 );
			ADD2V( P13 );
			ADD2V( R2 );
			ADD2V( P26 );
		END_STROKE();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P40 );
			ADD2V( P43 );
			ADD2V( P33 );
			ADD2V( R2 );
		END_STROKE();
	}

	static void draw_circonflex()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P20 );
			ADD2V( P14 );
			ADD2V( P28 );
		END_STROKE();
	}

	FINLINE	static void draw_dot_fix()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V(	R6	);
			ADD2V(	P45	);
			ADD2V(	P43	);
		END_STROKE();
	}
	FINLINE	static void draw_dot()
	{
		BEGIN_STROKE(	GL_LINE_LOOP );
			ADD2V(	R6MX5	);
			ADD2V(	P43MX3	);
			ADD2V(	P43MX5	);
		END_STROKE();
	}
	FINLINE	static void draw_dot_p2()
	{
		BEGIN_STROKE(	GL_LINE_LOOP );
			ADD2V(	R6MX3	);
			ADD2V(	P43L	);
			ADD2V(	P43MX3	);
		END_STROKE();
	}
	FINLINE	static void draw_dot_up()
	{
		BEGIN_STROKE(	GL_LINE_LOOP );
			ADD2V(	P20L	);
			ADD2V( P23LLL	);
			ADD2V( P23MX5	);
		END_STROKE();
	}
	FINLINE	static void draw_comma()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			//GOL::vertex2vf( P34MX5 );
			//GOL::vertex2vf( R6MX5  );
			ADD2V( S1 );
			ADD2V(	P43MX3	);
			ADD2V(	P43MX5	);
		END_STROKE();
	}

	static void draw_colom()
	{
		draw_dot_up();
		draw_dot();
	}

	static void draw_semicolom()
	{
		draw_dot_up();
		draw_comma();
	}

	static void draw_inferior()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P08  );
			ADD2V( P20 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_equal()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( I0 );
			ADD2V( I2 );
			ADD2V( R1 );
			ADD2V( R3 );
		END_STROKE();
	}

	static void draw_superior()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P00  );
			ADD2V( P28 );
			ADD2V( P40 );
		END_STROKE();
	}

	static void draw_interrogation()
	{
		draw_dot_fix();
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C0 );
			ADD2V( C1 );
			ADD2V( C2 );
			ADD2V( C3 );
			ADD2V( C4 );
			ADD2V( C5 );
			ADD2V( P24 );
			ADD2V( P34 );
		END_STROKE();
	}

	static void draw__()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P40M );
			ADD2V( P411 );
		END_STROKE();
	}

	static void draw_star()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P20);
			ADD2V( P28);
			ADD2V( P36);
			ADD2V( P12);
			ADD2V( P32);
			ADD2V( P16);
		END_STROKE();
	}

	static void draw_plus()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P21 );
			ADD2V( P28 );
			ADD2V( I14 );
			ADD2V( P34 );
		END_STROKE();
	}


	static void draw_minus()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P21 );
			ADD2V( P28 );
		END_STROKE();
	}

	static void draw_exclamation()
	{
		draw_dot_p2();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P34MX3 );
			ADD2V( C2MX3 );
		END_STROKE();
	}

	static void draw_slash()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P60 );
			ADD2V( P08 );
		END_STROKE();
	}

	static void draw_backslash()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( P00 );
			ADD2V( P68 );
		END_STROKE();
	}

	static void draw_bar_vert()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( C2 );
			ADD2V( B6 );
		END_STROKE();
	}

	static void draw_tilde()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( P30 );
			ADD2V( P31 );
			ADD2V( P32 );
			ADD2V( P44 );
			ADD2V( P46 );
			ADD2V( R5 );
			ADD2V( R4 );
		END_STROKE();
	}
// used for flatland
	static void draw_parent()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( C4 );
			ADD2V( R0 );
			ADD2V( B4 );
		END_STROKE();
	}

	static void draw_child()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( C0 );
			ADD2V( R4 );
			ADD2V( P50 );	//todo 50 is B0 or B4 is 54
		END_STROKE();
	}

	static void draw_plus_low()
	{
		//plus
		ADD2V( P34LL	);
		ADD2V( P36		);
		ADD2V( P24D	);
		ADD2V( P44U	);
	}

	static void draw_box()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( P20 );
			ADD2V( P40 );
			ADD2V( P48 );
			ADD2V( P28 );
		END_STROKE();
	}
	static void draw_ref()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( P20 );
			ADD2V( P40 );
			ADD2V( P312 );
		END_STROKE();
	}

	static void draw_box_empty()
	{
		draw_box();
		BEGIN_STROKE( GL_LINES );
			//top
			ADD2V( C2 );
			ADD2V( R2 );
			//bottom
			ADD2V( R6 );
			ADD2V( B6 );
			//right
			ADD2V( R4 );
			ADD2V( P312 );
		END_STROKE();
	}
	static void draw_box_plus()
	{
		draw_box();
		BEGIN_STROKE( GL_LINES );
			//top
			ADD2V( C2 );
			ADD2V( R2 );
			//bottom
			ADD2V( R6 );
			ADD2V( B6 );
			//right
			ADD2V( R4 );
			ADD2V( P312 );
			//plus
			draw_plus_low();
		END_STROKE();
	}
	static void draw_box_minus()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( C2 );
			ADD2V( B6 );
			ADD2V( P34 );
			ADD2V( P312 );
		END_STROKE();

	}


	static void draw_box_plus_end()
	{
		draw_box();
		BEGIN_STROKE( GL_LINES );
			//top
			ADD2V( C2 );
			ADD2V( R2 );
			//right
			ADD2V( R4 );
			ADD2V( P312 );

			draw_plus_low();
		END_STROKE();
	}
	static void draw_box_empty_end()
	{
		draw_box();
		BEGIN_STROKE( GL_LINES );
			//top
			ADD2V( C2 );
			ADD2V( R2 );
			//right
			ADD2V( R4 );
			ADD2V( P312 );
		END_STROKE();
	}
	static void draw_box_minus_end()
	{
		BEGIN_STROKE( GL_LINE_STRIP );
			ADD2V( C2 );
			ADD2V( P34 );
			ADD2V( P312 );
		END_STROKE();
	}

	static void draw_box_obj()
	{
		//draw_box();
		BEGIN_STROKE( GL_LINE_LOOP );
			GOL::vertex2f( PT_X(	-2	),	PT_Y(	6	) );
			GOL::vertex2f( PT_X(	10	),	PT_Y(	6	) );
			GOL::vertex2f( PT_X(	10	),	PT_Y(	18	) );
			GOL::vertex2f( PT_X(	-2	),	PT_Y(	18	) );
		END_STROKE();
	}

	static void draw_obj_plus_end()
	{
		draw_box_obj();
		BEGIN_STROKE( GL_LINES );
			//top
			//ADD2V( C2 );
			//ADD2V( I1L	);
			//right
			ADD2V( P310 );
			ADD2V( P312 );

			draw_plus_low();
		END_STROKE();
	}
	static void draw_obj_empty_end()
	{
		draw_box_obj();
		BEGIN_STROKE( GL_LINES );
			//top
			//ADD2V( C2 );
			//ADD2V( I1L );
			//right
			ADD2V( P310 );
			ADD2V( P312 );
		END_STROKE();
	}

	static void draw_connect_left_to_down()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( R0 );
			ADD2V( P310 );
			ADD2V( P34 );
			ADD2V( B6 );
		END_STROKE();
	}

	static void draw_connect_left_to_right()
	{
		BEGIN_STROKE( GL_LINES );
			ADD2V( R0 );
			ADD2V( P310 );
		END_STROKE();
	}

	static void draw_trax()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( C0 );
			ADD2V( C4 );
			ADD2V( B4 );
			ADD2V( P50 );
		END_STROKE();
		BEGIN_STROKE( GL_LINES );
			ADD2V( P0 );
			ADD2V( P1 );
		END_STROKE();
	}

	static void draw_no()
	{
		BEGIN_STROKE( GL_LINE_LOOP );
			ADD2V( P40 );
			ADD2V( P00 );
			ADD2V( P08 );
			ADD2V( P48 );
		END_STROKE();
	}

	static void draw_blank()
	{
	}

	static void draw_tab()
	{
		translate_char( 4 );
	}

	CONSTEXPR INT32 ALPHABET_CHAR_MAX = 143;
	p_fn_draw str_draw_array[ALPHABET_CHAR_MAX] =
	{	//0-15
		draw_no,				draw_no,				draw_no,				draw_no,
		draw_no,				draw_no,				draw_no,				draw_no,
		draw_no,				draw_tab,				draw_cr,				draw_no,
		draw_no,				draw_cr,				draw_no,				draw_no,
		//16-31
		draw_no,				draw_no,				draw_no,				draw_no,
		draw_no,				draw_no,				draw_no,				draw_no,
		draw_no,				draw_no,				draw_no,				draw_no,
		draw_no,				draw_no,				draw_no,				draw_no,
		//32-47
		draw_blank,				draw_exclamation,		draw_doublequote,		draw_dieze,
		draw_dollar,			draw_percent,			draw_and,				draw_quote,
		draw_parenthesis_open,	draw_parenthesis_close,	draw_star,				draw_plus,
		draw_comma,				draw_minus,				draw_dot,				draw_slash,
		//48-63
		draw_0,					draw_1,					draw_2,					draw_3,
		draw_4,					draw_5,					draw_6,					draw_7,
		draw_8,					draw_9,					draw_colom,				draw_semicolom,
		draw_inferior,			draw_equal,				draw_superior,			draw_interrogation,
		//64-79
		draw_arebase,			draw_A,					draw_B,					draw_C,
		draw_D,					draw_E,					draw_F,					draw_G,
		draw_H,					draw_I,					draw_J,					draw_K,
		draw_L,					draw_M,					draw_N,					draw_O,
		//80-95
		draw_P,					draw_Q,					draw_R,					draw_S,
		draw_T,					draw_U,					draw_V,					draw_W,
		draw_X,					draw_Y,					draw_Z,					draw_bracket_open,
		draw_backslash,			draw_bracket_close,		draw_circonflex,		draw__,
		//96-111
		draw_apostrophe,		draw_a,					draw_b,					draw_c,
		draw_d,					draw_e,					draw_f,					draw_g,
		draw_h,					draw_i,					draw_j,					draw_k,
		draw_l,					draw_m,					draw_n,					draw_o,
		//112-127
		draw_p,					draw_q,					draw_r,					draw_s,
		draw_t,					draw_u,					draw_v,					draw_w,
		draw_x,					draw_y,					draw_z,					draw_acolade_open,
		draw_bar_vert,			draw_acolade_close,		draw_tilde,				draw_no,
		//128-
		draw_parent,				// 128
		draw_child,					// 129

		draw_box_plus,				// 130
		draw_box_minus,				// 131
		draw_box_empty,				// 132

		draw_box_plus_end,			// 133
		draw_box_minus_end,			// 134
		draw_box_empty_end,			// 135

		draw_connect_left_to_down,	// 136
		draw_connect_left_to_right,	// 137

		draw_trax,					// 138
		draw_ref,					// 139	
		draw_blank,					// 140

		draw_obj_plus_end,			// 141
		draw_obj_empty_end,			// 142

	};

	REAL CONST MOVE_NO	= REAL(0.);
	REAL CONST MOVE_ONE	= REAL(1.);
	REAL CONST MOVE_I	= REAL(.5);
	REAL CONST MOVE_L	= REAL(.3);
	REAL CONST MOVE_DOT	= REAL(.3);
	REAL CONST MOVE_T	= REAL(.7);
	REAL CONST MOVE_N	= REAL(.8);
	REAL CONST MOVE_V	= REAL(.9);
	REAL CONST MOVE_W	= REAL(1.5);
	REAL CONST MOVE_M	= REAL(1.4);
	REAL	str_draw_translate_x[ALPHABET_CHAR_MAX] =
	{	//0-15
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		//16-31
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		MOVE_NO,		MOVE_NO,		MOVE_NO,		MOVE_NO,
		//32-47
		MOVE_N,			MOVE_I,			MOVE_N,			MOVE_ONE,	//	 !"#
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_I,		//	$%&'
		MOVE_T,			MOVE_T,			MOVE_ONE,		MOVE_ONE,	//	()*+
		MOVE_DOT,		MOVE_N,			MOVE_DOT,		MOVE_ONE,	//	,-./
		//48-63
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	0123
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	4567
		MOVE_ONE,		MOVE_ONE,		MOVE_DOT,		MOVE_DOT,		//	89:;
		MOVE_ONE,		MOVE_N,			MOVE_ONE,		MOVE_ONE,	//	<=>?
		//64-79
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	ArebaseABC
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	DEFG
		MOVE_ONE,		MOVE_I,			MOVE_ONE,		MOVE_ONE,	//	HIJK
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	LMNO
		//80-95
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	PQRS
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,	//	TUVW
		MOVE_ONE,		MOVE_ONE,		MOVE_ONE,		MOVE_T,		//	XYZ[
		MOVE_N,			MOVE_T,			MOVE_N,			MOVE_ONE,	//	\]^_
		//96-111
		MOVE_I,			MOVE_ONE,		MOVE_ONE,		MOVE_V,		//	'abc
		MOVE_ONE,		MOVE_ONE,		MOVE_T,			MOVE_ONE,	//	defg
		MOVE_ONE,		MOVE_I,			MOVE_V,			MOVE_ONE,	//	hijk
		MOVE_L,			MOVE_M,			MOVE_N,			MOVE_ONE,	//	lmno
		//112-127
		MOVE_ONE,		MOVE_ONE,		MOVE_N,			MOVE_V,		//	pqrs
		MOVE_T,			MOVE_N,			MOVE_V,			MOVE_W,		//	tuvw
		MOVE_ONE,		MOVE_V,			MOVE_ONE,		MOVE_T,		//	xyz{
		MOVE_ONE,		MOVE_T,			MOVE_N,			MOVE_NO,	//	|}~
		//128-
		MOVE_ONE,	MOVE_ONE,

		MOVE_ONE,	MOVE_ONE,	MOVE_ONE,
		MOVE_ONE,	MOVE_ONE,	MOVE_ONE,
		MOVE_ONE,	MOVE_ONE,
		MOVE_ONE,	MOVE_ONE,	MOVE_ONE,
		MOVE_ONE,	MOVE_ONE,
	};

	CONST	REAL	LEFT = -33.;
	CONST	REAL	LEN = 32.;
	CONST	REAL	RIGHT = LEFT + LEN;
	CONST	REAL	CENTER = LEFT + LEN * .5;
	CONST	REAL	TOP = 1.;
	CONST	REAL	TOP_SMALL = REAL(.9);
	CONST	REAL	BOTTOM = REAL(.2);
	CONST	REAL	BOTTOM_SMALL = REAL(.3);
	CONST	REAL	MIDDLE = ( TOP + BOTTOM ) * .5;
	CONST	REAL	LEN_B = 1.;
	CONST	REAL	LEN_BS = .5;


	INT32 LIST_SLIDER_BEFORE	=	0;
	INT32 LIST_SLIDER_AFTER		=	0;
	INT32 LIST_SLIDER_INT32		=	0;
	INT32 LIST_SLIDER_REAL		=	0;
	INT32 LIST_SLIDER_BOOL_ON	=	0;
	INT32 LIST_SLIDER_BOOL_OFF	=	0;

	INT32 	compile_lists()
	{
		INT32 list_str		=	GOL::gen_lists(	256 );
		if( list_str == 0 )
			return 0;
		INT32 LIST_SLIDER_BEFORE	=	list_str +	200;
		INT32 LIST_SLIDER_AFTER		=	list_str +	201;
		INT32 LIST_SLIDER_INT32		=	list_str +	202;
		INT32 LIST_SLIDER_REAL		=	list_str +	203;
		INT32 LIST_SLIDER_BOOL_ON	=	list_str +	204;
		INT32 LIST_SLIDER_BOOL_OFF	=	list_str +	205;

		INT32	i;
		for( i = 0; i < ALPHABET_CHAR_MAX; ++i )
		{
			GOL::compile_list( list_str + i );
				(*str_draw_array[i])();
				translate_char( str_draw_translate_x[i] );
			GOL::end_list();
		}

		REAL	f;
		GOL::compile_list( LIST_SLIDER_BEFORE );
			GOL::color_black();

			BEGIN_STROKE( GL_TRIANGLE_STRIP );
				GOL::vertex2( LEFT - LEN_B,		REAL(0.1) );
				GOL::vertex2( LEFT - LEN_B,		TOP );
				GOL::vertex2( RIGHT + LEN_B,	REAL(0.1) );
				GOL::vertex2( RIGHT + LEN_B,	TOP );
			END_STROKE();
		GOL::end_list();

		GOL::compile_list( LIST_SLIDER_INT32 );
			BEGIN_STROKE( GL_LINE_LOOP );
				GOL::vertex2( -LEN_B,	BOTTOM );
				GOL::vertex2( LEN_B,	BOTTOM );
				GOL::vertex2( LEN_B,	TOP );
				GOL::vertex2( -LEN_B,	TOP );
			END_STROKE();
		GOL::end_list();

		GOL::compile_list( LIST_SLIDER_REAL );
			BEGIN_STROKE( GL_LINE_LOOP );
				GOL::vertex2( -LEN_B,	BOTTOM );
				GOL::vertex2( LEN_B,	BOTTOM );
				GOL::vertex2( 0,		TOP );
			END_STROKE();
		GOL::end_list();

		GOL::compile_list( LIST_SLIDER_BOOL_ON );
			BEGIN_STROKE( GL_LINE_STRIP );
				GOL::vertex2( CENTER + LEN_BS, TOP );
				GOL::vertex2( CENTER - LEN_BS, TOP );
				GOL::vertex2( CENTER - LEN_BS, BOTTOM );
				GOL::vertex2( CENTER + LEN_BS, BOTTOM );
			END_STROKE();
			f = LEN_B;
			f *= 4.;
			f += CENTER;
		//	if( b_active_on)
		//		GOL::color4( 1-r, 1-g, 1-b, .75 );
			BEGIN_STROKE( GL_LINE_STRIP );
				GOL::vertex2( CENTER,	BOTTOM_SMALL );
				GOL::vertex2( f,		BOTTOM_SMALL );
				GOL::vertex2( f,		TOP_SMALL );
				GOL::vertex2( CENTER,	TOP_SMALL );
			END_STROKE();
		GOL::end_list();

		GOL::compile_list( LIST_SLIDER_BOOL_OFF );
			BEGIN_STROKE( GL_LINE_STRIP );
				GOL::vertex2( CENTER-LEN_BS, BOTTOM );
				GOL::vertex2( CENTER+LEN_BS, BOTTOM );
				GOL::vertex2( CENTER+LEN_BS, TOP );
				GOL::vertex2( CENTER-LEN_BS, TOP );
			END_STROKE();
			f = -LEN_B;
			f *= 4.;
			f += CENTER;
		//	if( b_active_on)
		//		GOL::color4( 1-r, 1-g, 1-b, .75 );
			BEGIN_STROKE( GL_LINE_STRIP );
				GOL::vertex2( CENTER, BOTTOM_SMALL );
				GOL::vertex2( f, BOTTOM_SMALL );
				GOL::vertex2( f, TOP_SMALL );
				GOL::vertex2( CENTER, TOP_SMALL );
			END_STROKE();
		GOL::end_list();

		return list_str;
	}


	INT32	build_vbo()
	{
		// Recording pass: run each printable char's draw fn with recording hooks
		gfn_begin = rec_begin;
		gfn_end   = rec_end;
		gfn_add2v = rec_add2v;

		s_verts.clear();
		s_strokes.clear();

		for( INT32 i = 0; i < 32; ++i )
		{
			s_char_data[i].first_stroke = 0;
			s_char_data[i].stroke_nb    = 0;
		}
		for( INT32 i = 32; i < ALPHABET_CHAR_MAX; ++i )
		{
			s_char_data[i].first_stroke = (GLint)s_strokes.size();
			(*str_draw_array[i])();
			s_char_data[i].stroke_nb    = (GLsizei)s_strokes.size() - s_char_data[i].first_stroke;
		}

		// Restore GL passthrough
		gfn_begin = gl_begin;
		gfn_end   = gl_end;
		gfn_add2v = gl_add2v;

		if( s_verts.empty() )
			return 0;

		// Upload to GPU
		GOL::gen_buffer( s_vbo );
		GOL::bind_buffer( GL_ARRAY_BUFFER, s_vbo );
		GOL::set_buffer_data( GL_ARRAY_BUFFER,
			(GLsizeiptr)( s_verts.size() * sizeof(GLfloat) ),
			s_verts.data(), GL_STATIC_DRAW );
		GOL::unbind_buffer( GL_ARRAY_BUFFER );

		s_verts = std::vector<GLfloat>();	// free CPU-side vertex buffer
		return (INT32)s_vbo;
	}

	void	stroke_char( UINT8 CONST* str, INT32 len )
	{
		GOL::bind_buffer( GL_ARRAY_BUFFER, s_vbo );
		GOL::set_pointer_vertex2( (FP32*)nullptr );

		for( ; len > 0; --len )
		{
			UINT8 code = *str++;
			if( code >= ALPHABET_CHAR_MAX )
				code = 0;
			maa_char_t CONST& ch = s_char_data[code];
			for( GLsizei s = 0; s < ch.stroke_nb; ++s )
			{
				maa_stroke_t CONST& sk = s_strokes[(INT32)ch.first_stroke + s];
				GOL::draw_arrays( sk.type, sk.count, sk.first );
			}
			translate_char( str_draw_translate_x[code] );
		}

		GOL::unbind_buffer( GL_ARRAY_BUFFER );
	}


	FINLINE	void	draw_control_before()
	{
	//	GOL::push_attrib(GL_CURRENT_BIT );
		//todoatt should we push the color ?
		GOL::call_list( LIST_SLIDER_BEFORE );
		GOL::color4v( color_char );
	}

	FINLINE	void	draw_control_after()
	{
	//	GOL::pop_attrib();
	}

	void	draw_switch_line( bool CONST b_on )
	{
		draw_control_before();
		if( b_on )
			GOL::call_list( LIST_SLIDER_BOOL_ON );
		else
			GOL::call_list( LIST_SLIDER_BOOL_OFF );
	// todo this function does nothing
	//	draw_control_after();
	}

	void	draw_slider_line( REAL CONST in, INT32 CONST mark_nb, bool CONST b_left, bool CONST b_right )
	{
		REAL	f = LEFT + in * LEN;
		draw_control_before();

		GOL::begin( GL_LINES );
			if( b_left )
			{
				GOL::vertex2( LEFT,	BOTTOM	);
				GOL::vertex2( LEFT,	TOP		);
			}
			if( b_right)
			{
				GOL::vertex2( RIGHT, BOTTOM	);
				GOL::vertex2( RIGHT, TOP	);
			}
			if( mark_nb > 1 && mark_nb < 33 )
			{
				REAL	step = LEN / mark_nb;
				REAL	x = LEFT + step;
	//			INT32	i;
				for( INT32 i = 1; i < mark_nb; ++i )
				{
					GOL::vertex2( x,	BOTTOM_SMALL );
					GOL::vertex2( x,	TOP_SMALL	);
					x += step;
				}
			}
		GOL::end();

		GOL::matrix::translate( f );
		if( mark_nb )
			GOL::call_list( LIST_SLIDER_INT32 );
		else
			GOL::call_list( LIST_SLIDER_REAL );
		GOL::matrix::translate( -f );
		

	// todo this does nothing
	//	draw_control_after();
	}	

}	//namespace aaa::alphabet::maa
