
#ifdef AAA_SCHEDULER_H
#error "SCHEDULER_H included more than once."
#endif
#define AAA_SCHEDULER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

enum PRIO_CLASS : INT32
{
	PRIO_CLASS_LOW = 0,
	PRIO_CLASS_NORMAL,
	PRIO_CLASS_HIGH,
	PRIO_CLASS_REALTIME,
};
enum THREAD_PRIO : INT32
{
	THREAD_PRIO_LOW = 0,
	THREAD_PRIO_NORMAL,
	THREAD_PRIO_HIGH,
	THREAD_PRIO_TIME_CRITICAL,
};
/*
class	c_priority final : public c_obj_ui
{
private:
#ifdef	WIN32
	PRIO_CLASS	prio_class_def;
	REAL		prio_level_def;
#endif

protected:
public:

	void	set_def_from_cur();
	void	set_thread_to_def();
	void	set_thread();
};
*/
//void	priority_set_default();
//void	priority_get_cur();

//	Win32 HANDLE-returning APIs gated to Windows ; HANDLE is `void*` from
//	<windows.h> and not defined on Mac. The corresponding .cpp bodies are
//	already gated with `#ifdef WIN32`. Mac callers of these were all
//	already inside Windows-only branches (verified: aaa_mem.cpp's
//	GetProcessMemoryInfo call site, serial.cpp / seed_stop.cpp's
//	OpenProcessToken calls, Native_ThreadSimple.cpp's GetThreadPriority).
#if AAA_OS_WINDOWS()
extern	HANDLE	get_process_cur();
extern	HANDLE	get_thread_cur();

extern	void	set_process_priority_class(	PRIO_CLASS CONST which );
extern	void	set_thread_priority(		THREAD_PRIO CONST which );
#endif //AAA_OS_WINDOWS

extern	AAA_ERR	kill_process_by_name(		C_PCHAR_C name );

struct	lua_State;

namespace aaalua
{
	namespace n_process
	{
		extern void	register_process( lua_State* L );
	}
}

