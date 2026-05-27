#include "text_dlg.h"
#include "err.h"
#include "stdio.h"
#include "strnum.h"
#include "Thread/aaa_thread.h"

//todo do it with o_str ?
AAA_ERR execute_str( CHAR* buf, INT32 size )
{
	/* Run DIR so that it writes its output to a pipe. Open this
	* pipe with read text attribute so that we can read it 
	* like a text file. 
	*/
	FILE*	chkdsk;
	if( !(chkdsk = _popen( buf, "rt" )) )
		return ERR_ANY;

	{
//	Sleep( 5000 );
//	if( !(chkdsk = _popen( "java Trajecto 2 1 3005", "rt" )) )

	/* Read pipe until end of file. End of file indicates that 
		* CHKDSK closed its standard out (probably meaning it 
		* terminated).
	*/

		INT32	marker_nb = 0;
		bool	b_skip = true;
		CHAR*	pt = buf;
		while( !feof( chkdsk ) )
		{
	//		if( 
			fgets( pt, size, chkdsk );
			INT32 CONST len = (INT32)strlen( pt);
			if( len > 1 )
			{
				if( *pt == 'C' && *(pt+1) == ':' )
					++marker_nb;
				else if( marker_nb >= 3 )
				{
					if( b_skip )
						b_skip = false;
					else
					{
						pt += len;
						size -= len;
					}
				}
			}

//			!= nullptr )
//	DLG_PRINTF( psBuffer );
		}


		/* Close pipe and print return value of CHKDSK. */
	//  DLG_PRINTF( "\nProcess returned %d\n", _pclose( chkdsk ) );
		_pclose( chkdsk );
		*pt = 0;
		return AAA_OK;
	}
	
}

INT32 generate_str( C_PCHAR_C command, CHAR* buf, INT32 size )
{
	strcpy( buf, command );
	return execute_str( buf, size );
}

bool	b_str_generating = false;
bool	b_str_generate_done = false;

namespace {
	CHAR*	generate_buf;
	INT32	generate_size;
	INT32	generate_index;
}

void	
#ifdef	WIN32
__cdecl
#endif
generate_str_fn( void *dummy )
{
//	if( SetThreadPriority( h_notify_thread, THREAD_PRIORITY_TIME_CRITICAL) == 0 )
//	if( SetThreadPriority( h_notify_thread, THREAD_PRIORITY_HIGHEST) == 0 )
//	if( SetThreadPriority( h_notify_thread, THREAD_PRIORITY_ABOVE_NORMAL) == 0 )
//	if( SetThreadPriority( get_thread_cur(), THREAD_PRIORITY_TIME_CRITICAL) == 0 )
//			ERR_PRINT_STRING( "Can't set thread priority");

	if( generate_index >= 0 && generate_index <= 99)
	{
		if( generate_index == 0 )
			generate_str( "generate", generate_buf, generate_size);
		else
		{
			CHAR command[] = "gen01";
			strnum::make( command+3, 2, generate_index );
			generate_str( command, generate_buf, generate_size);
		}
	}
	b_str_generating = false;
	b_str_generate_done = true;
	c_thread::end();
}

void th_generate_str( INT32	s_generate, CHAR* buf, INT32 size )
{
	if( !b_str_generating )
	{
		b_str_generating = true;
		b_str_generate_done = false;
		generate_index = s_generate;
		generate_buf = buf;
		generate_size = size;

		c_thread::begin( "generate_str", generate_str_fn );
	}
//nowin what happen if not windows
}


