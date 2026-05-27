
WRAP_LOADER(	fftw_execute,		void,		(const fftw_plan plan)															)
WRAP_LOADER(	fftw_destroy_plan,	void,		( fftw_plan plan )																)
WRAP_LOADER(	fftw_plan_r2r_1d,	fftw_plan,	( int n, double* in, double* out, fftw_r2r_kind kind, unsigned flags )		)
WRAP_LOADER(	fftw_cleanup,		void,		( void )		)
WRAP_LOADER(	fftw_cost,			double,		(const fftw_plan plan)	)
//WRAP_LOADER(	fftw_version,		const char,	(void)	)
