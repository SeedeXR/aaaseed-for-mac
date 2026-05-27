#include "aaa_def.h"
#include "license.h"
#include "license/info.h"
#include "license/license_ui.h"
#include "infrastructure/seed_stop.h"
#include "math/rand.h"
#include "obj_ui/com/net.h"
#include "time/aaa_date.h"
#include "time/aaa_time.h"
#include "file/aaa_dir.h"
#ifndef AAA_PREF_H
#	include "ui/pref.h"
#endif


static	CHAR	static_license_what[64];
static	CHAR	static_license_key[64];
static	bool	static_b_license = false;
static	bool	static_b_license_ok = false;
static	INT32	static_year = 2000;
static	INT32	static_month = 6;
static	INT32	static_day = 10;
static	CHAR	static_version_short[64];
static	CHAR	static_year_str[64];
static	CHAR	static_month_str[64];
static	CHAR	static_day_str[64];
static	CHAR	static_user_str[64];

INT32	license_get_year()			{	return static_year;			}
CHAR*	license_get_year_str()		{	return static_year_str;		}
INT32	license_get_month()			{	return static_month;		}
CHAR*	license_get_month_str()		{	return static_month_str;	}
INT32	license_get_day()			{	return static_day;			}
CHAR*	license_get_day_str()		{	return static_day_str;		}
CHAR*	license_get_user()			{	return static_user_str;		}

CHAR*	license_get_version_short()
{
	return static_version_short;
}

void	license_parse()
{
	if( static_b_license )
	{
		CHAR*	pt = static_license_what;
		while( *pt++ != '_' );
		CHAR*	dst = static_user_str;
		do
			*dst++ = *pt;
		while( *++pt != '_' );
		*dst = 0;
		++pt;
		sscanf( pt, "%04d%02d%02d", &static_year, &static_month, &static_day );
		sprintf( static_year_str,	"%04d", static_year);
		sprintf( static_month_str,	"%02d", static_month);
		sprintf( static_day_str,	"%02d", static_day);
		while( *pt++ != '_');
		dst = static_version_short;
		do
			*dst++ = *pt;
		while( *++pt != '_' );
		dst = 0;
	}
}

void	license_read()
{
	static_license_what[0]	= 0;
	static_license_key[0]	= 0;
	static_version_short[0]	= 0;
	
	o_str	s;
	//	first we try to read a license file in X:/AAA/license.txt X being the drive where we lauch the exe
	s.set( c_dir::get_start());
	s.keep( 2 );
	s.add( "\\AAA\\license.txt" );
	FILE* file = c_file::FOPEN( s.get() , "rt" );
	if( !file )
	{
		s.set( c_dir::get_start());
		s.add( "\\license.txt" );
		file = c_file::FOPEN( s.get() , "rt" );
	}
	if( file )
	{
		static_b_license = true;
		do
		{
			INT32	tmp = fscanf( file, "%64s %64s", static_license_what, static_license_key );
			if( tmp != 2 )
				break;
			license_check();
			license_parse();
		}
		while( !static_b_license_ok );

		c_file::FCLOSE( file);
	}
}

void	license_check_key(		C_PCHAR_C key,
								C_PCHAR_C what,
								C_PCHAR_C langroup,
								C_PCHAR_C machine)
{
	CHAR	tmp_key[64];
	license_generate_key( tmp_key, what, langroup, machine );
	static_b_license_ok = ( strcmp( key, tmp_key ) == 0 );
}

static	INT32	get_crazy( INT32 y, INT32 m, INT32 d )
{
	return( y * 365 + m * 31 + d );
}

INT32 g_license_day_left = 0;

bool	license_check_date()
{	
	time_t aclock;
	time( &aclock );			//Get time in seconds
	struct tm* ptm = localtime( &aclock );	//Convert time to struct tm form
	g_license_day_left = get_crazy( static_year, static_month, static_day ) - get_crazy( ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday );
	return 0 < g_license_day_left;
}

bool	license_check_version()
{
	return strcmp( g_info->get_version_short(), static_version_short ) <= 0 ;
}

void	license_update()
{
	if( static_b_license )
	{
		if( !license_check_date() )
			static_b_license_ok = false;
		if( !license_check_version() )
			static_b_license_ok = false;
	}
	if( !static_b_license_ok )
	{
		stop::b_quit_after_time	= true;
//		aaa::time::update();
		stop::demo_time_start	= .0F;		//aaa::time::get();

		MESSAGE_BOX( 0, nullptr, "AAASeed will be running for 45 minutes only.", "No valid License", MB_OK | MB_ICONSTOP | MB_TOPMOST );
		//license_demo();
//		license_generate();

//#if AAA_NEW_DESIGN()
//		//FatalAppExit( 0, "trouble with the license" );
//		FatalExit( 70 );
//#else
//		exit( 70 );
//#endif
	}
}


void	license_check()
{
	if( static_b_license )
		license_check_key( static_license_key, static_license_what, net->get_langroup(), net->get_machine() );
}


void	license_generate_what( CHAR* what, CHAR* user, INT32 year, INT32 month, INT32 day, CHAR* version_short, C_PCHAR_C from_who )
{
	sprintf( what, "AAASeed_%s_%04d%02d%02d_%s_%s", 
				user,
				year,
				month,
				day,
				version_short,
				from_who);
}

#define	KEY_LEN		16
#define	KEY_MASK	(KEY_LEN-1)
#define	SRC_NB		3

void	license_generate_key( CHAR* key, C_PCHAR_C what, C_PCHAR_C langroup, C_PCHAR_C machine )
{
	INT32	keys[KEY_LEN];
	for( INT32 i = 0; i < KEY_LEN; ++i )
		keys[i] = 0x89abcdef;

	INT32	src_index[SRC_NB];
	for( INT32 i = 0; i < SRC_NB; ++i )
		src_index[i] = 0;

	INT32	src_len[SRC_NB];
	src_len[0] = (INT32)strlen( what );
	src_len[1] = (INT32)strlen( langroup );
	src_len[2] = (INT32)strlen( machine );

	c_rand_license	therand;
	therand.set_seed(8888);
	INT32			tmp = 1024 + (therand.get_uint32()>>12)&0x3ff;
	INT32			key_index;
	INT32			source;
	C_PCHAR			src;
	for( INT32 i = tmp; i > 0; --i )
	{
		source = (therand.get_uint32()>>10) & 0x3;
		key_index = (therand.get_uint32()>>10) & KEY_MASK;
		switch ( source )
		{
			case 3:	source = 0;
			case 0:	src = what;		break;
			case 1:	src = langroup;	break;
			case 2:	src = machine;	break;
		}
		tmp = src_index[source]++;
		if( tmp > src_len[source] )
		{
			tmp = 0;
			src_index[source] = 0;
		}
		tmp = (INT32) *(src+tmp);
		keys[key_index] += tmp;
	}

	INT32			key_add = 0;
	INT32			key_xor = 0x77777777;
	for( INT32 i = 0; i < KEY_LEN; ++i )
	{
		key_add += keys[i];
		key_xor ^= keys[i];
	}
	for( INT32 i = 0; i < 8; ++i )
	{
		tmp = ((key_add )>>(4*i)) & 0xf;
		if( tmp < 10 )
			*key++= '0' + tmp;
		else
			*key++= 'a' + tmp - 10;
	}
	*key++ = '_';
	for( INT32 i = 0; i < 8; ++i )
	{
		tmp = ((key_xor)>>(4*i)) & 0xf;
		if( tmp < 10 )
			*key++= '0' + tmp;
		else
			*key++= 'a' + tmp - 10;
	}
	*key = 0;
}
