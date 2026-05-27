#include "bdd_special.h"
#include "file/file_csv.h"

//tdo check the size stuff
namespace	n_bdd_csv
{
	static	CONST	INT32	BASE_PARAM_NB	= 12;
	static	CONST	INT32	GROUP_PARAM_NB	= 0;
	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		{	NULL,	PARAM_INT32,	"col_nb",				1., 4.,		0, 128,	NULL, NULL },
		{	NULL,	PARAM_INT32,	"row_nb",				1., 4.,		0, 128,	NULL, NULL },
		{	NULL,	PARAM_FILENAME,	"data_filename",		0., 0.,		0, 0,	NULL, NULL },
		{	NULL,	PARAM_INT32,	"col_nb_draw",			1., 4.,		0, BDD_CSV_COL_MAX_NB,	NULL, NULL },
		{	NULL,	PARAM_INT32,	"row_nb_draw",			1., 4.,		0, 128,	NULL, NULL },
		{	NULL,	PARAM_INT32,	"col_cur",				1., 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_BOOL_OFF( draw_cube )
		{	NULL,	PARAM_REAL,		"size_u",				1., .8,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,		"size_v",				1., .8,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,		"size_axe",				1., .8,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,		"filter",				1., 0.,		0., .999,	NULL, NULL },
		{	NULL,	PARAM_FILENAME,	"model_filename",		0., 0.,		0, 0,	NULL, NULL },
	};
}

void	c_bdd_csv::param_init_pt()
{
INT32	h=0;
	bdd_csv_param[h++].pt = (void *)&nb_col;
	bdd_csv_param[h++].pt = (void *)&nb_row;
	bdd_csv_param[h++].pt = (void *)&data_filename;
	bdd_csv_param[h++].pt = (void *)&nb_col_draw;
	bdd_csv_param[h++].pt = (void *)&nb_row_draw;
	bdd_csv_param[h++].pt = (void *)&col_index_cur;
	bdd_csv_param[h++].pt = (void *)&b_draw_cube;
	bdd_csv_param[h++].pt = (void *)&size[0];
	bdd_csv_param[h++].pt = (void *)&size[1];
	bdd_csv_param[h++].pt = (void *)&size[2];
	bdd_csv_param[h++].pt = (void *)&filter_factor_ui;
	bdd_csv_param[h++].pt = (void *)&model_filename;

	err_param_init_pt(h);
}

static	REAL	max[BDD_CSV_COL_MAX_NB];
static	REAL	max_factor[BDD_CSV_COL_MAX_NB];

void	c_bdd_csv::fill_data()
{
	//INT32	i;
	REAL*		pt = data;
	c_rand_lin	rand;
	for( INT32 i=0; i<BDD_CSV_COL_MAX_NB; i++)
		*pt++ = rand.get_ufloat();
}

void	c_bdd_csv::reset_max_factor()
{
	for( INT32 i=0; i<BDD_CSV_COL_MAX_NB; i++)
	{
		max[i] = -1;
		max_factor[i] = 1.;
	}
}

void	c_bdd_csv::init()
{
	data_filename.set( "none" );
	model_filename.set( "none" );
	fill_data() ;
	averager = NULL;
	param_init_with( n_bdd_csv::param, n_bdd_csv::PARAM_NB_MAX ); // bdd_csv_param, BDD_CSV_PARAM_NB_MAX);
}

void	c_bdd_csv::param_init()
{
	time_modified = 0;
	filter_factor = -1.;
	nb_col_last = -1;
	nb_row_last = -1;
}

void	c_bdd_csv::alloc()
{
	averager = new c_averager[BDD_CSV_AVERAGER_MAX_NB];
}

void	c_bdd_csv::dealloc()
{
	SAFE_DELETE_ARRAY( averager );
}

CONSTRUCTOR_CREATE(c_bdd_csv)
{
	init();
//	alloc();
}

c_bdd_csv::~c_bdd_csv()
{
	dealloc();
}

void	c_bdd_csv::update()
{
	time_t	mtime;
	INT32	src;
	INT32	dst;

	if (!averager)
		alloc();

	c_file::dir_change_to_def();
	mtime = c_file::get_mdate( data_filename.get() );
	if ( mtime && time_modified != mtime)
	{
		if( file_csv::read_float( data_filename.get(),  data, 8, 32) != 0 )	//hack 8? 32 ?
		{
			nb_col = data[0];
			nb_row = data[1];
			nb_col_draw = data[2];
			nb_row_draw = data[3];
			col_index_cur = data[4];
			time_modified = mtime;
		}
		else
		{
			ERR_PRINT_STRING( "no read" );
		}
	}
	if( filter_factor != filter_factor_ui )
	{
		filter_factor = filter_factor_ui;
		for( INT32 i=0; i<BDD_CSV_COL_MAX_NB; i++)
			averager[i].set_filter_factor( filter_factor);
	}
	src = nb_col + col_index_cur;
	dst = 0;
	for( INT32 i=0; i<nb_row_draw; i++)
	{
		for( INT32 j=0; j<nb_col_draw; j++)
		{
			averager[dst++].put(data[src++]);
		}
		src += nb_col - nb_col_draw;
	}
	if ( nb_col_last != nb_col_draw || nb_row_last != nb_row_draw )
	{
		nb_col_last = nb_col_draw;
		nb_row_last = nb_row_draw;
		reset_max_factor();
	}
}

void	c_bdd_csv::draw_cube()
{
	//INT32	c;
	//INT32	r;
	REAL	u;
	REAL	du;
	REAL	v;
	REAL	dv;

	du = 1./nb_row_draw;
	dv = 1./nb_col_draw;

	REAL	size_u = du * size[0];
	REAL	size_v = dv * size[1];
	REAL	tmp;
	INT32	i= 0;

	u = -.5 + du*.5;
	for( INT32 r=0; r<nb_row_draw; r++)
	{
		v = -.5 + dv*.5;
		for( INT32 c=0; c<nb_col_draw; c++)
		{
			tmp = averager[i].get_average()*size[2];
			if ( tmp > max[c] )
			{
				max[c] = tmp;
				max_factor[c] = 1./tmp;
			}
			tmp *= max_factor[c]*size[2];
//				tmp *= size[2];
			tmp += size[2]*.01;
			tmp *= .5;
			draw_box( u-size_u, -tmp, v-size_v, u+size_u, tmp, v+size_v );
			v += dv;
			i++;
		}
		u += du;
	}
}
/*
void	c_bdd_csv::draw_bdd()
{
//INT32	r;
REAL		tmp;
INT32		i = 0;
c_bdd_tri*	bdd = (c_bdd_tri*)g_layer_prev->get_bdd_tri();

	if( bdd)
		{
		bdd->draw_before();
		for( INT32 r=0; r<nb_row_draw; r++)
			{
			GOL::push_matrix();
				tmp = averager[i].get_average();
				if ( tmp < 48.)
					GOL::color3( 1., .5, .5);
				else if ( tmp > 52. )
					GOL::color3( 1, 1, 1);
				else
					GOL::color3( .5, .5, 1.);
				tmp *= size[2] + .05;
//				GOL::scale( .0, .0, tmp );
				GOL::scale( 1.0, tmp, 1.0 );
				bdd->draw_obj(r);
			GOL::pop_matrix();
			i++;
			}
		}
}
*/
void	c_bdd_csv::draw()
{
	if( b_draw_cube )
		draw_cube();
//hack
//	else
//		draw_bdd();
}