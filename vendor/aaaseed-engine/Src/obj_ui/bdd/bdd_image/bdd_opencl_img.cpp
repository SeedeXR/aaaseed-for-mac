
#include "bdd_opencl_img.h"
#include "draw/tex.h"
#include "image/bind_img_2d.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_opencl, bdd_img_opencl, Image OpenCL, bdd_img_opencl, sub_menu="Image"; );

namespace n_bdd_img_opencl
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 12 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32 KERNEL_PARAM_NB	= 8;
	CONSTEXPR INT32 GROUP_PARAM_NB	= c_bdd_img_opencl::KERNEL_NB;

	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	KERNEL_PARAM_NB * c_bdd_img_opencl::KERNEL_NB
								+	GROUP_PARAM_NB;

#define	PARAM_DEF_KERNEL( nb )\
	PARAM_DEF_GROUP_CLOSED(		kernel_##nb,						KERNEL_PARAM_NB )\
	PARAM_DEF_BOOL_OFF(			kernel_##nb##_active				)\
	PARAM_DEF_STR(				kernel_##nb##_name					)\
	PARAM_DEF_INT32_LOCKED(		kernel_##nb##_max_workgroup_size	)\
	PARAM_DEF_INT32_POS(		kernel_##nb##_block_x_ui,			8, 64 )\
	PARAM_DEF_INT32_LOCKED_XY(	kernel_##nb##_block					)\
	PARAM_DEF_REAL_INF(			kernel_##nb##_param_01,				1, 0 )\
	PARAM_DEF_REAL_INF(			kernel_##nb##_param_02,				1, 0 )

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(			use_gl_texture			)
		PARAM_DEF_SYMBO_NEG(		platform,				0, -1,	-1, 12, AAA_OPENCL::str_platform_default		)
		PARAM_DEF_STR_LOCKED(		platform_name			)
		PARAM_DEF_SYMBO_PSTR_ZERO(	device_type,			AAA_OPENCL::str_cl_device_type )
		PARAM_DEF_INT32_POS(		device_index,			1, 0 )
		PARAM_DEF_STR_LOCKED(		device_name				)
		PARAM_DEF_STR_LOCKED(		device_version			)
		PARAM_DEF_STR_LOCKED(		device_driver_version	)
		PARAM_DEF_STR_LOCKED(		device_vendor			)
		PARAM_DEF_STR_LOCKED(		device_extension		)
		PARAM_DEF_REF(				filename				)
		PARAM_DEF_BOOL_OFF(			reload_trig				)

		PARAM_DEF_KERNEL( 01 )
		PARAM_DEF_KERNEL( 02 )
		PARAM_DEF_KERNEL( 03 )
		PARAM_DEF_KERNEL( 04 )
	};
}

void	c_bdd_img_opencl::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _b_use_gl_ui			);
	param_set_pt( h, _platform				);
	param_set_pt( h, _platform_name			);
	param_set_pt( h, _device_type			);
	param_set_pt( h, _device				);
	param_set_pt( h, _device_name			);
	param_set_pt( h, _device_version		);
	param_set_pt( h, _device_driver_version	);
	param_set_pt( h, _device_vendor			);
	param_set_pt( h, _device_extension		);
	param_set_pt( h, _script_filename_ui	);
	param_set_pt( h, _b_reload_trig			);

	for ( size_t i = 0; i < KERNEL_NB; ++i )
	{
		++h;
			param_set_pt( h, _kernel_active				[ i ] );
			param_set_pt( h, _kernel_name				[ i ] );
			param_set_pt( h, _kernel_max_workgroup_size	[ i ] );
			param_set_pt( h, _block_size_x_ui			[ i ] );
			param_set_pt( h, _block_size_x				[ i ] );
			param_set_pt( h, _block_size_y				[ i ] );
			param_set_pt( h, _param_ui_1				[ i ] );
			param_set_pt( h, _param_ui_2				[ i ] );
	}

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_opencl )
	,_b_init_done		( false )
	,_b_loaded			( false )
	,_byte_nb			( 4 )
#if AAA_OPENCL_USE()
	,_output_data		( nullptr )
	,_input_buffer		( nullptr )
	,_output_buffer		( nullptr )
	,_input_buffer_gl	( nullptr )		//todo we init here to avoid a crash but nothing is really done about it for now
	,_output_buffer_gl	( nullptr )
	,_tmp_data			( nullptr )
	,_b_source_loaded	( false )
#endif //AAA_OPENCL_USE
	,_data_size			( 0 )
	,_b_kernels			( false )
	,_b_use_gl_ui		( false )
	,_script_file_time	( c_file::TIME_UNDEFINED )
{
	for( size_t i = 0; i < KERNEL_NB; ++i )
	{
		_block_size_x[ i ] = 256;
		_block_size_y[ i ] = 1;
		_kernel_max_workgroup_size[ i ] = 0;
		_b_kernel_loaded[ i ] = false;
	}

	param_init_with( n_bdd_img_opencl::param, n_bdd_img_opencl::PARAM_NB );
}

c_bdd_img_opencl::~c_bdd_img_opencl()
{
	deinit_low();
}

#if AAA_OPENCL_USE()
void	c_bdd_img_opencl::do_process()
{
	if( !_b_data )
	{
		ERR_PRINT_STRING( "s%() data not ready !!!", __FUNCTION__ );
		return;
	}
	TBUF_ADD_OBJ( tbuf::CH_OPENCL, 1., "bdd_opencl_img", this );
	bool	b_updated = false;
	_b_source_loaded = false;
	for( UINT32 i = 0; i < KERNEL_NB; ++i )
	{
		if( _kernel_active[ i ] && _b_kernel_loaded[ i ] )
			b_updated |= update_kernels( i );
	}
	_open_cl->finish();
	if( b_updated )
	{
		cl_int	status;
		cl::Event	event;
		if( !_open_cl->buffer_read( _b_tmp_on ? _output_buffer : _input_buffer, true, 0, _data_size, _b_tmp_on ? _output_data : _tmp_data, 0, &event ) )
			return;

		status = event.wait();
		if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed on read", status) )
			return;

		c_img_2d* img_dst = g_bind_img_2d->get_img( _dst_img_index_ui, aaa::PIXEL_FORMAT::RGBA_8, _src_size[0], _src_size[1], true, nullptr, __FUNCTION__ );
		if( !img_dst )
			err_print( "%s() : no image dst at bind %d", __FUNCTION__, _dst_img_index_ui );
		else
		{
			if( _output_data )
			{
				MEMCPY( img_dst->get_data_uint8(), _b_tmp_on ? _output_data : _tmp_data, _data_size, __FUNCTION__ );
				img_dst->erase_filename();
				img_dst->set_changed();
			}
		}
	}
	TBUF_ADD_OBJ( tbuf::CH_OPENCL, 0., nullptr, this );
}
#endif //AAA_OPENCL_USE

void	c_bdd_img_opencl::init_low()
{
#if AAA_OPENCL_USE()
	if( _b_init_done )
		return;
	if( !AAA_OPENCL::c_opencl::gb_opencl_possible || !AAA_OPENCL::c_opencl::gb_opencl_allow_ui )
		return;

	// init device
	AAA_OPENCL::c_opencl* open_cl = new AAA_OPENCL::c_opencl();
	open_cl->setup( _platform, _device_type, _device, _b_use_gl_ui );
	_b_init_done = open_cl->is_valid();
	_device_name = open_cl->get_device_name();
	_device_version = open_cl->get_device_version();
	_device_vendor = open_cl->get_device_vendor();
	_platform_name = open_cl->get_platform_name();
	_device_driver_version = open_cl->get_device_driver_version();
	_device_extension = open_cl->get_device_extension();

	if( _b_init_done )
		_open_cl = open_cl;
	else
	{
		err_print( "c_opencl instance is invalid" );
		SAFE_DELETE( open_cl );
	}
#endif //AAA_OPENCL_USE
}

void	c_bdd_img_opencl::deinit_low()
{
#if AAA_OPENCL_USE()
	if( !_b_init_done )
		return;
	dealloc_data();
	SAFE_DELETE( _open_cl );
	_b_init_done = false;
#endif //AAA_OPENCL_USE
}

#if AAA_OPENCL_USE()
void	c_bdd_img_opencl::load_kernels()
{
	_b_kernels = false;
	for( UINT32 i = 0; i < KERNEL_NB; ++i )
	{
		if( _kernel_name[ i ].get_len() > 0 )
		{
			if( _open_cl->kernel_load( &_kernel_name[ i ] ) )
			{
				_b_kernel_loaded[ i ] = true;
				_b_kernels = true;
				_kernel_max_workgroup_size[ i ] = _open_cl->get_kernel_workgroup_size();
				//	Check group size against group size returned by kernel
				if( _block_size_x[ i ] > _kernel_max_workgroup_size[ i ] || ( _block_size_x[ i ] * _block_size_y[ i ] ) > _kernel_max_workgroup_size[ i ] )
				{
					ERR_PRINT_STRING( "%s() Max Group Size supported on the kernel : %d", __FUNCTION__, _kernel_max_workgroup_size[ i ] );
					_block_size_x[ i ] = MIN( _block_size_x[ i ], _kernel_max_workgroup_size[ i ] );
					_block_size_y[ i ] = _kernel_max_workgroup_size[ i ] / _block_size_x[ i ];
				}
			}
		}
	}
}

namespace {
	// Round Up Division function
	CONSTEXPR INT32	DIV_ROUND_UP( INT32 CONST a, INT32 CONST b ) NOEXCEPT
	{
		INT32 r = a % b;
		return r ? a + b - r : a;
	}
}

bool	c_bdd_img_opencl::update_kernels( UINT32 index )
{
	if ( _b_use_gl_ui )
		return update_kernels_gl( index );

	cl_int		status;
	cl::Event	events[2];

	if( !_tmp_data || !_input_buffer || !_output_buffer )
		return false;
	if( !_b_source_loaded )
	{
		c_img_2d*	img_src = g_bind_img_2d->get_img( get_src_img_index_ui(), aaa::PIXEL_FORMAT::RGBA_8, _src_size[0], _src_size[1], true, nullptr, __FUNCTION__ );

		if( !img_src )
		{
			err_print( "%s() : no image src at bind %d", __FUNCTION__, get_src_img_index_ui() );
			return false;
		}
		MEMCPY( _tmp_data, img_src->get_data_uint8(), _data_size, __FUNCTION__ );
		if ( _input_buffer )
		{
			cl::Event events;
			if( !_open_cl->buffer_write( _input_buffer, true, 0, _data_size, _tmp_data, 0, &events ) )
				return false;

			///* Wait for the read buffer to finish execution */
			status = events.wait();
			if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed. (event[1])", status) )
				return false;
		}
		_b_source_loaded = true;
		_b_tmp_on = false;
	}
	//	_block_size_x_ui[ index ] = MAX( _block_size_x_ui[ index ], 1 );
	_block_size_x_ui[ index ] = MAX( _block_size_x_ui[ index ], 4 );
	if( _block_size_x_ui[ index ] != _block_size_x[ index ] )
	{
		if( _kernel_max_workgroup_size[ index ] % _block_size_x_ui[ index ] == 0 )
		{
			_block_size_x[ index ] = _block_size_x_ui[ index ];
			_block_size_y[ index ] = _kernel_max_workgroup_size[ index ] / _block_size_x[ index ];
		}
	}
	cl::NDRange	localThreads( _block_size_x[ index ], _block_size_y[ index ] );
	cl::NDRange	globalThreads( DIV_ROUND_UP( _src_size[0], _block_size_x[index] ), DIV_ROUND_UP( _src_size[1], _block_size_y[index] ) );
	cl::KernelFunctor*	functor = nullptr;
	functor = _open_cl->kernel_bind( &_kernel_name[ index ], cl::NullRange, globalThreads, localThreads );
	if( functor )
	{
		status = (*functor)( _b_tmp_on ? *_output_buffer : *_input_buffer, _b_tmp_on ? *_input_buffer : *_output_buffer, _param_1[ index ] ).wait();
		if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed functor", status ) )
			return false;
		_b_tmp_on = !_b_tmp_on;
		return true;
	}
	return false;
	//if( _open_cl->kernel_bind( &_kernel_name[ index ] ) )
	//{
	//	if( !_open_cl->kernel_set_arg( 0, _b_tmp_on ? _output_buffer : _input_buffer ) )
	//	{
	//		ERR_PRINT_STRING( "%s set_arg failed on inputImageBuffer", __FUNCTION__ );
	//		return false;
	//	}
	//	if( !_open_cl->kernel_set_arg( 1, _b_tmp_on ? _input_buffer : _output_buffer ) )
	//	{
	//		ERR_PRINT_STRING( "%s set_arg failed on outputImageBuffer", __FUNCTION__ );
	//		return false;
	//	}
	//	if( !_open_cl->kernel_set_arg( 2, &_param_1[ index ] ) )
	//	{
	//		ERR_PRINT_STRING( "%s set_arg failed on param", __FUNCTION__ );
	//	}


	//	if( !_open_cl->kernel_run( cl::NullRange, globalThreads, localThreads, nullptr, &events[0] ) )
	//		return false;
	//	//status = _open_cl->_cl_queue->enqueueNDRangeKernel( *kernel, cl::NullRange, globalThreads, localThreads, nullptr, &events[0] );
	//	//if ( OpenCL::check_for_error( __FUNCTION__, "enqueueNDRangeKernel() failed", status) )
	//	//	return false;
	//	status = events[0].wait();
	//	if ( OpenCL::check_for_error( __FUNCTION__, "Event::wait() failed. (event[0])", status) )
	//		return false;

	//	_b_tmp_on = !_b_tmp_on;
	//	return true;
	//}
	//return false;
}

bool	c_bdd_img_opencl::update_kernels_gl( UINT32 index )
{
	cl_int status;
	cl::Event events[2];

	if( !_tmp_data || !_input_buffer_gl || !_output_buffer_gl )
		return false;

	//if( !_b_source_loaded )
	{
		//c_img_2d*	img_src = bind_img::get_img( _src_img_index, _src_size_x, _src_size_y, 4, true, nullptr, __FUNCTION__ );

		//if( !img_src )
		//{
		//	err_print( "%s() : no image src at bind %d", __FUNCTION__, _src_img_index );
		//	return false;
		//}
		//MEMCPY( _tmp_data, img_src->get_data(), _data_size );
		if( _input_buffer_gl )
		{
			VECTOR_CLASS<cl::Memory> v;
//			cl::vector<cl::Memory> v;
			v.push_back( *_input_buffer_gl );
			//v.push_back(partialDifss);
			//err = queueGPU.enqueueAcquireGLObjects(&v);
			//cl::vector<cl_mem> v;
			//v.push_back( cl::Memory(_input_buffer_gl )); //v.push_back(partialDifss);
		////	v.push_back( (cl_mem)_input_buffer_gl ); //v.push_back(partialDifss);
			//cl::Event			events;
		//	cl::CommandQueue*	queue = _open_cl->get_queue();
			//v, events, event
		//	queue->enqueueAcquireGLObjects( &v ); //, 0, &events );
		//	queue->enqueueAcquireGLObjects( _input_buffer_gl, 0, &events );
			cl::Event events;
			if( !_open_cl->gl_objects_acquire( &v, 0, &events ) )
				return false;

			/* Wait for the read buffer to finish execution */
			status = events.wait();
			if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed. (event[1])", status) )
				return false;
		}
		_b_source_loaded = true;
		_b_tmp_on = false;
	}
	_block_size_x_ui[ index ] = MAX( _block_size_x_ui[ index ], 4 );
	if( _block_size_x_ui[ index ] != _block_size_x[ index ] )
	{
		if( _kernel_max_workgroup_size[ index ] % _block_size_x_ui[ index ] == 0 )
		{
			_block_size_x[ index ] = _block_size_x_ui[ index ];
			_block_size_y[ index ] = _kernel_max_workgroup_size[ index ] / _block_size_x[ index ];
		}
	}
	cl::NDRange	localThreads( _block_size_x[ index ], _block_size_y[ index ] );
	cl::NDRange	globalThreads( DIV_ROUND_UP( _src_size[0], _block_size_x[index] ), DIV_ROUND_UP( _src_size[1], _block_size_y[index] ) );
	cl::KernelFunctor*	functor = nullptr;
	functor = _open_cl->kernel_bind( &_kernel_name[ index ], cl::NullRange, globalThreads, localThreads );
	if( functor )
	{
		status = (*functor)( _b_tmp_on ? *_output_buffer_gl : *_input_buffer_gl, _b_tmp_on ? *_input_buffer_gl : *_output_buffer_gl, _param_1[ index ] ).wait();
		if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed functor", status ) )
			return false;
		_b_tmp_on = !_b_tmp_on;
		return true;
	}
	return false;
}

bool	c_bdd_img_opencl::update_img_src()
{
	_p_in_to_use = nullptr;
	c_img_2d*	img = update_part_1();
	if( !img )
		return false;
//maa removed it why redo stuff here already done in update_part_1
//	do_size_check( img->get_size_x(), img->get_size_y() );
	return true;
}

void	c_bdd_img_opencl::dealloc_data()
{
	SAFE_DELETE( _input_buffer );
	SAFE_DELETE( _output_buffer );
	IF_FREE_ALIGNED_AND_NULL( _tmp_data );
	IF_FREE_ALIGNED_AND_NULL( _output_data );
	_b_data = false;
}

void	c_bdd_img_opencl::alloc_data()
{
	c_img_2d*	img_src = g_bind_img_2d->get_ready( get_src_img_index_ui() );
	if( !img_src  )
	{
		err_print( "%s() : no image source at bind %d", __FUNCTION__, get_src_img_index_ui() );
		return;
	}
	UINT8*	src = img_src->get_data_uint8();
	if( !src )
	{
		ERR_PRINT_STRING( "%s() src data is Null", __FUNCTION__ );
		return;
	}

	dealloc_data();

	_src_size[0] = img_src->get_size_x();
	_src_size[1] = img_src->get_size_y();
	
	c_img_2d* img_dst = g_bind_img_2d->get_img( _dst_img_index_ui, aaa::PIXEL_FORMAT::RGBA_8,_src_size[0], _src_size[1], true, nullptr, __FUNCTION__ );

	if( !img_dst )
	{
		err_print( "%s() : no image dst at bind %d", __FUNCTION__, _dst_img_index_ui );
		return;
	}
//	_byte_nb = sizeof(cl_uchar4);
	_data_size = c_img_2d::compute_data_size( _src_size[0], _src_size[1], aaa::PIXEL_FORMAT::RGBA_8 );

	_tmp_data = (cl_uchar4*) MALLOC_ALIGNED( _data_size, 0 );
	if( !_tmp_data )
	{
		ERR_PRINT_STRING( "%s() error allocating tmp_data", __FUNCTION__ );
		return;
	}
	MEMCPY( _tmp_data, src, _data_size, __FUNCTION__ );	//todo	can be too much with alignment 

	cl_int	status;
	_input_buffer = _open_cl->buffer_create( CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, _data_size, _tmp_data );
	if( !_input_buffer )
	{
		ERR_PRINT_STRING( "%s() error creating input_buffer", __FUNCTION__ );
		return;
	}

//todo copy already done at create this could be redundant
	cl::Event events;

	if( !_open_cl->buffer_write( _input_buffer, true, 0, _data_size, src, 0, &events ) )
	{
		ERR_PRINT_STRING( "%s() error writing input_buffer", __FUNCTION__ );
		return;
	}
	status = events.wait();
	if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed", status) )
	{
		return;
	}

	//tex::bind_2d( _src_img_index );
	//IF_FREE_AND_NULL( glinputImageBuffer );
	//glinputImageBuffer = new cl::ImageGL(_open_cl->context, CL_MEM_READ_ONLY,1, tex::get_name_2d( _src_img_index ), &err );
	//if( err != CL_SUCCESS )
	//{
	//	ERR_PRINT_STRING( "%s() cl::Buffer failed. (inputImageBuffer) : %s", __FUNCTION__, OpenCL::get_OpenCL_err_str( err ) );
	//	return;
	//}

	_output_data = (cl_uchar4*) MALLOC_ALIGNED( _data_size, 0 );
	if( !_output_data )
	{
		ERR_PRINT_STRING( "%s() error allocating output_data", __FUNCTION__ );
		return;
	}

	_output_buffer = _open_cl->buffer_create( CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, _data_size, _output_data );
	if( !_output_buffer )
	{
		ERR_PRINT_STRING( "%s() error creating output_buffer", __FUNCTION__ );
		return;
	}

	_b_data = true;
	//tex::bind_2d( _src_img_index );
	//IF_FREE_AND_NULL( gloutputImageBuffer );
	//gloutputImageBuffer = new cl::ImageGL(_open_cl->context, CL_MEM_READ_WRITE,1, tex::get_name_2d( _dst_img_index ), &err );
	//if( err != CL_SUCCESS )
	//{
	//	ERR_PRINT_STRING( "%s() cl::Buffer failed. (inputImageBuffer) : %s", __FUNCTION__, OpenCL::get_OpenCL_err_str( err ) );
	//	return;
	//}
}


bool	c_bdd_img_opencl::is_param_changed()
{
	bool	b_ret = false;
	for( UINT32 i = 0; i < KERNEL_NB; ++i )
	{
		if( _param_ui_1[ i ] != _param_1[ i ] || _param_ui_2[ i ] != _param_2[ i ] )
		{
			_param_1[ i ] = _param_ui_1[ i ];
			_param_2[ i ] = _param_ui_2[ i ];
			b_ret = true;
		}
	}
	return b_ret;
}
#endif //AAA_OPENCL_USE

void	c_bdd_img_opencl::update()
{
#if AAA_OPENCL_USE()
	if( !AAA_OPENCL::c_opencl::gb_opencl_possible || !AAA_OPENCL::c_opencl::gb_opencl_allow_ui )
		return;
	if( !is_active() )						return;

	if( !_b_init_done )
	{
		init_low();
		if( !_b_init_done )					return;
	}

	if( _script_filename_ui.is_empty() )	return;
	if( _script_filename.is_empty() )
	{
		//todo doesn't work with absolute path!!!
		if( !c_obj_ui::make_script_filename( _script_filename, _script_filename_ui, "cl" ) )
		{
			debug_break( "%s() can't build script filename !", __FUNCTION__ );
			return;
		}
	}

	if( _b_reload_trig || !_b_loaded || c_file::is_time_changed( _script_filename, _script_file_time ) )	//	|| !_b_program_submitted )
	{
		_b_reload_trig = false;
		o_str include_text;
		if( _open_cl->program_build_from_file( _script_filename, include_text ) )
			_b_loaded = true;
	}
	if( !_b_loaded )
		return;

	if( !_b_kernels )
	{
		load_kernels();
		if( !_b_kernels )
			return;
	}

	bool	do_it = is_param_changed();
	if( update_img_src() )
	{
		if( is_size_changed() )
			alloc_data();
		if( is_content_changed() )
			do_it = true;
	}
	if( do_it )
		do_process();
#endif //AAA_OPENCL_USE
}

void	c_bdd_img_opencl::draw()
{
/*
	// strange this do nothing
	if( !AAA_OPENCL::c_opencl::gb_opencl_possible || !AAA_OPENCL::c_opencl::gb_opencl_allow_ui )
		{	return;	}
	if( !is_active() )	{	return;	}
*/
}


