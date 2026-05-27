#include "bdd_emotiv.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include <queue>
//#include "obj_ui/tracker/epoc.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"

FACTORY_CREATE_PROP_V1( c_bdd_emotiv, emotiv, Emotiv, emotiv, sub_menu="Hardware"; );

#include "obj_ui/tracker/emokit.h"

std::deque<REAL> elec[14];

namespace n_bdd_emotiv
{
	CONSTEXPR INT32 BASE_PARAM_NB		=	17 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 ELECTRODE_PARAM_NB	=	14;
	CONSTEXPR INT32 GROUP_PARAM_NB		=	2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ELECTRODE_PARAM_NB
									+	ELECTRODE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active	)
		PARAM_DEF_BOOL_OFF(			open	)
		PARAM_DEF_BOOL_LOCKED(		opened	)
		PARAM_DEF_BOOL_OFF(			verbose )
		PARAM_DEF_INT32_LOCKED(		nb		)

		PARAM_DEF_INT32_LOCKED(		capture_index_raw	)
		PARAM_DEF_INT32_LOCKED(		battery	)
		
		PARAM_DEF_INT32_LOCKED_XY(	gyro_raw	)
		PARAM_DEF_XY_LOCKED(		gyro		)

		PARAM_DEF_GROUP_CLOSED( Electrode Raw, ELECTRODE_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(		electrode_raw_1		)	//F3
			PARAM_DEF_REAL_LOCKED(		electrode_raw_2		)	//FC6
			PARAM_DEF_REAL_LOCKED(		electrode_raw_3		)	//P7
			PARAM_DEF_REAL_LOCKED(		electrode_raw_4		)	//T8
			PARAM_DEF_REAL_LOCKED(		electrode_raw_5		)	//F7
			PARAM_DEF_REAL_LOCKED(		electrode_raw_6		)	//F8			
			PARAM_DEF_REAL_LOCKED(		electrode_raw_7		)	//T7	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_8		)	//P8	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_9		)	//AF4	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_10	)	//F4	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_11	)	//AF3	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_12	)	//O2	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_13	)	//O1	
			PARAM_DEF_REAL_LOCKED(		electrode_raw_14	)	//FC5	

		PARAM_DEF_BOOL_OFF(		use_average	)
		PARAM_DEF_REAL_ONE(		factor		)
		PARAM_DEF_REAL_ZERO(	offset		)
		PARAM_DEF_BOOL_OFF(		color_auto	)
		PARAM_DEF_INT32(		curve_begin, 4, 1, 1, ELECTRODE_PARAM_NB		)
		PARAM_DEF_INT32(		curve_end, 4, ELECTRODE_PARAM_NB, 1, ELECTRODE_PARAM_NB		)

		PARAM_DEF_GROUP( Electrode, ELECTRODE_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(		electrode_1			)	//F3	
			PARAM_DEF_REAL_LOCKED(		electrode_2			)	//FC6	
			PARAM_DEF_REAL_LOCKED(		electrode_3			)	//P7	
			PARAM_DEF_REAL_LOCKED(		electrode_4			)	//T8	
			PARAM_DEF_REAL_LOCKED(		electrode_5			)	//F7	
			PARAM_DEF_REAL_LOCKED(		electrode_6			)	//F8		
			PARAM_DEF_REAL_LOCKED(		electrode_7			)	//T7	
			PARAM_DEF_REAL_LOCKED(		electrode_8			)	//P8	
			PARAM_DEF_REAL_LOCKED(		electrode_9			)	//AF4	
			PARAM_DEF_REAL_LOCKED(		electrode_10		)	//F4	
			PARAM_DEF_REAL_LOCKED(		electrode_11		)	//AF3	
			PARAM_DEF_REAL_LOCKED(		electrode_12		)	//O2	
			PARAM_DEF_REAL_LOCKED(		electrode_13		)	//O1	
			PARAM_DEF_REAL_LOCKED(		electrode_14		)	//FC5	

/*
		PARAM_DEF_INT32_POS_ONE( com_port_nb )
		//PARAM_DEF_INT32_POS( com_port_nb_last, 4, 3 )
		//PARAM_DEF_BOOL_OFF( com_port_scan )
		//PARAM_DEF_INT32( com_port_scan_start, 4, 3, 1, 32 )
		//PARAM_DEF_INT32( com_port_scan_stop, 3, 4, 1, 32 )
		PARAM_DEF_INT32(			max_touch, c_bdd_ir_touch::IRT_MAX_TOUCH, 1, 1, c_bdd_ir_touch::IRT_MAX_TOUCH )
*/

	};
}

void	c_bdd_emotiv::param_init_pt()
{
	INT32	h = 0;

	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_open );
	param_set_pt( h, _b_verbose_ui );
	param_set_pt( h, _nb );

	param_set_pt( h, _index_raw );
	param_set_pt( h, _battery );
	param_set_pt_2( h, _gyro_raw );
	param_set_pt_2( h, _gyro );

	++h;
		param_set_pt_n( h, _electrodes_raw, 14 );

	param_set_pt( h, _b_moy );
	param_set_pt( h, _factor );
	param_set_pt( h, _offset );
	param_set_pt( h, _b_color_auto );
	param_set_pt( h, _curve_begin );
	param_set_pt( h, _curve_end );

	++h;
		param_set_pt_n( h, _electrodes, 14 );

	err_param_init_pt(h);

}

CONSTRUCTOR_CREATE(c_bdd_emotiv)
,_b_open		{false}
{
	param_init_with( n_bdd_emotiv::param, n_bdd_emotiv::PARAM_NB_MAX );
}

c_bdd_emotiv::~c_bdd_emotiv()
{
	close();
}


void	c_bdd_emotiv::update_low()
{
#if	AAA_USE_EMOKIT()
//	if( _b_mocap_feed_ui )
//	{	//	feed mocap if needed
	c_bdd_mocap*	mocap = nullptr;
	//c_bdd_mocap::get_from_channel( 56 );

//	}
	while( emokit_read_data(_epoc_device) > 0 )
	{
		emokit_frame frame = emokit_get_next_frame(_epoc_device);

		_index_raw = frame.counter;
		_battery = frame.battery;

		REAL offset = - 8192.;
		_electrodes_raw[0]	= ( frame.F3  )  * 1. ;
		_electrodes_raw[1]	= ( frame.F4  )  * 1. ;
		_electrodes_raw[2]	= ( frame.AF3 )  * 1. ;
		_electrodes_raw[3]	= ( frame.AF4 )  * 1. ;
		_electrodes_raw[4]	= ( frame.F7  )  * 1. ;
		_electrodes_raw[5]	= ( frame.F8  )  * 1. ;
		_electrodes_raw[6]	= ( frame.FC5 )  * 1. ;
		_electrodes_raw[7]	= ( frame.FC6 )  * 1. ;
		_electrodes_raw[8]	= ( frame.P7  )  * 1. ;
		_electrodes_raw[9]	= ( frame.P8  )  * 1. ;
		_electrodes_raw[10]	= ( frame.T7  )  * 1. ;
		_electrodes_raw[11]	= ( frame.T8  )  * 1. ;
		_electrodes_raw[12]	= ( frame.O1  )  * 1. ;
		_electrodes_raw[13]	= ( frame.O2  )  * 1. ;

		REAL moy = 0;
		if( _b_moy )
		{
			for( INT32 i=0; i<14; ++i )
			{
				moy += _electrodes_raw[i];
			}
			moy /= 14.;
		}

		for( INT32 i=0; i<14; ++i )
		{
			_electrodes[i] = (_electrodes_raw[i] - moy + _offset) * _factor;
			elec[i].push_back( _electrodes[i] );
			if( elec[i].size() > 512 )
				elec[i].pop_front();
		}

		if( mocap )		
		{
			REAL vec[3];
			for( INT32 i=0; i<14; ++i )
			{
				vec[0] = 0.;
				vec[1] = _electrodes[i];
				vec[2] = i/13 - .5;
				mocap->store_tra( i, vec );
			}
			mocap->push_data();
		}



		_gyro_raw[0] = frame.gyroX;
		_gyro_raw[1] = frame.gyroY;

		_gyro[0] = frame.gyroX / 128.;
		_gyro[1] = frame.gyroY / 128.;
	}
#endif
}

void	c_bdd_emotiv::update()
{
	if( is_active() )
	{
		if( !_b_open )
		{
			if( open() )
				_b_open = true;
/*			if( !_serial )
				open();
			if( _serial )
				update_low();
			if( _serial )
			{

*/
		}
		else
		{
			update_low();
/*			blobs_reset();
			if( _serial )
				close();
*/
		}

	}
//	else
//		blobs_reset();

}

void	c_bdd_emotiv::close()
{
/*
	if( _serial )
	{
		_serial->close();
		obj_delete( _serial );
	}
*/
}


/// Vendor ID for all emotiv devices
static uint32_t VID = 0x21a1;
/// Product ID for all emotiv devices
static uint32_t PID = 0x0001;


bool	c_bdd_emotiv::open()
{
#if	AAA_USE_EMOKIT()
	_epoc_device = emokit_create();

	//more recent headset ? block with those
	//VID = 0x1234;
	//PID = 0xed02;

	_nb = emokit_get_count( _epoc_device, VID, PID );
	emokit_open( _epoc_device, VID, PID, 1 );
	
//	emokit_read_data( epoc_device );
//	emokit_get_next_frame( epoc_device );

   

/*
	epoc_device* epoc = epoc_create();
	_nb = epoc_get_count( epoc, EPOC_VID, EPOC_PID );
	if( _nb > 0 )
	{
		if( epoc_open( epoc, EPOC_VID, EPOC_PID, 0) != 0 )
		{
			ERR_PRINT_STRING( "cannot open epoc (libusb) " ); 
			//return false;
		}
	}
*/	
/*
	if( !_serial )
	{
		bool	b_open = false;
		_serial = new c_serial;
		if( _serial )
		{
			_serial->set_baudrate( c_serial::BAUDRATE_38400 );
			//UINT32	start, stop;
			//start = stop = _com_port_nb;
			if ( NOERR( _serial->open( _com_port_nb ) ) )
			{
				b_open = true;
			}
			//if ( _b_com_port_scan )
			//{
			//	start = MIN( _com_port_start, _com_port_stop );
			//	stop = MAX( _com_port_start, _com_port_stop );
			//	// try last com port used first, might speed up things...
			//	if( _com_port_nb_last > 0 && NOERR( _serial->open( _com_port_nb_last ) ) )
			//		_b_open = true;
			//}
			//if ( !_b_open )
			//{
			//	for( UINT32 i = start; i <= stop; ++i )
			//	{
			//		if( _com_port_nb_last != i && NOERR( _serial->open( i ) ) )
			//		{
			//			_com_port_nb_last = i;
			//			_b_open = true;
			//			break;
			//		}
			//	}
			//}
		}
		if( !b_open )
		{
			err_print( "can't start" );( "Can't start IRTouch on COM %d", _com_port_nb );
			close();
		}
	}
*/
#else
	debug_break( "emokit not implemented in x64 for now" );
#endif
	return true;
}

static	FP32	color_auto[] =
{
	1,	0,	0,	1,	
	0,	1,	0,	1,
	0,	0,	1,	1,
	1,	1,	1,	1,
	1,	1,	0,	1,
	0,	1,	1,	1,
	1,	0,	1,	1,
	1,	1,	1,	.5,
};

void c_bdd_emotiv::draw()
{
	REAL vec[3];
	REAL fx = 1./512.;
	INT32 nb_curve = _curve_end - _curve_begin + 1;
	for( INT32 i=_curve_begin-1; i<_curve_end; ++i )
	{
		if( _b_color_auto )
			GOL::color3v( &color_auto[4*(i%8)] );
		std::deque<REAL>& e = elec[i];
		vec[2] =  (i-_curve_begin+1) / REAL(nb_curve);
		GOL::begin( GL_LINE_STRIP );
		INT32 nb = (INT32)e.size();
		for( INT32 ix=0; ix<nb; ++ix )
		{
			vec[0] = ix * fx - REAL(.5);
			vec[1] = e[ix];
			GOL::vertex3v( vec );
		}
		GOL::end();
	}
}