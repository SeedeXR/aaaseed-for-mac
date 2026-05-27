#include "transfo.h"

FACTORY_ABSTRACT_CREATE_V1( c_transfo, transfo, Transformation );
CONSTRUCTOR_ABSTRACT_CREATE( c_transfo )	{}
EMPTY_DESTRUCTOR( c_transfo )

/*

#define	TRANSFO_DPENDULUM_PARAM_NB_MAX	14
c_param_def	param_transfo_dpendulum[TRANSFO_DPENDULUM_PARAM_NB_MAX] =
{
	{PARAM_DEF_BOOL_OFF( Active )

	{	nullptr,	PARAM_REAL,	"Radius_factor", 4., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"Frequency_factor", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
																
	{	nullptr,	PARAM_REAL,	"A_Radius", 1., 0.,				PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"A_Frequency_u", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"A_Frequency_v", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"A_Frequency_axe", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"A_Frequency_factor", 1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },

	{	nullptr,	PARAM_REAL,	"B_Radius", 1., 0.,				PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"B_Frequency_u", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"B_Frequency_v", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"B_Frequency_axe", 1., 0.,		PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	{	nullptr,	PARAM_REAL,	"B_Frequency_factor", 1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },

	PARAM_DEF_AXE_Y( Axe )
};

void	c_transfo_dpendulum::param_init_pt()
{
INT32	h=0;
	param_set_pt( h, b_active);
	param_set_pt( h, rayon_factor);
	param_set_pt( h, frequency_factor);

	param_set_pt( h, a_rayon_ui);
	param_set_pt_3( h, a_frequency );
	param_set_pt( h, a_frequency_factor_ui);

	param_set_pt( h, b_rayon_ui);
	param_set_pt_3( h, b_frequency );
	param_set_pt( h, b_frequency_factor_ui);

	param_set_pt( h, axe);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_transfo_dpendulum)
{
	set_name("DoublePendulum");
	param_init_with( param_transfo_dpendulum, TRANSFO_DPENDULUM_PARAM_NB_MAX);
}

void	c_transfo_dpendulum::update()
{
	//	prepare the axes
	axe_build_index( i_u, i_v, axe);

	a_rayon = a_rayon_ui * rayon_factor;
	b_rayon = b_rayon_ui * rayon_factor;

	a_frequency_factor = a_frequency_factor_ui * frequency_factor;
	b_frequency_factor = b_frequency_factor_ui * frequency_factor;
}

void	c_transfo_dpendulum::set_time( REAL t)
{
REAL	tmp;
	a_freq = a_frequency[2] * a_frequency_factor;
	tmp = a_freq * t;
	GET_SIN_COS_INT( a_sin, a_cos, tmp);

	b_freq = b_frequency[2] * b_frequency_factor;
	tmp += b_freq * t;
	GET_SIN_COS_INT( b_sin, b_cos, tmp);
}

//todo	the next 3 else case cost too much
void	c_transfo_dpendulum::apply_move( pt_3d* in, pt_3d* out)
{
	if( b_active)
		{
		(*out)[i_u] = ((*in)[i_u] + a_rayon)*a_cos - (*in)[i_v]*a_sin + b_rayon*b_cos - b_rayon*b_sin;
		(*out)[i_v] = ((*in)[i_u] + a_rayon)*a_sin + (*in)[i_v]*a_cos + b_rayon*b_sin + b_rayon*b_cos;
		(*out)[axe] = (*in)[axe];
		}
	else
		ERR_PRINT_STRING( "c_transfo_dpendulum::apply_move should not be called when inactive");
}

void	c_transfo_dpendulum::apply_rotate( pt_3d* in, pt_3d* out)
{
	if( b_active)
		{
		(*out)[i_u] = ((*in)[i_u])*a_cos - (*in)[i_v]*a_sin;
		(*out)[i_v] = ((*in)[i_u])*a_sin + (*in)[i_v]*a_cos;
		(*out)[axe] = (*in	)[axe];
		}
	else
		ERR_PRINT_STRING( "c_transfo_dpendulum::apply_rotate should not be called when inactive");
}


void	c_transfo_dpendulum::get_speed( pt_3d* in, pt_3d* out)
{
	if( b_active)
		{
		(*out)[i_u] = -a_freq * (((*in)[i_u]+a_rayon)*a_sin + (*in)[i_v]*a_cos);
		(*out)[i_v] = a_freq * (((*in)[i_u]+a_rayon)*a_cos - (*in)[i_v]*a_sin);
		(*out)[axe] = 0;
		}
	else
		ERR_PRINT_STRING( "c_transfo_dpendulum::get_speed should not be called when inactive");
}
*/