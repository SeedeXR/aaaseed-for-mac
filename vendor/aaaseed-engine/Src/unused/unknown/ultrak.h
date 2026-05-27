
#ifdef AAA_ULTRAK_H
#error "ULTRAK_H included more than once."
#endif
#define AAA_ULTRAK_H 1


#ifdef	TRACKER
#ifndef	ULTRATRAK
#define	ULTRATRAK		TRUE
#endif
#endif

#ifdef	ULTRATRAK
#ifndef AAA_TRACKER_H
#	include    "tracker.h"
#endif

class	c_ultratrak final : public c_tracker
	{
public:
	c_ultratrak();
	~c_ultratrak();

	void	start();
	void	start_on_serial( INT32 port_number);
	void	stop();
	void	idle();
	void	calibrate_default();
	};
#endif	//	ULTRATRAK

#endif	//  __ULTRAK_H__
