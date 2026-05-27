#include "bdd_img_send.h"
#include "obj_ui/com/net.h"
#include "image/bind_img_2d.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_send, bdd_img_send, Image Sender, bdd_img_send, sub_menu="Image"; );

namespace
{
	C_PCHAR_C	str_same_bind[] = { "Same" };
	
	CONSTEXPR UINT32 BASE_PARAM_NB	= 11 + c_bdd_img::SUPER_RESIZE_PARAM_NB;
	CONSTEXPR UINT32 TIMING_PARAM_NB	= 0;
	CONSTEXPR UINT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	 PARAM_NB	=	BASE_PARAM_NB
								+	TIMING_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_RESIZE_PARAMS

	//	PARAM_DEF_BOOL_ON(			difference				)
	//	PARAM_DEF_INT32(			difference_bind_dst,	0, -1, -1, PARAM_MAX_INT32		)
		PARAM_DEF_NET_LINK(			link_index,				1, 0	)
		PARAM_DEF_INT32(			channel_id,				2, 1,	1, 256*256-1		)
		PARAM_DEF_BIND_2D_SYMBO(	out_bind_dst,			0, -1,	str_same_bind )
		PARAM_DEF_BOOL_OFF(			crop					)
		PARAM_DEF_RECT_LRBT(		crop,					PARAM_DEF_INT32_POS_ZERO		)
		PARAM_DEF_BOOL_ON(			compress				)
		PARAM_DEF_INT32_LOCKED(		blk_nb					)
		PARAM_DEF_INT32_LOCKED(		memory_size				)	//UINT64_SOON
/*
		PARAM_DEF_GROUP_CLOSED(	Timings, TIMING_PARAM_NB	)
			PARAM_DEF_REAL_LOCKED(	process_time			)
			PARAM_DEF_REAL_LOCKED(	detect_time				)
			PARAM_DEF_REAL_LOCKED(	landmark_time			)
*/
	};

	PARAM_DEF_MAKE_INDEX( link_index );
}

void	c_bdd_img_send::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, true );

//	param_set_pt( h, _st_send._b_difference		);
//	param_set_pt( h, _difference_bind_ui		);
	param_set_pt( h, _st_send._net_link_index	);
	param_set_pt( h, _st_send._channel_id		);
	param_set_pt( h, _bind_dst_ui				);
	param_set_pt( h, _st_send._b_crop			);
	param_set_pt( h, _st_send._crop.left		);
	param_set_pt( h, _st_send._crop.right		);
	param_set_pt( h, _st_send._crop.bottom		);
	param_set_pt( h, _st_send._crop.top			);
	param_set_pt( h, _st_send._b_compress		);
	param_set_pt( h, _st_send._blk_nb			);
	param_set_pt( h, _st_send._sent_size		);
/*
	++h;
		param_set_pt( h, _speed_bdd->get_pt_interval_ms()						);
		param_set_pt( h, _face_tracker->_speed_detect->get_pt_interval_ms()		);
		param_set_pt( h, _face_tracker->_speed_landmark->get_pt_interval_ms()	);
*/
	err_param_init_pt( h );
}
void	c_bdd_img_send::prepare_for_ui()
{
	c_net_link* p_link = net->get_link( _st_send._net_link_index );
	param_attach_obj_no_inc( PARAM_INDEX_link_index, (c_obj_ui* )p_link );
}
CONSTRUCTOR_CREATE( c_bdd_img_send )
{
	_st_send._blk_nb = 0;
	_st_send._sent_size = 0;
//	_speed_bdd = new c_speed( false, 4, g_speed_master->get_pb_print_bdd(),	"update"			, this );
	param_init_with( param, PARAM_NB );
}


c_bdd_img_send::~c_bdd_img_send()
{
}

void	c_bdd_img_send::update()
{
	bool b_change_it;
	if( update_mat_src(b_change_it) )
	{
		INT32 index;
		if( b_change_it )
		{
			index = build_dst_index( _dst_img_index_ui );
			update_dst_index( index, "c_bdd_img_send" );
		}
		else
		{
			index = build_dst_index( get_src_img_index_ui() );	
		}

		c_img_2d* p_img = g_bind_img_2d->get_always( index );

//		_st_send._bind_difference	= _difference_bind_ui;	//	_difference_bind_ui == -1 ? index : _difference_bind_ui;
		_st_send._bind_dst = _bind_dst_ui == -1	? index : _bind_dst_ui;
		if( p_img )
			p_img->send( &_st_send );
	}
}
