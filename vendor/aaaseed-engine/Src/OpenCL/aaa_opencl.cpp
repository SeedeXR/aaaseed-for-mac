#include "aaa_opencl.h"

#if AAA_OPENCL_USE()
#	include "wrap_OpenCL.h"			// do wrapped DLL calls //sr@20100415
#	include "aaa_util.h"
#	include "file/aaa_dir.h"
#	include "aaa_mem.h"
#endif //#if AAA_OPENCL_USE()

#include "spy.h"

namespace	AAA_OPENCL
{

	C_PCHAR_C	str_cl_device_type[ OPENCL_DEVICE_TYPE_MAX_NB ] = 
	{
		"Default",
		"CPU",
		"GPU",
		"Accelerator",
		"All",
	};
	C_PCHAR_C	str_platform_default[ PLATFORM_DEF_MAX_NB ] =
	{
		"NVidia then GPU second then GPU first"
	};

	CONSTEXPR C_PCHAR_C	OPENCL_HEADER  = "# OPENCL : ";
	void	OPENCL_PRINT_STRING( C_PCHAR_C fmt, ... )
		{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENCL_HEADER,		fmt, args );	va_end( args );
		}
	void	OPENCL_PRINT_STRING()	{	OPENCL_PRINT_STRING( "" );	}
	void	OPENCL_PRINT_STRING_1T( C_PCHAR_C fmt, ... )
		{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( "# OPENCL :    ",	fmt, args );	va_end( args );
		}
	void	OPENCL_PRINT_STRING_2T( C_PCHAR_C fmt, ... )
		{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( "# OPENCL :\t",		fmt, args );	va_end( args );
		}
	void	OPENCL_PRINT_STRING_3T( C_PCHAR_C fmt, ... )
		{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( "# OPENCL :\t    ",	fmt, args );	va_end( args );
		}
	void	OPENCL_PRINT_STRING_4T( C_PCHAR_C fmt, ... )
		{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( "# OPENCL :\t\t",	fmt, args );	va_end( args );
		}

	c_opencl::c_opencl()
		:_b_valid(false)
#if AAA_OPENCL_USE()
		,_context(nullptr)
		,_queue(nullptr)
		,_program(nullptr)
		,_device(0)
		,_kernel_work_group_size(256)
		,_b_opengl_can(false)
		,_b_gl_context(false)
		,_kernel(nullptr)
		,_b_program_build(false)
#endif //#if AAA_OPENCL_USE()
	{
	}

	c_opencl::~c_opencl()
	{
#if AAA_OPENCL_USE()
		deinit();
		_platforms.clear();
#endif //#if AAA_OPENCL_USE()
	}

	bool	c_opencl::gb_context_by_program_ui			= true;
	bool	c_opencl::gb_opencl_image_format_verbose_ui	= true;
	bool	c_opencl::gb_opencl_possible				= false;
	bool	c_opencl::gb_opencl_allow_ui				= false;

#if AAA_OPENCL_USE()
	VECTOR_CLASS<cl::Platform>	c_opencl::_platforms;

	FINLINE	bool	check_for_error( C_PCHAR func, C_PCHAR txt, cl_int status )
	{
		if( status != CL_SUCCESS )
		{
			ERR_PRINT_STRING( "%s() %s : %s", func, txt, get_cl_err_str( status ) );
			return true;
		}
		return false;
	}

	static	C_PCHAR	get_cl_device_type_str( cl_device_type type )
	{
		C_PCHAR	str;
		switch( type )
		{
		case CL_DEVICE_TYPE_DEFAULT:		str = "CL_DEVICE_TYPE_DEFAULT";		break;
		case CL_DEVICE_TYPE_CPU:			str = "CL_DEVICE_TYPE_CPU";			break;
		case CL_DEVICE_TYPE_GPU:			str = "CL_DEVICE_TYPE_GPU";			break;
		case CL_DEVICE_TYPE_ACCELERATOR:	str = "CL_DEVICE_TYPE_ACCELERATOR";	break;
		case CL_DEVICE_TYPE_ALL:			str = "CL_DEVICE_TYPE_ALL";			break;
		default:							str = "TYPE_MIXED";					break;
		}
		return str;
	}


	static CHAR error_str[256];

	C_PCHAR	get_cl_err_str( cl_int error_code )
	{
		C_PCHAR str;
		switch( error_code )
		{
		case CL_DEVICE_NOT_FOUND:					str = "CL_DEVICE_NOT_FOUND";			break;
		case CL_DEVICE_NOT_AVAILABLE:				str = "CL_DEVICE_NOT_AVAILABLE";		break;
		case CL_COMPILER_NOT_AVAILABLE:				str = "CL_COMPILER_NOT_AVAILABLE";		break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:		str = "CL_MEM_OBJECT_ALLOCATION_FAILURE";	break;
		case CL_OUT_OF_RESOURCES:					str = "CL_OUT_OF_RESOURCES";			break;
		case CL_OUT_OF_HOST_MEMORY:					str = "CL_OUT_OF_HOST_MEMORY";			break;
		case CL_PROFILING_INFO_NOT_AVAILABLE:		str = "CL_PROFILING_INFO_NOT_AVAILABLE";	break;
		case CL_MEM_COPY_OVERLAP:					str = "CL_MEM_COPY_OVERLAP";			break;
		case CL_IMAGE_FORMAT_MISMATCH:				str = "CL_IMAGE_FORMAT_MISMATCH";		break;
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:			str = "CL_IMAGE_FORMAT_NOT_SUPPORTED";	break;
		case CL_BUILD_PROGRAM_FAILURE:				str = "CL_BUILD_PROGRAM_FAILURE";		break;
		case CL_MAP_FAILURE:						str = "CL_MAP_FAILURE"; 				break;
		case CL_INVALID_VALUE:						str = "CL_INVALID_VALUE"; 				break;
		case CL_INVALID_DEVICE_TYPE:				str = "CL_INVALID_DEVICE_TYPE";			break;
		case CL_INVALID_PLATFORM:					str = "CL_INVALID_PLATFORM"; 			break;
		case CL_INVALID_DEVICE:						str = "CL_INVALID_DEVICE";				break;
		case CL_INVALID_CONTEXT:					str = "CL_INVALID_CONTEXT";				break;
		case CL_INVALID_QUEUE_PROPERTIES:			str = "CL_INVALID_QUEUE_PROPERTIES";	break;
		case CL_INVALID_COMMAND_QUEUE:				str = "CL_INVALID_COMMAND_QUEUE";		break;
		case CL_INVALID_HOST_PTR:					str = "CL_INVALID_HOST_PTR"; 			break;
		case CL_INVALID_MEM_OBJECT:					str = "CL_INVALID_MEM_OBJECT";			break;
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:	str = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"; 	break;
		case CL_INVALID_IMAGE_SIZE:					str = "CL_INVALID_IMAGE_SIZE";			break;
		case CL_INVALID_SAMPLER:					str = "CL_INVALID_SAMPLER";				break;
		case CL_INVALID_BINARY:						str = "CL_INVALID_BINARY";				break;
		case CL_INVALID_BUILD_OPTIONS:				str = "CL_INVALID_BUILD_OPTIONS";		break;
		case CL_INVALID_PROGRAM:					str = "CL_INVALID_PROGRAM";				break;
		case CL_INVALID_PROGRAM_EXECUTABLE:			str = "CL_INVALID_PROGRAM_EXECUTABLE";	break;
		case CL_INVALID_KERNEL_NAME:				str = "CL_INVALID_KERNEL_NAME";			break;
		case CL_INVALID_KERNEL_DEFINITION:			str = "CL_INVALID_KERNEL_DEFINITION";	break;
		case CL_INVALID_KERNEL:						str = "CL_INVALID_KERNEL";				break;
		case CL_INVALID_ARG_INDEX:					str = "CL_INVALID_ARG_INDEX";			break;
		case CL_INVALID_ARG_VALUE:					str = "CL_INVALID_ARG_VALUE";			break;
		case CL_INVALID_ARG_SIZE:					str = "CL_INVALID_ARG_SIZE";			break;
		case CL_INVALID_KERNEL_ARGS:				str = "CL_INVALID_KERNEL_ARGS";			break;
		case CL_INVALID_WORK_DIMENSION:				str = "CL_INVALID_WORK_DIMENSION";		break;
		case CL_INVALID_WORK_GROUP_SIZE:			str = "CL_INVALID_WORK_GROUP_SIZE";		break;
		case CL_INVALID_WORK_ITEM_SIZE:				str = "CL_INVALID_WORK_ITEM_SIZE";		break;
		case CL_INVALID_GLOBAL_OFFSET:				str = "CL_INVALID_GLOBAL_OFFSET";		break;
		case CL_INVALID_EVENT_WAIT_LIST:			str = "CL_INVALID_EVENT_WAIT_LIST";		break;
		case CL_INVALID_EVENT:						str = "CL_INVALID_EVENT"; 				break;
		case CL_INVALID_OPERATION:					str = "CL_INVALID_OPERATION";			break;
		case CL_INVALID_GL_OBJECT: 					str = "CL_INVALID_GL_OBJECT";			break;
		case CL_INVALID_BUFFER_SIZE:				str = "CL_INVALID_BUFFER_SIZE";			break;
		case CL_INVALID_MIP_LEVEL:					str = "CL_INVALID_MIP_LEVEL";			break;
		case CL_INVALID_GLOBAL_WORK_SIZE:			str = "CL_INVALID_GLOBAL_WORK_SIZE";	break;
		case CL_INVALID_PROPERTY:					str = "CL_INVALID_PROPERTY";			break;
		default:									sprintf( error_str,  "unknown error code : %d", error_code );
													str = error_str;
													break;
		}
		return str; 
	}

	bool	c_opencl::have_extension( C_PCHAR_C ext )
	{
		bool b = false;
		if( ext && *ext )
		{
			b = strstr( _device_extension.get(), ext ) != nullptr;
			if( b ) 
				OPENCL_PRINT_STRING( "Extension found %s", ext );
			else
				WARNING_PRINT_STRING( "OpenCL Extension not found %s", ext );
		}
		return b;
	}

	namespace{
		INT32 first_gpu_platform = -1;
		INT32 second_gpu_platform = -1;
		INT32 first_nvidia_platform = -1; 
	}

	void	c_opencl::print_platform_info()
	{
		OPENCL_PRINT_STRING( "Number of platforms : %d", _platforms.size() );
		INT32 platform_index = 0;
		for( VECTOR_CLASS<cl::Platform>::iterator p = _platforms.begin(); p != _platforms.end(); ++p, ++platform_index )
		{
			OPENCL_PRINT_STRING();
			OPENCL_PRINT_STRING();

			OPENCL_PRINT_STRING( "Platform index %d : ____________________________________________________________",	platform_index );
				OPENCL_PRINT_STRING_1T( "Platform Name : %s",		(*p).getInfo<CL_PLATFORM_NAME>().c_str()	);
				OPENCL_PRINT_STRING_1T( "Platform Vendor : %s",		(*p).getInfo<CL_PLATFORM_VENDOR>().c_str()	);
				OPENCL_PRINT_STRING_1T( "Platform Version : %s",	(*p).getInfo<CL_PLATFORM_VERSION>().c_str()	);
				OPENCL_PRINT_STRING_1T( "Platform Profile : %s",	(*p).getInfo<CL_PLATFORM_PROFILE>().c_str()	);
				if ( (*p).getInfo<CL_PLATFORM_EXTENSIONS>().size() > 0 )
				{
					OPENCL_PRINT_STRING_1T( "Extensions : "  );
					PRINT_STRING_BY_BLOCK( (*p).getInfo<CL_PLATFORM_EXTENSIONS>().c_str(), 80 );
				}
			VECTOR_CLASS<cl::Device> devices;
			(*p).getDevices( CL_DEVICE_TYPE_ALL, &devices );
			OPENCL_PRINT_STRING_1T( "Number of devices : %d",	devices.size() );


			INT32	device_index = 0;
			for( VECTOR_CLASS<cl::Device>::iterator it = devices.begin(); it != devices.end(); ++it, ++device_index )
			{
				OPENCL_PRINT_STRING();
				OPENCL_PRINT_STRING_2T( "Device index : %d ____________________________________________________________",				device_index );
				OPENCL_PRINT_STRING_3T( "Name : %s",						(*it).getInfo<CL_DEVICE_NAME>().c_str()		);
				o_str vendor( (*it).getInfo<CL_DEVICE_VENDOR>().c_str() );
				OPENCL_PRINT_STRING_3T( "Vendor : %s",						vendor.get()	);
				OPENCL_PRINT_STRING_3T( "Driver version : %s",				(*it).getInfo<CL_DRIVER_VERSION>().c_str()	);
				OPENCL_PRINT_STRING_3T( "Profile : %s",						(*it).getInfo<CL_DEVICE_PROFILE>().c_str()	);
				cl_device_type type = (*it).getInfo<CL_DEVICE_TYPE>();
				OPENCL_PRINT_STRING_3T( "Type : %s",						get_cl_device_type_str(type)	);
				OPENCL_PRINT_STRING_3T( "Platform ID : %d",					(*it).getInfo<CL_DEVICE_PLATFORM>()			);
				OPENCL_PRINT_STRING_3T( "ID : \t%d",						(*it).getInfo<CL_DEVICE_VENDOR_ID>()		);

				if( first_gpu_platform < 0 )
				{
					if( type == CL_DEVICE_TYPE_GPU )
						first_gpu_platform = platform_index;
				}
				else if( second_gpu_platform < 0 )
				{
					if( type == CL_DEVICE_TYPE_GPU )
						second_gpu_platform = platform_index;
				}
				if( first_nvidia_platform < 0 )
				{
					if( str_find_str_nocase( vendor.get(), "nvidia" ) )
						first_nvidia_platform = platform_index;
				}

				OPENCL_PRINT_STRING_2T( "Extensions : " );
				PRINT_STRING_BY_BLOCK( (*it).getInfo<CL_DEVICE_EXTENSIONS>().c_str(), 80 );


				OPENCL_PRINT_STRING_2T( "Max compute units : %lld",			(*it).getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>()		);
				OPENCL_PRINT_STRING_2T( "Max work items dimensions : %lld",	(*it).getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>()	);
				VECTOR_CLASS< ::size_t> work_items = (*it).getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
				for ( UINT32 x = 0; x < (*it).getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>(); ++x )
				{
					OPENCL_PRINT_STRING_3T( "Max work items[%d] : %lld",	x, work_items[ x ] );
				}
				OPENCL_PRINT_STRING_2T( "Max work group size : ll%d",		(*it).getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()		);
				OPENCL_PRINT_STRING_2T( "Preferred vector width :"	);
					OPENCL_PRINT_STRING_3T( "char   : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR>()		);
					OPENCL_PRINT_STRING_3T( "short  : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT>()		);
					OPENCL_PRINT_STRING_3T( "int    : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT>()		);
					OPENCL_PRINT_STRING_3T( "long   : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG>()		);
					OPENCL_PRINT_STRING_3T( "float  : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>()		);
					OPENCL_PRINT_STRING_3T( "double : %lld",	(*it).getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>()	);
				OPENCL_PRINT_STRING_2T( "Max clock frequency : %lld Mhz",	(*it).getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>()				);
				OPENCL_PRINT_STRING_2T( "Address bits : %lld",				(*it).getInfo<CL_DEVICE_ADDRESS_BITS>()						);
				OPENCL_PRINT_STRING_2T( "Max memory allocation : %lld",		(*it).getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>()				);

				OPENCL_PRINT_STRING_2T( "Image support : %s",				(*it).getInfo<CL_DEVICE_IMAGE_SUPPORT>() ? "Yes" : "No"		);
				if ( (*it).getInfo<CL_DEVICE_IMAGE_SUPPORT>() )
				{
					OPENCL_PRINT_STRING_3T( "Max number of images read arguments : %lld",		(*it).getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS>()	);
					OPENCL_PRINT_STRING_3T( "                     write arguments : %lld",		(*it).getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS>()	);
					OPENCL_PRINT_STRING_3T( "Max 2D width  : %lld",								(*it).getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>()	);
					OPENCL_PRINT_STRING_3T( "       height : %lld",								(*it).getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>()	);
					OPENCL_PRINT_STRING_3T( "Max 3D width  : %lld",								(*it).getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>()	);
					OPENCL_PRINT_STRING_3T( "       height : %lld",								(*it).getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>()	);
					OPENCL_PRINT_STRING_3T( "       depth  : %lld",								(*it).getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>()	);
					OPENCL_PRINT_STRING_3T( "Max samplers within kernel :  %lld",				(*it).getInfo<CL_DEVICE_MAX_SAMPLERS>()			);
				}
				OPENCL_PRINT_STRING_2T( "Max size of kernel argument : %lld",					(*it).getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>()		);
				OPENCL_PRINT_STRING_2T( "Alignment (bits) of base address : %lld",				(*it).getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>()		);
				OPENCL_PRINT_STRING_2T( "Minimum alignment (bytes) for any data type : %lld",	(*it).getInfo<CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE>() );

				OPENCL_PRINT_STRING_2T( "Global memory size : %lld",							(*it).getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()			);
				C_PCHAR str;
				switch ( (*it).getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>() )
				{
				case CL_NONE:				str = "None";		break;
				case CL_READ_ONLY_CACHE:	str = "Read only";	break;
				case CL_READ_WRITE_CACHE:	str = "Read/Write";	break;
				default:					str = "Unknown";	break;
				}
					OPENCL_PRINT_STRING_3T( "Cache type : %s",					str );
					OPENCL_PRINT_STRING_3T( "Cache line size : %lld",				(*it).getInfo<CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE>()	);
					OPENCL_PRINT_STRING_3T( "Cache size : %lld",					(*it).getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>()		);
				OPENCL_PRINT_STRING_2T( "Constant buffer size : %lld",				(*it).getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>()		);
					OPENCL_PRINT_STRING_3T( "Max number of constant args : %lld",	(*it).getInfo<CL_DEVICE_MAX_CONSTANT_ARGS>()			);

				OPENCL_PRINT_STRING_2T( "Local memory size : %lld",					(*it).getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()				);
				switch ( (*it).getInfo<CL_DEVICE_LOCAL_MEM_TYPE>() )
				{
				case CL_LOCAL:	str = "Scratchpad";	break;
				case CL_GLOBAL:	str = "Global";		break;
				default:		str = "Unknown";	break;
				}
					OPENCL_PRINT_STRING_3T( "Local memory type : %s", str );

				OPENCL_PRINT_STRING_2T( "Profiling timer resolution : %lld",		(*it).getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION>()	);
				OPENCL_PRINT_STRING_2T( "Device endianness : %lld",					(*it).getInfo<CL_DEVICE_ENDIAN_LITTLE>()				);
				OPENCL_PRINT_STRING_2T( "Available : %lld",							(*it).getInfo<CL_DEVICE_AVAILABLE>()					);
				OPENCL_PRINT_STRING_2T( "Compiler available : %lld",				(*it).getInfo<CL_DEVICE_COMPILER_AVAILABLE>()			);
				OPENCL_PRINT_STRING_2T( "Execution capabilities :"			);
					OPENCL_PRINT_STRING_3T( "Execute OpenCL kernels : %s",			(*it).getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>() & CL_EXEC_KERNEL						? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "Execute native function : %s",			(*it).getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>() & CL_EXEC_NATIVE_KERNEL				? "Yes" : "No"	);
				OPENCL_PRINT_STRING_2T( "Queue properties :"				);
					OPENCL_PRINT_STRING_3T( "Out-of-Order : %s",					(*it).getInfo<CL_DEVICE_QUEUE_PROPERTIES>() & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE	? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "Profiling : %s",						(*it).getInfo<CL_DEVICE_QUEUE_PROPERTIES>() & CL_QUEUE_PROFILING_ENABLE					? "Yes" : "No"	);
				OPENCL_PRINT_STRING_2T( "Single precision floating point capability :" );
					auto device_info = (*it).getInfo<CL_DEVICE_SINGLE_FP_CONFIG>();
					OPENCL_PRINT_STRING_3T( "Denorms : %s",							device_info & CL_FP_DENORM				? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "Quiet NaNs : %s",						device_info & CL_FP_INF_NAN				? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "Round to nearest even : %s",			device_info & CL_FP_ROUND_TO_NEAREST	? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "Round to zero : %s",					device_info & CL_FP_ROUND_TO_ZERO		? "Yes" : "No"	);
					OPENCL_PRINT_STRING_3T( "IEEE754-2008 fused multiply-add : %s",	device_info & CL_FP_ROUND_TO_INF		? "Yes" : "No"	);
			}
		}
	}


	void	c_opencl::init()
	{
		static bool b_init = false;
		if( !b_init )
		{
			cl_int	err;
			{
				UINT32	winerr = wrap_OpenCL_Init(); // is implemented in both 'linked' and 'wrappped' form
				if( ERROR_SUCCESS!=winerr )
					return;
			}
			err = cl::Platform::get( &_platforms );
			if ( check_for_error( __FUNCTION__, "OpenCL : error getting Platform", err ) )
				return;
			if ( _platforms.size() == 0 )
			{
				ERR_PRINT_STRING( "OpenCL : error no Platform detected" );
				return;
			}
			gb_opencl_possible = true;
			print_platform_info();
			b_init = true;
		}
	}

	void	c_opencl::print_image_format( INT32 order, INT32 type )
	{
		C_PCHAR order_str;
		switch( order )
		{
		case CL_R:			order_str = "CL_R";				break;
		case CL_A:			order_str = "CL_A";				break;
		case CL_RG:			order_str = "CL_RG";			break;
		case CL_RA:			order_str = "CL_RA";			break;
		case CL_RGB: 		order_str = "CL_RGB";			break;
		case CL_RGBA:		order_str = "CL_RGBA";			break;
		case CL_BGRA:		order_str = "CL_BGRA";			break;
		case CL_ARGB:		order_str = "CL_ARGB";			break;
		case CL_INTENSITY:	order_str = "CL_INTENSITY";		break;
		case CL_LUMINANCE:	order_str = "CL_LUMINANCE";		break;
		case CL_Rx:			order_str = "CL_Rx";			break;
		case CL_RGx:		order_str = "CL_RGx";			break;
		case CL_RGBx:		order_str = "CL_RGBx";			break;
		default:			order_str = "unknown order";	break;
		}
		C_PCHAR type_str;
		switch( type )	// cl_channel_type 
		{
		case CL_SNORM_INT8:			type_str = "CL_SNORM_INT8";			break;
		case CL_SNORM_INT16:		type_str = "CL_SNORM_INT16";		break;
		case CL_UNORM_INT8:			type_str = "CL_UNORM_INT8";			break;
		case CL_UNORM_INT16:		type_str = "CL_UNORM_INT16";		break;
		case CL_UNORM_SHORT_565:	type_str = "CL_UNORM_SHORT_565";	break;
		case CL_UNORM_SHORT_555:	type_str = "CL_UNORM_SHORT_555";	break;
		case CL_UNORM_INT_101010:	type_str = "CL_UNORM_INT_101010";	break;
		case CL_SIGNED_INT8:		type_str = "CL_SIGNED_INT8";		break;
		case CL_SIGNED_INT16:		type_str = "CL_SIGNED_INT16";		break;
		case CL_SIGNED_INT32:		type_str = "CL_SIGNED_INT32";		break;
		case CL_UNSIGNED_INT8:		type_str = "CL_UNSIGNED_INT8";		break;
		case CL_UNSIGNED_INT16:		type_str = "CL_UNSIGNED_INT16";		break;
		case CL_UNSIGNED_INT32:		type_str = "CL_UNSIGNED_INT32";		break;
		case CL_HALF_FLOAT:			type_str = "CL_HALF_FLOAT";			break;
		case CL_FLOAT:				type_str = "CL_FLOAT";				break;
		default:					type_str = "unknown type";			break;
		}
		OPENCL_PRINT_STRING_1T( "%s\t%s", order_str, type_str );
	}

	void	c_opencl::print_supported_image_format( std::vector<cl::ImageFormat>&	image_formats )
	{
		OPENCL_PRINT_STRING( "supported image format are " );
		INT32 size = (INT32)image_formats.size();
		for( INT32 i=0; i<size; ++i )
		{
			cl::ImageFormat& format = image_formats[i];
			print_image_format( format.image_channel_order, format.image_channel_data_type );
		}
	}

	typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)
		(const cl_context_properties * /* properties */,
		cl_gl_context_info /* param_name */,
		size_t /* param_value_size */,
		void * /* param_value */,
		size_t * /*param_value_size_ret*/);

	INT32	c_opencl::choose_platform_default()
	{
		INT32 platform_index;
		if( first_nvidia_platform >= 0 )
		{
			OPENCL_PRINT_STRING( "forcing to first nvida platform %d", first_nvidia_platform );
			platform_index = first_nvidia_platform;
		}
		else if( second_gpu_platform >= 0 )
		{
			OPENCL_PRINT_STRING( "forcing to second platform with a GPU device %d", second_gpu_platform );
			platform_index = second_gpu_platform;
		}
		else if( first_gpu_platform >= 0 )
		{
			OPENCL_PRINT_STRING( "forcing to first platform with a GPU device %d", first_gpu_platform );
			platform_index = first_gpu_platform;
		}
		else
		{
			OPENCL_PRINT_STRING( "forcing to first platform" );
			platform_index = 0;
		}
		return platform_index;
	}

	void	c_opencl::setup( INT32 platform_index, UINT32 device_type, UINT32 device_index, bool b_use_opengl_context )
	{
		OPENCL_PRINT_STRING( "------------------------- setting up c_opencl" );
		cl_int	err;
		if( _platforms.empty() )
		{
			ERR_PRINT_STRING( "%s() : No platform.", __FUNCTION__ );
			goto exit;
		}

		if( !INSIDE_MIN_MAX( platform_index, 0, ((INT32)_platforms.size())-1 ) )
			platform_index = choose_platform_default();

		_platform_name.set( _platforms[ platform_index ].getInfo<CL_PLATFORM_NAME>().c_str() );
		OPENCL_PRINT_STRING_1T( "selecting platform %d (%s)", platform_index, _platform_name.get() );

		deinit();

		cl_device_type	loc_cl_device_type;
		switch( device_type )
		{
		case OPENCL_DEVICE_TYPE_ACCELERATOR:	loc_cl_device_type = CL_DEVICE_TYPE_ACCELERATOR;	break;
		case OPENCL_DEVICE_TYPE_CPU:			loc_cl_device_type = CL_DEVICE_TYPE_CPU;			break;
		case OPENCL_DEVICE_TYPE_GPU:			loc_cl_device_type = CL_DEVICE_TYPE_GPU;			break;
		case OPENCL_DEVICE_TYPE_DEFAULT :
		default:								loc_cl_device_type = CL_DEVICE_TYPE_DEFAULT;		break;
		}

		{
			cl_context_properties	properties[] =
			{
				CL_CONTEXT_PLATFORM, (cl_context_properties)_platforms[ platform_index ](),
				0
			};


			cl_context_properties	gl_properties[] =
			{
				CL_CONTEXT_PLATFORM,	(cl_context_properties)_platforms[ platform_index ](),
				CL_GL_CONTEXT_KHR,		(cl_context_properties)wglGetCurrentContext(),
				CL_WGL_HDC_KHR,			(cl_context_properties)wglGetCurrentDC(),
				0
			};

			cl_context_properties*	prop;
			if( b_use_opengl_context )
			{
				loc_cl_device_type	= CL_DEVICE_TYPE_GPU;
				device_type			= OPENCL_DEVICE_TYPE_GPU;
				prop = gl_properties;
			}
			else
			{
				prop = properties;
			}
			{
				OPENCL_PRINT_STRING_1T( "trying to create OpenCl context%s of type %s", b_use_opengl_context ? " with OpenGl Interop" : "", get_cl_device_type_str( loc_cl_device_type ) );
				INT32 index_prop = 0;
				while( prop[index_prop] )
				{
					OPENCL_PRINT_STRING_2T( " prop :\t0x%x\t:\t0x%x", prop[index_prop], prop[index_prop+1] );
					index_prop += 2 ;
				}
			}

			{
				clGetGLContextInfoKHR_fn _clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn )clGetExtensionFunctionAddress( "clGetGLContextInfoKHR" );

				if( _clGetGLContextInfoKHR && reinterpret_cast<uintptr_t>(_clGetGLContextInfoKHR) != -1 )
				{
					OPENCL_PRINT_STRING_1T( "try with clGetGLContextInfoKHR" );
					cl_device_id	loc_devices[32];
					size_t			size;
					INT32 ret = _clGetGLContextInfoKHR( prop, CL_DEVICES_FOR_GL_CONTEXT_KHR, 32 * sizeof( cl_device_id ), loc_devices, &size );
					if( ret == CL_SUCCESS && size > 0 )
					{
						/*
						VECTOR_CLASS< class cl::Device > tmp;
						for( INT32 i=0; i<size; ++i )
							tmp.push_back( loc_devices[i] );
						*/
						//because size is in bytes
						_context = new cl::Context( prop, 1, loc_devices, nullptr, nullptr, &err );
						if( _context == 0 )
							ERR_PRINT_STRING( "new cl::Context() returned a NULL context" );
					}
					else
						ERR_PRINT_STRING( "_clGetGLContextInfoKHR() did not return a device" );
				}
			}

			if( !_context )
			{
				OPENCL_PRINT_STRING_1T( "try with just with the properties" );
				_context = new cl::Context( loc_cl_device_type, prop, nullptr, nullptr, &err );
			}
		}

		if( _context == 0 )
			ERR_PRINT_STRING( "new cl::Context() returned a NULL context" );
		if( check_for_error( __FUNCTION__, "OpenCL : error getting context", err ) || _context== 0 )
			goto exit;
		else
		{
			OPENCL_PRINT_STRING_1T( "created OpenCL context" );
			_b_gl_context = b_use_opengl_context;		// todo maybe check context result b4
			if( _b_gl_context )
				OPENCL_PRINT_STRING_2T( "using GL context" );
		}

		_devices = _context->getInfo<CL_CONTEXT_DEVICES>( &err );
		if( check_for_error( __FUNCTION__, "OpenCL : error getting device", err ) )
			goto exit;

		if( _devices.size() == 0 ) 
		{
			ERR_PRINT_STRING( "OpenCL : no device available" );
			goto exit;
		}
		if( device_index >= _devices.size() )
		{
			ERR_PRINT_STRING( "OpenCL : asking for device %d but only %d device(s) exist", device_index, _devices.size() );
			goto exit;
		}
		OPENCL_PRINT_STRING_1T( "Device type %s index %d", str_cl_device_type[ device_type ], device_index );

		_device = _devices[device_index];

		_device_name.set(			_devices[device_index].getInfo<CL_DEVICE_NAME>().c_str()		);
			OPENCL_PRINT_STRING_2T( "Name : %s",			_device_name.get()						);
		_device_vendor.set(			_devices[device_index].getInfo<CL_DEVICE_VENDOR>().c_str()		);
			OPENCL_PRINT_STRING_2T( "Vendor : %s",			_device_vendor.get()					);
		_device_version.set(		_devices[device_index].getInfo<CL_DEVICE_VERSION>().c_str()		);
			OPENCL_PRINT_STRING_2T( "Version : %s",			_device_version.get()					);
		_device_driver_version.set(	_devices[device_index].getInfo<CL_DRIVER_VERSION>().c_str()		);
			OPENCL_PRINT_STRING_2T( "Driver Version : %s",	_device_driver_version.get()			);
		_device_extension.set(		_devices[device_index].getInfo<CL_DEVICE_EXTENSIONS>().c_str()	);
			OPENCL_PRINT_STRING_2T( "Extensions : "													);
			PRINT_STRING_BY_BLOCK( _device_extension.get(), 80 );

		//	this should be refined
		if( have_extension( "cl_khr_gl_sharing" ) )
		{
			_b_opengl_can = true;
			OPENCL_PRINT_STRING_2T( "supports GL Interop." );
		}

		if( gb_opencl_image_format_verbose_ui )
		{
			std::vector<cl::ImageFormat>	image_formats;
			cl_int status = _context->getSupportedImageFormats(
												CL_MEM_READ_ONLY,	//cl_mem_flags flags,
												CL_MEM_OBJECT_IMAGE2D,	//cl_mem_object_type type,
												&image_formats );
			if( status == CL_SUCCESS )
			{
				print_supported_image_format( image_formats );
			}
			else
			{
				ERR_PRINT_STRING( "could not read texture supported formats" );
			}
		}

		_queue = new cl::CommandQueue( *_context, _devices[device_index], 0, &err );
		if ( check_for_error( __FUNCTION__, "OpenCL : error getting device", err) )
			goto exit;
		else
		{
			OPENCL_PRINT_STRING( "------------------------- queue created" );
			PRINT_CR();
		}
		_b_valid = true;
exit:
		return;
	}

	bool	c_opencl::build()
	{
		_b_program_build = false;
		if( _program )
		{
			if( _program->build( _devices ) != CL_SUCCESS )
			{
				cl::STRING_CLASS str = _program->getBuildInfo<CL_PROGRAM_BUILD_LOG>( _devices[0] );
				ERR_PRINT_STRING( "OpenCL : Error building program  %s", str.c_str() );
			}
			else
			{
				_b_program_build = true;
				OPENCL_PRINT_STRING( "Program built" );
			}
		}
		return _b_program_build;
	}

	bool	c_opencl::program_build_from_source( std::vector< o_str CONST *> CONST * sources )
	{
		cl::Program::Sources	program_src;
		for( o_str CONST * elt : *sources )
			program_src.push_back( std::make_pair( elt->get(), elt->get_len() ) );
	
		SAFE_DELETE( _program );
		cl_int	err;
		_program = new cl::Program( *_context, program_src, &err );
		if ( check_for_error( __FUNCTION__, "OpenCL Program : error creating program", err ) )
			return false;
		else
			OPENCL_PRINT_STRING( "creating program" );
		return build();
	}

	bool	c_opencl::program_build_from_file( o_str CONST & filename, o_str CONST & include, bool isBinary )
	{ 
		if( isBinary || !_context )
			return false;

		o_str	src;
		AAA_ERR	error = src.read_file( filename );
		if( ERR(error) )
		{
			ERR_PRINT_STRING( "OpenCL : could not read \"%s\"", filename.get() );
			return false;
		}

		OPENCL_PRINT_STRING( "loading %s", filename.get() );
		std::vector< o_str CONST * > sources;
		sources.push_back( &include );
		o_str include_text[9];
		for( int i=1; i<=9; ++i )
		{
			o_str& include_fname = o_str::push_name( c_dir::get_kernel() ); // this is the aaa Kernel not apoencl related here
				include_fname.add( "/OpenCl/include_0" );
				include_fname.add_char( '0'+i );
				include_fname.add( ".cl" );
				if( c_file::is_exist( include_fname ) )
				{
					AAA_ERR	error = include_text[i-1].read_file( include_fname );
					if( NOERR(error) )
						sources.push_back( &include_text[i-1] );
				}
			o_str::pop_name();
		}

		sources.push_back( &src );
		return program_build_from_source( &sources );
	}

	bool	c_opencl::gl_objects_acquire( VECTOR_CLASS<cl::Memory>* v, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		if( _queue )
		{
			SPY_PUSH_RANGE( "gl_objects_acquire", spy::OPENCL );
				cl_int	status = _queue->enqueueAcquireGLObjects( v, events, event );
			SPY_POP_RANGE();
			if( check_for_error( __FUNCTION__, "enqueueAcquireGLObjects", status) )
				return false;
			return true;
		}
		return false;
	}

	bool	c_opencl::gl_objects_release( VECTOR_CLASS<cl::Memory>* v, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		if( _queue )
		{
			SPY_PUSH_RANGE( "gl_objects_release", spy::OPENCL );
				cl_int	status = _queue->enqueueReleaseGLObjects( v, events, event );
			SPY_POP_RANGE();
			if ( check_for_error( __FUNCTION__, "enqueueReleaseGLObjects", status) )
				return false;
			return true;
		}
		return false;
	}

	cl::Memory*	c_opencl::buffer_render_gl_create( cl_mem_flags flags, UINT32 render_index )
	{
		cl::Memory*	buffer = nullptr;
		if( _b_gl_context )
		{
			cl_int	status;
			buffer = new cl::BufferRenderGL( *_context, flags, render_index, &status );
			if ( check_for_error( __FUNCTION__, "cl::Buffer failed", status ) )
				SAFE_DELETE( buffer );
		}
		return buffer;
	}

	cl::Image2DGL*	c_opencl::image_2d_gl_create( cl_mem_flags flags, GLuint tex_name )
	{
		cl::Image2DGL*	image = nullptr;
		if( _b_gl_context )
		{
			cl_int	status;
			image = new cl::Image2DGL( *_context, flags, GL_TEXTURE_2D, 0, tex_name, &status );
			if ( check_for_error( __FUNCTION__, "cl::Image2DGL failed", status ) )
				SAFE_DELETE( image );
		}
		return image;
	}

	cl::Memory*	c_opencl::buffer_gl_create( cl_mem_flags flags, UINT32 tex_id )
	{
		cl::Memory*		buffer = nullptr;
		if( _b_gl_context )
		{
			cl_int	status;
			buffer = new cl::BufferGL( *_context, flags, tex_id, &status );
			if ( check_for_error( __FUNCTION__, "cl::Buffer failed", status ) )
				SAFE_DELETE( buffer );
		}
		return buffer;
	}

	//void*	c_opencl::buffer_gl_map_read( cl::Buffer* buffer, bool b_blocking, INT32 offset, INT32 size, CONST VECTOR_CLASS<cl::Event>* events = nullptr, cl::Event* event  = nullptr )
	//{
	//	cl_int	status;
	//	void*	tmp = nullptr;
	//	//	void* enqueueMapBuffer(CONST Buffer& buffer,cl_bool blocking,cl_map_flags flags,::size_t offset,::size_t size,CONST VECTOR_CLASS<Event>* events = nullptr, Event* event = nullptr, cl_int* err = nullptr) CONST
	//	tmp = _queue->enqueueMapBuffer( *buffer, b_blocking ? CL_TRUE : CL_FALSE, CL_MAP_READ, offset, size, events, event, &status );
	//	if ( check_for_error( __FUNCTION__, "cl::Buffer write enqueue", status ) )
	//		return tmp;
	//	return tmp;
	//}

	cl::Buffer*	c_opencl::buffer_create( cl_mem_flags flags, UINT64 size, void* data )
	{
		cl_int		status;
		cl::Buffer* buffer = new cl::Buffer( *_context, flags, size, data, &status );
		if ( check_for_error( __FUNCTION__, "cl::Buffer failed", status) )
			SAFE_DELETE( buffer );
		return buffer;
	}

	bool	c_opencl::buffer_write( cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, void* data, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		SPY_PUSH_RANGE( "buffer_write", spy::OPENCL );
			cl_int status = _queue->enqueueWriteBuffer( *buffer, b_blocking ? CL_TRUE : CL_FALSE, offset, size, data, events, event );
		SPY_POP_RANGE();
		if ( check_for_error( __FUNCTION__, "cl::Buffer write enqueue", status) )
			return false;
		return true;
	}

	bool	c_opencl::buffer_read( cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, void* data, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		if( _queue )
		{
			SPY_PUSH_RANGE( "buffer_read", spy::OPENCL );
				cl_int	status = _queue->enqueueReadBuffer( *buffer, b_blocking ? CL_TRUE : CL_FALSE, offset, size, data, events, event );
			SPY_POP_RANGE();
			if ( check_for_error( __FUNCTION__, "cl::Buffer read enqueue", status) )
				return false;
			return true;
		}
		return false;
	}

	void*	c_opencl::buffer_map_read( cl::Buffer* buffer, bool b_blocking, UINT64 offset, UINT64 size, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		if( _queue )
		{
			cl_int	status;
			SPY_PUSH_RANGE( "buffer_map_read", spy::OPENCL );
			//void*	tmp = nullptr;
			//	void* enqueueMapBuffer(CONST Buffer& buffer,cl_bool blocking,cl_map_flags flags,::size_t offset,::size_t size,CONST VECTOR_CLASS<Event>* events = nullptr,Event* event = nullptr,cl_int* err = nullptr) CONST
				void* tmp = _queue->enqueueMapBuffer( *buffer, b_blocking ? CL_TRUE : CL_FALSE, CL_MAP_READ, offset, size, events, event, &status );
			SPY_POP_RANGE();
			if( check_for_error( __FUNCTION__, "cl::Buffer map enqueue", status ) )
				return tmp;
			return tmp;
		}
		return nullptr;
	}

	bool	c_opencl::kernel_run( CONST cl::NDRange& offset, CONST cl::NDRange& global, CONST cl::NDRange& local, CONST VECTOR_CLASS<cl::Event>* events, cl::Event* event )
	{
		if( _kernel && _queue )
		{
			SPY_PUSH_RANGE( "kernel_run", spy::OPENCL );
			cl_int	status = _queue->enqueueNDRangeKernel( *_kernel, offset, global, local, events, event );
			SPY_POP_RANGE();
			if ( check_for_error( __FUNCTION__, "cl::Kernel run", status ) )
				return false;
			return true;
		}
		return false;
	}

	bool	c_opencl::kernel_load( o_str* kernel_name )
	{
		if( !_program )
			return false;

		auto kernel = _kernels[ kernel_name ];
		if( kernel )
		{
			delete kernel;
			_kernels[ kernel_name ] =  nullptr;
		}

		cl_int		err;
		C_PCHAR_C name = kernel_name->get();
		kernel = new cl::Kernel( *_program, name,  &err );
		if( check_for_error( __FUNCTION__, "OpenCL : error creating kernel", err ) )
		{
			ERR_PRINT_STRING( "OpenCL : error creating kernel %s", name );
			return false;
		}
		OPENCL_PRINT_STRING( "Kernel %s created", name );
		//todox64 do it better
		_kernel_work_group_size = (INT32) kernel->getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>( _devices[0], &err );
		check_for_error( __FUNCTION__, "OpenCL : failed getting kernelWorkGroupSize", err );
		_kernels[ kernel_name ] = kernel;
		_kernel = kernel;
		return true;
	}

	cl::KernelFunctor*	c_opencl::kernel_bind( o_str* kernel_name, CONST cl::NDRange& offset, CONST cl::NDRange& global, CONST cl::NDRange& local )
	{
		cl::Kernel*	k = _kernels[ kernel_name];
		if( !k )
		{
			if( !kernel_load( kernel_name ) )
				return nullptr;
			k = _kernel;
		}
		if ( k )
		{
			_kernel = k;
			cl::KernelFunctor*	functor = new cl::KernelFunctor( _kernel->bind( *_queue, offset, global, local ) );
			return functor;
		}
		return nullptr;
	}

	bool	c_opencl::kernel_bind( o_str* kernel_name  )
	{
		cl::Kernel*	k = _kernels[ kernel_name ];
		if( k )
		{
			_kernel = k;
			return true;
		}
		else
		{
			return kernel_load( kernel_name );
		}
	}

	void	c_opencl::flush()
	{
		if( _queue )
		{
			SPY_PUSH_RANGE( "flush", spy::OPENCL );
				cl_int	status = _queue->flush();
			SPY_POP_RANGE();
			check_for_error( __FUNCTION__, "cl::queue flush failed", status );
		}
	}

	void	c_opencl::finish()
	{
		if( _queue )
		{
			SPY_PUSH_RANGE( "finish", spy::OPENCL );
				cl_int	status = _queue->finish();
			SPY_POP_RANGE();
			check_for_error( __FUNCTION__, "cl::queue finish failed", status );
		}
	}

	void	c_opencl::close()
	{
		finish();
		_kernel = nullptr;
		for( auto const & elt : _kernels )
			delete elt.second;
		_kernels.clear();

		SAFE_DELETE( _program );
		SAFE_DELETE( _queue );
	}

	void	c_opencl::deinit()
	{
		close();
		
		SAFE_DELETE( _context );
		_device = nullptr;
		_devices.clear();
	}
#endif //#if AAA_OPENCL_USE()

	void	init()
	{
#if AAA_OPENCL_USE()
		//hack we trying to add this to solve a problem in Wuhan
		if( c_opencl::gb_opencl_allow_ui )
			c_opencl::init();
#endif //#if AAA_OPENCL_USE()
	}
}

