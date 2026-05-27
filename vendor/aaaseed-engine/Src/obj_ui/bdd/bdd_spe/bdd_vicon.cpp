#include "bdd_vicon.h"

#if AAA_LIB_USE_VICON()
#	include <lib_use.h>
	AAA_LIB_USE32( "Tracker/Vicon/ViconDataStreamSDK_CPP" )
	using namespace ViconDataStreamSDK::CPP;
#endif

FACTORY_CREATE_PROP_V1( c_bdd_vicon, bdd_vicon, Vicon, bdd_vicon, sub_menu = "Special"; );

namespace n_vicon
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 8;
	CONSTEXPR INT32 INFO_PARAM_NB		= 4;
	CONSTEXPR INT32 FILTER_PARAM_NB		= 5;
	CONSTEXPR INT32 ROT_PARAM_NB		= 19;
	CONSTEXPR INT32 SUBJECT_PARAM_NB	= 34;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 1 + c_bdd_vicon::SUBJECT_NB;
	CONSTEXPR INT32 PARAM_NB	= BASE_PARAM_NB
								+ INFO_PARAM_NB
								+ SUBJECT_PARAM_NB * c_bdd_vicon::SUBJECT_NB
								+ GROUP_PARAM_NB;


#define	PARAM_DEF_SUBJECT( nb )\
	PARAM_DEF_GROUP_CLOSED( Subject_##nb##, SUBJECT_PARAM_NB )			\
		PARAM_DEF_STR_LOCKED(	subject_##nb##_name				)		\
		PARAM_DEF_STR_LOCKED(	subject_##nb##_root_name		)		\
		PARAM_DEF_INT32_LOCKED(	subject_##nb##_segment_nb		)		\
		PARAM_DEF_STR_LOCKED(	subject_##nb##_segment_name		)		\
		PARAM_DEF_INT32_LOCKED(	subject_##nb##_marker_nb		)		\
		PARAM_DEF_POINT_XYZ(	subject_##nb##_tra				)		\
		PARAM_DEF_SCALE_XYZF(	subject_##nb##_sca				)		\
		PARAM_DEF_XYZ_LOCKED(	subject_##nb##_pos				)		\
		PARAM_DEF_BOOL_LOCKED(	subject_##nb##_pos_occluded		)		\
		PARAM_DEF_GROUP_CLOSED( Rot, ROT_PARAM_NB )						\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_01 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_02 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_03 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_04 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_05 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_06 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_07 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_08 )				\
			PARAM_DEF_REAL_LOCKED(	subject_##nb##_rot_09 )				\
			PARAM_DEF_XYZW_LOCKED(	subject_##nb##_rot_quat )			\
			PARAM_DEF_XYZ_LOCKED(	subject_##nb##_rot_euler_xyz	)	\
			PARAM_DEF_BOOL_LOCKED(	subject_##nb##_rot_occluded )

	CONST c_param_def param[ PARAM_NB ] =
	{
		PARAM_DEF_BOOL_OFF(		active			)
		PARAM_DEF_BOOL_OFF(		verbose			)

		PARAM_DEF_REF(			hostname		)
		PARAM_DEF_BOOL_OFF(		open_trig		)
		PARAM_DEF_BOOL_OFF(		stop_trig		)
		PARAM_DEF_BOOL_LOCKED(	opened			)
		PARAM_DEF_INT32_LOCKED(	subject_count	)

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	version			)
			PARAM_DEF_INT32_LOCKED(	frame_count		)
			PARAM_DEF_REAL_LOCKED(	frame_rate		)
			PARAM_DEF_REAL_LOCKED(	latency			)

		PARAM_DEF_BOOL_ON( translation_out_in_meter )

		PARAM_DEF_SUBJECT( 01 )
		PARAM_DEF_SUBJECT( 02 )
		PARAM_DEF_SUBJECT( 03 )
		PARAM_DEF_SUBJECT( 04 )
	};
}

void	c_bdd_vicon::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()	);
	param_set_pt( h, _b_verbose			);

	param_set_pt( h, _hostname_ui		);

	param_set_pt( h, _b_open_trig_ui	);
	param_set_pt( h, _b_close_trig_ui	);
	param_set_pt( h, _b_opened			);
	param_set_pt( h, _subject_count		);

	++h;
	param_set_pt( h, _version			);
	param_set_pt( h, _frame_count		);
	param_set_pt( h, _frame_rate		);
	param_set_pt( h, _latency			);

	param_set_pt( h, _b_tra_in_meter	);
	for ( size_t i = 0; i < SUBJECT_NB; ++i )
	{
		++h;
		c_vicon_subject& subject = _subject[ i ];
		param_set_pt( h, subject.name			);
		param_set_pt( h, subject.root_name		);
		param_set_pt( h, subject.segment_nb		);
		param_set_pt( h, subject.segment_name	);
		param_set_pt( h, subject.marker_nb		);

		param_set_pt_3( h, _tra_ui[ i ]		);
		param_set_pt_4( h, _sca_ui[ i ]		);

		param_set_pt_3( h, subject.pos			);
		param_set_pt( h, subject.b_pos_occluded );
		++h,
			param_set_pt_n( h, subject.rot, 9	);
		param_set_pt_4( h, subject.rot_quat	);
		param_set_pt_3( h, subject.rot_euler	);
		param_set_pt( h, subject.b_rot_occluded );
	}
	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_vicon )
	,_b_opened			(	false	)
	,_subject_count		(	0		)
{
	for ( size_t i = 0; i < SUBJECT_NB; ++i )
	{
		c_vicon_subject& subject = _subject[ i ];
		clear_v3(	subject.pos			);
		clear_vr(	subject.rot, 9		);
		clear_v4(	subject.rot_quat	);
		clear_v3(	subject.rot_euler	);
		subject.segment_nb		= 0;
		subject.marker_nb		= 0;
		subject.b_pos_occluded	= true;
		subject.b_pos_occluded	= true;
	}
	param_init_with( n_vicon::param, n_vicon::PARAM_NB );
}

c_bdd_vicon::~c_bdd_vicon()
{
	close();
}

AAA_ERR	c_bdd_vicon::open()
{
	if( _b_opened )
		return AAA_OK;
#if AAA_LIB_USE_VICON()
	if( _client.Connect( _hostname_ui.get() ).Result == ViconDataStreamSDK::CPP::Result::Success )
	{
		_b_opened = true;
		GOOD_PRINT_STRING( "Connected to Vicon server %d", _hostname_ui.get() );

		// Discover the version number
		ViconDataStreamSDK::CPP::Output_GetVersion version = _client.GetVersion();
		//_version.set( version.Major );
		//_version.add_space();
		//_version.add( version.Minor );
		//_version.add_space();
		//_version.add( version.Point );

		_version = version.Major;
		_version += '.';
		_version += version.Minor;
		_version += '.';
		_version += version.Point;

		_client.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPullPreFetch );

		// Enable some different data types
		_client.EnableSegmentData();

		GOOD_PRINT_STRING( "Segment Data Enabled : %s", _client.IsSegmentDataEnabled().Enabled ? "ON" : "OFF" );

		// Set the global up axis
		_client.SetAxisMapping( ViconDataStreamSDK::CPP::Direction::Forward, ViconDataStreamSDK::CPP::Direction::Left, ViconDataStreamSDK::CPP::Direction::Up ); // Z-up
	}
	else
#endif
	{
		if ( _b_verbose )
			ERR_PRINT_STRING( "Could not connect to Vicon server %s", _hostname_ui.get() );
	}
	return _b_opened ? AAA_OK : ERR_ANY;
}

void	c_bdd_vicon::close()
{
	if ( _b_opened )
	{
#if AAA_LIB_USE_VICON()
		_client.DisableSegmentData();
		_client.Disconnect();
#endif
		_b_opened = false;
	}
}

void	c_bdd_vicon::update()
{
	if ( !is_active() )
		return;

	if ( _b_open_trig_ui )
	{
		_b_open_trig_ui = false;
		if( !_b_opened )
			open();
	}
	if ( _b_close_trig_ui )
	{
		_b_close_trig_ui = false;
		if( _b_opened )
			close();
	}

	if ( !_b_opened )
		return;

	for ( auto i = 0; i < SUBJECT_NB; ++i )
	{
		REAL factor = _sca_ui[ i ][ 3 ];
		if ( _b_tra_in_meter )
			factor *= REAL(.001);
		scale_v3( _sca[ i ], _sca_ui[ i ], factor );
	}
#if AAA_LIB_USE_VICON()
	if ( _client.GetFrame().Result == ViconDataStreamSDK::CPP::Result::Success )
	{
		// Get the frame number
		Output_GetFrameNumber _Output_GetFrameNumber = _client.GetFrameNumber();
		_frame_count = _Output_GetFrameNumber.FrameNumber;

		Output_GetFrameRate Rate = _client.GetFrameRate();
		_frame_rate = Rate.FrameRateHz;

		//// Get the timecode
		//Output_GetTimecode _Output_GetTimecode  = _client.GetTimecode();
		//output_stream << "Timecode: "
		//			<< _Output_GetTimecode.Hours               << "h "
		//			<< _Output_GetTimecode.Minutes             << "m " 
		//			<< _Output_GetTimecode.Seconds             << "s "
		//			<< _Output_GetTimecode.Frames              << "f "
		//			<< _Output_GetTimecode.SubFrame            << "sf "
		//			<< Adapt( _Output_GetTimecode.FieldFlag ) << " " 
		//			<< _Output_GetTimecode.Standard            << " " 
		//			<< _Output_GetTimecode.SubFramesPerFrame   << " " 
		//			<< _Output_GetTimecode.UserBits            << std::endl << std::endl;

		// Get the latency
		_latency = _client.GetLatencyTotal().Total;


		// Count the number of subjects
		_subject_count = _client.GetSubjectCount().SubjectCount;
		UINT32	subject_count = MIN( _subject_count, SUBJECT_NB );

		for( UINT32 i = 0; i < subject_count; ++i )
		{
			c_vicon_subject&	subject = _subject[ i ];
			// Get the subject name
			std::string		name = _client.GetSubjectName( i ).SubjectName;
			subject.name.set( name.c_str() );

			// Get the root segment
			std::string root_name = _client.GetSubjectRootSegmentName( name ).SegmentName;
			subject.root_name.set( root_name.c_str() );

			// Count the number of segments
			subject.segment_nb = _client.GetSegmentCount( name ).SegmentCount;

			// Get the segment name
			std::string segment_name = _client.GetSegmentName( name, 0 ).SegmentName;
			subject.segment_name.set( segment_name.c_str() );

			// Get the global segment translation
			Output_GetSegmentGlobalTranslation _Output_GetSegmentGlobalTranslation = _client.GetSegmentGlobalTranslation( name, segment_name );

			for ( auto j = 0; j < 3; ++j )
			{
				subject.pos[ j ] = _Output_GetSegmentGlobalTranslation.Translation[ j ];
			}
			add_then_mul_v3r( subject.pos, _tra_ui[ i ], _sca[ i ] );
			subject.b_pos_occluded = _Output_GetSegmentGlobalTranslation.Occluded;

			// Get the global segment rotation as a matrix
			Output_GetSegmentGlobalRotationMatrix _Output_GetSegmentGlobalRotationMatrix = _client.GetSegmentGlobalRotationMatrix( name, segment_name );
			for ( auto j = 0; j < 9; ++j )
			{
				subject.rot[ j ] = _Output_GetSegmentGlobalRotationMatrix.Rotation[ j ];
			}
			subject.b_rot_occluded = _Output_GetSegmentGlobalRotationMatrix.Occluded;

			// Get the global segment rotation in quaternion co-ordinates
			Output_GetSegmentGlobalRotationQuaternion _Output_GetSegmentGlobalRotationQuaternion = _client.GetSegmentGlobalRotationQuaternion( name, segment_name );
			for ( auto j = 0; j < 4; ++j )
			{
				subject.rot_quat[ j ] = _Output_GetSegmentGlobalRotationMatrix.Rotation[ j ];
			}

			// Get the global segment rotation in EulerXYZ co-ordinates
			Output_GetSegmentGlobalRotationEulerXYZ _Output_GetSegmentGlobalRotationEulerXYZ = _client.GetSegmentGlobalRotationEulerXYZ( name, segment_name );
			for ( auto j = 0; j < 3; ++j )
			{
				subject.rot_euler[ j ] = _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ j ] / M_PI;
			}

			// Count the number of markers
			subject.marker_nb = _client.GetMarkerCount( name ).MarkerCount;
		}
	}
	else
#endif
	{
		if ( _b_verbose )
			ERR_PRINT_STRING( "Error getting frame from Vicon server" );
	}
}
