#include "file/file_hrc.h"
#include "file/aaa_dir.h"
#include "stream_util.h"
#include "math/aaa_math.h"
#include <stdio.h>
#include "math/v_base.h"

void	file_hrc_count( FILE* file, INT32* nb_pt, INT32* nb_frame )
{
	INT32	key_nb = 0;
	INT32	tmp;

	tmp = stream_read_until_str_nocase( file, "nbkeys" );
	if( tmp == 1 )
		tmp = fscanf( file, "%d", &key_nb );
	if( tmp == 1 )
		VERBOSE_PRINT_STRING( "found %d keys", key_nb );

	*nb_pt = 1;
	*nb_frame = key_nb;
}

void	file_hrc_count( C_PCHAR_C filename, INT32* nb_pt, INT32* nb_frame )
{
	//INT32	key_nb = 0;

	//	open file
	FILE*	file = c_file::FOPEN( filename, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "Can't Open hrc file %s.", filename );
	}
	else
	{
		file_hrc_count( file, nb_pt, nb_frame );
		c_file::FCLOSE(file);
	}

	//count the file
	o_str	str;
	o_str	root;
	o_str	ext;
	UINT32	number;
	INT32	number_len;
	
	fname::serie_split( filename, root, &number, &number_len, ext );
	if( number )
	{
		for(;;)
		{
			++number;
			fname::serie_make( str, root, number, number_len, ext );
			if( !c_file::is_exist(str) )
			{
				--number;
				break;
			}
		}
		*nb_pt = number;
		VERBOSE_PRINT_STRING( "found %d files", number );
	}
}

AAA_ERR	file_hrc_read( C_PCHAR_C filename, INT32 nb_pt, INT32 nb_frame, REAL* data )
{
	o_str	fname;
	o_str	root;
	o_str	ext;
	INT32	number_len;
	FILE*	file;
	AAA_ERR	retcode = AAA_OK;
	UINT32	nb;

	//todoquick, i was passed to series_split, but was set to zero right after!!!
	fname::serie_split( (char*) filename, root, &nb, &number_len, ext );
	for( INT32 i = 0; i < nb_pt; ++i )
	{
		//todo was noted a hack fro years ?
		if(	nb_pt != 1)
		{
			fname::serie_make( fname, root, i+1, number_len, ext );
			//	open file
			file = c_file::FOPEN( fname, "rt" );
		}
		else
			file = c_file::FOPEN( filename, "rt" );
			
		if( !file )
		{
			ERR_PRINT_STRING( "Can't Open hrc file %s.", filename );
		}
		else
		{
			char	str[16];
			DOUBLE	f[3];
			REAL*	pt = data + 3*i;

			for( INT32 j = 0; j < nb_frame; ++j )
			{
				sprintf( str, "[%d]", j );
				INT32 tmp = stream_read_until_str_nocase( file, str );
				if( tmp == 1)
				{
					tmp = fscanf( file, " position %lf %lf %lf", f, f+1, f+2 );	
					if( tmp == 3)
					{
						cpy_v3( pt, f );
						pt += 3*nb_pt;
					}
				}
				if( tmp != 3)
				{
					BOX_ERR( "pb reading hrc file" );
					retcode = ERR_ANY;
					break;
				}
			}

			c_file::FCLOSE(file);
		}
	}
	return retcode;
}

void	file_srt_count( C_PCHAR_C filename, INT32* nb_pt, INT32* nb_frame )
{
	INT32	key_nb = 0;

	//	open file
	FILE*	file = c_file::FOPEN( filename, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "Can't Open srt file %s.", filename );
	}
	else
	{
		INT32	tmp;
		for(;;)
		{
			tmp = stream_read_until_str_nocase( file, "frame" );
			if( tmp == 1 )
				++key_nb;
			else
				break;
		}
		c_file::FCLOSE(file);
	}

	//count the file
	o_str	str;
	o_str	root;
	o_str	ext;
	UINT32	number;
	INT32	number_len;
	
	fname::serie_split( (char*) filename, root, &number, &number_len, ext );
	for(;;)
	{
		++number;
		fname::serie_make( str, root, number, number_len, ext );
		if( !c_file::is_exist(str) )
		{
			--number;
			break;
		}
	}
	VERBOSE_PRINT_STRING( "found %d files with %d keys", number, key_nb );

	*nb_pt = number;
	*nb_frame = key_nb;
}

AAA_ERR	file_srt_read( C_PCHAR_C filename, INT32 nb_pt, INT32 nb_frame, REAL* data )
{
	o_str	fname;
	o_str	root;
	o_str	ext;
	INT32	number_len;
	FILE*	file;
	AAA_ERR	retcode = AAA_OK;
	DOUBLE	f[3];
	UINT32	nb;
	
	//todo quick, i (now it's nb) was passed to series_split, but was set to zero right after!!!
	fname::serie_split( (char*) filename, root, &nb, &number_len, ext );
	for( INT32 i = 0; i < nb_pt; ++i )
	{
		//todo was noted a hack fro years ?
		if(	nb_pt != 1 )
		{
			fname::serie_make( fname, root, i+1, number_len, ext );
			//	open file
			file = c_file::FOPEN( fname, "rt" );
		}
		else
			file = c_file::FOPEN( filename, "rt" );
			
		if( !file )
		{
			ERR_PRINT_STRING( "Can't Open srt file %s.", filename );
		}
		else
		{
			INT32	j;
			INT32	tmp;
			INT32	nb2;
			REAL*	pt = data + 3*i;
			//REAL	rl[3] = { 0,0,0 };
//			REAL	r;
//			INT32	d;

			retcode = ERR_ANY;

			for( j = 0; j < nb_frame; ++j )
			{
				tmp = stream_read_until_str_nocase( file, "frame" );
				if( tmp != 1 )
					break;
				tmp = fscanf( file, "%d", &nb2 );
				if( tmp != 1 || nb2 != j )
					break;
				
				if( !stream_read_until_lf( file ) )
					break;
				if( !stream_read_until_lf( file ) )
					break;

				tmp = fscanf( file, "%lf %lf %lf", f, f+1, f+2 );
				if( tmp != 3 )
					break;
				cpy_v3( pt+6, f );

				tmp = fscanf( file, "%lf %lf %lf", f, f+1, f+2 );
				if( tmp != 3 )
					break;
				cpy_v3( pt+3, f );

				tmp = fscanf( file, "%lf %lf %lf", f, f+1, f+2 );
				if( tmp != 3 )
					break;
				cpy_v3( pt, f );

/*				for ( d=0; d<3; ++d )
					{
					r = *(ptf+3+d );
					if( ABS(r-rl[d]) > 45.)
						{
						REAL	f;
						f = FMOD( rl[d], 360.);
						f = rl[d]-f;
						r = FMOD( r, 360.) + f;
						*(pt+3+d ) = r;
						}
					rl[d] = r;
					}
*/
				pt += 9 * nb_pt;
			}
			if( j == nb_frame )
				retcode = AAA_OK;
			else
			{
				BOX_ERR( "pb reading srt file" );
				break;
			}
			c_file::FCLOSE( file );
		}
	}
	return retcode;
}
