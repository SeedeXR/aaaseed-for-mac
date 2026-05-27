
#ifdef AAA_MEDIAPIPE_H
#error "MEDIAPIPE_H included more than once."
#endif
#define AAA_MEDIAPIPE_H 1


#ifndef	AAA_TRACKER_H
#	include "obj_ui/tracker/tracker.h"
#endif
#include "MediapipeHolisticTrackingDll.h"

//toto should be a singleton with members instead of static
class	c_mediapipe final : public c_tracker
{
	FACTORY_DECLARE( c_mediapipe, c_tracker );
public:
	static bool							b_start_with;
	static bool							b_dll_loaded;
	static bool							b_verbose;
	static bool							b_holistic_model_loaded;

	static MediapipeHolisticTrackingDll	dll;

private:
public:
private:
//	REAL*	get_data_for_all_node( Frame* pt_frame, REAL* data, INT32 real_by_node );
public:

	static	void	c_init();
	static	void	c_deinit();
		
	virtual	AAA_ERR	open();
	virtual void	close();
	virtual void	update_low();

			void	init();
	virtual	void	param_init_pt();

};

extern	c_mediapipe*	g_mediapipe;


