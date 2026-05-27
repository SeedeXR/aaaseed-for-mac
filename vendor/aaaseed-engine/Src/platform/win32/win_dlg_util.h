
#ifdef AAA_WIN_DLG_UTIL_H
#error "WIN_DLG_UTIL_H included more than once."
#endif
#define AAA_WIN_DLG_UTIL_H 1



#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class o_str;

namespace aaa {
namespace dialog {

//hack
//todoqq	should vanish
static	CONSTEXPR	INT32	STR_LEN_MAX = 1024 * 8;

extern	void	get_str(		HWND hd_dlg, INT32 CONST ctrl_id, o_str& dst );
extern	void	get_str(		HWND hd_dlg, INT32 CONST ctrl_id, CHAR* CONST dst, INT32 CONST dst_size );

extern	void	set_str(		HWND hd_dlg, INT32 CONST ctrl_id, C_PCHAR_C src );
extern	void	insert_str(		HWND hd_dlg, INT32 CONST ctrl_id, C_PCHAR_C src );

extern	void	get_str_select( HWND hd_dlg, INT32 CONST ctrl_id, INT32& start, INT32& end,	C_PCHAR_C str_debug	);
extern	void	set_str_select( HWND hd_dlg, INT32 CONST ctrl_id, INT32 start,	INT32 end,	C_PCHAR_C str_debug	);

extern	void	set_real(		HWND hd_dlg, INT32 CONST ctrl_id, REAL CONST r );
extern	bool	get_double(		HWND hd_dlg, INT32 CONST ctrl_id, DOUBLE& d );

extern	void	set_int32(		HWND hd_dlg, INT32 CONST ctrl_id, INT32 CONST i );
extern	bool	get_int32(		HWND hd_dlg, INT32 CONST ctrl_id, INT32& i );

}	// namespace dialog
}	// namespace aaa
