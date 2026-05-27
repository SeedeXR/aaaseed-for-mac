// Copyright (C) 2006  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
//#ifndef DLIB_ALL_SOURCe_
//#define DLIB_ALL_SOURCe_

//maa
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#if 1
#	ifndef _FILESYSTEM_
#		include <filesystem>
#	endif
#else
	// Boost stuff
#	define BOOST_LIB_DIAGNOSTIC
#	include <boost/filesystem.hpp>
#	include <boost/filesystem/fstream.hpp>
#endif

// ISO C++ code
	#include "dlib/base64/base64_kernel_1.cpp"
//#include "../bigint/bigint_kernel_1.cpp"
//#include "../bigint/bigint_kernel_2.cpp"
//maa	#include "../bit_stream/bit_stream_kernel_1.cpp"
	#include "dlib/entropy_decoder/entropy_decoder_kernel_1.cpp"
	#include "dlib/entropy_decoder/entropy_decoder_kernel_2.cpp"
	#include "dlib/entropy_encoder/entropy_encoder_kernel_1.cpp"
	#include "dlib/entropy_encoder/entropy_encoder_kernel_2.cpp"
//#include "../md5/md5_kernel_1.cpp"
//maa	#include "../tokenizer/tokenizer_kernel_1.cpp"
//#include "../unicode/unicode.cpp"
//#include "../data_io/image_dataset_metadata.cpp"


#ifndef DLIB_ISO_CPP_ONLY
// Code that depends on OS specific APIs

// include this first so that it can disable the older version
// of the winsock API when compiled in windows.
//maa	#include "../sockets/sockets_kernel_1.cpp"
//#include "../bsp/bsp.cpp"

//#include "../dir_nav/dir_nav_kernel_1.cpp"
//#include "../dir_nav/dir_nav_kernel_2.cpp"
//#include "../dir_nav/dir_nav_extensions.cpp"
//#include "../linker/linker_kernel_1.cpp"
//#include "../logger/extra_logger_headers.cpp"
//#include "../logger/logger_kernel_1.cpp"
//#include "../logger/logger_config_file.cpp"
//maa	#include "../misc_api/misc_api_kernel_1.cpp"
//maa	#include "../misc_api/misc_api_kernel_2.cpp"
//maa	#include "../sockets/sockets_extensions.cpp"
//maa	#include "../sockets/sockets_kernel_2.cpp"
//maa	#include "../sockstreambuf/sockstreambuf.cpp"
//maa	#include "../sockstreambuf/sockstreambuf_unbuffered.cpp"
//#include "../server/server_kernel.cpp"
//#include "../server/server_iostream.cpp"
//#include "../server/server_http.cpp"
//#include "../threads/multithreaded_object_extension.cpp"
	#include "dlib/threads/threaded_object_extension.cpp"
	#include "dlib/threads/threads_kernel_1.cpp"
	#include "dlib/threads/threads_kernel_2.cpp"
	#include "dlib/threads/threads_kernel_shared.cpp"
//#include "../threads/thread_pool_extension.cpp"
//maa	#include "../timer/timer.cpp"
//#include "../stack_trace.cpp"

#ifdef DLIB_PNG_SUPPORT
#include "../image_loader/png_loader.cpp"
#include "../image_saver/save_png.cpp"
#endif

#ifdef DLIB_JPEG_SUPPORT
#include "../image_loader/jpeg_loader.cpp"
#endif

//maa
/*
#ifndef DLIB_NO_GUI_SUPPORT
#include "../gui_widgets/fonts.cpp"
#include "../gui_widgets/widgets.cpp"
#include "../gui_widgets/drawable.cpp"
#include "../gui_widgets/canvas_drawing.cpp"
#include "../gui_widgets/style.cpp"
#include "../gui_widgets/base_widgets.cpp"
#include "../gui_core/gui_core_kernel_1.cpp"
#include "../gui_core/gui_core_kernel_2.cpp"
#endif // DLIB_NO_GUI_SUPPORT
*/

#endif // DLIB_ISO_CPP_ONLY


//#endif // DLIB_ALL_SOURCe_

