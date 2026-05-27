
#ifdef AAA_CENSYS_TRACKING_H
#error "CENSYS_TRACKING_H included more than once."
#endif
#define AAA_CENSYS_TRACKING_H 1



#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()

#ifndef _INC_MATH
#	include <math.h>
#endif
#ifndef __CENSYS_H__
#	include "censys.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_censys_tracking
{
private:
	// Tracked people data
#if CENSYS_VERSION >= 20000000
	CENSYS_TIMESTAMP	_time_stamp;
	CENSYS_TIMESTAMP	_time_stamp_save;
#else
	__timeb32			_time_stamp;
	__timeb32			_time_stamp_save;
#endif

	INT32		_nb_total_tracked_people;

	bool		_b_dot_send;
	bool		_b_dot_send_verbose;
	bool		_b_dot_send_value_verbose;
	bool		_b_dot_save;
	bool		_b_timestamp_save;


	INT32		_dot_speed;
	REAL		_dot_offset[3];
	REAL		_dot_scale[3];

	FILE*		_f_save;

	void	save_dot_new(		CENSYS_TIMESTAMP timestamp, INT32 person_id );
	void	save_dot_new(		CENSYS_TIMESTAMP timestamp, INT32 person_id, REAL pos_x, REAL pos_y );
	void	save_dot_delete(	CENSYS_TIMESTAMP timestamp, INT32 person_id );
	void	save_dot_pos(		CENSYS_TIMESTAMP timestamp, INT32 person_id, REAL pos_x, REAL pos_y, REAL pos_z, REAL speed );
	void	save_dot_timestamp( CENSYS_TIMESTAMP timestamp );

	void	open_file();
	void	close_file();
	void	save_file();

public:
	c_censys_tracking();
	~c_censys_tracking();

	// Data-set functions
			bool	set_censys_tracking( CONST TRACKER_DATA &tracker_data );

			void	set_dot_save(				bool p_in );
	FINLINE	void	set_dot_send(				bool p_in )		{ _b_dot_send = p_in; };
	FINLINE	void	set_dot_send_verbose(		bool p_in )		{ _b_dot_send_verbose = p_in; };
	FINLINE	void	set_dot_send_value_verbose( bool p_in )		{ _b_dot_send_value_verbose = p_in; };
	FINLINE void	set_dot_speed(				INT32 p_in )	{ _dot_speed = p_in; };

	FINLINE	void	set_dot_offset_x(			REAL p_in )		{ _dot_offset[0] = p_in; };
	FINLINE	void	set_dot_offset_y(			REAL p_in )		{ _dot_offset[1] = p_in; };
	FINLINE	void	set_dot_offset_z(			REAL p_in )		{ _dot_offset[2] = p_in; };
	FINLINE	void	set_dot_scale_x(			REAL p_in )		{ _dot_scale[0] = p_in; };
	FINLINE	void	set_dot_scale_y(			REAL p_in )		{ _dot_scale[1] = p_in; };
	FINLINE	void	set_dot_scale_z(			REAL p_in )		{ _dot_scale[2] = p_in; };
};

#endif	//#if AAA_USE_POINT_GREY()


