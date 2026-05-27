
#ifdef AAA_DEF_DISTANCE_FIELD_H
#error "DEF_DISTANCE_FIELD_H included more than once."
#endif
#define AAA_DEF_DISTANCE_FIELD_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef AAA_BDD_FIELD_GENE_H
#	include "obj_ui/bdd/bdd_field/bdd_field_gene.h"
#endif

class	c_def_distance_field final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_distance_field, c_deformer );
private:
	CONSTEXPR static INT32 BDD_SRC_NB = 4;
	o_str				_target_name_symbo_KL{ "distance_field_KL" };
	o_str				_target_name_symbo_KC{ "distance_field_KC" };
	o_str				_target_name_symbo_KR{ "distance_field_KR" };
	o_str				_target_name_symbo_SO{ "distance_field_SO" };

	c_bdd_field_gene *	_field_target_src[BDD_SRC_NB]	{nullptr,nullptr,nullptr,nullptr};
	bool				_b_field_valid[BDD_SRC_NB]		{false,false,false,false};

	REAL				_fish_size		{ 1. };
	REAL				_min_scaling	{ REAL(1.) };
	REAL				_max_scaling	{ REAL(3.) };

	REAL				_grid_step		{ REAL(0.1) };
	REAL				_scale			{ 1. };
	REAL				_decay_factor	{ REAL(2.) };
	REAL				_precision		{ REAL(0.2) };
	REAL				_tangent_ratio	{ REAL(0.) };

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();

			void	update_fish_length(REAL CONST len);
};

