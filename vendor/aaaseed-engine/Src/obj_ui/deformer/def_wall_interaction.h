
#ifdef AAA_DEF_WALL_INTERACTION_H
#error "AAA_DEF_WALL_INTERACTION_H included more than once."
#endif
#define AAA_DEF_WALL_INTERACTION_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif


class c_wall
{
private:
	REAL		_coverage{ REAL(0.) };
	REAL		sphere_center[3];
	REAL		_barycenter[3]	{ 0.0f, 0.0f, 0.0f };

	REAL		_left_point[3]	{ 0.0f, 0.0f, 0.0f };
	REAL		_right_point[3]	{ 0.0f, 0.0f, 0.0f };

public:

				void		set_points(REAL CONST * CONST left, REAL CONST * CONST right);
					
				REAL		get_distance_to_wall(REAL CONST * CONST pos) CONST;
				REAL		get_distance_to_cylinder_center(REAL CONST * CONST pos) CONST;

				bool		is_relevant(REAL CONST * CONST pos) CONST;

	FINLINE		REAL CONST *get_barycenter() CONST		{ return _barycenter; }
	FINLINE		REAL		get_coverage()	 CONST		{ return _coverage; }

				void		reset();
				void		add_human(REAL CONST * CONST pos, REAL CONST coverage, REAL CONST interaction_dist);
				void		end_humans();

				bool		is_KC() CONST;
				bool		is_KL() CONST;
				bool		is_KR() CONST;

				REAL		get_force(
					REAL CONST * CONST pos,
					REAL CONST min_radius,
					REAL CONST max_radius,
					REAL CONST scale,
					REAL CONST decay_factor,
					REAL CONST interaction_dist) CONST;

				void		get_sphere_center(REAL * sphere, REAL CONST sphere_height) CONST;

};

class	c_def_wall_interaction final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_wall_interaction, c_deformer );
private:
	c_wall			_KL1;
	c_wall			_KL2;
	c_wall			_KL3;
	c_wall			_KL4;
	c_wall			_KC;
	c_wall			_KR1;
	c_wall			_KR2;
	c_wall			_KR3;
	c_wall			_KR4;

	c_wall **		_walls{ nullptr };

	bool			_b_debug{ false };

	// Distance from which we consider the people.
	REAL			_interaction_dist{ REAL(2.5) };

	// gamma.
	REAL			_min_radius	{ REAL(2.) };
	REAL			_max_radius	{ REAL(7.) };

	REAL			_scale{ 1. };
	REAL			_decay_factor{ REAL(2.) };

	REAL			_sphere_height{ REAL(0.0f) };
	REAL			_flatten_ratio_x{ REAL(1.0f) };
	REAL			_flatten_ratio_y{ REAL(1.0f) };
	REAL			_flatten_ratio_z{ REAL(0.1f) };


public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
			void	init();

			void	reset_walls();
			void	add_human(REAL CONST * CONST pos, REAL CONST coverage);
			void	end_humans();
};

