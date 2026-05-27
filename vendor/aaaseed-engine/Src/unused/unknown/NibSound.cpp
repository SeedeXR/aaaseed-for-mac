#include "nibsound.h"
#include "seedcam.h"
#include "bind_img.h"
#include "layer.h"
#include "nib.h"
#include "aaa_math.h"
#include "model.h"
#include "layers.h"
#include "serial.h"
#include "aaa_util.h"
#include "img.h"

c_nibsound *	nibsound = NULL;

typedef enum {
	RED_LEFT,
	GREEN_LEFT,
	BLUE_LEFT,
	ALPHA_LEFT,

	RED_CENTER,
	GREEN_CENTER,
	BLUE_CENTER,
	ALPHA_CENTER,

	RED_RIGHT,
	GREEN_RIGHT,
	BLUE_RIGHT,
	ALPHA_RIGHT,

	NIB_SOUND_VAR_MAX_NB
	}
NIB_SOUND_VAR;

static	INT32	last_int32[26];
static	REAL	last_real[26];

#define	NIBSOUND_PARAM_NB_MAX	11
ST_PARAM	nibsound_param[NIBSOUND_PARAM_NB_MAX]
	=
	{
		{	(void *) NULL,			PARAM_BOOL, "verbose",			.1, 0.,	0., 1., NULL, NULL },
		{	(void *) NULL,			PARAM_BOOL,	"send_only_change",	.1, 0.,	0., 1., NULL, NULL },
		{	(void *) NULL,			PARAM_FLOAT,"scaling_factor",	100., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,			PARAM_BOOL,	"ring_send",		.1, 0.,	0., 1., NULL, NULL },
		{	(void *) NULL,			PARAM_BOOL,	"position_send",	.1, 0.,	0., 1., NULL, NULL },
		{	(void *) NULL,			PARAM_BOOL,	"rgba_send",		.1, 0.,	0., 1., NULL, NULL },
		{	(void *) NULL,			PARAM_FLOAT,"rgba_dist",		.1, 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,			PARAM_FLOAT,"rgba_angle",		.25, .33,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,			PARAM_INT32,"rgba_bind",		0, 1,	1, BIND_2D_MAX_NB, NULL, NULL },
		{	(void *) NULL,			PARAM_FLOAT,"rgba_size_u",		0, 1,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,			PARAM_FLOAT,"rgba_size_v",		0, 1,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
	};

void	c_nibsound::param_init_pt()
{
INT32	h=0;
	param_pt[h++].pt = (void *)&b_verbose;
	param_pt[h++].pt = (void *)&b_send_only_change;
	param_pt[h++].pt = (void *)&scaling_factor;	
	param_pt[h++].pt = (void *)&b_ring_send;
	param_pt[h++].pt = (void *)&b_position_send;
	param_pt[h++].pt = (void *)&b_rgba_send;
	param_pt[h++].pt = (void *)&rgba_dist;
	param_pt[h++].pt = (void *)&rgba_angle;
	param_pt[h++].pt = (void *)&rgba_bind;
	param_pt[h++].pt = (void *)&rgba_size_u;
	param_pt[h++].pt = (void *)&rgba_size_v;

	err_param_init_pt(h);
}

c_nibsound::c_nibsound()
{
	set_name("NibSound");
	param_init_with( nibsound_param, NIBSOUND_PARAM_NB_MAX);
}

c_nibsound::~c_nibsound()
{
}

#ifdef	WIN32
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <conio.h>
#endif

#define	BUF_SIZE	64
UINT8	str_max[BUF_SIZE];

void
#ifdef	WIN32
__cdecl
#endif
loop_receive(void *dummy)
{
c_nibsound *	dad = (c_nibsound *) dummy;
c_serial *		ser = dad->max_serial;	
INT32	i;

	GOOD_PRINT_STRING( "looping reception nibsound" );
	while(1)
		{
		for( i=0; i<BUF_SIZE; i++)
			{
			while( ERR( ser->read( &str_max[i]) ) );
			if( str_max[i] == 0xff )
				break;
			}
		if ( i == BUF_SIZE)
			GOOD_PRINT_STRING( "Nibsound didn't get full str" );
		else
			{
//			printf( "Nibsound get %d\n", i);
			str_max[i] = 0;
			if ( i == 0 )
				GOOD_PRINT_STRING( "o" );
			else if ( i == 2 )
				{
				GOOD_PRINT_STRING( "NIBSOUND: %s", str_max );
				switch( str_max[0] )
					{
					case 'A':
						GOOD_PRINT_STRING( "A %d", (INT32) str_max[1] );
						break;
					case 'B':
						GOOD_PRINT_STRING( "B %d", (INT32) str_max[1] );
						break;
					case 'C':
						GOOD_PRINT_STRING( "C %d", (INT32) str_max[1] );
						break;
					case 'D':
						GOOD_PRINT_STRING( "D %d", (INT32) str_max[1] );
						break;
					case 'P':
						GOOD_PRINT_STRING( "P %d", (INT32) str_max[1] );
						break;
					case 'V':
						GOOD_PRINT_STRING( "V %d", (INT32) str_max[1] );
						break;
					}
				}
			else
				ERR_PRINT_STRING( "too long" );
			}
		}
	/* _endthread could be implied */
//    _endthread();
}
  

void	c_nibsound::init( INT32 port_nb)
{
	refresh_index = 0;
	max_serial = new c_serial;
	max_serial->set_baudrate( SERIAL_BAUDRATE_57600);
	if( ERR( max_serial->open( port_nb) ) )
		{
		BOX_ERR("Can open communication with MAX");
		}
	else
		{
#ifdef	WIN32
		_beginthread( loop_receive, 0, this );
#endif
		//nowin what happen if not windows
		}
}

void	c_nibsound::print_str_out()
{
	if( b_verbose )
	{
		if( str_cur != str_out )
		{
			//*str_cur++ = '\n';
			*str_cur = 0;
			GOOD_PRINT_STRING( str_out );
		}
		str_cur = str_out;
	}
}


void	c_nibsound::send_ring()
{
REAL	rb, rs;
REAL	x,y,z;
c_model	*model_ring;

//toclean	to recusitate
//	model_ring = layer_array[LAYER_RING].get_model();
	rb = ABS( model_ring->get_size_index(0) ) * .5;
	rs = ABS( model_ring->get_size_index(1) ) * .5;

	send_value( 0, nib->ring_pos_[1] );		//A	hauteur
	send_value( 1, rb );					//B	rayon anneau
	send_value( 2, rs );					//C rayon boudin

	x = camera_cur->get_pos(0);
	y = camera_cur->get_pos(1);
	z = camera_cur->get_pos(2);

	x -= nib->ring_pos[0];
	y -= nib->ring_pos[1];
	z -= nib->ring_pos[2];

	x = SQRT( x*x + z*z);
	send_value( 3, x );		//D	distance horizontale au centre

	x = x - rb + rs;
	x = ABS(x);
	x = SQRT( x*x + y*y);
	x -= rs;
	send_value( 4, x );		//E distance a la peau de l'anneau
							//
	print_str_out();

}

void	c_nibsound::send_position()
{
REAL x,y,z;
REAL tmp;

//cam
	x = camera_cur->get_pos(0);
	y = camera_cur->get_pos(1);
	z = camera_cur->get_pos(2);

	send_value( 5, y);	//F hauteur sous plafond
	print_str_out();

//toclean	to recusitate
/*
	x -= layer_array[LAYER_FONTAIN].tra[0]; 
	y -= layer_array[LAYER_FONTAIN].tra[1]; 
	z -= layer_array[LAYER_FONTAIN].tra[2]; 
*/
	tmp = SQRT( x*x + z*z);
	send_value( 8, tmp);	//F hauteur sous plafond
	print_str_out();

}

extern	void	tex_2d_get_image_data( ST_IMAGE **hd_img, INT32 index);
extern	ST_IMAGE    *img_bind[];

void	c_nibsound::send_rgba_low( REAL u, REAL v, INT32 index)
{
INT32	color;

	u = u / rgba_size_u + .5;
	u = CLAMP( (REAL) 0., u, (REAL) 1.);
	v = v / rgba_size_v + .5;
	v = CLAMP( (REAL) 0., v, (REAL) 1.);
	color = *(INT32 *) img_get_color4ubv_from_uv( img_bind[rgba_bind], v, u);

	send_value( index+0, (INT32) (color & 0xff) );
	send_value( index+1, (INT32) ( (color>>8) & 0xff) );
	send_value( index+2, (INT32) ( (color>>16) & 0xff) );
	send_value( index+3, (INT32) ( (color>>24) & 0xff) );
}

void	c_nibsound::send_rgba()
{
	FP32 vec[3];
	tex_2d_get_image_data( &img_bind[rgba_bind], rgba_bind);

	send_rgba_low( camera_cur->get_pos(0), camera_cur->get_pos(2), 0);

	vec[0] = -SIN_INT(rgba_angle) * rgba_dist;
	vec[1] = 0;
	vec[2] = -COS_INT(rgba_angle) * rgba_dist;
	camera_cur->coor_camera_to_world( vec);
	send_rgba_low( vec[0], vec[2], 4);

	vec[0] = SIN_INT(rgba_angle) * rgba_dist;
	vec[1] = 0;
	vec[2] = -COS_INT(rgba_angle) * rgba_dist;
	camera_cur->coor_camera_to_world( vec);
	send_rgba_low( vec[0], vec[2], 8);

	print_str_out();
}

void	c_nibsound::send_refresh()
{
INT32	ind;
	refresh_index++;
	refresh_index = IMOD( refresh_index, 52);
	ind = refresh_index >> 1;
	if ( refresh_index & 1 )
		send_value( ind, last_int32[ind] );
	else
		send_value( ind, last_real[ind] );
	print_str_out();
}

void	c_nibsound::send()
{
	send_begin();

//	send_value( 0, dummy_int32[0]);	
//	send_value( 1, dummy_float[0]);

	if( b_ring_send)
		send_ring();
	if( b_position_send)
		send_position();
	if( b_rgba_send)
		send_rgba();
	if( b_send_only_change )
		send_refresh();

	send_end();
}

void	c_nibsound::send_begin()
{
	if( b_verbose)
		str_cur = &str_out[0];
	send_char( (char)0xff);
}

void	c_nibsound::send_end()
{
	print_str_out();
}

void	c_nibsound::send_value( INT32 symbol, INT32	value_in)
{
	if ( !b_send_only_change || last_int32[symbol] != value_in )
		{
		INT8	symbol_letter = 'a' + symbol;
	
		send_char( symbol_letter);
		if( b_verbose)
			{
			*str_cur++ = symbol_letter; 
			*str_cur++ = ':';
			}
		send_int8( value_in);
		send_char( (char)0xff);
		last_int32[symbol] = value_in;
		}
}

void	c_nibsound::send_value( INT32 symbol, REAL	value_in)
{
	if ( !b_send_only_change || last_real[symbol] != value_in )
		{
		INT8	symbol_letter = 'A' + symbol;
		
		send_char( symbol_letter);
		if( b_verbose)
			{
			*str_cur++ = symbol_letter; 
			*str_cur++ = ':';
			}
		send_real( value_in * scaling_factor);
		send_char( (char)0xff);
		last_real[symbol] = value_in;
		}
}

void	c_nibsound::send_char( unsigned char value_in)
{
	max_serial->write( (const unsigned char *)&value_in  );
}

void	c_nibsound::send_int8( UINT32 value_in)
{
unsigned char	nib_str;
	nib_str = (char) MIN( value_in, (UINT32)254);
	send_char( nib_str);

	if( b_verbose)
		{
		sprintf( str_cur, "%d ", (UINT32) nib_str);
		str_cur += strlen( str_cur);
		}
}

void	c_nibsound::send_real( REAL value_in)
{
char	nib_str[4];
INT32	value;
	value = value_in*128.;
	if ( value < 0)
		{
		value = -value;
		nib_str[0] = 0x40 | ( (value>>14) & 0x3f );
		}
	else
		nib_str[0] = (value>>14) & 0x3f;
	nib_str[1] = (value>>7) & 0x7f;
	nib_str[2] = value & 0x7f;
	max_serial->write( (const unsigned char *)nib_str, 3);

	if( b_verbose)
		{
		sprintf( str_cur, "%f ", value_in);
		str_cur += strlen( str_cur);
		}

}