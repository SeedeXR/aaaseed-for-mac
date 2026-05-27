
#ifdef AAA_DEF_BDD_BLOB_H
#error "DEF_BDD_BLOB_H included more than once."
#endif
#define AAA_DEF_BDD_BLOB_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class	c_bdd_blob;

class	c_def_bdd_blob final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_bdd_blob,c_deformer);
private:
	REAL		_origin[3];
	INT32		_i_src_u;
	INT32		_i_src_v;
	INT32		_i_src_axe;
	INT32		_i_dst_axe;
	REAL		_offset;
	REAL		_scale[3];
	REAL		_scale_ui[4];
	REAL		_strenght;
	bool		_b_gradient;
	bool		_b_clamped;
	bool		_b_white_multiply;
	REAL		_normal;

	o_str		_scr_name_sym;
	c_bdd_blob*	_bdd_target;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
			void	init();
};

