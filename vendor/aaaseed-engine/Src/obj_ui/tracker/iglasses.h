
#ifdef AAA_IGLASSES_H
#error "IGLASSES_H included more than once."
#endif
#define AAA_IGLASSES_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif
#ifndef AAA_HMD_H
#	include "hmd.h"
#endif

#if AAA_TRACKER_IGLASSES()
extern	AAA_ERR	iglasses_print_version();
extern	AAA_ERR	iglasses_init( int ttynum );
extern	void	iglasses_close();
extern	AAA_ERR	iglasses_reset( INT32 mf = 3, INT32 tf = 3, DATA_MODE dm = EULER, SEND_MODE sm = POLLED, SEND_FORMAT sf = BINARY );
extern	void	iglasses_ask_euler();
extern	AAA_ERR iglasses_get_euler( c_euler &p );
extern	AAA_ERR	iglasses_start( HMD_TYPE hmd_type = HMD_VIO );
extern	AAA_ERR	iglasses_stop();
extern	void	iglasses_flip_verbose();
#endif	//AAA_TRACKER_IGLASSES

extern	void	polhemus_boresight();


