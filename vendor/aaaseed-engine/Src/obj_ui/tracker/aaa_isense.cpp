#include "aaa_isense.h"

#if AAA_TRACKER_ISENSE()

#include "infrastructure/param/param_declare.h"
#include "wrap_isense.h"

/*****************************************************************************
*
*   functionName:   showTrackerStats
*   Description:    
*   Created:        12/7/98
*   Author:         Yury Altshuler
*
*   Comments:       Get and display tracker information
*
******************************************************************************/
/*#include "aaa_mem.h"
void showTrackerStats( ISD_TRACKER_HANDLE handle )
{
	ISD_TRACKER_TYPE Tracker;
	ISD_STATION_CONFIG_TYPE Station;
	WORD i, numStations = 4;
	char buf[20];

	C_PCHAR_C	systemType[3] = {"Unknown", "IS Precision Series", "InterTrax Series"};
	C_PCHAR_C	modelName[5]  = {"Unknown", "IS-300 Series", "IS-600 Series", "IS-900 Series", "InterTrax 30"};


	if( ISLIB_GetTrackerState( handle, &Tracker, TRUE ) )
	{
		printf("\n********** InterSense Tracker Information ***********\n\n");

		printf("Type:     %s device on port %d\n", systemType[Tracker.TrackerType], Tracker.CommPort);
		printf("Model:    %s\n", modelName[Tracker.TrackerModel]);

		if(Tracker.TrackerType == ISD_PRECISION_SERIES)
		{
			if( Tracker.TrackerModel == ISD_IS600 || Tracker.TrackerModel == ISD_IS900)
			{
				numStations = ISD_MAX_STATIONS;
			}
			printf("\nStation\tTime\tState\tCube  Enhancement  Sensitivity  Prediction\n");

			for(i = 1; i <= numStations; ++i )
			{
				printf("%d\t", i);

				if(ISLIB_GetStationState( handle, &Station, i, FALSE ))
				{
					printf("%s\t%s\t%s\t   %u\t\t%u\t   %u\n", 
						Station.TimeStamped ? "ON" : "OFF", 
						Station.State ? "ON" : "OFF", 
						Station.InertiaCube == -1 ? "None" : itoa(Station.InertiaCube, buf, 10), 
						Station.Enhancement, 
						Station.Sensitivity, 
						Station.Prediction);
				}
				else
				{
					printf("ISLIB_GetStationState failed\n");
					break;
				}
			}
			printf("\n");
		}
	}
	else
	{
		printf("ISLIB_GetTrackerState failed\n");
	}
}
*/

bool	c_isense::b_dll_loaded = false;

FACTORY_CREATE_V1( c_isense, isense, InterSense, isense );

c_isense*		isense = nullptr;

namespace	n_isense
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_REAL_ZERO( controller_1 )
	};
}

CONSTRUCTOR_CREATE(c_isense)
{

	data_alloc();
	_data->Station[0].Orientation[0] = 0.;
	_data->Station[0].Orientation[1] = 0.;
	_data->Station[0].Orientation[2] = 0.;
	param_init_with( n_isense::param, n_isense::PARAM_NB_MAX ); // isense_param, ISENSE_PARAM_NB);
}

void	c_isense::data_alloc()
{
	_data = (ISD_DATA_TYPE *) MALLOC( sizeof(ISD_DATA_TYPE) + 0x100 );	//hack + 0x100 protect from bug
}

void	c_isense::data_dealloc()
{
	FREE_AND_NULL( _data );
}

void	c_isense::set_port( INT32 port_id )
{
	_com_port_nb = port_id;
}

void	c_isense::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_attach_obj( h, (c_obj_ui*) &_sensor );

	err_param_init_pt(h);
}

c_isense::~c_isense()
{
	stop();
	data_dealloc();
}

bool	get_tracker_data( ISD_TRACKER_HANDLE handle, ISD_DATA_TYPE *pt_data )
{
	if( c_isense::b_dll_loaded )
		return	bool(dll_isense.ISLIB_GetTrackerData( handle, pt_data ));

	return false;
}


void	c_isense::update_low()
{
//	if( isense->is_active() )
	if( b_dll_loaded )
	{
		dll_isense.ISLIB_GetTrackerData( _handle, _data );
	// 		ISD_TRACKER_TYPE         tracker;
	//		get_tracker_data(_handle, &_data );
	//		ISLIB_GetTrackerData( _handle, &_data );
	/*			printf("%7.3f %7.3f %7.3f %7.2f %7.2f %7.2f\n", 
				_data.Station[0].Position[0], 
				_data.Station[0].Position[1], 
				_data.Station[0].Position[2], 
				_data.Station[0].Orientation[0], 
				_data.Station[0].Orientation[1], 
				_data.Station[0].Orientation[2]);
	*/
		CONST ISD_STATION_DATA_TYPE& station = _data->Station[0];
		_sensor.set_tra_in( station.Position );
		_sensor.set_rot_in( station.Orientation[0]/360., -station.Orientation[1]/360., station.Orientation[2]/360. );
	}

	_sensor.update_sensor_6dof();

	/* Get comm port statistics for display with tracker _data */
	/*
	if(ISLIB_GetCommState( _handle, &tracker ))
		{
		printf("%5.2fKbps %d Records/s %7.3f %7.3f %7.3f %7.2f %7.2f %7.2f\n", 
			tracker.KBitsPerSec, tracker.RecordsPerSec, 
			_data.Station[0].Position[0], 
			_data.Station[0].Position[1], 
			_data.Station[0].Position[2], 
			_data.Station[0].Orientation[0], 
			_data.Station[0].Orientation[1], 
			_data.Station[0].Orientation[2]);
		}
	*/
}

void	c_isense::close()
{
	if( b_dll_loaded && _handle )
	{
		dll_isense.ISLIB_CloseTracker( _handle );
		_handle = 0;
	}
	set_active( false );
}

AAA_ERR	c_isense::open()
{
	/* Detect first tracker. If you have more than one InterSense device and would like to have
	a specific tracker, connected to a known port, initialized first, then enter the port 
	number instead of 0. Otherwise, tracker connected to the port with lower number is
	found first */
	_handle = 0;
	if( b_dll_loaded )
		_handle = dll_isense.ISLIB_OpenTracker( nullptr, _com_port_nb, FALSE, _b_verbose_ui ? TRUE : FALSE );
	if(( _handle != -1 )&&( _handle!=0 ))   // sr@20100421 - looks like a weakeness... "0"or "-1" ?
	{
		GOOD_PRINT_STRING( "ISense Tracker Open" );
		return AAA_OK;
	}
	else
	{
		_handle = 0;
		return ERR_ANY;
	}
}

void	isense_load_lib()
{
	c_isense::b_dll_loaded = dll_isense.init();
	//if( wrap_isense_Init() == 0 )
	//{
	//	c_isense::b_dll_loaded = true;
	//}
}
void	isense_unload_lib()
{
	//wrap_isense_Term();
	if( c_isense::b_dll_loaded )
	{
		dll_isense.deinit();
	}
}

#endif	//#if AAA_TRACKER_ISENSE()
