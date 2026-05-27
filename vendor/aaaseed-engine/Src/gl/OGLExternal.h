
#ifdef AAA_OGL_EXTERNAL_H
#error "OGL_EXTERNAL_H included more than once."
#endif
#define AAA_OGL_EXTERNAL_H 1


// Native header
#include "system/Native_System.h"


#if defined( NATIVE_OS_WIN32 )
#	include "glew.h"
#	include "wglew.h"
#	define glfGetProcAddress wglGetProcAddress
# endif 


#if defined( NATIVE_OS_LINUX )
#	include <GL/glew.h>
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#	include <GL/glext.h>
# endif


#if defined( NATIVE_OS_DARWIN )
#	include <OpenGL/gl.h>
#   define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#	include <OpenGL/gl3.h>
#	include <OpenGL/glext.h>
# endif




