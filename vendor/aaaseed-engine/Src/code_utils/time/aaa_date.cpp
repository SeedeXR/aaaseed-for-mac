#include "time/aaa_date.h"
#include "time.h"
#include "time/aaa_time.h"
#include "aaa_util.h"


/* these works fine
static	struct tm *newtime;
static	time_t aclock;

	time( &aclock );				//Get time in seconds
	newtime = localtime( &aclock );	//Convert time to struct tm form
	VERBOSE_PRINTF( "The current date and time are: %s", asctime( newtime ) );
	VERBOSE_PRINTF( "The time is %s", ctime( &aclock ) );
char tmpbuf[128];
	VERBOSE_PRINTF( "The date is %s", _strdate( tmpbuf ) );
	VERBOSE_PRINTF( "Year %d, Months %d, days %d", newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday );
*/

namespace aaa
{
namespace date
{
	INT32	year;
	INT32	month;
	INT32	day;
	INT32	hour;
	INT32	minute;
	INT32	second;
	INT32	day_of_week;
	INT32	day_of_year;

	bool	b_simulate;
	INT32	factor;

	static	INT32	get_crazy( INT32 y, INT32 m, INT32 d)
	{
		return( y*400+m*32+d );
	}

	static	time_t date_last;


void	update()	//todo refine to last past 2038
{
	time_t date;
	if( b_simulate )
		date = time_t( DOUBLE(time::get_real_time()) * DOUBLE(factor) ); 
	else
		::time( &date );			//Get time in seconds
	if( date_last != date )
	{
		struct tm*	ptm;
		ptm = localtime( &date );	//Convert time to struct tm form
		date_last = date;
		year = 1900 + ptm->tm_year;		//	direct value
		month = ptm->tm_mon+1;			//	1 to 12
		day = ptm->tm_mday;			//	1 to 31
		hour = ptm->tm_hour;			//	0 to 23
		minute = ptm->tm_min;			//	0 to 59
		second = ptm->tm_sec;			//	0 to 59
		day_of_week = ptm->tm_wday;		//	1 to 7, 1 is monday
		if( day_of_week==0 )			//careful with asynchronous
			day_of_week = 7;
		day_of_year = ptm->tm_yday+1;	//	1 to 366
	}		
}

static	CONSTEXPR	INT32	exp_y = 2000;
static	CONSTEXPR	INT32	exp_m = 6;
static	CONSTEXPR	INT32	exp_d = 15;

bool	is_valid()
{
	update();
	return( get_crazy( year, month, day ) < get_crazy( exp_y, exp_m, exp_d ) );
}

INT32	get_day_of_millenium_approx()
{
	return ::MAX( get_year()-2000, 0 ) * 385 + get_month() * 32 + get_day();
}

/*
bool	is_valid()
{
struct tm *ptm;
	time( &aclock );			//Get time in seconds
	ptm = localtime( &aclock );	//Convert time to struct tm form
	return( get_crazy( ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday ) < get_crazy( exp_y, exp_m, exp_d ) );
}
*/

}	//namespace date
}	//namespace aaa
