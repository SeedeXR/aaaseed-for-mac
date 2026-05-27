// tests/unit/aaalua_layer_app_shim/draw/aaa_gl.h
//
// Tests-private SHADOW for vendor/aaaseed-engine/Src/draw/aaa_gl.h.
// app.cpp pulls draw/aaa_gl.h transitively via gol/gol_tex.h <- gol/gol_base.h.
// The real header includes <glew.h>, <GL/gl.h>, <GL/glu.h>. macOS deprecated
// the legacy OpenGL stack in 10.14 and has no GLU.framework in modern SDKs ;
// the Mac port routes through GOL::MetalBackend not OpenGL anyway.
//
// For the compile-only build, we provide minimum-viable GL type forward decls
// so the engine headers that touch GLenum/GLuint etc. type-check without
// the real OpenGL include cone.

#ifdef AAA_AAA_GL_H
#error "AAA_GL_H included more than once."
#endif
#define AAA_AAA_GL_H 1

//	Forward-declare the GL primitive types that engine headers commonly
//	reference as type tags. Matches the contract of the existing
//	aaalua_array_shim/gol/gol_base.h pattern (where GLenum + 8 enum values
//	are exposed as opaque integer type tags).
typedef unsigned int    GLenum;
typedef unsigned int    GLuint;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLboolean;
typedef unsigned char   GLubyte;
typedef float           GLfloat;
typedef double          GLdouble;
typedef void            GLvoid;
typedef char            GLchar;
typedef signed long     GLintptr;
typedef signed long     GLsizeiptr;
typedef unsigned int    GLbitfield;

//	Avoid pulling Apple's deprecated OpenGL framework. GLEW + GLU are NOT
//	available on modern macOS SDKs ; the Mac port uses Metal via GOL backend.
