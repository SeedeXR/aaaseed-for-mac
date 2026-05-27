
#ifdef AAA_BDD_PQLABS_H
#error "BDD_PQLABS_H included more than once."
#endif
#define AAA_BDD_PQLABS_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef PQMT_CLIENT_H_
//maa was #if !defined(PQMT_CLIENT_H_) && !AAA_WIN64()
#	include "tracker/PQLabs/PQMTClient.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

using namespace PQ_SDK_MultiTouch;

class	c_bdd_pqlabs final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_pqlabs, c_bdd );
private:
	bool				_b_opened;
	bool				_b_close_trig;
	bool				_b_open_trig;
	bool				_b_verbose;

	INT32				_screen_size_x;
	INT32				_screen_size_y;
	INT32				_server_size_x;
	INT32				_server_size_y;
	o_str				_serial;
	INT32				_touch_nb;

	o_str				_server_ip;
	INT32				_server_port;
//	INT32				_move_threshold;
	
	std::vector<c_blob>	_v_blobs;

	mutable aaa::MUTEX	_lock_blobs;

	o_str				_target_name_symbo;
	c_bdd*				_bdd_target;
	REAL				_transfer_translate[ 3 ];
	REAL				_transfer_scale_ui[ 4 ];
	REAL				_transfer_blob_radius;

//////////////////////call back functions///////////////////////
	static	void	MAACALLBACK	callback_pointframe(int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array, void * call_back_object);
	static	void	MAACALLBACK	callback_serverbreak(void * param, void * call_back_object);
	static	void	MAACALLBACK	callback_error(int err_code,void * call_back_object);
	static	void	MAACALLBACK	callback_server_resolution(int x, int y, void * call_back_object);
	static	void	MAACALLBACK	callback_device_info(const TouchDeviceInfo & device_info, void * call_back_object);
	static	void	MAACALLBACK	callback_gesture(const TouchGesture & ges, void * call_back_object);
//////////////////////call back functions end ///////////////////////
			void	set_callbacks();
public:
	virtual	void	param_init_pt();

	static	INT32 enumerate();
			void	set_server_resolution( INT32 x, INT32 y );
			void	set_info( const CHAR* serial, INT32 w, INT32 h );
			void	do_frame(int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array );
			void	init();
			void	transfer_blobs_to( BLOBS_CONT& blobs );
	AAA_ERR	open();
	void	close();

	virtual	void	update();
	virtual	void	draw();
};
