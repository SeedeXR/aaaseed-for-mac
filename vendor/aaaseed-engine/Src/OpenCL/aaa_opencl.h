#ifdef AAA_AAA_OPENCL_H
#error "AAA_OPENCL_H included more than once."
#endif
#define AAA_AAA_OPENCL_H 1


//#define __CL_ENABLE_EXCEPTIONS
#define		__NO_STD_VECTOR
#define		__NO_STD_STRING

#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef CL_HPP_
#	include "CL\cl.hpp"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

#ifdef AAA_OPENCL_NO
#	define AAA_OPENCL_USE() 0
#else
#	if AAA_WIN64()
#		define AAA_OPENCL_USE() 1
#	else
#		define AAA_OPENCL_USE() 0
#	endif
#endif

namespace	AAA_OPENCL
{
	extern 	C_PCHAR	get_cl_err_str( cl_int error_code );
	extern	bool	check_for_error( C_PCHAR func, C_PCHAR txt, cl_int status );

	enum OPENCL_DEVICE_TYPE : INT32
	{
		OPENCL_DEVICE_TYPE_DEFAULT = 0,
		OPENCL_DEVICE_TYPE_CPU,
		OPENCL_DEVICE_TYPE_GPU,
		OPENCL_DEVICE_TYPE_ACCELERATOR,
		OPENCL_DEVICE_TYPE_ALL,
		OPENCL_DEVICE_TYPE_MAX_NB
	};

	extern	C_PCHAR_C	str_cl_device_type[ OPENCL_DEVICE_TYPE_MAX_NB ];

	enum PLATFORM_DEF_TYPE : INT32
	{
		PLATFORM_DEF_NVIDIA_GPU1_GPU0_0 = 0,		
		PLATFORM_DEF_MAX_NB
	};
	extern	C_PCHAR_C	str_platform_default[ PLATFORM_DEF_MAX_NB ];

	class	c_opencl
	{
	public:
		static	bool	gb_context_by_program_ui;
		static	bool	gb_opencl_image_format_verbose_ui;
		static	bool	gb_opencl_possible;
		static	bool	gb_opencl_allow_ui;

		static	void		print_platform_info();
		static	void		print_image_format( INT32 order, INT32 type );
		static	void		print_supported_image_format( std::vector<cl::ImageFormat>&	image_formats );
		static	void		init();

	public:
		c_opencl();
		~c_opencl();

#if AAA_OPENCL_USE()
		o_str CONST &		get_device_name()			{ return _device_name; }
		o_str CONST &		get_device_version()		{ return _device_version; }
		o_str CONST &		get_device_vendor()			{ return _device_vendor; }
		o_str CONST &		get_platform_name()			{ return _platform_name; }
		o_str CONST &		get_device_driver_version()	{ return _device_driver_version; }
		o_str CONST &		get_device_extension()		{ return _device_extension; }
		UINT32				get_kernel_workgroup_size()	{ return _kernel_work_group_size; }
		INT32				get_platform_nb()			{ return _platforms.size(); }

		bool				gl_interop_can()			{ return _b_opengl_can; }

		INT32				choose_platform_default();
		void				setup( INT32 platform, UINT32 device_type = OPENCL_DEVICE_TYPE_GPU, UINT32 device_index = 0, bool b_use_opengl_context = false );

		cl::CommandQueue*	get_queue()			{ return _queue; }
#endif //#if AAA_OPENCL_USE()

		bool				have_extension( C_PCHAR_C ext );

		void				close();
		void				flush();
		void				finish();

	private:
		bool				build();
	public:
		bool				program_build_from_source(	std::vector< o_str CONST *> CONST * sources );
		bool				program_build_from_file(	o_str CONST & filename, o_str CONST & include, bool b_binary = false );

#if AAA_OPENCL_USE()
		bool				gl_objects_acquire( VECTOR_CLASS<cl::Memory>* v, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr );
		bool				gl_objects_release( VECTOR_CLASS<cl::Memory>* v, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr );
		cl::Memory*			buffer_gl_create(			cl_mem_flags flags, UINT32 tex_id );
		cl::Memory*			buffer_render_gl_create(	cl_mem_flags flags, UINT32 render_id );
		cl::Image2DGL*		image_2d_gl_create(			cl_mem_flags flags, GLuint tex_name );

//		void*				buffer_gl_map_read( cl::Buffer* buffer, bool b_blocking, INT32 offset, INT32 size, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr );
//		bool				image_read( cl::Image* image, bool b_blocking, INT32 offset, INT32 size, void* data, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event );

		bool				buffer_write(	cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, void* data, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr );
		bool				buffer_read(	cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, void* data, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr );
		cl::Buffer*			buffer_create(	cl_mem_flags flags, UINT64 size, void* data );
		void*				buffer_map_read(cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event  = nullptr );

		bool				kernel_load(	o_str* kernel_name );
		bool				kernel_bind(	o_str* kernel_name );
		cl::KernelFunctor*	kernel_bind(	o_str* kernel_name,  CONST cl::NDRange& offset, CONST cl::NDRange& global, CONST cl::NDRange& local );
		bool				kernel_run( CONST cl::NDRange& offset, CONST cl::NDRange& global, CONST cl::NDRange& local, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event = nullptr);
#endif //#if AAA_OPENCL_USE()

		template <typename T>
		bool		kernel_set_arg( INT32 index, T* val, C_PCHAR_C str_error )
		{
#if AAA_OPENCL_USE()
			if ( !_kernel )
				return false;
			cl_int	status = _kernel->setArg( (cl_int)index, *val );
			if ( !AAA_OPENCL::check_for_error( __FUNCTION__, "failed", status ) )
				return true;
			if( str_error )
				ERR_PRINT_STRING( "\t%s() on arg %d named %s", __FUNCTION__, index, str_error );
			else
				ERR_PRINT_STRING( "\t%s() on arg %d", __FUNCTION__, index );
#endif //#if AAA_OPENCL_USE()
			return false;
		}
		bool		kernel_set_arg_direct( INT32 index, ::size_t size, void* argPtr, C_PCHAR_C str_error )
		{
#if AAA_OPENCL_USE()
			if ( !_kernel )
				return false;
			cl_int	status = _kernel->setArg( (cl_int)index, size, argPtr );
			if ( !AAA_OPENCL::check_for_error( __FUNCTION__, "failed", status ) )
				return true;
			if( str_error )
				ERR_PRINT_STRING( "\t%s() on arg %d named %s", __FUNCTION__, index, str_error );
			else
				ERR_PRINT_STRING( "\t%s() on arg %d", __FUNCTION__, index );
#endif //#if AAA_OPENCL_USE()
			return false;
		}
	protected:
		o_str	_platform_name;
		o_str	_device_name;
		o_str	_device_version;
		o_str	_device_driver_version;
		o_str	_device_vendor;
		o_str	_device_extension;

		bool	_b_valid;
		bool	_b_opengl_can;
		bool	_b_gl_context;

#if AAA_OPENCL_USE()
		static	VECTOR_CLASS<cl::Platform>				_platforms;
		cl::Context*									_context;
		VECTOR_CLASS<cl::Device>						_devices;
//		VECTOR_CLASS<cl::ImageFormat>					_image_format;
		cl::Device										_device;
		cl::CommandQueue*								_queue;


		cl::Program*									_program;

		std::unordered_map< void*, cl::Kernel* >		_kernels;	//we will use the name pointer as a unique id
		cl::Kernel*										_kernel;
//		bool											_b_loaded;
		bool											_b_program_build;
		INT32											_kernel_work_group_size;
//		std::vector<c_OpenCL_program*>					_programs;
#endif //#if AAA_OPENCL_USE()
		void		deinit();

	public:
		FINLINE	bool	is_valid()	{ return _b_valid; }
	};

	extern	void	init();
}

