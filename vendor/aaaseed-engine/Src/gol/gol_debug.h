
#ifdef AAA_GOL_DEBUG_H
#error "GOL_DEBUG_H included more than once."
#endif
#define AAA_GOL_DEBUG_H 1


#ifndef	AAA_AAA_GL_H
#	include "draw/aaa_gl.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace GOL	{

extern	void	init_debug();
extern	void	deinit_debug();
extern	void	update_debug();

extern	bool	b_debug_callback_ui;
extern	bool	b_debug_error_ui;

extern	UINT32	error_count				;
extern	UINT32	error_count_low			;
extern	UINT32	error_count_medium		;
extern	UINT32	error_count_high		;
extern	UINT32	error_count_notification;

extern	bool	b_debug_deprecated_ui;
extern	bool	b_debug_undefined_ui;
extern	bool	b_debug_portability_ui;

extern	bool	b_debug_performance_ui;
extern	bool	b_debug_performance_pbo_nvidia_ui;
extern	bool	b_debug_performance_shader_recompiled_by_state_nvidia_ui;
extern	bool	b_debug_performance_buffer_usage_ui;

extern	bool	b_debug_other_ui;
extern	bool	b_debug_other_uniform_array_ignored_nvidia_ui;
extern	bool	b_debug_other_buffer_nvidia_ui;
extern	bool	b_debug_other_buffer_detailed_nvidia_ui;
extern	bool	b_debug_other_framebuffer_detailed_nvidia_ui;
extern	bool	b_debug_other_texture_base_level_undefined_ui;
extern	bool	b_debug_other_buffer_usage_ui;

extern	bool	b_debug_severity_low_ui;
extern	bool	b_debug_severity_medium_ui;
extern	bool	b_debug_severity_high_ui;
extern	bool	b_debug_severity_notification_ui;

}	//namespace GOL
