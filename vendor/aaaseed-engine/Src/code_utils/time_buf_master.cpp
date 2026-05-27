#include "time_buf_master.h"
#include "time_buf.h"
#include "infrastructure/param/param_declare.h"
#include "draw/NsightEvents.h"
#include "time/aaa_time.h"
#include "spy.h"


namespace nsight
{
	extern	bool	b_started_out;
	extern	bool	b_active_ui;
	extern	bool	b_active_event_ui;
};

FACTORY_CREATE_V1( c_tbuf_master, time_buffer_master, Time Buffer Master, time_buffer_master );

namespace	n_time_buffer
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 0;
	CONSTEXPR INT32 SPY_PARAM_NB	= 6;
	CONSTEXPR INT32 TBUF_PARAM_NB	= 7;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	SPY_PARAM_NB
										+	TBUF_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP( Spy, SPY_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(			nsight_started				)
			PARAM_DEF_BOOL_OFF(				nsight_active				)
			PARAM_DEF_BOOL_OFF(				nsight_active_event			)
			PARAM_DEF_INT32_INF_SAVE_NOT(	spy_stack_check,			1,0	)
			PARAM_DEF_BOOL_OFF(				spy_print_frame_trig		)
			PARAM_DEF_BOOL_OFF(				spy_print					)
		PARAM_DEF_GROUP( TBuf, TBUF_PARAM_NB )
			PARAM_DEF_BOOL_OFF(				active						)
			PARAM_DEF_REAL_POS(				buffer_time_len,			1, 32 )
	//todo doit
	//		PARAM_DEF_BOOL_ON(				follow						)
			PARAM_DEF_REAL_POS(				draw_time_len,				8, 1 )
			PARAM_DEF_REAL_ONE(				text_size					)
			PARAM_DEF_BOOL_OFF(				text_always					)
			PARAM_DEF_INT32_LOCKED(			nb_elt						)
			PARAM_DEF_INT32_LOCKED(			nb_elt_free					)
	};
}

CONSTRUCTOR_CREATE(c_tbuf_master)
{
//	DBG_PRINT_STRING( "Begin Constructor c_tbuf_master" );
	init();
	set_name( "Time Buffer Master" );
	param_init_with( n_time_buffer::param, n_time_buffer::PARAM_NB_MAX ); //  time_buffer_master_param, TIME_BUFFER_MASTER_PARAM_NB_MAX);
//	DBG_PRINT_STRING( "End Constructor c_tbuf_master" );
}

c_tbuf_master::~c_tbuf_master()
{
	deinit();
}

void	c_tbuf_master::param_init_pt()
{
	INT32	h = 0;
	//todo add a global active for bothe tbuf and spy
	++h;
		param_set_pt( h, nsight::b_started_out		);
		param_set_pt( h, nsight::b_active_ui		);
		param_set_pt( h, nsight::b_active_event_ui	);
		param_set_pt( h, spy::stack_count			);
		param_set_pt( h, spy::b_print_frame_trig_ui	);
		param_set_pt( h, spy::b_print				);

	++h;
		param_set_pt( h, c_tbuf::b_active_ui		);
		param_set_pt( h, _buf_time_len				);
//todo doit
//		param_set_pt( h, _b_follow_phase_ui			);
		param_set_pt( h, _draw_time_len				);
		param_set_pt( h, _text_size					);
		param_set_pt( h, _b_text_always				);
		param_set_pt( h, _nb_elt					);
		param_set_pt( h, _nb_elt_free				);

	err_param_init_pt( h );
}

void	c_tbuf_master::init()
{
	_p_tbuf = (c_tbuf**) MALLOC( tbuf::CH_MAX_NB * sizeof(c_tbuf*) );
	for( UINT32 i = 0; i < tbuf::CH_MAX_NB; ++i )
		_p_tbuf[i] = nullptr;
	_time_update = -1.;
	get_always( tbuf::CH_VBL )->set_const( false );
	_nb_elt = 0;
}

void	c_tbuf_master::deinit()
{
	for( UINT32 i = 0; i < tbuf::CH_MAX_NB; ++i )
		obj_delete( _p_tbuf[i] );
}

void	c_tbuf_master::update()
{
	if( is_active() )
	{
		DOUBLE		l_time;
		c_tbuf**	pt;
		l_time = aaa::time::get_real_time_from_start();

		_nb_elt_free = 0;
		pt = &_p_tbuf[0];
		for( UINT32 i = 0; i < tbuf::CH_MAX_NB; ++i )
		{
			if( *pt)
				(*pt)->update( l_time );
			++pt;
		}
		_time_update = l_time;
		_b_need_time_scale = true;
	}
}

c_tbuf*	c_tbuf_master::get( INT32 channel_id )
{
	CLAMP_REF( channel_id, 0, INT32(tbuf::CH_MAX_NB) );
	return _p_tbuf[channel_id];
}

c_tbuf*	c_tbuf_master::get_always( INT32 CONST channel_id )
{
	c_tbuf*	pt = get( channel_id );
	if( !pt )
	{
		pt = new c_tbuf;
		if( pt )
		{
// only for obj_ui			pt->set_root();
			_p_tbuf[ channel_id ] = pt;
		}
		else
			ERR_PRINT_STRING( "time buffer: Can't allocate()" );
	}
	return pt;
}

c_tbuf_master*	g_tbuf_master = nullptr;