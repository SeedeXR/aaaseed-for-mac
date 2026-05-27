
#ifdef AAA_FBO_H
#error "FBO_H included more than once."
#endif
#define AAA_FBO_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif


class c_render_buffer;
class c_bdd_clear_screen;
class c_frame_buffer_object;

class	c_fbo final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_fbo, c_obj_active_ui );
private:
	static	c_fbo*	cur;
	static	c_fbo*	prev;

public:
	static	bool	b_allow;

			static	void	c_init();

	FINLINE	static	c_fbo*	get_cur()				{	return cur;		}
			static	void	set_cur( c_fbo* in );
	FINLINE static	UINT32	get_cur_name_gl()		{	return cur ? cur->get_id_used() : 0;	}

	FINLINE	static	c_fbo*	get_prev()				{	return prev;	}
	FINLINE	static	void	set_prev( c_fbo* in )	{	prev = in;		}

			static	void	disable();

	static CONSTEXPR INT32	COLOR_BUFFER_NB = 8;

	static	INT32			get_attachment_bind_prev( INT32 which ); //return negative if not found

private:
//	INT32					_b_obj_active_last;
	INT32					_nb_active_tex;
	INT32					_active_tex_index_max;

	INT32					_size_ui[2];	// param
	INT32					_size_asked[2];	// computed from param
	INT32					_size_out[2];		// in use

	bool					_b_bgr;
	bool					_b_depth_use;
	bool					_b_depth_as_texture;
	INT32					_depth_bind_2d_out;
	INT32					_s_depth_format;
	bool					_b_stencil_use;
	INT32					_s_stencil_format;
	bool					_b_define_size_format_ui;
	bool					_b_define_size_format;
	bool					_b_size_from_previous_ui;

	DOUBLE					_buffer_size_depth;
	DOUBLE					_buffer_size_stencil;

	INT32					_s_first_color_attachment;
	bool					_b_valid;
	bool					_b_ready_to_use;
	bool					_b_verbose_ui;
	bool					_b_attach_out;	//

	c_bdd_clear_screen*		_bdd_clear_screen	;
	c_frame_buffer_object*	_frame_buffer_object;	// The framebuffer object used for rendering to the texture(s)
	c_render_buffer*		_depth_buffer		;	// depth buffer
	c_render_buffer*		_stencil_buffer		;	// stencil buffer

	GLenum					_attachment_slot	;

	GLuint					_fbo_id_gl_name_used;
//	bool					_b_ring_buffer_active_ui;
//	INT32					_ring_buffer_nb_ui;

	INT32					_channel_nb_def_ui;
	GOL::INTERNAL_TYPE		_s_channel_type_def_ui;
	bool					_b_bgr_ui;

	bool					_b_depth_use_ui;
	bool					_b_depth_as_texture_ui;
	INT32					_depth_bind_2d_ui;
	INT32					_s_depth_format_ui;
	bool					_b_depth_on_cpu_ui;
	bool					_b_depth_on_cpu_use_pbo_ui;

	bool					_b_stencil_use_ui;
	INT32					_s_stencil_format_ui;

	struct st_attachment
	{
		bool					_b_active_ui;
		bool					_b_active;
		bool					_b_mipmap_generate_ui;
		bool					_b_mipmap_generate;

		INT32					_bind_2d_dst_ui;
		INT32					_bind_2d_dst_out;
//		INT32					_bind_2d_stride_ui;

		bool					_b_define_format_ui;
		bool					_b_on_cpu_ui;
		bool					_b_on_cpu_use_pbo_ui;

		INT32					_channel_nb_ui;
		INT32					_channel_nb;
		GOL::INTERNAL_TYPE		_channel_type_ui;
		GOL::INTERNAL_TYPE		_channel_type;
		aaa::PIXEL_FORMAT		_pixel_format;

		INT32					_s_minification_ui;
		INT32					_s_magnification_ui;

		DOUBLE					_tex_size_mb;

		INT32					_pbo_bind		[ 2 ];
		INT32					_pbo_buffer_index_cur;
	};

	st_attachment			_attach_info		[COLOR_BUFFER_NB];
	INT32					_active_tex_index	[COLOR_BUFFER_NB];

	INT32					_pbo_depth_bind			[ 2 ];
	INT32					_pbo_depth_buffer_cur	;

			void	detach_all();
			void	dealloc();
			void	attach();
			void	init();
			bool	update_check_param();

	FINLINE	bool	is_attachment_asked() CONST;
			void	leave();
public:
			void	build_sumup_one	( o_str& o, INT32 CONST index ) CONST;
			void	build_sumup		( o_str& o ) CONST;

			INT32	get_nb_tex_active()	CONST	{	return _nb_active_tex;			}
			INT32	get_size_x()		CONST	{	return _size_out[0];			}
			INT32	get_size_y()		CONST	{	return _size_out[1];			}
			GLuint	get_id_used()		CONST	{	return _fbo_id_gl_name_used;	}

	FINLINE	bool	is_color()			CONST	{	return _nb_active_tex > 0;		}
	FINLINE	bool	is_depth()			CONST	{	return _b_depth_use;			}
	FINLINE	bool	is_stencil()		CONST	{	return _b_stencil_use;			}

//	FINLINE	bool	is_valid()			CONST	{	return _b_valid;				}
	FINLINE	bool	is_ready_to_use()	CONST	{	return _b_ready_to_use;			}	// valid and active

	virtual	void	param_init_pt();
public:

	//todo should we ?	
	//		void	use();
	virtual	void	update();
	virtual void	draw();
			void	clear();

	FINLINE	INT32	get_color_attachment_first()			CONST { return _s_first_color_attachment; }
	FINLINE	INT32	get_color_attachment_bind( INT32 i )	CONST { return _attach_info[i]._bind_2d_dst_out; }

	//		void	bind();
	virtual AAA_ERR	load_do_after( o_str CONST & filename );
	virtual AAA_ERR	save_do_after( o_str CONST & filename );
};

