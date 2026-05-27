
#ifdef AAA_DEF_CARTOGRAM_H
#error "DEF_CARTOGRAM_H included more than once."
#endif
#define AAA_DEF_CARTOGRAM_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class c_bdd_tri;

class CPointF  // un point 2D ( portage from QT-trolltech )
{
public:
	REAL	x;
	REAL	y;
	CPointF() { x = .0; y = .0;}
};

class c_country  // objet descriptif d'un pays
{

public:

	// --- from country ---
	CPointF	_center;		// centre du pays initial
	CPointF	_mod_center;	// nouveau centre du pays

	bool	_b_process;

	REAL	_weight;		// poid d'un pays (qui determinera la deformation) [0 100] 50=neutre
	REAL	_area;			// aire initiale
	REAL	_mod_area;		// nouvelle aire
	REAL	_expected_area;	// aire idéale à obtenir par pays
	INT32	_country_id;	// index contenu dans le nom du pays 

	// --- from c_obj_info -// géometrie du pays
	INT32	_point_start;	// position depart dans tableau de points global
	INT32	_point_nb;		// nombre de points 
	INT32	_tri_nb;		// nombre de triangles

	REAL*	_point_tab;		// tableau de points global venant de bdd_tri
	REAL*	_point_mod_tab;	// tableau local de points modifies 
	UINT32*	_tri_tab;		// pointeur relatif à ce pays sur le tableau de triangles


	c_country() { init(); }		// constructeur

	void init()
	{
		_weight = 50.0;		// valeur 50 neutre
		_area = 0;			// important
		_mod_area = 0;		//
		_mod_center.x = _center.x = 0.0;
		_mod_center.y = _center.y = 0.0;
		_b_process = true;
	}

	CPointF	get_center( bool b_mod = true );	// bMod= centre modifié
	REAL	get_area( bool b_mod = false );	// bMod = aire modifié
	void	set_changed( bool const b );			// on a modifié les frontieres de ce pays
};


class	c_bdd;
class	c_def_cartogram final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_cartogram,c_deformer);
private:
	o_str		_target_name_symbo;
	c_bdd*		_bdd_target;
	c_bdd_tri*	_bdd_tri;		// pointer on the bdd_tri (= bdd_target )

	REAL*		_mod_point;		// pointeur de points alloué localement

	//o_country *_tab_country;         // tableau d'objets pays alloué localement

	std::vector<c_country>	_tab_country;


	bool	_b_start_trig;		// trigger de restart de l'animation du cartogramme

	REAL	_weight_power;		// calcul du poid d'un pays 
	REAL	_weight_min;		// calcul du poid d'un pays
	REAL	_weight_max;		// calcul du poid d'un pays  

	REAL	_file_min;			// calcul du poid d'un pays ( calculé )
	REAL	_file_max;			// calcul du poid d'un pays ( calculé )

	INT32	_iter_max;			// nombre d'itérations de l'algo
	INT32	_iter_current;		// iteration courante

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();						// allocation et chargement des données
			INT32	get_nb_country();			// nombre de pays
			REAL	get_expected_area( c_country c );	// aire attendue
			void	iterate_one();						// algo principal
			void	start();							// trigger 
			bool	load_weight( C_PCHAR_C fileName );		// TMP chargement 
			INT32	get_country_by_country_id( INT32 id );	// index dans tableau from name
};

