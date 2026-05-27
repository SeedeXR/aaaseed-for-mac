#include "polhemus.h"

#if AAA_TRACKER_POLHEMUS()
#include "aaa_os.h"
#include "Tracker/polhemus/PDI/pdi.h"
#include "infrastructure/param/param_declare.h"

#include "spy.h"

FACTORY_CREATE_V1( c_polhemus, polhemus, Polhemus, polhemus );

c_polhemus*		polhemus = nullptr;

namespace	n_polhemus
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 4;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(	active			)
		PARAM_DEF_BOOL_OFF(	open			)
		PARAM_DEF_NONE(		controller_1	)
		PARAM_DEF_NONE(		controller_2	)
	};
}

void	c_polhemus::param_init_pt()
{
	INT32	h=0;
	param_set_pt(		h, get_pt_active() );
	param_set_pt(		h, _b_open_ui );
	param_attach_obj(	h, (c_obj_ui*) &_sensors[0] );
	param_attach_obj(	h, (c_obj_ui*) &_sensors[1] );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_polhemus)
{
	pdi = nullptr;
	_sensors[0].set_absolute(true);
	_sensors[0].set_root(this);
	_sensors[1].set_absolute(true);
	_sensors[1].set_root(this);

	param_init_with( n_polhemus::param, n_polhemus::PARAM_NB_MAX ); // polhemus_param, POLHEMUS_PARAM_NB);
//	clear_v3r( angle );
//	clear_v3r( a_last);
}

c_polhemus::~c_polhemus()
{
	stop();
}

//todo we should add a verbose flag and make sure this print
VOID	c_polhemus::DisplayFrame( INT8* pBuf, INT32 dwSize )
{
	CHAR			szFrame[200];
	INT32			i = 0;
	INT32			i_sensor = 0;
	while ( i<dwSize)
	{
		BYTE ucSensor = pBuf[i+2];
		SHORT shSize = pBuf[i+6];

		// skip rest of header
		i += 8;

		PFLOAT pPno = (PFLOAT)(&pBuf[i]);

		sprintf( szFrame, "%2d   %+011.6f %+011.6f %+011.6f   %+011.6f %+011.6f %+011.6f\r\n", 
				ucSensor, pPno[0], pPno[1], pPno[2], pPno[3], pPno[4], pPno[5] );
		_sensors[i_sensor].set_tra_in( pPno[0], pPno[1], pPno[2] );
		_sensors[i_sensor].set_rot_in( pPno[3]/360., pPno[4]/360., pPno[5]/360. );
		_sensors[i_sensor].update_sensor_6dof();

		i += shSize;
		++i_sensor;
	}
}
//REAL CONST	bird_div = 256.*256.;
void	c_polhemus::update_low()
{
	if( pdi )
	{
		PBYTE	pBuf;
		DWORD	dwSize;
//		if( pdi->ReadSinglePnoBuf ( pBuf, dwSize ) )
		if( pdi->LastPnoPtr ( pBuf, dwSize ) )
		{
			DisplayFrame( (INT8*)pBuf, dwSize );
		}
	}
/*
BOOL		b;
BOOL		found = FALSE;
BIRDFRAME	frame;

	while( b = birdFrameReady( 0) )
		{
		found = birdGetFrame( 0, &frame);
		if( !found )
			ERR_PRINT_STRING( "PCBird can't get frame");
		}

	if( found )
		{
		REAL	a[3];
		REAL	r;

		BIRDPOSITION* ppos = &frame.reading[0].position;
		set_data_in( 0, ppos->nX / bird_div );
		set_data_in( 1, ppos->nZ / bird_div );
		set_data_in( 2, ppos->nY / bird_div );

		BIRDANGLES* pangles = &frame.reading[0].angles;
		a[0] = -pangles->nElevation	/ bird_div;
		a[1] = pangles->nAzimuth	/ bird_div;
		a[2] = pangles->nRoll		/ bird_div;

//		printf( "%d %f\n", pangles->nAzimuth, a[1] );
		for( INT32 i=0; i<3; ++i )
			{
			r = a[i] - a_last[i];
			if( r > .5 )
				angle[i] -= 1;
			else if( r < -.5 )
				angle[i] += 1;
			set_data_in( i+3, REAL(angle[i]) + a[i] );
			a_last[i] = a[i];
			}

		c_sensor_6dof::update_low();
		}
*/
}

void	c_polhemus::close()
{
	if( pdi )
	{
		if( pdi->Disconnect() )
			trackers::PRINT_STRING( "Polhemus", "Disconnected");
		else
			ERR_PRINT_STRING( "Polhemus Can't Disconnect" );
		obj_delete( pdi );
	}
/*	birdStopFrameStream(0);
	birdShutDown( 0);
*/
}

#define	MY_MOTION_BUF_SIZE		0x0800
BYTE	motion_buf[ MY_MOTION_BUF_SIZE ];

AAA_ERR	c_polhemus::open()
{
	if( !pdi )
		pdi = new CPDIdev;
	if( pdi )
	{
		ePiCommType eType = pdi->DiscoverCnx();
		if( eType == PI_CNX_NONE || eType == PI_CNX_UNKNOWN )
		{
			ERR_PRINT_STRING( "Polhemus Can't Connect" );
			return ERR_ANY;
		}
		else
		{
			pdi->SetPnoBuffer( motion_buf, MY_MOTION_BUF_SIZE );
			trackers::PRINT_STRING( "Polhemus", "Connected");
			pdi->StartContPno(0);
			spy::sleep( 1000, "sleep c_polhemus::open()" );
			return AAA_OK;
		}
	}
/*
bool		b;
WORD	word[4];

	word[0] = 772;
	word[1] = 0;
	word[2] = 0;
	word[3] = 0;

	if( b = birdISAWakeUp( 0, TRUE, 1, word, 1000, 1000) )
		{
		BIRDSYSTEMCONFIG	config_sys;
		if( b = birdGetSystemConfig( 0, &config_sys) )
			{
			BIRDDEVICECONFIG	config_dev;
			if( b = birdGetDeviceConfig( 0, 1, &config_dev) )
				{
				//	b = birdStartSingleFrame( 0);
				if( b = birdStartFrameStream( 0) )
					{
					spy::sleep( 20, "sleep c_polhemus::open()" );

					b = birdFrameReady( 0);
					if(b)
						{
						BIRDFRAME	frame;
						b = birdGetFrame( 0, &frame);
						goto goon;
						}
					else
						err_print( "PCBird can't FrameReady");
					birdStopFrameStream(0);
					}
				else
					err_print( "PCBird can't StartFrameStream");
				}
			else
				err_print( "PCBird can't config device");
			}
		else
			err_print( "PCBird can't config system");
		birdShutDown( 0);
		}
	else
		err_print( "PCBird can't wake up");
goon:
	if( b)
		{
		GOOD_PRINT_STRING( "PCBird Tracker Open");
		return AAA_OK;
		}
	else
		return ERR_ANY;
*/
	return AAA_OK;
}

AAA_ERR	c_polhemus::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		AAA_ERR ret = AAA_OK;
		for( INT32 i=0; i<2; ++i )
		{
			make_name( filename, filename_in, i );
			if( ERR( _sensors[i].load_from_file_add_ext(filename) ) )
				ret = ERR_ANY;
		}
	o_str::pop_name();
	return ret;
}

AAA_ERR	c_polhemus::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		AAA_ERR ret = AAA_OK;
		for( INT32 i=0; i<2; ++i )
		{
			make_name( filename, filename_in, i );
			if( ERR( _sensors[i].save_to_file_add_ext(filename) ) )
				ret = ERR_ANY;
		}
	o_str::pop_name();
	return ret;
}

#endif	//AAA_TRACKER_POLHEMUS