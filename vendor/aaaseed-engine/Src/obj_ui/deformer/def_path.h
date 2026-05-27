
#ifdef AAA_DEF_PATH_H
#error "DEF_PATH_H included more than once."
#endif
#define AAA_DEF_PATH_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_bdd;

class	c_def_path final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_path, c_deformer );

private:
	o_str			_target_name_symbo;
	c_bdd*			_bdd_target;
	INT32			_channel;
	bool			_b_size_canonical;
	bool			_b_tra;
	REAL			_tra[3];
	REAL			_sca_ui[4];
	bool			_b_sca;
	REAL			_sca[3];
	bool			_b_curve;
public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

