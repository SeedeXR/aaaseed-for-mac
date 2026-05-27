// File: wrap_OpenCL.cpp - wrappers for the "OpenCL.dll"
//
// Created: sr@20100415, generalized wrappers: sr@20100502
//

#include "aaa_opencl.h"

#include "aaa_def.h"

#define		MULTI_SECTION_NAME	0
#define	WRAP_SECTION_NAME	"OPENCL$a"
#include "platform/win32/wrap_dll.h"		// wrapping macros
#include "wrap_OpenCL.h"					// my own header


#if AAA_OPENCL_USE()
#	define USE_LINKED_OPENCLLIB
#endif

#	define USE_LINKED_OPENCLLIB		// protector


#ifdef	USE_LINKED_OPENCLLIB		// do wrapped DLL calls //sr@20100415

#include <lib_use.h>
AAA_LIB_USE( "OpenCL" )

#define		WRAPPER_NAME	OpenCL
//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_OpenCL_Init ( void )
{
	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );			// just give a printout
}//wrap_OpenCL_init
//-----------------------------------------------------------------------------
UINT32	wrap_OpenCL_Term ( void )
{
	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a printout
}//wrap_OpenCL_Term
//-----------------------------------------------------------------------------

#else

//static	int toto[2048];
//toto[3] = 5;

#pragma message ("**********************************************************************")
#pragma message ("*** USING OpenCL wrapper. Stack is unbalanced after UnresolvedCall ***")
#pragma message (" Please read the comment in the wrap_OpenCL.cpp file sr@20100416	 ***")
#pragma message ("**********************************************************************")

//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we use "__stdcall", since the 
//	callee thinks so. The library function will balance the stack (as promised)
//	The problem arise only, when calling the undefined function, which should
//	not be the case (in a good application) after the wrap_OpenCL_Init returns 
//	an error.

#	undef	CL_API_ENTRY
#	define	CL_API_ENTRY	__declspec(naked)		// we need it "naked" to skip the prologue code
#	undef	CL_API_CALL
#	define	CL_API_CALL		__stdcall

#	define	CL_API_SUFFIX__VERSION_1_				// some "versioning protection" stuff

//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( OPENCL, "OpenCL.dll" )
//
// ============================================================================

// ============================================================================
//
// Wrappers, see the "OpenCL.h" for semantic declarations
//
//----------------------------------------------- ------------------------------
CL_API_ENTRY cl_int CL_API_CALL
clGetPlatformIDs(	cl_uint				/* num_entries */,
					cl_platform_id *	/* platforms */,
					cl_uint *			/* num_platforms */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetPlatformIDs )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL 
clGetPlatformInfo(	cl_platform_id		/* platform */, 
					cl_platform_info	/* param_name */,
					size_t				/* param_value_size */, 
					void *				/* param_value */,
					size_t *			/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetPlatformInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDs(		cl_platform_id		/* platform */,
					cl_device_type		/* device_type */, 
					cl_uint				/* num_entries */, 
					cl_device_id *		/* devices */, 
					cl_uint *			/* num_devices */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetDeviceIDs )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(	cl_device_id		/* device */,
					cl_device_info		/* param_name */, 
					size_t				/* param_value_size */, 
					void *				/* param_value */,
					size_t *			/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetDeviceInfo )
//-----------------------------------------------------------------------------

#if 1
extern	CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(	const cl_context_properties *	/* properties */,
					cl_uint							/* num_devices */,
					const cl_device_id *			/* devices */,
					void (CL_CALLBACK *				/* pfn_notify*/ )(const char *, const void *, size_t, void *) /* pfn_notify */,
					void *							/* user_data */,
					cl_int *						/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateContext )
//-----------------------------------------------------------------------------
extern	CL_API_ENTRY cl_context CL_API_CALL
clCreateContextFromType(	const cl_context_properties *	/* properties */,
							cl_device_type					/* device_type */,
							void (CL_CALLBACK *				/* pfn_notify*/ )(const char *, const void *, size_t, void *),
							void *							/* user_data */,
							cl_int *						/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateContextFromType )
#else
CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(	const cl_context_properties *	/* properties */,
					cl_uint							/* num_devices */,
					const cl_device_id *			/* devices */,
					void (*pfn_notify)(const char *, const void *, size_t, void *) /* pfn_notify */,
					void *							/* user_data */,
					cl_int *						/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateContext )
//-----------------------------------------------------------------------------
CL_API_ENTRY cl_context CL_API_CALL
clCreateContextFromType(	const cl_context_properties *	/* properties */,
							cl_device_type					/* device_type */,
							void (*pfn_notify)(const char *, const void *, size_t, void *) /* pfn_notify */,
							void *							/* user_data */,
							cl_int *						/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateContextFromType )
#endif
//-----------------------------------------------------------------------------
#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$b"
#endif // MULTI_SECTION_NAME


CL_API_ENTRY cl_int CL_API_CALL
clRetainContext(	cl_context /* context */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainContext )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseContext(	cl_context /* context */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseContext )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetContextInfo(	cl_context		/* context */, 
					cl_context_info	/* param_name */, 
					size_t			/* param_value_size */, 
					void *			/* param_value */, 
					size_t *		/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetContextInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_command_queue CL_API_CALL
clCreateCommandQueue(	cl_context					/* context */, 
						cl_device_id				/* device */, 
						cl_command_queue_properties	/* properties */,
						cl_int *					/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateCommandQueue )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainCommandQueue(	cl_command_queue	/* command_queue */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainCommandQueue )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseCommandQueue(	cl_command_queue	/* command_queue */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseCommandQueue )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetCommandQueueInfo(	cl_command_queue		/* command_queue */,
						cl_command_queue_info	/* param_name */,
						size_t					/* param_value_size */,
						void *					/* param_value */,
						size_t *				/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetCommandQueueInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clSetCommandQueueProperty(	cl_command_queue				/* command_queue */,
							cl_command_queue_properties		/* properties */, 
							cl_bool							/* enable */,
							cl_command_queue_properties *	/* old_properties */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clSetCommandQueueProperty )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(	cl_context		/* context */,
				cl_mem_flags	/* flags */,
				size_t			/* size */,
				void *			/* host_ptr */,
				cl_int *		/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateBuffer )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$c"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_mem CL_API_CALL
clCreateImage2D(	cl_context				/* context */,
					cl_mem_flags			/* flags */,
					const cl_image_format * /* image_format */,
					size_t					/* image_width */,
					size_t					/* image_height */,
					size_t					/* image_row_pitch */, 
					void *					/* host_ptr */,
					cl_int *				/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateImage2D )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_mem CL_API_CALL
clCreateImage3D(	cl_context				/* context */,
					cl_mem_flags			/* flags */,
					const cl_image_format *	/* image_format */,
					size_t					/* image_width */, 
					size_t					/* image_height */,
					size_t					/* image_depth */, 
					size_t					/* image_row_pitch */, 
					size_t					/* image_slice_pitch */, 
					void *					/* host_ptr */,
					cl_int *				/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateImage3D )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainMemObject(	cl_mem /* memobj */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainMemObject )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseMemObject(	cl_mem /* memobj */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseMemObject )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetSupportedImageFormats(	cl_context			/* context */,
							cl_mem_flags		/* flags */,
							cl_mem_object_type	/* image_type */,
							cl_uint				/* num_entries */,
							cl_image_format *	/* image_formats */,
							cl_uint *			/* num_image_formats */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetSupportedImageFormats )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(	cl_mem		/* memobj */,
					cl_mem_info	/* param_name */, 
					size_t		/* param_value_size */,
					void *		/* param_value */,
					size_t *	/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetMemObjectInfo	)
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetImageInfo(	cl_mem			/* image */,
				cl_image_info	/* param_name */, 
				size_t			/* param_value_size */,
				void *			/* param_value */,
				size_t *		/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetImageInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_sampler CL_API_CALL
clCreateSampler(	cl_context			/* context */,
					cl_bool				/* normalized_coords */, 
					cl_addressing_mode	/* addressing_mode */, 
					cl_filter_mode		/* filter_mode */,
					cl_int *			/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateSampler )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainSampler(	cl_sampler /* sampler */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainSampler )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$d"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_int CL_API_CALL
clReleaseSampler(	cl_sampler /* sampler */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseSampler )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetSamplerInfo(	cl_sampler		/* sampler */,
					cl_sampler_info	/* param_name */,
					size_t			/* param_value_size */,
					void *			/* param_value */,
					size_t *		/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetSamplerInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(	cl_context		/* context */,
							cl_uint			/* count */,
							const char **	/* strings */,
							const size_t *	/* lengths */,
							cl_int *		/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateProgramWithSource )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithBinary(	cl_context				/* context */,
							cl_uint					/* num_devices */,
							const cl_device_id *	/* device_list */,
							const size_t *			/* lengths */,
							const unsigned char **	/* binaries */,
							cl_int *				/* binary_status */,
							cl_int *				/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateProgramWithBinary )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainProgram(	cl_program /* program */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainProgram )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseProgram(	cl_program /* program */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseProgram )
//-----------------------------------------------------------------------------
#if 1
extern CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(	cl_program				/* program */,
				cl_uint					/* num_devices */,
				const cl_device_id *	/* device_list */,
				const char *			/* options */, 
				void (CL_CALLBACK *		/* pfn_notify */)(cl_program /* program */, void * /* user_data */),
				void *					/* user_data */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clBuildProgram )
#else
CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(	cl_program				/* program */,
				cl_uint					/* num_devices */,
				const cl_device_id *	/* device_list */,
				const char *			/* options */, 
				void (*pfn_notify)(cl_program /* program */, void * /* user_data */),
				void *					/* user_data */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clBuildProgram )
#endif
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clUnloadCompiler(	void	)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clUnloadCompiler )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramInfo(	cl_program			/* program */,
					cl_program_info		/* param_name */,
					size_t				/* param_value_size */,
					void *				/* param_value */,
					size_t *			/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetProgramInfo )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$e"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramBuildInfo(	cl_program				/* program */,
						cl_device_id			/* device */,
						cl_program_build_info	/* param_name */,
						size_t					/* param_value_size */,
						void *					/* param_value */,
						size_t *				/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetProgramBuildInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(	cl_program		/* program */,
				const char *	/* kernel_name */,
				cl_int *		/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateKernel )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clCreateKernelsInProgram(	cl_program		/* program */,
							cl_uint			/* num_kernels */,
							cl_kernel *		/* kernels */,
							cl_uint *		/* num_kernels_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateKernelsInProgram )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainKernel(	cl_kernel		/* kernel */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP(	clRetainKernel )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseKernel(	cl_kernel	/* kernel */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseKernel )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(	cl_kernel		/* kernel */,
				cl_uint			/* arg_index */,
				size_t			/* arg_size */,
				const void *	/* arg_value */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clSetKernelArg )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetKernelInfo(	cl_kernel		/* kernel */,
					cl_kernel_info	/* param_name */,
					size_t			/* param_value_size */,
					void *			/* param_value */,
					size_t *		/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetKernelInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetKernelWorkGroupInfo(	cl_kernel					/* kernel */,
							cl_device_id				/* device */,
							cl_kernel_work_group_info	/* param_name */,
							size_t						/* param_value_size */,
							void*						/* param_value */,
							size_t*						/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetKernelWorkGroupInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clWaitForEvents(	cl_uint				/* num_events */,
					const cl_event *	/* event_list */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clWaitForEvents )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$f"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_int CL_API_CALL
clGetEventInfo(	cl_event		/* event */,
				cl_event_info	/* param_name */,
				size_t			/* param_value_size */,
				void *			/* param_value */,
				size_t *		/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetEventInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clRetainEvent(	cl_event /* event */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clRetainEvent	)
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clReleaseEvent(	cl_event /* event */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clReleaseEvent	)
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clGetEventProfilingInfo(	cl_event			/* event */,
							cl_profiling_info	/* param_name */,
							size_t				/* param_value_size */,
							void *				/* param_value */,
							size_t *			/* param_value_size_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetEventProfilingInfo )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clFlush(	cl_command_queue /* command_queue */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clFlush )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clFinish(	cl_command_queue /* command_queue */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clFinish )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBuffer(	cl_command_queue	/* command_queue */,
						cl_mem				/* buffer */,
						cl_bool				/* blocking_read */,
						size_t				/* offset */,
						size_t				/* cb */, 
						void *				/* ptr */,
						cl_uint				/* num_events_in_wait_list */,
						const cl_event *	/* event_wait_list */,
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueReadBuffer )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBuffer(	cl_command_queue	/* command_queue */, 
						cl_mem				/* buffer */, 
						cl_bool				/* blocking_write */, 
						size_t				/* offset */, 
						size_t				/* cb */, 
						const void *		/* ptr */, 
						cl_uint				/* num_events_in_wait_list */, 
						const cl_event *	/* event_wait_list */, 
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueWriteBuffer )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBuffer(	cl_command_queue	/* command_queue */, 
						cl_mem				/* src_buffer */,
						cl_mem				/* dst_buffer */, 
						size_t				/* src_offset */,
						size_t				/* dst_offset */,
						size_t				/* cb */, 
						cl_uint				/* num_events_in_wait_list */,
						const cl_event *	/* event_wait_list */,
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueCopyBuffer )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$g"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadImage(	cl_command_queue	/* command_queue */,
					cl_mem				/* image */,
					cl_bool				/* blocking_read */, 
					const size_t *		/* origin[3] */,
					const size_t *		/* region[3] */,
					size_t				/* row_pitch */,
					size_t				/* slice_pitch */, 
					void *				/* ptr */,
					cl_uint				/* num_events_in_wait_list */,
					const cl_event *	/* event_wait_list */,
					cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueReadImage )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteImage(	cl_command_queue	/* command_queue */,
						cl_mem				/* image */,
						cl_bool				/* blocking_write */, 
						const size_t *		/* origin[3] */,
						const size_t *		/* region[3] */,
						size_t				/* input_row_pitch */,
						size_t				/* input_slice_pitch */, 
						const void *		/* ptr */,
						cl_uint				/* num_events_in_wait_list */,
						const cl_event *	/* event_wait_list */,
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueWriteImage )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyImage(	cl_command_queue	/* command_queue */,
					cl_mem				/* src_image */,
					cl_mem				/* dst_image */, 
					const size_t *		/* src_origin[3] */,
					const size_t *		/* dst_origin[3] */,
					const size_t *		/* region[3] */, 
					cl_uint				/* num_events_in_wait_list */,
					const cl_event *	/* event_wait_list */,
					cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueCopyImage )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyImageToBuffer(	cl_command_queue	/* command_queue */,
							cl_mem				/* src_image */,
							cl_mem				/* dst_buffer */, 
							const size_t *		/* src_origin[3] */,
							const size_t *		/* region[3] */, 
							size_t				/* dst_offset */,
							cl_uint				/* num_events_in_wait_list */,
							const cl_event *	/* event_wait_list */,
							cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueCopyImageToBuffer )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBufferToImage(	cl_command_queue	/* command_queue */,
							cl_mem				/* src_buffer */,
							cl_mem				/* dst_image */, 
							size_t				/* src_offset */,
							const size_t *		/* dst_origin[3] */,
							const size_t *		/* region[3] */, 
							cl_uint				/* num_events_in_wait_list */,
							const cl_event *	/* event_wait_list */,
							cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueCopyBufferToImage )
//-----------------------------------------------------------------------------

CL_API_ENTRY void *CL_API_CALL
clEnqueueMapBuffer(	cl_command_queue	/* command_queue */,
					cl_mem				/* buffer */,
					cl_bool				/* blocking_map */, 
					cl_map_flags		/* map_flags */,
					size_t				/* offset */,
					size_t				/* cb */,
					cl_uint				/* num_events_in_wait_list */,
					const cl_event *	/* event_wait_list */,
					cl_event *			/* event */,
					cl_int *			/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueMapBuffer )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$h"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY void *CL_API_CALL
clEnqueueMapImage(	cl_command_queue	/* command_queue */,
					cl_mem				/* image */, 
					cl_bool				/* blocking_map */, 
					cl_map_flags		/* map_flags */, 
					const size_t *		/* origin[3] */,
					const size_t *		/* region[3] */,
					size_t *			/* image_row_pitch */,
					size_t *			/* image_slice_pitch */,
					cl_uint				/* num_events_in_wait_list */,
					const cl_event *	/* event_wait_list */,
					cl_event *			/* event */,
					cl_int *			/* errcode_ret */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueMapImage )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueUnmapMemObject(	cl_command_queue	/* command_queue */,
							cl_mem				/* memobj */,
							void *				/* mapped_ptr */,
							cl_uint				/* num_events_in_wait_list */,
							const cl_event *	/* event_wait_list */,
							cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueUnmapMemObject )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNDRangeKernel(	cl_command_queue	/* command_queue */,
						cl_kernel			/* kernel */,
						cl_uint				/* work_dim */,
						const size_t *		/* global_work_offset */,
						const size_t *		/* global_work_size */,
						const size_t *		/* local_work_size */,
						cl_uint				/* num_events_in_wait_list */,
						const cl_event *	/* event_wait_list */,
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueNDRangeKernel )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueTask(	cl_command_queue	/* command_queue */,
				cl_kernel			/* kernel */,
				cl_uint				/* num_events_in_wait_list */,
				const cl_event *	/* event_wait_list */,
				cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueTask )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNativeKernel(	cl_command_queue	/* command_queue */,
						void (*user_func)(void *), 
						void *				/* args */,
						size_t				/* cb_args */, 
						cl_uint				/* num_mem_objects */,
						const cl_mem *		/* mem_list */,
						const void **		/* args_mem_loc */,
						cl_uint				/* num_events_in_wait_list */,
						const cl_event *	/* event_wait_list */,
						cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueNativeKernel )
//--------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueMarker(	cl_command_queue	/* command_queue */,
					cl_event *			/* event */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueMarker )
//-----------------------------------------------------------------------------

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWaitForEvents(	cl_command_queue	/* command_queue */,
						cl_uint				/* num_events */,
						const cl_event *	/* event_list */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueWaitForEvents )
//-----------------------------------------------------------------------------

#if MULTI_SECTION_NAME
#define	WRAP_SECTION_NAME	"OPENCL$i"
#endif // MULTI_SECTION_NAME

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueBarrier(	cl_command_queue	/* command_queue */)CL_API_SUFFIX__VERSION_1_
	WRAP_CALL2JUMP( clEnqueueBarrier )
//-----------------------------------------------------------------------------

CL_API_ENTRY void *CL_API_CALL
clGetExtensionFunctionAddress(	const char *	/* func_name */)CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetExtensionFunctionAddress )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLBuffer(	cl_context		/* context */,
						cl_mem_flags	/* flags */,
						cl_GLuint		/* bufobj */,
						int *			/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateFromGLBuffer )
//-----------------------------------------------------------------------------

//depreciated on 1.2
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLTexture2D(	cl_context		/* context */,
							cl_mem_flags	/* flags */,
							cl_GLenum		/* target */,
							cl_GLint		/* miplevel */,
							cl_GLuint		/* texture */,
							cl_int *		/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateFromGLTexture2D )
//-----------------------------------------------------------------------------
//	same than previous but 1.2
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLTexture(		cl_context		/* context */,
							cl_mem_flags	/* flags */,
							cl_GLenum		/* target */,
							cl_GLint		/* miplevel */,
							cl_GLuint		/* texture */,
							cl_int *		/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateFromGLTexture )
//-----------------------------------------------------------------------------
//depreciated on 1.2 ?
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLTexture3D(	cl_context		/* context */,
							cl_mem_flags	/* flags */,
							cl_GLenum		/* target */,
							cl_GLint		/* miplevel */,
							cl_GLuint		/* texture */,
							cl_int *		/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateFromGLTexture3D )
//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromGLRenderbuffer(	cl_context		/* context */,
							cl_mem_flags	/* flags */,
							cl_GLuint		/* renderbuffer */,
							cl_int *		/* errcode_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clCreateFromGLRenderbuffer )
//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_int CL_API_CALL
clGetGLObjectInfo(	cl_mem					/* memobj */,
					cl_gl_object_type *		/* gl_object_type */,
					cl_GLuint *				/* gl_object_name */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetGLObjectInfo )
//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_int CL_API_CALL
clGetGLTextureInfo(	cl_mem				/* memobj */,
					cl_gl_texture_info	/* param_name */,
					size_t				/* param_value_size */,
					void *				/* param_value */,
					size_t *			/* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clGetGLTextureInfo )
//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireGLObjects(	cl_command_queue	/* command_queue */,
							cl_uint				/* num_objects */,
							const cl_mem *		/* mem_objects */,
							cl_uint				/* num_events_in_wait_list */,
							const cl_event *	/* event_wait_list */,
							cl_event *			/* event */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueAcquireGLObjects )
//-----------------------------------------------------------------------------
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseGLObjects(	cl_command_queue	/* command_queue */,
							cl_uint				/* num_objects */,
							const cl_mem *		/* mem_objects */,
							cl_uint				/* num_events_in_wait_list */,
							const cl_event *	/* event_wait_list */,
							cl_event *			/* event */) CL_API_SUFFIX__VERSION_1_0
	WRAP_CALL2JUMP( clEnqueueReleaseGLObjects )

//-----------------------------------------------------------------------------
//	this is an extension
//		we have to get the fn pointer directly in the code
//extern CL_API_ENTRY cl_int CL_API_CALL
//clGetGLContextInfoKHR(	const cl_context_properties *	/* properties */,
//							cl_gl_context_info				/* param_name */,
//							size_t							/* param_value_size */,
//							void *							/* param_value */,
//							size_t *						/* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0
//	WRAP_CALL2JUMP( clGetGLContextInfoKHR )
//-----------------------------------------------------------------------------

//=============================================================================
// Wrapper end (ritual dance part 3)
//=============================================================================
//
DECL_WRAPPER_END( OPENCL )


//
UINT32	wrap_OpenCL_Init ( void )
{
	return( WRAP_DLL( OPENCL ) );		// just give a data structure to work
}//wrap_OpenCL_init
//-----------------------------------------------------------------------------
UINT32	wrap_OpenCL_Term ( void )
{
	return( UNWRAP_DLL( OPENCL ) );	// just give a data structure to work
}//wrap_OpenCL_Term
//-----------------------------------------------------------------------------
int 
_unresolved_call ( void )
{
	WRAP_UNRESOLVED( OPENCL );		// just to get a 'fancy' error printout
	return( CL_DEVICE_NOT_FOUND );			//
}//

#endif//USE_LINKED_OPENCLLIB

// EOF: wrap_OpenCL.cpp
