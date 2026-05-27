#include "obj_ui/bdd/bdd_geo/bdd_tube.h"
#include "draw/render.h"
#include "draw/map.h"
#include "gol/gol_draw.h"
#include "obj_ui/deformer/def_node.h"
#include "time/aaa_time.h"
#include "math/rand.h"
#include "image/img.h"
#include "infrastructure/seedfile.h"

typedef	struct	SECTION2D
{
	INT32				nb;
	INT32				index;
	bool				b_close;
	c_point_xyz_real	ame;
//	REAL				rayon_min;
//	REAL				rayon_max;
	c_point_xyz_real*	point;
	c_point_xyz_real*	point_def;
	c_point_xyz_real*	n3d;
}	SECTION2D;

typedef	struct	SECTION3D
{
	INT32				nb;
	c_point_xyz_real*	p3d;
}	SECTION3D;

typedef	struct	LEGO
{
	SECTION2D*			prev;
	SECTION2D*			next;
	c_point_xyz_real	tra;
	c_point_xyz_real	rot;
}	LEGO;

FACTORY_CREATE_PROP_V1( c_bdd_tube, bdd_tube, Old Tube, tub, sub_menu="Old"; );

namespace n_bdd_tube
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 25;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REAL_INF(		Time_Factor,		.1, 1.	)
		PARAM_DEF_REAL_INF(		Phase_increment,	.1, 1.	)

		PARAM_DEF_BOOL_ON(		Tube_flat			)
		PARAM_DEF_REAL(			Rayon_angle,		PI, PI_TIME_2,	-PI_TIME_2, PI_TIME_2 )
		PARAM_DEF_BOOL_OFF(		Rayon_Height		)
		PARAM_DEF_REAL_INF(		Rayon,				.1, 4.	)
		PARAM_DEF_REAL_INF(		Rayon_variation,	.5, 0.	)
		PARAM_DEF_REAL_ONE(		Rayon_Time_Freq		)
		PARAM_DEF_REAL_ONE(		Rayon_Space_Freq	)
		PARAM_DEF_REAL(			Rayon_Shape,		0.3, 0.,	-10., 10. )
		PARAM_DEF_REAL_ZERO(	Rayon_Shape_Time_influence		)
		PARAM_DEF_BOOL_OFF(		Rayon_small_perturbation		)

		PARAM_DEF_REAL_INF(	Length,				.1, 8.		)
		//todo solve this for all the params
		//		{	nullptr,	PARAM_INT32,	"Lego_Nb", 64., 32.,	1., 1000., tube_reset, nullptr },
		PARAM_DEF_INT32(		Lego_Nb,			64, 32,		1, 1000	)
		PARAM_DEF_INT32(		Section_Modulo,		4, 128,		0, 2048	)

	//		{	nullptr,	PARAM_INT32,	"Section_Nb", 16., 8.,	1., 2048., tube_reset, nullptr },
		PARAM_DEF_INT32(		Section_Nb,			8, 32,		1, 2048	)
		PARAM_DEF_REAL(			Section_Rot,		.1, .0,		-PI, PI	)

		PARAM_DEF_REAL(			Map_U_Nb,			2, 1,		-1000, 1000	)
		PARAM_DEF_REAL(			Map_V_Nb,			2, 1,		-1000, 1000	)
		PARAM_DEF_REAL_INF(		Map_U_speed,		.1, 0.		)
		PARAM_DEF_REAL_INF(		Map_V_speed,		.1, 0.		)
		PARAM_DEF_BOOL_OFF(		Map_Animate_Texture )
		PARAM_DEF_INT32(		Map_U_Sub,			2, 1,		1, 512	)
		PARAM_DEF_INT32(		Map_V_Sub,			2, 1,		1, 512	)

	//todo	should be a deformer
	/*
		{	nullptr,	PARAM_REAL,	"Def_X_Range",		2., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
		{	nullptr,	PARAM_REAL,	"Def_X_Freq",		.25, .25,	PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
		{	nullptr,	PARAM_REAL,	"Def_Y_Range",		2., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
		{	nullptr,	PARAM_REAL,	"Def_Y_Freq",		.25, .25,	PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
		{	nullptr,	PARAM_REAL,	"Def_XY_Phase",		PI/2., 0.,	0., PI_BY_2,					nullptr, nullptr },
		{	nullptr,	PARAM_REAL,	"Def_Time_Freq",	1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
	*/
		PARAM_DEF_FILENAME(		Height_map,			aaa::file::TYPE_IO_HEIGHT_MAP, 0. )
	};
}

void	c_bdd_tube::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _time_factor );
	param_set_pt( h, _section_phase_step );

	param_set_pt( h, _b_flat );
	param_set_pt( h, _rayon_angle );
	param_set_pt( h, _b_rayon_height );
	param_set_pt( h, _rayon );
	param_set_pt( h, _rayon_var );
	param_set_pt( h, _rayon_time_freq );
	param_set_pt( h, _rayon_space_freq );
	param_set_pt( h, _rayon_shape_ui );
	param_set_pt( h, _rayon_shape_time );
	param_set_pt( h, _b_rayon_def_small_on );

	param_set_pt( h, _len );
	param_set_pt( h, _lego_nb_ui );

	param_set_pt( h, _section_modulo );
	param_set_pt( h, _section_nb_ui );
	param_set_pt( h, _section_rotation );

	param_set_pt( h, _tex_lego_nb_u );
	param_set_pt( h, _tex_lego_nb_v );
	param_set_pt( h, _tex_speed_u );
	param_set_pt( h, _tex_speed_v );
	param_set_pt( h, _b_sub_on );
	param_set_pt( h, _tex_sub_nb_u );
	param_set_pt( h, _tex_sub_nb_v );

	param_set_pt( h, _img_height_fname );

	err_param_init_pt(h);
}

AAA_ERR	c_bdd_tube::load_do_after( o_str CONST & filename )
{
	//AAA_ERR retcode;
	if( !_img_height_fname.is_empty() )
		_img_height = c_img_2d::read( _img_height, _img_height_fname );
	//	alloc_and_build();
	return AAA_OK;
}

//	goto next frame in case of sub and b_sub_on
void	c_bdd_tube::map_uv_init()
{
	if( _b_sub_on )
	{
		if( _tex_sub_counter++ >= (_tex_sub_nb_u*_tex_sub_nb_v)-1 )
			_tex_sub_counter = 0;
	}
	else
	{
		_tex_sub_counter = 0;
	}
	_tex_lego_u_max = (_tex_sub_counter&(_tex_sub_nb_u-1)) / (REAL)_tex_sub_nb_u  + _tube_time*_tex_speed_u;
	_tex_lego_u_min = _tex_lego_u_max - OVER_ONE_AS_REAL(_tex_sub_nb_u);

	_tex_lego_v_min = (_tex_sub_counter/_tex_sub_nb_u) / (REAL)_tex_sub_nb_v - _tube_time*_tex_speed_v;
	_tex_lego_v_max = _tex_lego_v_min + OVER_ONE_AS_REAL(_tex_sub_nb_v);

	_tex_lego_v = _tex_lego_v_min;
}

void	c_bdd_tube::section2d_data_alloc( SECTION2D* s2d )
{
	if( !s2d )
		return;

	if( !s2d->point )
		s2d->point = (c_point_xyz_real*) MALLOC_ALIGNED( sizeof(c_point_xyz_real) * ((s2d->nb+1) * 2 +1), 0 );	//todo this last +1 cover a bug
	if( !s2d->n3d )
		s2d->n3d = (c_point_xyz_real*) MALLOC_ALIGNED( sizeof(c_point_xyz_real) * (s2d->nb+1), 0 );
}

void	c_bdd_tube::section2d_data_dealloc( SECTION2D* s2d )
{
	if( !s2d )	{	return;	}

	IF_FREE_ALIGNED_AND_NULL( s2d->point );
	IF_FREE_ALIGNED_AND_NULL( s2d->n3d );
}

void	c_bdd_tube::section2d_init( SECTION2D* s2d )
{
	s2d->nb = _section_nb;
	s2d->point = nullptr;
	s2d->n3d = nullptr;
	s2d->index = _section_index++;
	section2d_data_alloc( s2d );
}


void	c_bdd_tube::section2d_build( SECTION2D* s2d, c_point_xyz_real* ame )
{
	//todoqq using register is bad, compiler knows what to do
	c_point_xyz_real*	p3d;	
	INT32				nb;	
	REAL				angle_inc;	
	REAL				angle;	
	REAL				rayon_base;
	REAL				rayon_fixe;
	REAL				rayon_factor;
	REAL				rayon_shape;
	//register			INT32		i;
		//todo	min max is just a hack right now
	//	register	REAL	rayon_min;
	//	register	REAL	rayon_max;

	nb = s2d->nb;	
	angle_inc = _rayon_angle / REAL(nb);	
	angle = REAL(PI) - _rayon_angle * REAL(.5);	
	p3d = s2d->point;
	s2d->ame = *ame;

	_section_phase += _section_phase_step;

	rayon_base = _rayon;

	rayon_fixe = rayon_base;

	rayon_factor = (REAL(1) + COS_RAD(_tube_time*_rayon_time_freq+_section_phase*_rayon_space_freq) ) * REAL(.5);

	rayon_base += _rayon_var * rayon_factor;
	rayon_shape = _rayon_shape_ui * (_rayon_shape_time * (rayon_factor - REAL(1)) + REAL(1));

	angle += _section_phase * _section_rotation;

	s2d->b_close = (!_b_flat) || ((REAL(PI_BY_2-.01) < _rayon_angle) && ( _rayon_angle < REAL(PI_BY_2+.01)));
	if( !s2d->b_close )
		++nb;

	UINT8*	src = _img_height->get_data_uint8();
	if( !_b_rayon_height || !_img_height->is_ok() || !src )
	{	//	no height map
		if( _b_flat )
		{
			REAL	x = rayon_base;
			REAL	x_step = -rayon_base * REAL(2) / (nb-1);
			
			for( INT32 i=nb; i>0; --i )
			{
				//printf( "%2x", *p_pixel);  
				p3d->x = ame->x + x;
				p3d->y = ame->y + rayon_shape * (REAL(1) + SIN_RAD( x*3+s2d->index*.25) * REAL(.2) );
				p3d->z = ame->z;
				x += x_step;
				++p3d;
			}
		}
		else
		{
			REAL r;
//			rayon_min = FLOAT_BIG_VALUE;
//			rayon_max = -FLOAT_BIG_VALUE;

			if( !(s2d->index & 8) )
				rayon_base += rayon_shape;
			
			for( INT32 i=nb; i>0; --i )
			{
				r = ( i & _section_modulo)?rayon_fixe:rayon_base;

//				if ( r < rayon_min)
//					rayon_min = r;
//				if ( r > rayon_max)
//					rayon_max = r;
				if( _b_rayon_def_small_on )
				{
					p3d->x = ame->x + r * (REAL(1) + SIN_RAD(angle*3+s2d->index*.25) * REAL(.2)) * SIN_RAD(angle);	// + SIN_RAD(s2d->index/6.)*2;
					p3d->y = ame->y + r * (REAL(1) + SIN_RAD(angle*3)                * REAL(.2)) * COS_RAD(angle);
				}
				else
				{
					p3d->x = ame->x + r * SIN_RAD(angle);
					p3d->y = ame->y + r * COS_RAD(angle);
				}
				p3d->z = ame->z;
				angle += angle_inc;
				++p3d;
			}
		}
	}
	else
	{
		UINT8*	p_pixel;
		INT32	size_x;
		INT32	mask;
	
		// avoid crash
		mask = _img_height->get_size_y();

		size_x = _img_height->get_byte_pitch();

		if( _b_flat )
		{
			REAL	x = rayon_base;
			REAL	x_step = -rayon_base * REAL(2) / (nb-1);
			
			for( INT32 i=0; i<nb; ++i )
			{
				if( (i % mask) == 0 )
				{
					//todo cache this
					p_pixel = src + ( (s2d->index*sizeof(INT32)) % size_x ) ;
					p_pixel += size_x * (mask-1);
				}

				p3d->x = ame->x + x;
				p3d->y = ame->y + (rayon_shape * (*p_pixel)) / REAL(255);
				p3d->z = ame->z;

				x += x_step;
				p_pixel -= size_x;
				++p3d;
			}
		}
		else
		{
			REAL r;

//			rayon_min = FLOAT_BIG_VALUE;
//			rayon_max = -FLOAT_BIG_VALUE;

			for( INT32 i = 0; i < nb; ++i )
			{
				if( (i % mask) == 0 )
				{
					//todo cache this
					p_pixel = src + ( (s2d->index*sizeof(INT32)) % size_x ) ;
					p_pixel += size_x * (mask-1);
				}
				r = ( i & _section_modulo)?rayon_fixe:rayon_base;
				r = r + rayon_shape * ( (*p_pixel) / REAL(255) );
//				if ( r < rayon_min)
//					rayon_min = r;
//				if ( r > rayon_max)
//					rayon_max = r;
				//printf( "%2x", *p_pixel);  
				p3d->x = ame->x + r * SIN_RAD(angle);
				p3d->y = ame->y + r * COS_RAD(angle);
				p3d->z = ame->z;

				p_pixel -= size_x;
				angle += angle_inc;
				++p3d;
			}
		}
	}
//	s2d->rayon_min = rayon_min;
//	s2d->rayon_max = rayon_max;
}

/*
void	section2d_init_from_map_height( SECTION2D *s2d )
{
	register	POING2D*	p2d;	
	register	INT32		nb = s2d->nb;	
	register	REAL		angle_inc = PI_BY_2	/ nb;	
	register	REAL		angle = 0.;	
	register	REAL		rayon_int;
	register	REAL		rayon;
	register	UINT8*		p_pixel;
				c_img_2d*	img;


	section2d_alloc(s2d );
	p2d = s2d->p2d;

	img = img_height;
	p_pixel = img->data + (s2d->index%img->size_x) * sizeof(INT32);
	
	if ( b_flat )
		{
		REAL	x = rayon;
		REAL	x_step = -rayon * 2. / nb;
		
		for (  ;nb>0; --nb )
			{
			//printf( "%2x", *p_pixel);  
			p2d->x = x;
			p2d->y = rayon_rnd * ( ((INT32)(*p_pixel)-128) / 255. );

			x += x_step;
			p_pixel += img->size_x * sizeof(INT32);
			++p2d;
			}
		}
	else
		{
		rayon_int = rayon;
		for (  ;nb>0; --nb )
			{
			rayon = rayon_int + rayon_rnd * ( ((*p_pixel)) / 255. );
			//printf( "%2x", *p_pixel);  
			p2d->x = rayon * SIN_RAD(angle);
			p2d->y = rayon * COS_RAD(angle);
			p_pixel += img->size_x * sizeof(INT32);
			angle += angle_inc;
			++p2d;
			}
		*p2d = *s2d->p2d;
		}
	//printf( "\n");  
	//	we duplicate the first point in the last one
}
*/

//todo deal with alloc pb
void	c_bdd_tube::alloc( INT32 nb_in )
{
	if( _lego )		{	return;	}

	_lego = new LEGO[nb_in];
	if ( _lego )
	{
		SECTION2D* s2d = new SECTION2D[nb_in + 1];
		if ( s2d )
		{
			_lego_nb = nb_in;
			_section_nb = _section_nb_ui;
			LEGO* l = _lego;

			section2d_init( s2d );

			for( INT32 i = _lego_nb ; i > 0; --i )
			{
				//	attach section
				l->prev = s2d;
				++s2d;
				l->next = s2d;

				section2d_init( s2d );
				++l;
			}
		}
	}
	else
	{
		ERR_PRINT_STRING( "PB in c_bdd_tube::alloc");
		dealloc();
	}
}

void	c_bdd_tube::alloc()
{
	alloc( _lego_nb_ui ); 
}

void	c_bdd_tube::dealloc()
{
	if( !_lego )	{	return;	}

	if( _lego->prev )
	{
		LEGO* l = _lego;
		section2d_data_dealloc( l->prev );
		for( INT32 i = _lego_nb; i > 0; --i )
		{
			//	free section
			section2d_data_dealloc( l->next );
			++l;
		}
		SAFE_DELETE_ARRAY( _lego->prev );
	}
	SAFE_DELETE_ARRAY( _lego );
}

/*
void c_bdd_tube::param_init()
{//todo a voir quoi garder
	time_factor = 1.;
	section_phase_step = .1;

	b_flat = false;
	rayon_angle	= PI_BY_2;
	b_rayon_height = false;
	rayon		= 2.0;
	rayon_var	= .5;
	rayon_time_freq = 0.;
	rayon_space_freq = 0.;
	rayon_shape_ui = 0.3;
	rayon_shape_time = .0;
	b_rayon_def_small_on = true;

	len	= 8.;
	lego_nb_ui = 4;

	section_nb_ui = 4;
	section_rotation = 0.;
	section_modulo = 128;

	tex_lego_nb_u	= 2.;
	tex_lego_nb_v	= 8.;

	tex_sub_nb_u	= 1;
	tex_sub_nb_v	= 1;
	tex_speed_u = 0.;
	tex_speed_v = 0.;
	b_sub_on = false;
}
*/

void	c_bdd_tube::init()
{
	_img_height = c_img_2d::create( __FUNCTION__ );
	_img_height_fname.erase();
	param_init_with( n_bdd_tube::param, n_bdd_tube::PARAM_NB_MAX ); // bdd_tube_param, BDD_TUBE_PARAM_NB_MAX);

	_lego = nullptr;
	_lego_nb = 0;

	_section_nb = 0;
	_section_index = 0;

	_b_normal_to_compute = false;

	_tex_lego_u_min = 0;
	_tex_lego_u_max = 0;
}

void	c_bdd_tube::build()
{
	//INT32		i;
	c_point_xyz_real	ame;
	LEGO *				l = _lego;

	_section_phase = 0;

	ame.z = -_len * REAL(.5);
	ame.x = 0.;
	ame.y = 0.;
	section2d_build( l->prev, &ame );

	for( INT32 i = _lego_nb ; i > 0; --i )
	{
		l->tra.x = 0.;
		l->tra.y = 0.;
		l->tra.z = _len / _lego_nb;
		l->rot.x = 0.;
		l->rot.y = 0.;
		l->rot.z = 0.;
		ame.z = ame.z + l->tra.z;
		section2d_build( l->next, &ame );
		++l;
	}

	_b_normal_to_compute = true;
}

CONSTRUCTOR_CREATE( c_bdd_tube )
{
	init();
}	

c_bdd_tube::~c_bdd_tube()
{
	dealloc();
	obj_delete( _img_height);
}	

void	c_bdd_tube::alloc_and_build()
{
	dealloc();
	alloc();
	build();
}

void	section2d_compute_normal( SECTION2D * CONST sp, SECTION2D * CONST sc, SECTION2D * CONST sn )
{
	REAL				x1,y1,z1;
	REAL				x2,y2,z2;
	REAL				size;
	//	convention is
	//	prev section	.a.
	//	cur	section		cnd
	//	next section	.b.
	c_point_xyz_real*	n;
	c_point_xyz_real*	a;
	c_point_xyz_real*	b;
	c_point_xyz_real*	c;
	c_point_xyz_real*	d;
	INT32				nb;	

	n = sc->n3d;
	nb = sc->nb;

	a = sp->point_def;
	b = sn->point_def;
	if( sc->b_close)
	{
		nb -= 1;
		c = sc->point_def + nb;
	}
	else
	{
		nb -= 1;
		c = sc->point_def;
	}
	d = sc->point_def + 1;

	x1 = b->x - a->x;
	y1 = b->y - a->y;
	z1 = b->z - a->z;

	x2 = d->x - c->x;
	y2 = d->y - c->y;
	z2 = d->z - c->z;

	n->x = -y1*z2 + z1*y2;
	n->y = -z1*x2 + x1*z2;
	n->z = -x1*y2 + y1*x2;

	size = OVER_ONE_AS_REAL( SQRT( n->x*n->x + n->y*n->y + n->z*n->z ) );
	n->x *= size;
	n->y *= size;
	n->z *= size;

	++a;
	++b;
	c = sc->point_def;
	++d;
	++n;

	for( ; nb>0; --nb )
	{
		x1 = b->x - a->x;
		y1 = b->y - a->y;
		z1 = b->z - a->z;

		x2 = d->x - c->x;
		y2 = d->y - c->y;
		z2 = d->z - c->z;

		n->x = -y1*z2 + z1*y2;
		n->y = -z1*x2 + x1*z2;
		n->z = -x1*y2 + y1*x2;

		size = OVER_ONE_AS_REAL( SQRT( n->x*n->x + n->y*n->y + n->z*n->z ) );
		n->x *= size;
		n->y *= size;
		n->z *= size;
		
		++a;
		++b;
		++c;
		++d;
		++n;
	}
	if( sc->b_close )
	{
		*n = *sc->n3d;
	}
	else
	{
		--d;
		x1 = b->x - a->x;
		y1 = b->y - a->y;
		z1 = b->z - a->z;

		x2 = d->x - c->x;
		y2 = d->y - c->y;
		z2 = d->z - c->z;

		n->x = -y1*z2 + z1*y2;
		n->y = -z1*x2 + x1*z2;
		n->z = -x1*y2 + y1*x2;
		
		size = OVER_ONE_AS_REAL( SQRT( n->x*n->x + n->y*n->y + n->z*n->z ) );
		n->x *= size;
		n->y *= size;
		n->z *= size;
	}
}

void	c_bdd_tube::compute_normal()
{
	if( is_normal_needed() )
	{
		LEGO*	l = _lego;

		SECTION2D*	prev;
		SECTION2D*	cur;
		prev = cur = l->prev;
		section2d_compute_normal( prev, cur, l->next );

		for( INT32 i=_lego_nb-1; i>0; --i )
		{
			prev = cur;
			cur = l->next;
			++l;
			section2d_compute_normal( prev, cur, l->next );
		}
		prev = cur;
		cur = l->next;
		section2d_compute_normal( prev, cur, cur);

		_b_normal_to_compute = false;
	}
}

void	c_bdd_tube::section2d_deform( SECTION2D *s2d )
{
	INT32	nb = s2d->b_close ? s2d->nb : s2d->nb + 1;

	s2d->point_def = s2d->point + nb + 1;
	c_def_node::get_cur()->apply( (REAL *) s2d->point_def, (REAL *) s2d->point, nb );

	if( s2d->b_close )
	{
		//	we duplicate the first point in the last one
		*(s2d->point_def + nb) = *s2d->point_def;
	}
}

void	c_bdd_tube::section2d_deform_no( SECTION2D *s2d )
{
	s2d->point_def = s2d->point;

	if( s2d->b_close )
	{
		//	we duplicate the first point in the last one
		*(s2d->point_def + s2d->nb) = *s2d->point_def;
	}
}

void	c_bdd_tube::do_deform()
{
	LEGO*	l = _lego;
	if( c_def_node::get_cur()->is_deforming() )
	{
		for( INT32 i=_lego_nb; i>0; --i )
		{
			section2d_deform( l->prev );
			++l;
		}
		--l;
		section2d_deform( l->next );
		_b_normal_to_compute = true;
	}
	else
	{
		for( INT32 i=_lego_nb; i>0; --i )
		{
			section2d_deform_no( l->prev );
			++l;
		}
		--l;
		section2d_deform_no( l->next );
	}
}

void	quad_do_normal_std( c_point_xyz_real* a, c_point_xyz_real* b, c_point_xyz_real* c )
{
	REAL	x1 = b->x - a->x;
	REAL	y1 = b->y - a->y;
	REAL	z1 = b->z - a->z;

	REAL	x2 = c->x - a->x;
	REAL	y2 = c->y - a->y;
	REAL	z2 = c->z - a->z;

	c_point_xyz_real	n;

	n.x = -y1*z2 + z1*y2;
	n.y = -z1*x2 + x1*z2;
	n.z = -x1*y2 + y1*x2;

	GOL::normal3v( &n.x );
}

//	we know that a.z = c.z
void	quad_do_normal( c_point_xyz_real* a, c_point_xyz_real* b, c_point_xyz_real* c )
{
	REAL	x2 = a->x - c->x;
	REAL	y2 = c->y - a->y;

	REAL	tmp = b->z - a->z;

	c_point_xyz_real	n;

	n.x = tmp * y2;
	n.y = tmp * x2;
	x2 *= a->y - b->y;
	y2 *= a->x - b->x;
	n.z = y2 + x2;

	GOL::normal3v( &n.x );
}

#define	TEXCOORD2D(a,b)	if ( b_tex ) GOL::texcoord2( a, b )
//#define	TEXCOORD2D(a,b)	if ( b_tex ) { REAL f[3]; f[0]=a; f[1]=0; f[2]=b; \
//											if( tube_deformer) tube_deformer->apply( f, 1 ); \
//											GOL::texcoord2( f[0], f[2]); }

void	c_bdd_tube::lego_draw( LEGO *l )
{
	bool b_tex = c_map::get_cur()->is_implicit();

	c_point_xyz_real*	p	= l->prev->point_def;
	c_point_xyz_real*	n	= l->next->point_def;
	INT32				nb	= l->prev->nb;

	_tex_lego_u = _tex_lego_u_max;

	GOL::begin( c_render::get_cur()->get_draw_primitive() );
	if( is_normal_draw() )
	{
		c_point_xyz_real*	pb = p + 1;

		for( ; nb>0; --nb )
		{
			quad_do_normal_std( p, n, pb );
			TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
			GOL::vertex3v( &n->x );

			TEXCOORD2D( _tex_lego_v, _tex_lego_u );
			GOL::vertex3v( &p->x );
			//	next points
			++p;
			++pb;
			++n;
			_tex_lego_u -= _tex_lego_step_u;
			//if (tex_lego_u <= tex_lego_u_min)
			//	tex_lego_u += tex_lego_u_max-tex_lego_u_min;
		}
		TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
		GOL::vertex3v( &n->x );

		TEXCOORD2D( _tex_lego_v, _tex_lego_u );
		GOL::vertex3v( &p->x );
	}
	else
	{
		for( ; nb>0; --nb )
		{
			TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
			GOL::vertex3v( &n->x );

			TEXCOORD2D( _tex_lego_v, _tex_lego_u );
			GOL::vertex3v( &p->x );

			//	next points
			++p;
			++n;
			_tex_lego_u -= _tex_lego_step_u;
			//if (tex_lego_u <= tex_lego_u_min)
			//	tex_lego_u += tex_lego_u_max-tex_lego_u_min;
		}
		TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
		GOL::vertex3v( &n->x );

		TEXCOORD2D( _tex_lego_v, _tex_lego_u );
		GOL::vertex3v( &p->x );
	}
	GOL::end();

	_tex_lego_v += _tex_lego_step_v;
	if( _tex_lego_v >= _tex_lego_v_max )
	{
		//tube_map_uv_init();
		_tex_lego_v += _tex_lego_v_min-_tex_lego_v_max;
	}
}

void	c_bdd_tube::lego_draw_gouraud( LEGO *l )
{
	bool b_tex = ( c_map::get_cur()->is_implicit() );
	
	c_point_xyz_real*	p = l->prev->point_def;
	c_point_xyz_real*	n = l->next->point_def;
	INT32				nb = l->prev->nb;

	_tex_lego_u = _tex_lego_u_max;

	GOL::begin( c_render::get_cur()->get_draw_primitive() );
	if ( is_normal_draw() )
	{
		c_point_xyz_real*	pn = l->prev->n3d;
		c_point_xyz_real*	nn = l->next->n3d;

		//todoq	normal ne marche en mode list
		//		verifier calcul
		//	semble etre pb plus generic
		GOL::normal3v( &nn->x );
		TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
		GOL::vertex3v( (REAL *) n );

		GOL::normal3v( &pn->x );
		TEXCOORD2D( _tex_lego_v, _tex_lego_u );
		GOL::vertex3v( (REAL *) p );

		for( ; nb>0; --nb )
		{
			//	next points
			++p;
			++pn;
			++n;
			++nn;

			_tex_lego_u -= _tex_lego_step_u;
			//if (tex_lego_u <= tex_lego_u_min)
			//	tex_lego_u += tex_lego_u_max-tex_lego_u_min;

			GOL::normal3v( &nn->x);
			TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
			GOL::vertex3v( (REAL *) n );

			GOL::normal3v( &pn->x);
			TEXCOORD2D( _tex_lego_v, _tex_lego_u );
			GOL::vertex3v( (REAL *) p );
		}
	}
	else
	{
		TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
		GOL::vertex3v( (REAL *) n );

		TEXCOORD2D( _tex_lego_v, _tex_lego_u );
		GOL::vertex3v( (REAL *) p );

		for( ; nb>0; --nb )
		{
			//	next points
			++p;
			++n;
			_tex_lego_u -= _tex_lego_step_u;
			//if (tex_lego_u <= tex_lego_u_min)
			//	tex_lego_u += tex_lego_u_max-tex_lego_u_min;

			TEXCOORD2D( _tex_lego_v+_tex_lego_step_v, _tex_lego_u );
			GOL::vertex3v( (REAL *) n );

			TEXCOORD2D( _tex_lego_v, _tex_lego_u );
			GOL::vertex3v( (REAL *) p );
		}
	}
	GOL::end();

	_tex_lego_v += _tex_lego_step_v;
	if( _tex_lego_v >= _tex_lego_v_max )
	{
		//tube_map_uv_init();
		_tex_lego_v += _tex_lego_v_min-_tex_lego_v_max;
	}
}


void	c_bdd_tube::update()
{
	SRAND(2);
	
	_tube_time = REAL(aaa::time::get()) * _time_factor;
	if( _lego_nb != _lego_nb_ui || _section_nb != _section_nb_ui )
	{
		dealloc();
		alloc();
	}
	build();
	do_deform();
	if( _b_normal_to_compute )
	{
		compute_normal();
	}

	_tex_lego_step_u = (_tex_lego_nb_u/_section_nb)	/ _tex_sub_nb_u;
	_tex_lego_step_v = (_tex_lego_nb_v/_lego_nb)		/ _tex_sub_nb_v;

	map_uv_init();	
}

//	INT32	test = 0;
void	c_bdd_tube::draw()
{
	bool	use_gouraud = c_render::get_cur()->is_gouraud();

	//	if (!(++test&3))
	//		return;

	INT32	l_nb = _lego_nb;
	LEGO*	l = _lego;
	if( use_gouraud )
	{
		GOL::push_att();
			//todoatt
		GOL::disable( GL_NORMALIZE );
			for(  ; l_nb>0; --l_nb )
			{
				lego_draw_gouraud( l );
				++l;
			}
		GOL::pop_att();
	}
	else
	{
		for(  ; l_nb>0; --l_nb )
		{
			lego_draw( l );
			++l;
		}
	}
}

void	c_bdd_tube::flip_height()
{
	_b_rayon_height = !_b_rayon_height;
//	build();
}

void	c_bdd_tube::flip_flat()
{
	_b_flat = !_b_flat;
//	build();
}

void	c_bdd_tube::load_img_height( o_str CONST & filename )
{
	_img_height = c_img_2d::read( _img_height, filename );
	_img_height_fname.set( _img_height->get_filename_o_str() );
	alloc_and_build();
}

