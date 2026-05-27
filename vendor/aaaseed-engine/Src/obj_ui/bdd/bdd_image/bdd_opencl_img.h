
#ifdef AAA_BDD_OPENCL_IMG_H
#error "BDD_OPENCL_IMG_H included more than once."
#endif
#define AAA_BDD_OPENCL_IMG_H 1


#ifndef AAA_BDD_IMG_H
#	include "bdd_img.h"
#endif
#ifndef	AAA_AAA_OPENCL_H
#	include "OpenCL/aaa_opencl.h"
#endif

class	c_bdd_img_opencl final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_opencl, c_bdd_img );
public:
	static	CONST	INT32	KERNEL_NB = 4;
protected:
	bool		_b_use_gl_ui;
	bool		_b_init_done;
	bool		_b_loaded;
	bool		_b_data;
	
	INT32		_platform;
	INT32		_device;
	o_str		_device_name;
	o_str		_device_version;
	o_str		_device_vendor;
	o_str		_platform_name;
	o_str		_device_driver_version;
	o_str		_device_extension;
	INT32		_device_type;
	o_str		_script_filename_ui;
	o_str		_script_filename;
	time_t		_script_file_time;

	bool		_b_kernels;
//	bool		_b_verbose;
	bool		_b_reload_trig;
 
	bool		_b_kernel_loaded[ KERNEL_NB ];
	bool		_kernel_active[ KERNEL_NB ];
	o_str		_kernel_name[ KERNEL_NB ];
	REAL		_param_1[ KERNEL_NB ];
	REAL		_param_2[ KERNEL_NB ];
	REAL		_param_ui_1[ KERNEL_NB ];
	REAL		_param_ui_2[ KERNEL_NB ];
	INT32		_kernel_max_workgroup_size[ KERNEL_NB ];
	INT32		_block_size_x[ KERNEL_NB ];
	INT32		_block_size_y[ KERNEL_NB ];
	INT32		_block_size_x_ui[ KERNEL_NB ];

//	INT32		_block_size_y_ui;
	UINT32		_byte_nb;
	UINT64		_data_size;

#if AAA_OPENCL_USE()
	cl::Buffer*	_input_buffer_gl;
	cl::Buffer*	_output_buffer_gl;

	bool		_b_source_loaded;
	bool		_b_tmp_on;
	cl_uchar4*	_output_data;
	cl_uchar4*	_tmp_data;

	AAA_OPENCL::c_opencl*	_open_cl;
	cl::Buffer*	_input_buffer;
	cl::Buffer*	_output_buffer;
#endif //AAA_OPENCL_USE

	//cl::Buffer*			_tmp_buffer;
	//	cl::ImageGL*		glinputImageBuffer;
	//	cl::ImageGL*		gloutputImageBuffer;

	void			dealloc_data();
	void			alloc_data();
/*
void		do_size_check( INT32 x, INT32 y )
	{	
		if( _src_size_x == x && _src_size_y == y )
			_b_size_changed = false;
		else
		{
			_src_size_x = x;
			_src_size_y = y;
			_b_size_changed = true;
		}
	}
*/
#if AAA_OPENCL_USE()
	bool			update_img_src();
	bool			update_kernels_gl(	UINT32 index );
	bool			update_kernels(		UINT32 index );
	void			load_kernels();
	bool			is_param_changed();
	void			do_process();
#endif //AAA_OPENCL_USE
	void			init_low();
	void			deinit_low();

public:
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
