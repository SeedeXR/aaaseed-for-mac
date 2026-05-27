
#ifdef AAA_AAA_DATE_H
#error "AAA_DATE_H included more than once."
#endif
#define AAA_AAA_DATE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace aaa
{
namespace date
{
	extern	INT32	year;				//	direct value
	extern	INT32	month;				//	1 to 12
	extern	INT32	day;				//	1 to 31
	extern	INT32	hour;				//	0 to 23
	extern	INT32	minute;				//	0 to 59
	extern	INT32	second;				//	0 to 59
	extern	INT32	day_of_week;		//	1 to 7, 1 is monday
	extern	INT32	day_of_year;		//	1 to 366

	extern	bool	b_simulate;
	extern	INT32	factor;

	FINLINE	INT32	get_year()			{	return year;		}
	FINLINE	INT32	get_month()			{	return month;		}
	FINLINE	INT32	get_day()			{	return day;			}
	FINLINE	INT32	get_hour()			{	return hour;		}
	FINLINE	INT32	get_minute()		{	return minute;		}
	FINLINE	INT32	get_second()		{	return second;		}
	FINLINE	INT32	get_day_of_week()	{	return day_of_week;	}
	FINLINE	INT32	get_day_of_year()	{	return day_of_year;	}
	extern	INT32	get_day_of_millenium_approx();

	extern	bool	is_valid();
	extern	void	update();

}	//	namespace date
}	//	namespace aaa


