
#ifdef AAA_AAAVR_H
#error "AAAVR_H included more than once."
#endif
#define AAA_AAAVR_H 1


#ifndef	AAA_TRACKER_H
#	include "obj_ui/tracker/tracker.h"
#endif

class c_sensor_6dof;

namespace vr {
	class IVRSystem;
	class IVRCompositor;
}

class	c_aaavr final : public c_tracker
{
	FACTORY_DECLARE( c_aaavr, c_tracker );
private:
	static	vr::IVRSystem*					g_system;

			bool							_b_vr_init;
			std::vector< c_sensor_6dof* >	sensors;
			bool							_b_submit;
			bool							_b_submit_clear_trig;

			INT32							_submit_bind_left;
			INT32							_submit_bind_right;
protected:
			vr::IVRCompositor*				_compositor				{nullptr};

	void handle_input();
	void get_matrices();
	void get_controller_info();

public:

	static	bool		b_start_with;
	static	bool		b_verbose;
	static	c_aaavr*	cur;

	static	void	c_init();
	static	void	c_deinit();

	virtual	AAA_ERR	open();
	virtual	void	close();

	virtual	void	update_low();
			void	update_post_swap();

			void	submit_tex_to_hmd(	bool b_right, INT32 bind=-1 );
			void	submit_texs_to_hmd( INT32 bind_left=-1, INT32 bind_right=-1 );
			void	clear_submit();

	virtual	void	param_init_pt();


	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

//	virtual	void	calibrate_default() {};
};




