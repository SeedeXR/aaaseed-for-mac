
#ifdef AAA_BDD_LUMIO_H
#error "BDD_LUMIO_H included more than once."
#endif
#define AAA_BDD_LUMIO_H 1

#ifndef AAA_AAA_TYPE_H
	#include "aaa_type.h"
#endif
#ifdef WIN64
#	define AAA_USE_LUMIO() 0
#else
#	define AAA_USE_LUMIO() 1
#endif

#if AAA_USE_LUMIO()
extern "C"
{
	// This file is in the Windows DDK available from Microsoft.
	#include "hidsdi.h"
	#include <setupapi.h>
}
#endif

#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_BLOB_H
#	include "obj_ui/bdd/bdd_multitouch/blob.h"
#endif

class	c_thread_lumio;

class	c_bdd_lumio final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_lumio, c_bdd );
protected:
	static	CONST	INT32	MAX_INPUT_LENGHT = 12;
	static	CONST	INT32	MAX_LUMIO_TOUCH = 2;
	bool				_b_open_ui;
	bool				_b_open;
	bool				_b_verbose;
	bool				_b_close_thread;	//to should be atomic

	bool				_b_hid_ok;
	bool				_b_set_mouse_mode;

#if AAA_USE_LUMIO()
	HANDLE				_thread_handle;

	HIDP_CAPS			_hid_caps;
	HANDLE				_device_handle;
	HANDLE				_h_dev_info;
	GUID				_hid_guid;
	UCHAR				_feature_report[ MAX_INPUT_LENGHT ];
	UCHAR				_input_report[ MAX_INPUT_LENGHT ];
	UCHAR				_input_report2[ MAX_INPUT_LENGHT ];
	UCHAR				_output_report[ MAX_INPUT_LENGHT ];
	HANDLE				_io_handle;

	mutable aaa::MUTEX*	_p_lock_hid;
	c_thread_lumio*		_p_thread_lumio;
#endif	//#if AAA_USE_LUMIO()

	bool				_b_device_detected; 
	o_str				_device_pathname;

	o_str				_s_vendor_id;
	o_str				_s_device_id;

	c_blob				_blobs[ MAX_LUMIO_TOUCH ];
	bool				_b_blobs[ MAX_LUMIO_TOUCH ];

	o_str				_target_name_symbo;
	c_bdd*				_bdd_target;
	REAL				_transfer_translate[ 3 ];
	REAL				_transfer_scale_ui[ 4 ];
	REAL				_transfer_blob_radius;



	bool				_b_first;
	bool				_b_process;


//			void	register_for_device_notifications();

#if AAA_USE_LUMIO()
			void	GetDeviceCapabilities();
			bool	find_device( INT32 vendor_id, INT32 device_id );
			void	open_hid();
			void	close_handles();
			void	display_last_error( C_PCHAR msg );
			void	write_feature_report();
			void	write_output_report();
			void	read_input_report( INT32 interval, UCHAR* input_report );
			void	analyse_input_report();
			void	clear_feature_report();
			void	clear_output_report();
			void	clear_input_report();
			void	start_loop_hid_read();
			void	set_mouse_mode();
			void	set_driver_mode();
#endif	//#if AAA_USE_LUMIO()


public:
#if AAA_USE_LUMIO()
			HANDLE	get_thread_handle()	{ return _thread_handle; }
			void	loop_hid_read();
			void	read_hid();
	virtual	void	update_async();
#endif	//#if AAA_USE_LUMIO()
			void	init();
	virtual	void	param_init_pt();

#if AAA_USE_LUMIO()
			void	open();
			void	close();
#endif	//#if AAA_USE_LUMIO()

	virtual	void	update();
	virtual	void	draw();
	virtual	void	transfer_blobs_to( BLOBS_CONT& blobs );
};

