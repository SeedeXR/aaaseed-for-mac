
#ifdef AAA_AAA_MEM_WIN_H
#error "AAA_MEM_WIN_H included more than once."
#endif
#define AAA_AAA_MEM_WIN_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


class c_memory_leak_detector_wrapper
{
public:
	c_memory_leak_detector_wrapper();
	virtual ~c_memory_leak_detector_wrapper();

	void restore_hook();
};
