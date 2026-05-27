
#ifdef AAA_FILE_HRC_H
#error "FILE_HRC_H included more than once."
#endif
#define AAA_FILE_HRC_H 1

#ifndef AAA_AAA_STR_H
#	include "aaa_type.h"
#endif

void	file_hrc_count(	C_PCHAR_C filename, INT32* nb_pt,	INT32* nb_frame				);
AAA_ERR	file_hrc_read(	C_PCHAR_C filename, INT32 nb_pt,	INT32 nb_frame, REAL* data	);

void	file_srt_count(	C_PCHAR_C filename, INT32* nb_pt,	INT32* nb_frame				);
AAA_ERR	file_srt_read(	C_PCHAR_C filename, INT32 nb_pt,	INT32 nb_frame, REAL* data	);

