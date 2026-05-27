
#ifdef AAA_BDD_IMG_GENE_H
#error "BDD_IMG_GENE_H included more than once."
#endif
#define AAA_BDD_IMG_GENE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
class c_img_2d;
class c_img_3d;

class	c_bdd_img_gene final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_img_gene,c_bdd);
public:
private:
	INT32	_size_pixel[3];
	INT32	_size_pixel_ui[3];
	INT32	_bind_last_2d		{-42};
	INT32	_bind_last_3d		{-42};

	bool	_b_texture_size_min;

	REAL	_min_max_ui[2];

	bool	_b_3d_ui;
	bool	_b_sphere_ui;

private:
	//todo
	//FINLINE REAL	make_u( INT32 iu, REAL su );
	//FINLINE REAL	make_v( INT32 iv, REAL sv );
public:
			void	init();
	
	virtual bool	can_implicit() final override;
	virtual	void	draw();
	virtual	void	update();

			void	compute_2d( c_img_2d* img );
			void	compute_3d( c_img_3d* img );

	virtual	void	param_init_pt();
};
