
#ifdef AAA_BDD_VICON_H
#error "BDD_VICON_H included more than once."
#endif
#define AAA_BDD_VICON_H 1


//todo update and deal with x64 version
#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#if AAA_OS_WINDOWS() && !AAA_WIN64()
#	define AAA_LIB_USE_VICON() 1
#else
#	define AAA_LIB_USE_VICON() 0
#endif

#if AAA_LIB_USE_VICON()
#	include "Tracker/Vicon/Client.h"
#endif

class	c_vicon_subject
{
public:
	REAL	rot_quat[ 4 ];
	REAL	pos[ 3 ];
	REAL	rot_euler[ 3 ];
	REAL	rot[ 9 ];
	UINT32	segment_nb;
	UINT32	marker_nb;
	bool	b_pos_occluded;
	bool	b_rot_occluded;
	o_str	name;
	o_str	root_name;
	o_str	segment_name;
};

class	c_bdd_vicon final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_vicon, c_bdd );
public:
	static UINT32 CONST		SUBJECT_NB = 4;
private:
	bool	_b_opened;
	bool	_b_open_trig_ui;
	bool	_b_close_trig_ui;
	bool	_b_verbose;
#if AAA_LIB_USE_VICON()
	ViconDataStreamSDK::CPP::Client	_client;
#endif
	o_str	_hostname_ui;
	o_str	_version;

	UINT32	_frame_count;
	REAL	_frame_rate;
	REAL	_latency;
	UINT32	_subject_count;

	bool	_b_tra_in_meter;
	REAL	_tra_ui[ SUBJECT_NB ][ 3 ];
	REAL	_sca[ SUBJECT_NB ][ 3 ];
	REAL	_sca_ui[ SUBJECT_NB ][ 4 ];

	c_vicon_subject	_subject[ SUBJECT_NB ];

			void	init();
			AAA_ERR	open();
			void	close();
public:
	virtual	void	param_init_pt();


	virtual	void	update();
	//virtual	void	draw();


};

