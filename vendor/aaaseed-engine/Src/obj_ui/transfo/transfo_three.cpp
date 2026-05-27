#include "transfo_three.h"
#include "ui/strsymbo.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"


FACTORY_CREATE_V1( c_transfo_three, transfo_three, Transformation Mocap, transfo_three );
c_transfo_three*	c_transfo_three::cur = nullptr;

template <typename T, size_t N>
FINLINE size_t sizeofarray( const T(&)[ N ] )	{	return N;	}

static	C_PCHAR_C	str_scale[] =
{
	"None",
	"Uniform",
	"Axial only",
	"By Axe",
};
INT32 CONST SCALE_SYMBO_NB = (INT32)sizeofarray(str_scale);

#define	TYPE_SYMBO_NB	8
static	C_PCHAR_C	str_type[TYPE_SYMBO_NB] =
{
	"Rotation at start",
	"Rotation at stop",
	"Rotation at third",
	"Segment and Normal X",
	"Segment and Normal Y",
	"Segment and Normal Z",
	"Segment and Normal Third",
	"Segment and Center to Third",
};

namespace n_transfo_three
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 6;
	CONSTEXPR INT32 MOCAP_PARAM_NB		= 6;
	CONSTEXPR INT32 POSITION_PARAM_NB	= 9;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	MOCAP_PARAM_NB
									+	POSITION_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_REF( name_symbo )

		PARAM_DEF_SYMBO(				type,	1, 0,		TYPE_SYMBO_NB-1,		str_type	)
		PARAM_DEF_AXE_X( axe )
		PARAM_DEF_SYMBO(				scale,	0, 1,		SCALE_SYMBO_NB-1,		str_scale	)
		PARAM_DEF_REAL_ZERO( inter )

		PARAM_DEF_GROUP_CLOSED( Mocap, MOCAP_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO( start_channel	)
			PARAM_DEF_INT32_POS_ZERO( start_node		)
			PARAM_DEF_INT32_POS_ZERO( stop_channel	)
			PARAM_DEF_INT32_POS_ZERO( stop_node		)
			PARAM_DEF_INT32_POS_ZERO( third_channel	)
			PARAM_DEF_INT32_POS_ZERO( third_node		)

		PARAM_DEF_GROUP_CLOSED( Position, POSITION_PARAM_NB )
			PARAM_DEF_POINT_XYZ( start	)
			PARAM_DEF_POINT_XYZ( stop	)
			PARAM_DEF_POINT_XYZ( third	)
	};
}

void	c_transfo_three::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, get_name_symbo() );

	param_set_pt( h, _s_type	);
	param_set_pt( h, _s_axe		);
	param_set_pt( h, _s_scale	);
	param_set_pt( h, _inter		);

	++h;
		param_set_pt( h, _start_channel	);
		param_set_pt( h, _start_node		);
		param_set_pt( h, _stop_channel	);
		param_set_pt( h, _stop_node		);
		param_set_pt( h, _third_channel	);
		param_set_pt( h, _third_node		);

	++h;
		param_set_pt_3( h, _start	);
		param_set_pt_3( h, _stop	);
		param_set_pt_3( h, _third	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_transfo_three)
{
	param_init_with( n_transfo_three::param, n_transfo_three::PARAM_NB_MAX );

	_mat[3] = 0;
	_mat[7] = 0;
	_mat[11] = 0;
	_mat[15] = 1;
}

c_transfo_three::~c_transfo_three()
{
	if( cur == this )
		cur = nullptr;
}

void	c_transfo_three::update_private()
{
	c_bdd_mocap*	mocap_start = nullptr;
	if( _start_node )
	{
		mocap_start = c_bdd_mocap::get_from_channel( _start_channel );
		if( mocap_start )
			mocap_start->get_tra_cur( _start, _start_node );
	}

	c_bdd_mocap*	mocap_stop = nullptr;
	if( _stop_node )
	{
		mocap_stop = c_bdd_mocap::get_from_channel( _stop_channel );
		if( mocap_stop )
			mocap_stop->get_tra_cur( _stop, _stop_node );
	}

	c_bdd_mocap*	mocap_third = nullptr;
	if( _third_node )
	{
		mocap_third = c_bdd_mocap::get_from_channel( _third_channel );
		if( mocap_third )
			mocap_third->get_tra_cur( _third, _third_node );
	}

	interpolate_v3( &_mat[12], _start, _stop, _inter );

	INT32	i_u;
	INT32	i_v;
	axe_build_index( i_u, i_v, _s_axe );
	i_u *= 4;
	i_v *= 4;
	INT32	i_axe = _s_axe * 4;

	if( _s_type <= 2 )	//	Rotation at start / stop / third
	{
		switch( _s_type )
		{
		case 0:
			if( mocap_start)
				mocap_start->get_matrix_rows( &_mat[i_axe], &_mat[i_u], &_mat[i_v], _start_node );
			break;
		case 1:
			if( mocap_stop)
				mocap_stop->get_matrix_rows( &_mat[i_axe], &_mat[i_u], &_mat[i_v], _stop_node );
			break;
		case 2:
			if( mocap_third )
				mocap_third->get_matrix_rows( &_mat[i_axe], &_mat[i_u], &_mat[i_v], _third_node );
			break;
		}
		switch( _s_scale )
		{
		case 1:	//uniform
			{
				REAL	size = dist_v3r( _start, _stop );
				scale_v3( &_mat[0], size );
				scale_v3( &_mat[4], size );
				scale_v3( &_mat[8], size );
			}
			break;
		case 2:	//axial only
			{
				REAL	size = dist_v3r( _start, _stop );
				scale_v3( &_mat[i_axe], size );
			}
			break;
		}
	}
	else
	{
		REAL	vec[3];
		sub_v3( &_mat[i_axe], _stop, _start );
		switch (_s_type)
		{
		case 3:	//	Segment and Normal X
		case 4:	//	Segment and Normal Y
		case 5:	//	Segment and Normal Z
			clear_v3( vec);
			vec[_s_type-3] = 1.;
			cross_v3r( &_mat[i_v], &_mat[i_axe], vec );
			cross_v3r( &_mat[i_u], &_mat[i_v], &_mat[i_axe] );
			break;
		case 6:	//	Segment and Normal Third
			sub_v3( vec, _third, &_mat[12] );
			cross_v3r( &_mat[i_v], &_mat[i_axe], vec );
			cross_v3r( &_mat[i_u], &_mat[i_v], &_mat[i_axe] );
			break;
		case 7:	//	Segment and Center to Third
			sub_v3( vec, _third, &_mat[12] );
			cpy_v3( &_mat[i_u], vec );
			cross_v3r( &_mat[i_v], &_mat[i_axe], &_mat[i_u] );
			break;
		}
		switch( _s_scale )
		{
		case 0:	//	none
			normalize_v3r( &_mat[0] );
			normalize_v3r( &_mat[4] );
			normalize_v3r( &_mat[8] );
			break;
		case 1:	//	uniform
			{
				REAL size = norm_v3r( &_mat[i_axe] );
				normalize_scale_v3r( &_mat[i_u], size );
				normalize_scale_v3r( &_mat[i_v], size );
			}
			break;
		case 2:	//	axial only
			normalize_v3r( &_mat[i_u] );
			normalize_v3r( &_mat[i_v] );
			break;
		case 3:	//	By Axe
			{	//todo smething fishy here: why we compute size and why we do same as axial only
				REAL size = norm_v3r( &_mat[i_axe] );
			//	normalize_v3r( &_mat[i_axe] );
				normalize_v3r( &_mat[i_u] );
				normalize_v3r( &_mat[i_v] );
//				normalize_scale_v3r( &_mat[i_u], size );
//				normalize_scale_v3r( &_mat[i_v], size );
				//V3D		vec;
				//sub_v3( vec, _third, &_mat[12] );
				//size = norm_v3r( vec );
				//normalize_scale_v3r( &_mat[i_axe], size );
			}
			break;
		}
	}
	//todo	make it smarter
	_b_todo = true;
}

void	c_transfo_three::do_it_low()
{
	GOL::matrix::mul_matrix( (glm::mat4 CONST *) _mat );
}
