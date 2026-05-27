
#ifdef AAA_AAA_DEF_H
#error "AAA_DEF_H included more than once."
#endif
#define AAA_AAA_DEF_H 1


//	make a demo version
#ifndef	AAA_DEMO_INTERGRAPH
#	define	AAA_DEMO_INTERGRAPH()	0
#endif
#if	AAA_DEMO_INTERGRAPH()
#	define	AAA_DEMO_NAME()	"IntergraphDemo/Intergraph2.demo"
#endif
#ifndef	AAA_DEMO
#	define	AAA_DEMO()			0
#endif
//	totally lock the application once a demo is started
#ifndef	AAA_DEMO_LOCKED
#	define	AAA_DEMO_LOCKED()	0
#endif


//All the menu except the focus one
#ifndef	AAA_MENU_LOCKED
#	define	AAA_MENU_LOCKED()			0
#endif
#ifndef	AAA_MENU_FOCUS_LOCKED
#	define	AAA_MENU_FOCUS_LOCKED()		0
#endif
#ifndef	AAA_MOUSE_LOCKED
#	define	AAA_MOUSE_LOCKED()			0
#endif

//	force the lock on the checksum
#ifndef	AAA_CHECKSUM_ENV_LOCKED
#	define	AAA_CHECKSUM_ENV_LOCKED()	0
#endif
#if	AAA_CHECKSUM_ENV_LOCKED()
#	define	AAA_CHECKSUM_PARAM_DO()		1	//there is a bug with it
#endif
#ifndef	AAA_CHECKSUM_PARAM_DO
#	define	AAA_CHECKSUM_PARAM_DO()		0
#endif

#ifndef	AAA_EDIT
#	define	AAA_EDIT()					1
#endif

// AAA_NEW_DESIGN() is defined in Src/aaa_build_config.h, force-included in every TU.
#ifndef AAA_NEW_DESIGN
#	error "AAA_NEW_DESIGN() must be defined in aaa_build_config.h"
#endif

//now the app change with using the name, so there no more watchdog version
#define AAA_WATCHDOG()	0

