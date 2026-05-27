
#ifdef AAA_AAA_GLUT_H
#error "AAA_GLUT_H included more than once."
#endif
#define AAA_AAA_GLUT_H 1


#ifndef AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif

#define AAA_GLUT_USE() (AAA_NEW_DESIGN()==0)

#if	AAA_GLUT_USE()
#	ifndef	AAA_AAA_TYPE_H
#		include "aaa_type.h"
#	endif
#	define	AAA_GLUT_MAA()			1
#	define	GLUT_NO_LIB_PRAGMA()	1
//#	ifndef	AAA_AAA_OS_H
//#		include "aaa_os.h"
//#	endif
#	ifndef __glut_h__
#		include <GlutMaa/GL/glut.h>
#	endif
//#else
//#	ifdef	WIN32
////#		include <GL/glaux.h>
//#	else
//#		include <GL/glx.h>
//#		include "aux.h"
//#		include "joydrv.h"
//#		include "tk.h"
//#	endif
#endif	//AAA_GLUT_USE()


