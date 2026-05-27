#include "capture_censys_tracking.h"

#if AAA_USE_POINT_GREY()

#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "strnum.h"

C_PCHAR_C	tracked_filename = "tracking.censys";
CHAR	used_filename[256];

c_censys_tracking::c_censys_tracking()
{
	_nb_total_tracked_people = 0;
	_f_save = nullptr;
	_b_dot_save = false;

	//// clear when beginning
	//bdd_point_cur->clear();

	_dot_offset[0] = _dot_offset[1] = _dot_offset[2] = 0.0f;
	_dot_scale[0] = _dot_scale[1] = _dot_scale[2] = 1.0f;
	memset( &_time_stamp, 0, sizeof( _time_stamp ) );
}

c_censys_tracking::~c_censys_tracking()
{
	if( _f_save )
		close_file();
}

bool	c_censys_tracking::set_censys_tracking( CONST TRACKER_DATA &tracker_data )
{
	if( bdd_point_cur && _b_dot_send )
		bdd_point_cur->lock();

	// Set size & person array pointer
	_b_timestamp_save = false;
	_time_stamp = tracker_data.timestamp;
	_nb_total_tracked_people = tracker_data.tracked_people.size;

	if( _nb_total_tracked_people > 0 && _b_dot_send_value_verbose )
		VERBOSE_PRINT_STRING("Total tracked people %d", tracker_data.tracked_people.size);

	// Remove people no longer being tracked
	for( INT32 i = 0; i < tracker_data.tracked_people_removed.size; ++i )
	{
		INT32 person_id = tracker_data.tracked_people_removed.person_array[i].nID;

		if( _b_dot_send_verbose )
			VERBOSE_PRINT_STRING( "dot_delete( %d )", person_id );

		if( bdd_point_cur && _b_dot_send )
			bdd_point_cur->delete_dot( person_id );

		if( _b_dot_save )
			save_dot_delete( tracker_data.timestamp, person_id );
	}

	for( INT32 i = 0; i < tracker_data.tracked_people.size; ++i )
	{
		INT32 person_id = tracker_data.tracked_people.person_array[i].nID;

		// compute person speed
		REAL	speed;
//		REAL	time;
//		REAL	x_coord, y_coord;
//		INT32	k;
		COLOUR_POINT3D		point;
//		COLOUR_POINT3D		point2;

		point = tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - 1 ].point3D;

/*		if( tracker_data.tracked_people.person_array[i].tracked_path.size > 2)
			{
			switch( dot_speed )
				{
				case 4 :
					if( tracker_data.tracked_people.person_array[i].tracked_path.size > 4)
						k = 4;
					else
						k = tracker_data.tracked_people.person_array[i].tracked_path.size - 1;
					break;
				case 3 :
					if( tracker_data.tracked_people.person_array[i].tracked_path.size > 3)
						k = 3;
					else
						k = tracker_data.tracked_people.person_array[i].tracked_path.size - 1;
					break;
				case 2 :
				default :
					k = 2;
				}
			point2 = tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - k ].point3D;

			x_coord = point.x - point2.x;
			y_coord = point.y - point2.y;
			speed = sqrt( x_coord * x_coord + y_coord * y_coord);
			time = (tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - 1 ].timestamp.time - tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - k ].timestamp.time);
			time += (tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - 1 ].timestamp.millitm - tracker_data.tracked_people.person_array[i].tracked_path.pathpoint_array[ tracker_data.tracked_people.person_array[i].tracked_path.size - k ].timestamp.millitm)/1000.0;
			speed = 0.0f;
			}
		else
*/
		speed = 0.0f;

		if( _b_dot_send_value_verbose )
		{
			VERBOSE_PRINT_STRING( "Person ID : %d, Position X : %.3f, Position Y : %.3f, Position Z : %.3f, Speed : %.3f",
										person_id, ( point.x * _dot_scale[0] ) + _dot_offset[0], ( point.y * _dot_scale[1] ) + _dot_offset[1], ( point.z * _dot_scale[2] ) + _dot_offset[2], speed );
		}

		if( bdd_point_cur && _b_dot_send )
			bdd_point_cur->set_dot_pos(	person_id, ( point.x * _dot_scale[0] ) + _dot_offset[0], ( point.y * _dot_scale[1] ) + _dot_offset[1], ( point.z * _dot_scale[2] ) + _dot_offset[2], speed );

		if( _b_dot_save )
			save_dot_pos( tracker_data.timestamp,
										person_id, ( point.x * _dot_scale[0] ) + _dot_offset[0], ( point.y * _dot_scale[1] ) + _dot_offset[1], ( point.z * _dot_scale[2] ) + _dot_offset[2], speed );
	}

	if( bdd_point_cur && _b_dot_send )
		bdd_point_cur->unlock();

//	unlock_tracked_people();
	save_file();
	return true;
}

void	c_censys_tracking::set_dot_save( bool p_in )
{
	_b_dot_save = p_in;

	if( _b_dot_save )
		open_file();
	else
		close_file();
}

void	c_censys_tracking::open_file()
{
	bool	b_find = false;
	CHAR	buf[256];
	INT32	nb = 0;

	if( _f_save )
		return;

	// first find filename
	do
	{
		sprintf( buf, "%s.%03d", tracked_filename, nb);
		_f_save = c_file::FOPEN( buf, "rt");
		if( _f_save )
		{
			c_file::FCLOSE( _f_save );
			++nb;
		}
		else
			b_find = true;
	}
	while ( !b_find );

	// now open file for writing
	_f_save = c_file::FOPEN( buf, "wt");
	strcpy( used_filename, buf );

	// reset time
	memset( &_time_stamp_save, 0, sizeof( _time_stamp_save ) );

}

void	c_censys_tracking::close_file()
{
	if( _f_save )
	{
		c_file::FCLOSE( _f_save );
		_f_save = nullptr;
	}
}

void	c_censys_tracking::save_file()
{
	if( _f_save )
	{
		c_file::FCLOSE( _f_save );
		_f_save = c_file::FOPEN( used_filename, "a+" );
	}
}

void	c_censys_tracking::save_dot_timestamp( CENSYS_TIMESTAMP timestamp )
{
	REAL	time;

	if( _f_save && !_b_timestamp_save )
	{
		if( _time_stamp_save.time == 0 )
		{
			_time_stamp_save.time = timestamp.time;
			_time_stamp_save.millitm = timestamp.millitm;
		time = 0.0f;
			}
		else
		{
			// calculate time since we begun to save
			time = ( timestamp.time - _time_stamp_save.time ) + ( timestamp.millitm - _time_stamp_save.millitm ) / 1000.0f;
		}
	
		fprintf( _f_save, "TIME_STAMP %f\n", time );
		_b_timestamp_save = true;
	}

}

void	c_censys_tracking::save_dot_new( CENSYS_TIMESTAMP timestamp, INT32 person_id )
{
	if( _f_save )
	{
		save_dot_timestamp( timestamp );
		fprintf( _f_save, "DOT_ON %d\n", person_id );
	}
}

void	c_censys_tracking::save_dot_new( CENSYS_TIMESTAMP timestamp, INT32 person_id, REAL pos_x, REAL pos_y )
{
	if( _f_save )
	{
		save_dot_timestamp( timestamp );
		fprintf( _f_save, "DOT_ON %d %f %f\n", person_id, pos_x, pos_y );
	}
}

void	c_censys_tracking::save_dot_delete( CENSYS_TIMESTAMP timestamp, INT32 person_id )
{
	if( _f_save )
	{
		save_dot_timestamp( timestamp );
		fprintf( _f_save, "DOT_OFF %d\n", person_id );
	}
}

void	c_censys_tracking::save_dot_pos( CENSYS_TIMESTAMP timestamp, INT32 person_id, REAL pos_x, REAL pos_y, REAL pos_z, REAL speed )
{
	if( _f_save )
	{
		save_dot_timestamp( timestamp );
		fprintf( _f_save, "DOT %d %f %f %f\n", person_id, pos_x, pos_y, speed );
	}
}

#endif //#if AAA_USE_POINT_GREY()
