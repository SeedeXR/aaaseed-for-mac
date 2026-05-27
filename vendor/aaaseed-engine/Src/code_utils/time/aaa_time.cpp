
#include "aaa_time.h"
#include "aaa_date.h"
#include "obj_ui/com/net.h"

/*#ifdef WIN32
UINT32	msec_get(void)
{
LARGE_INTEGER Count, Freq;
	QueryPerformanceCounter(&Count);
	QueryPerformanceFrequency(&Freq);

	return	(UINT32) ((1000.0 * Count.LowPart)/Freq.LowPart);
}
#else //WIN32
UINT32	msec_get(void)
{
	return 1000000;
}
//#define GETTIMEOFDAY(_x) gettimeofday(_x, nullptr)
#endif	//WIN32
*/

//todo use this
/*
#include <chrono>

class StopWatch {
public:
	typedef std::chrono::steady_clock clock;
	typedef std::chrono::microseconds microseconds;
	typedef std::chrono::milliseconds milliseconds;
	typedef std::chrono::seconds seconds;

	StopWatch();
	StopWatch(const StopWatch&);
	StopWatch& operator=(const StopWatch& rhs);

	uint64_t ElapsedUs() const;
	uint64_t ElapsedMs() const;
	uint64_t ElapsedSec() const;

	std::chrono::steady_clock::time_point Restart();

private:
	clock::time_point mStart;
};

StopWatch::StopWatch() : mStart(clock::now()) {
	static_assert(std::chrono::steady_clock::is_steady, "Serious OS/C++ library issues. Steady clock is not steady");
	// FYI:  This would fail  static_assert(std::chrono::high_resolution_clock::is_steady(), "High Resolution Clock is NOT steady on CentOS?!");
}

StopWatch::StopWatch(const StopWatch& other): mStart(other.mStart) { 
}

/// @return StopWatch::StopWatch&  - assignment operator.
StopWatch& StopWatch::operator=(const StopWatch& rhs) {
	mStart = rhs.mStart;
	return *this;
}

/// @return the elapsed microseconds since start
uint64_t StopWatch::ElapsedUs() const {
	return std::chrono::duration_cast<microseconds>(clock::now() - mStart).count();
}

/// @return the elapsed milliseconds since start
uint64_t StopWatch::ElapsedMs() const {
	return std::chrono::duration_cast<milliseconds>(clock::now() - mStart).count();
}

/// @return the elapsed seconds since start
uint64_t StopWatch::ElapsedSec() const {
	return std::chrono::duration_cast<seconds>(clock::now() - mStart).count();
}

//	Resets the start point
std::chrono::steady_clock::time_point StopWatch::Restart()
{
	mStart = clock::now();
	return mStart;
}
*/
namespace aaa
{
namespace time
{

	DOUBLE	official;
	DOUBLE	step = .1;
	bool	b_real_time = false;
	bool	b_absolute = true;
	bool	b_restart_trig = false;
	DOUBLE	factor = 1.;
	DOUBLE	origin = 0.;
	DOUBLE	cur_real;
	DOUBLE	cur;
	DOUBLE	last;
	DOUBLE	delta;
	REAL	delta_mess_min = REAL(.1);
	REAL	time_print_interval = 60;

#if AAA_OS_WINDOWS()
	static LARGE_INTEGER	_perf_freq;
#endif
	static	DOUBLE	time_print_last = 0;	
	static	DOUBLE	internal;
	static	DOUBLE	official_last;
	static	bool	b_real_possible = true;
	static	ST_TIME	zero;
	static	ST_TIME	start_absolute;
	static	INT32	update_count;

 
FINLINE	void	store( ST_TIME* pt )
{
#if AAA_OS_WINDOWS()
	if( !QueryPerformanceCounter( pt ) )
	{
		ERR_PRINT_STRING( "QueryPerformanceCounter() error" );
	}
#elif AAA_OS_MAC()
	//	CLOCK_MONOTONIC_RAW is preferred over CLOCK_MONOTONIC : not slewed
	//	by NTP, ideal for a performance counter. Effective frequency = 1 GHz
	//	since tv_nsec is in nanoseconds.
	if( clock_gettime( CLOCK_MONOTONIC_RAW, pt ) != 0 )
	{
		ERR_PRINT_STRING( "clock_gettime( CLOCK_MONOTONIC_RAW ) error" );
	}
#else
	gettimeofday( pt, nullptr );
#endif
}

UINT32	get_interval_micro_sec( CONST ST_TIME& last, CONST ST_TIME& start )
{
	UINT32	interval;
#if AAA_OS_WINDOWS()
	if( last.HighPart != start.HighPart )
	{
		//		VERBOSE_PRINTF("!!!!!!!!!!!!!! change in HighPart");
		interval = I_FLOOR( (DOUBLE) ( 0x1000000 * ( last.HighPart - start.HighPart ) + ( last.LowPart >> 8 ) - ( start.LowPart >> 8 ) )
			* 1000000. / ( _perf_freq.LowPart >> 8 ) );
		//		VERBOSE_PRINTF( "jump high part ");
	}
	else
		//		interval = (DOUBLE) (last->LowPart-start->LowPart) * 1000000. / perf_freq.LowPart;
		interval = UINT32( ( ( last.QuadPart - start.QuadPart ) * 1000000 ) / _perf_freq.QuadPart );
#elif AAA_OS_MAC()
	//	timespec : tv_sec * 1e6 us + tv_nsec / 1e3 us.
	DOUBLE const us = (DOUBLE) ( last.tv_sec - start.tv_sec ) * 1000000.
		+ (DOUBLE) ( last.tv_nsec - start.tv_nsec ) / 1000.;
	interval = UINT32( us );
#else
	interval = (DOUBLE) ( last.tv_sec - start.tv_sec ) * 1000000. + (DOUBLE) ( last.tv_usec - start.tv_usec );
#endif
	return	interval;
}

FINLINE	DOUBLE	get_interval_sec( CONST ST_TIME& cur, CONST ST_TIME& start )
{
	DOUBLE	interval;
	/*
	#ifdef	WIN32
	if( cur->LowPart > start->LowPart )
	interval = (DOUBLE) (cur->LowPart - start->LowPart) / 4. / (perf_freq.LowPart>>2);
	else
	interval = -(DOUBLE) (start->LowPart - cur->LowPart) / 4. / (perf_freq.LowPart>>2);
	interval += (DOUBLE) MulDiv( cur->HighPart - start->HighPart, 1<<30, perf_freq.LowPart >> 2);
	#else
	interval = (DOUBLE) (last->tv_sec - start->tv_sec) + (DOUBLE) (last->tv_usec - start->tv_usec) / 1000000.;
	#endif
	*/
#if AAA_OS_WINDOWS()
	LONGLONG tmp;
	tmp = cur.QuadPart - start.QuadPart;
	interval = (DOUBLE) ( tmp ) / _perf_freq.LowPart;
#elif AAA_OS_MAC()
	interval = (DOUBLE) ( cur.tv_sec - start.tv_sec ) + (DOUBLE) ( cur.tv_nsec - start.tv_nsec ) / 1000000000.;
#else
	interval = (DOUBLE) ( cur.tv_sec - start.tv_sec ) + (DOUBLE) ( cur.tv_usec - start.tv_usec ) / 1000000.;
#endif
	return	interval;
}

bool	test_real_time_possible()
{
#if AAA_OS_WINDOWS()
	if( !QueryPerformanceFrequency( &_perf_freq ) )
	{
		BELL();
		BOX_ERR( "No real time option\nBecause no QueryPerformanceCounter() on this machine.");
		b_real_possible = false;
	}
	else if( _perf_freq.HighPart != 0 )
	{
		BELL();
		BOX_ERR( "No real time option\nBecause code don't implement Frequency with HighPart not zero.");
		b_real_possible = false;
	}
	else
	{
		GOOD_PRINT_STRING( "QueryPerformanceFrequency High:%u, Low:%u", _perf_freq.HighPart, _perf_freq.LowPart);
		b_real_possible = true;
	}
#elif AAA_OS_MAC()
	//	CLOCK_MONOTONIC_RAW is always available on macOS 13+ ; effective
	//	frequency is 1 GHz (tv_nsec resolution). Probe once to be safe.
	struct timespec ts;
	if( clock_gettime( CLOCK_MONOTONIC_RAW, &ts ) != 0 )
	{
		BELL();
		BOX_ERR( "No real time option\nBecause clock_gettime( CLOCK_MONOTONIC_RAW ) failed.");
		b_real_possible = false;
	}
	else
	{
		GOOD_PRINT_STRING( "clock_gettime( CLOCK_MONOTONIC_RAW ) ok, effective frequency 1 GHz" );
		b_real_possible = true;
	}
#else
	//todo need to be done
	b_real_possible = false;
#endif
	return b_real_possible;
}

bool	set_real_time( bool flag )
{
	bool	b_last = b_real_time;
	if( flag != b_real_time )
	{
		if( flag )
		{
			if( b_real_possible )
				b_real_time = true;
			else	//todo this look weird
			{
				b_real_time = false;
				DBG_PRINT_STRING( "Can't switch to impossible real time");
			}
		}
		else
			b_real_time = false;
		if( b_last != b_real_time )
		{
			SWITCH_PRINT_STATE( "Real Time", b_real_time);
			start();
		}
	}
	return b_real_time;
}

void	start()
{
	GOOD_PRINT_STRING( "Time Reset" );
	if( b_real_time )
		official_last = official_last - official;
	else
		official_last = - step;
	store( &zero );
	update_count = -1;
	b_restart_trig = false;
}


void	init()
{
	test_real_time_possible();
	start();
	store( &start_absolute );
}

DOUBLE	get_real_time()
{
	ST_TIME	time_cur;
	store( &time_cur );
	return get_interval_sec( time_cur, zero );
}

DOUBLE	get_real_time_from_start()
{
	ST_TIME	time_cur;
	store( &time_cur );
	return get_interval_sec( time_cur, start_absolute );
}



DOUBLE	net_time_offset = 0;
DOUBLE	net_time_offset_factor = .1;
	
DOUBLE	real_time_last;

void set_low( DOUBLE time_in )
{
	official = time_in;

	cur = official + origin;
	last = official_last + origin;
	delta = cur - last;
	if( delta > delta_mess_min )
	{
		//WARNING_PRINT_STRING_INV( "AAASeed too slow : %d ms at %02dh%02dm%02ds, could be a FREEZE.", INT32(delta*1000), date::hour, date::minute, date::second );
	}
	++update_count;
}


void	update()
{
	if( b_restart_trig )
		start();

	if( update_count < 0 )
	{
		internal = 0;
		real_time_last = 0;
		cur_real = 0;
	}
	else
	{
		DOUBLE	time_new = get_real_time();
		DOUBLE	dt = time_new - real_time_last;
		real_time_last = time_new;
		cur_real += dt;

		official_last = official;
		if( b_real_time )
		{
			if( b_absolute )
				internal = time_new * factor;
			else
				internal += dt * factor;
		}
		else
			internal += step;
	}

	if( net && net->is_time_slave() && net->is_time_received() )
	{
		DOUBLE tmp = net->get_time_received() - internal;
		net_time_offset = interpolate( net_time_offset, tmp, net_time_offset_factor );
	}

	set_low( internal + net_time_offset );
	
	cur = official + origin;
	last = official_last + origin;
	delta = cur - last;
	++update_count;
}

void	set( DOUBLE time_in, DOUBLE step_in )
{
	
	official_last = time_in - step_in;
	set_low( time_in );
}

INT32	get_update_count()	//todo unused but is it still correct ?
{
	return	update_count;
}

void	alive_print()
{
	DOUBLE	t_real = get_real_time();
	if( t_real - time_print_last > time_print_interval )
	{
		time_print_last = t_real;
		GOOD_PRINT_STRING( "AAASeed has been running for %.0f h %.0f mins %.0f seconds", F_FLOOR(t_real/3600.0), F_FLOOR(FMOD(t_real,3600.0)/60.0), FMOD(t_real,60.0) );
	}
}

}	//	namespace time
}	//	namespace aaa