
#ifdef AAA_BDD_GENE_H
#error "BDD_GENE_H included more than once."
#endif
#define AAA_BDD_GENE_H 1


#ifndef AAA_BDD_H
#include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_gene final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_gene,c_bdd);
public:
	static C_PCHAR_C	get_sub_name( INT32 index_sub );
private:
	REAL	_center[3];
	INT32	_s_bdd;
	REAL	_size[3];
	bool	_b_bsp_loaded;
public:
			void	init();
			void	alloc(); 
			void	dealloc();

			void	build();

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override;

	virtual	bool	compute_intersection( REAL CONST* origin, REAL CONST* point, REAL* intersection );

	// Begin : UGLY from GLUT
//	static void		init_dodecahedron( void );

	static void		draw_pentagon(		INT32 CONST gl_primitive,	int CONST a, int CONST b, int CONST c, int CONST d, int CONST e );
	static void		draw_dodecahedron(	INT32 CONST gl_primitive );

	static void		record_item(		INT32 CONST gl_primitive,	REAL CONST * CONST n1, REAL CONST * CONST n2, REAL CONST * CONST n3 );
	static void		subdivide(			INT32 CONST gl_primitive,	REAL CONST * CONST v0, REAL CONST * CONST v1, REAL CONST * CONST v2 );
	static void		draw_triangle(		INT32 CONST gl_primitive,	int CONST i, REAL CONST data[][3], int CONST ndx[][3]	);

	static void		draw_icosahedron(	INT32 CONST gl_primitive );
	static void		draw_octahedron(	INT32 CONST gl_primitive );
	static void		draw_tetrahedron(	INT32 CONST gl_primitive );	
	// End : UGLY from GLUT
};



