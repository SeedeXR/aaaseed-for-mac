
/*
Make spots and stripes with reaction-diffusion.

The spot-formation system is described in the article:

  "A Model for Generating Aspects of Zebra and Other Mammalian
   Coat Patterns"
  Jonathan B. L. Bard
  Journal of Theoretical Biology, Vol. 93, No. 2, pp. 363-385
  (November 1981)

The stripe-formation system is described in the book:

  Models of Biological Pattern Formation
  Hans Meinhardt
  Academic Press, 1982


Permission is granted to modify and/or distribute this program so long
as the program is distributed free of charge and this header is retained
as part of the program.

Copyright (c) Greg Turk, 1991

*/
//todonow	no such license anymore

#include "spots.h"
#include "image/img.h"		 
#include "image/bind_img.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/param/param_declare.h"

extern	double	atof();
#ifdef WIN32
extern	REAL	drand48();
#endif

namespace	n_difrea
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 13;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REAL_ZERO( speed )

		PARAM_DEF_REAL_ZERO( p1 )
		PARAM_DEF_REAL_ZERO( p2 )
		PARAM_DEF_REAL_ZERO( p3 )

		PARAM_DEF_REAL_ZERO( diff1 )
		PARAM_DEF_REAL_ZERO( diff2 )

		PARAM_DEF_REAL_ZERO( beta_init )
		PARAM_DEF_REAL_ZERO( beta_rand )

		PARAM_DEF_REAL_ZERO( arand )
		PARAM_DEF_REAL_ZERO( a_steady )
		PARAM_DEF_REAL_ZERO( b_steady )

		PARAM_DEF_INT32_INF( interval,		10, 0. )
		PARAM_DEF_INT32(	value_switch,	2, 1,	1, 5	)
	};
}

namespace	n_spots
{
	c_img_2d*	img_difrea;

	/* screen stuff */

	static	INT32	xsize = 64;
	static	INT32	ysize = 64;
	//INT32 psize = 4;

	/* simulation variables */

	static	INT32	interval = 20;
	static	INT32	interation_nb;

	static	INT32	value_switch = 1;
//	static	bool	b_stripe = false;

	CONST	INT32	STRIPES = 1;
	CONST	INT32	SPOTS = 2;

	static	CONST	INT32	DIFREA_MAX = 512;

	static	REAL*	a  = nullptr;
	static	REAL*	b  = nullptr;
	static	REAL*	c  = nullptr;
	static	REAL*	d  = nullptr;
	static	REAL*	e  = nullptr;

	static	REAL*	da = nullptr;
	static	REAL*	db = nullptr;
	static	REAL*	dc = nullptr;
	static	REAL*	dd = nullptr;
	static	REAL*	de = nullptr;

	static	REAL*	ai = nullptr;

	static	REAL	p1,p2,p3;

	static	REAL	diff1,diff2;

	static	REAL	arand;
	static	REAL	a_steady;
	static	REAL	b_steady;

	static	REAL	beta_init;
	static	REAL	beta_rand;

	static	REAL	speed = 1.0;

	static	INT32	sim = 1;

	/******************************************************************************
	Display the activator.
	******************************************************************************/
	void	show( REAL* values )
	{
		UINT8*	dst = img_difrea->get_data_uint8();
		if( !dst )	{	return;	}
		
		UINT8	result;
		REAL	output;
		REAL	min = REAL( 1e20);
		REAL	max = REAL(-1e20);
		INT32	index;

		//	find minimum and maximum values
		for( INT32 i = 0; i < xsize; ++i )
		{
			for( INT32 j = 0; j < ysize; ++j )
			{
				index = j*DIFREA_MAX + i;
				if( values[index] < min )	min = values[index];
				if( values[index] > max )	max = values[index];
			}
		}

		if( min == max )
		{
			min = max - 1;
			max = min + 2;
		}

		DBG_PRINT_STRING( "min max diff: %f %f %f", (DOUBLE)min, (DOUBLE)max, (DOUBLE)(max - min) );

		//	display the values

		for( INT32 j = 0; j < ysize; ++j )
		{
			UINT8* pt = dst + j * img_difrea->get_byte_pitch();
			for( INT32 i = 0; i < xsize; ++i )
			{
				index = j*DIFREA_MAX + i;
				output = (values[index] - min) / (max - min);
				result = UINT8(output * 255.0);
				*pt++ = result;
				*pt++ = result;
				*pt++ = result;
				*pt++ = 255;
			}
		}
		img_difrea->erase_filename();
		img_difrea->set_changed();
	}

	/******************************************************************************
	Diffuse and react.
	******************************************************************************/
	void	show()
	{
		switch( value_switch )
		{
			case 1:		show(a);	break;
			case 2:		show(b);	break;
			case 3:		show(c);	break;
			case 4:		show(d);	break;
			case 5:		show(e);	break;
			default:	ERR_PRINT_STRING( "bad switch in compute: %d", value_switch );	break;
		}
		//todo why this line
		tex_2d_bind( -2 );
		tex_2d_bind( c_layer::get_ui() ? c_layer::get_ui()->get_bind_2d() : 0 );
	}

	void	difrea_show_to_screen()
	{
		show();
	}

	//todo	implement again
	void	set_focus()	//par
	{
		INT32	h = 0;
/*
		n_difrea::param[h++]._dum = (void *)&speed;
		n_difrea::param[h++]._dum = (void *)&p1;
		n_difrea::param[h++]._dum = (void *)&p2;
		n_difrea::param[h++]._dum = (void *)&p3;
		n_difrea::param[h++]._dum = (void *)&diff1;
		n_difrea::param[h++]._dum = (void *)&diff2;
		n_difrea::param[h++]._dum = (void *)&beta_init;
		n_difrea::param[h++]._dum = (void *)&beta_rand;
		n_difrea::param[h++]._dum = (void *)&arand;
		n_difrea::param[h++]._dum = (void *)&a_steady;
		n_difrea::param[h++]._dum = (void *)&b_steady;
		n_difrea::param[h++]._dum = (void *)&interval;
		n_difrea::param[h++]._dum = (void *)&value_switch;
*/
	//par	focus_list_set( n_difrea::param, DIFREA_PARAM_NB_MAX, "DifRea" );
//		return n_difrea::param;
	}

/******************************************************************************
Main routine.
******************************************************************************/
/*
main (argc,argv)
  int  argc;
  char *argv[];
{
  char *s;

	//	parse the command line options

  while(--argc >0 && (*++argv)[0]=='-') {
	for(s = argv[0]+1; *s; ++s )
	  switch(*s) {
	case 'n':
	  xsize = atoi (*++argv);
	  ysize = atoi (*++argv);
		  if( xsize > DIFREA_MAX || ysize > DIFREA_MAX )
			printf ("oops, too large a screen size\n");
	  argc -= 2;
	  break;
	case 'x':
	  b_stripe = true;
	  break;
	default:
	  break;
	  }
  }

	// setup graphics

  init_graphics (psize * xsize, psize * ysize, 40);
  set_pixel_size (psize);

	//	make spots or stripes

  if( b_stripe )
	do_stripes();
  else
	do_spots();
}
*/

	/******************************************************************************
	Pick a random number between min and max.
	******************************************************************************/
	FINLINE REAL frand( REAL	min, REAL	max)
	{
		return (min + drand48() * (max - min));
	}

	FINLINE	void	difrea_alloc()
	{
		if( !a )
		{
			a = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			b = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			c = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			d = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			e = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );

			da = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			db = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			dc = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			dd = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
			de = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );

			ai = (REAL*) MALLOC( DIFREA_MAX*DIFREA_MAX*sizeof(REAL) );
		}
	}

	FINLINE	void	deinit()
	{
		IF_FREE_AND_NULL( a );
		IF_FREE_AND_NULL( b );
		IF_FREE_AND_NULL( c );
		IF_FREE_AND_NULL( d );
		IF_FREE_AND_NULL( e );
		IF_FREE_AND_NULL( da );
		IF_FREE_AND_NULL( db );
		IF_FREE_AND_NULL( dc );
		IF_FREE_AND_NULL( dd );
		IF_FREE_AND_NULL( de );
		IF_FREE_AND_NULL( ai );
	}

	/******************************************************************************
	Create stripes with what Hans Meinhardt calls a two-species balance.
	******************************************************************************/
	void	difrea_multiplicative_help()
	{
		//INT32 i,j;
		INT32	iprev,inext,jprev,jnext;
		REAL	aval,bval,cval,dval,eval;
		REAL	ka,kc,kd;
		REAL	temp1,temp2;
		REAL	dda,ddb;
		REAL	ddd,dde;
		REAL*	pa;
		REAL*	pb;
		REAL*	pc;
		REAL*	pd;
		REAL*	pe;
		INT32	index;

		difrea_alloc();
		//	compute change in each cell

		for( INT32 i = 0; i < xsize; ++i )
		{
			ka = -p1 - 4 * diff1;
			kc = -p2;
			kd = -p3 - 4 * diff2;

			iprev = (i + xsize - 1) % xsize;
			inext = (i + 1) % xsize;

			for( INT32 j = 0; j < ysize; ++j )
			{
				jprev = (j + ysize - 1) % ysize;
				jnext = (j + 1) % ysize;

				index = j*DIFREA_MAX + i ;

				aval = a[ index ];
				bval = b[ index ];
				cval = c[ index ];
				dval = d[ index ];
				eval = e[ index ];

				temp1 = REAL(0.01) * aval * aval * eval * ai[ j*DIFREA_MAX + i ];
				temp2 = REAL(0.01) * bval * bval * dval;

				dda = a[jprev*DIFREA_MAX+i] + a[jnext*DIFREA_MAX+i] + a[j*DIFREA_MAX+iprev] + a[j*DIFREA_MAX+inext];
				ddb = b[jprev*DIFREA_MAX+i] + b[jnext*DIFREA_MAX+i] + b[j*DIFREA_MAX+iprev] + b[j*DIFREA_MAX+inext];
				ddd = d[jprev*DIFREA_MAX+i] + d[jnext*DIFREA_MAX+i] + d[j*DIFREA_MAX+iprev] + d[j*DIFREA_MAX+inext];
				dde = e[jprev*DIFREA_MAX+i] + e[jnext*DIFREA_MAX+i] + e[j*DIFREA_MAX+iprev] + e[j*DIFREA_MAX+inext];

				da[ index ] = aval * ka + diff1 * dda + temp1 / cval;
				db[ index ] = bval * ka + diff1 * ddb + temp2 / cval;
				dc[ index ] = cval * kc + temp1 + temp2;
				dd[ index ] = dval * kd + diff2 * ddd + p3 * aval;
				de[ index ] = eval * kd + diff2 * dde + p3 * bval;
			}
		}

		//	affect change

		pa = &a[0];
		pb = &b[0];
		pc = &c[0];
		pd = &d[0];
		pe = &e[0];
		for( INT32 j = 0; j < ysize; ++j )
		{
			for( INT32 i = 0; i < xsize; ++i )
			{
				index = j*DIFREA_MAX + i ;
	//			*pa++ += (speed * da[index]);
	//			*pb++ += (speed * db[index]);
	//			*pc++ += (speed * dc[index]);
	//			*pd++ += (speed * dd[index]);
	//			*pe++ += (speed * de[index]);
				a[index] += (speed * da[index]);
				b[index] += (speed * db[index]);
				c[index] += (speed * dc[index]);
				d[index] += (speed * dd[index]);
				e[index] += (speed * de[index]);
			}
		}
	}


	/******************************************************************************
	Turing's reaction-diffusion equations.
	******************************************************************************/
	void	difrea_turing()
	{
		//int i,j;
		INT32	iprev,inext,jprev,jnext;
		REAL	aval,bval;
		REAL	ka;
		REAL	dda,ddb;
		REAL	Diff1,Diff2;
		INT32	index;

		difrea_alloc();

		Diff1 = diff1 / REAL(2.);
		Diff2 = diff2 / REAL(2.);
		ka = p1 / REAL(16.);

		//	compute change in each cell

		for( INT32 i = 0; i < xsize; ++i )
		{
			iprev = (i + xsize - 1) % xsize;
			inext = (i + 1) % xsize;

			for( INT32 j = 0; j < ysize; ++j )
			{
				jprev = (j + ysize - 1) % ysize;
				jnext = (j + 1) % ysize;

				index = j*DIFREA_MAX+i;
				aval = a[index];
				bval = b[index];

				dda = a[jprev*DIFREA_MAX+i] + a[jnext*DIFREA_MAX+i] + a[j*DIFREA_MAX+iprev] + a[j*DIFREA_MAX+inext] - 4 * aval;
				ddb = b[jprev*DIFREA_MAX+i] + b[jnext*DIFREA_MAX+i] + b[j*DIFREA_MAX+iprev] + b[j*DIFREA_MAX+inext] - 4 * bval;

				da[index] = ka * (16 - aval * bval) + Diff1 * dda;
				db[index] = ka * (aval * bval - bval - c[index]) + Diff2 * ddb;
			}
		}

		//	affect change

		for( INT32 i = 0; i < xsize; ++i )
		{
			for( INT32 j = 0; j < ysize; ++j )
			{
				index = j*DIFREA_MAX+i;
				a[index] += (speed * da[index]);
				b[index] += (speed * db[index]);
				if( b[index] < 0 )
				{
					b[index] = 0;
				}
			}
		}
	}


	/******************************************************************************
	Calculate semi-stable equilibria.
	******************************************************************************/
	void	difrea_semi_equilibria()
	{
		//INT32 i,j;
		REAL	ainit,binit;
		REAL	cinit,dinit,einit;
		INT32	index;

		difrea_alloc();

		ainit = binit = cinit = dinit = einit = 0;

	  /* figure the values */

		switch( sim )
		{
		case STRIPES:
			for( INT32 i = 0; i < xsize; ++i )
			{
				ainit = p2 / (2 * p1);
				binit = ainit;
				cinit = REAL(0.02) * ainit * ainit * ainit / p2;
				dinit = ainit;
				einit = ainit;

				for( INT32 j = 0; j < ysize; ++j )
				{
					index = j*DIFREA_MAX+i;
					a[index] = ainit;
					b[index] = binit;
					c[index] = cinit;
					d[index] = dinit;
					e[index] = einit;
					ai[index] = 1 + frand( REAL(-0.5 * arand), REAL(0.5 * arand) );
				}
			}
			break;

		case SPOTS:
			for( INT32 i = 0; i < xsize; ++i )
			{
				for( INT32 j = 0; j < ysize; ++j )
				{
					index = j*DIFREA_MAX+i;
					a[index] = a_steady;
					b[index] = b_steady;
					c[index] = beta_init + frand (-beta_rand, beta_rand );
				}
			}
			break;

		default:
			ERR_PRINT_STRING( "bad case in semi_equilibria");
			break;
		}
	}

	/******************************************************************************
	Rescale values in array.

	Entry:
	  values    - array to rescale
	  min_final - minimum value to map to
	  max_final - maximum value to map to
	******************************************************************************/
	void	difrea_rescale_values( REAL* values, REAL min_final, REAL max_final )
	{
		//INT32 i,j;
		REAL	val;
		REAL	min = REAL( 1e20);
		REAL	max = REAL(-1e20);
		INT32	index;
		//	find minimum and maximum values

		for( INT32 i = 0; i < xsize; ++i )
		{
			for( INT32 j = 0; j < ysize; ++j )
			{
				index = j*DIFREA_MAX+i;
				if( values[index] < min )	{	min = values[index];	}
				if( values[index] > max )	{	max = values[index];	}
			}
		}

		if( min == max )
		{
			min = max - REAL(.001);
			max = min + REAL(.002);
		}

		//	rescale the values

		for( INT32 i = 0; i < xsize; ++i )
		{
			for( INT32 j = 0; j < ysize; ++j )
			{
				index = j*DIFREA_MAX+i;
				val = (values[index] - min) / (max - min);
				val = min_final + val * (max_final - min_final);
				values[index] = val;
			}
		}
	}

	/******************************************************************************
	Switch for picking array to rescale.
	******************************************************************************/
	void	difrea_do_rescale ( INT32 index, REAL min, REAL max )
	{
		difrea_alloc();
		switch (index)
		{
		case 1:		difrea_rescale_values( a, min, max );	break;
		case 2:		difrea_rescale_values( b, min, max );	break;
		case 3:		difrea_rescale_values( c, min, max );	break;
		case 4:		difrea_rescale_values( d, min, max );	break;
		case 5:		difrea_rescale_values( e, min, max );	break;
		default:	ERR_PRINT_STRING( "bad switch in difrea_do_rescale: %d", index );	break;
		}
	}

	void	compute()
	{
		for( INT32 k = interval; k > 0; --k )
		{
			//	perform reaction and diffusion
			switch( sim )
			{
			case STRIPES:	difrea_multiplicative_help();	break;
			case SPOTS:		difrea_turing();				break;
			default:		break;
			}
		}

		interation_nb += interval;
		DBG_PRINT_STRING( "iteration %d", interation_nb );
	}

	/******************************************************************************
	Run Meinhardt's stripe-formation system.
	******************************************************************************/
	static	void	difrea_do_stripes()
	{
		p1 = REAL(0.04);
		p2 = REAL(0.06);
		p3 = REAL(0.04);

		diff1 = REAL(0.009);
		diff2 = REAL(0.2);

		arand = REAL(0.02);

		sim = STRIPES;
		value_switch = 1;
		//	calculate semistable equilibria
		difrea_semi_equilibria();
	}

	/******************************************************************************
	Run Turing reaction-diffusion system.
	******************************************************************************/
	static	void	difrea_do_spots()
	{
		beta_init = 12;
		beta_rand = REAL(.1);

		a_steady = 4;
		b_steady = 4;

		diff1 = 0.25;
		diff2 = 0.0625;

		p1 = REAL(0.2);
		p2 = 0.0;
		p3 = 0.0;

		sim = SPOTS;
		value_switch = 2;
		//	calculate semistable equilibria
		difrea_semi_equilibria();
	}

	void	init_from_image( c_img_2d* image )
	{
		//INT32	i,j;
		INT32	sum;
		REAL	fsum;
		INT32	rgba;

		img_difrea = image;

		if( image && image->is_format(aaa::PIXEL_FORMAT::RGBA_8) )
		{
			//todo deal with RGB and RGBA

			if( !image->is_data_valid( __FUNCTION__ ) || image->get_size_x()<xsize || image->get_size_y()<ysize )
				//todo done quickly finish this or dump this class
				c_img_2d::img_init_with_size( image, xsize, ysize, image->get_pixel_format(),  __FUNCTION__ );
			UINT8* data8 = image->get_data_valid_rgb_uint8( __FUNCTION__ );
			if( data8 )
			{
				if( image->get_channel_nb() == 4 )
				{
					INT32* a = (INT32*) data8;
					for( INT32 j = 0; j < ysize; ++j )
					{
						for( INT32 i = 0; i < xsize; ++i )
						{
							rgba = *a;
	#ifdef	WIN32
							sum = (rgba & 0xff) + ((rgba>>8) & 0xff) + ((rgba>>16) & 0xff);
	#else
							sum = ((rgba>>24) & 0xff) + ((rgba>>16) & 0xff) + ((rgba>>8) & 0xff);
	#endif
							fsum = REAL(sum);
							fsum /= REAL(3. * 256.);
							c[ j * DIFREA_MAX + i ] = REAL( beta_init + beta_rand * (fsum -.5) * 2. );
							++a;
						}
					}
				}
			}
		}
		else
			ERR_PRINT_STRING( "%s() only deal with RGBA format", __FUNCTION__ );
	}

	void	start( c_img_2d* image, bool in )
	{
		if( !image->is_data_valid( __FUNCTION__ ) )
			//todo done quickly finish this or dump this class
			c_img_2d::img_init_with_size( image, img_difrea->get_size_x(), img_difrea->get_size_y(), image->get_pixel_format(),  __FUNCTION__ );
		if( image->is_data_valid( __FUNCTION__ ) )
		{
			image->set_cpu_keep( true );

			img_difrea = image;
			
			xsize = img_difrea->get_size_x();
			ysize = img_difrea->get_size_y();
//			b_stripe = in;

			interation_nb = 0;
			if( in )
				difrea_do_stripes();
			else
				difrea_do_spots();
		}
	}

}		// namespace n_spots
