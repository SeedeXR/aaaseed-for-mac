#include "def_cartogram.h"
#include "time/aaa_time.h"
#include "math/gainbias.h"
#include "obj_ui/bdd/bdd_geo/bdd_tri.h"

// =================================================================================
// --- calcule la surface d'un triangle ---

REAL tri_area( REAL Xa, REAL Ya, REAL Xb, REAL Yb, REAL Xc, REAL Yc )
{
	return ABS( ( Xb * Ya - Xa * Yb ) + ( Xc * Yb - Xb * Yc ) + ( Xa * Yc - Xc * Ya ) ) / 2;
}

// =================================================================================
// --- distance entre 2 points

REAL get_dist( CPointF a, CPointF b )
{
	return SQRT( POW_R( (REAL)( b.x - a.x ) , REAL(2) ) + POW_R( (REAL)( b.y - a.y ), REAL(2) ) );
}


// =================================================================================
// === c_country =====================================================================
// =================================================================================

CPointF c_country::get_center( bool b_mod )
{
	CPointF min,max;

	// --- un centre!= de 0 a déja été calculé ... on recalcule pas
	if ( !b_mod && ( _center.x || _center.y ) )
		return _center;		// déja calculé 
	if ( b_mod && ( _mod_center.x || _mod_center.y ) )
		return _mod_center;	// déja calculé 

	REAL* pt = ( b_mod ? _point_mod_tab : _point_tab ) + _point_start * 3 ;

	min.x =max.x = *pt;     // premier par def
	min.y = max.y = *(pt + 1); // premier par def

	for ( INT32 i = 0; i < _point_nb; ++i )
	{
		if( *pt < min.x )
			min.x = *pt;
		if( *pt > max.x )
			max.x = *pt;
		++pt;
		if( *pt < min.y )
			min.y = *pt;
		if( *pt > max.y )
			max.y = *pt;
		++pt;
		++pt;
	}
	( b_mod ? _mod_center.x : _center.x ) = ( min.x + max.x ) / 2;
	( b_mod ? _mod_center.y : _center.y ) = ( min.y + max.y ) / 2;

	return ( b_mod ? _mod_center : _center ); // memorisation
}

// =============================================================================================

REAL c_country::get_area( bool b_mod )
{
	REAL aire = .0;

	if( !b_mod && _area!=0 )
		return _area;		// eviter de recalculer
	if( b_mod && _mod_area!=0 )
		return _mod_area;	// eviter de recalculer

	UINT32* pt = _tri_tab;  // pointer triangle
	REAL*	pp = ( b_mod ? _point_mod_tab : _point_tab );

	CPointF a, b, c;
	INT32	u, v, w;

	for ( INT32 i = 0; i < _tri_nb; ++i )
	{
		u = *pt;
		a.x = pp[ *pt * 3     ]; 
		a.y = pp[ *pt * 3 + 1 ]; 
		++pt;
		v = *pt;
		b.x = pp[ *pt * 3     ]; 
		b.y = pp[ *pt * 3 + 1 ]; 
		++pt;
		w = *pt;
		c.x = pp[ *pt * 3     ]; 
		c.y = pp[ *pt * 3 + 1 ]; 
		++pt;

		aire += tri_area( a.x, a.y, b.x, b.y, c.x, c.y );
	}
	( b_mod ? _mod_area : _area ) = aire; // memorise le calcul
  return aire;
}


// =============================================================================================
// --- indication de modification des frontieres d'un pays
void c_country::set_changed( bool b ) 
{
	if( b )
	{
		_mod_center.x = _mod_center.y = 0;
		_mod_area = 0;
	}
}

// =================================================================================
// === c_def_cartogram =============================================================
// =================================================================================

FACTORY_INSTANCE_V1( c_def_cartogram, def_cartogram, Deformer Cartogram, def );

namespace n_def_cartogram
{
	CONSTEXPR INT32	BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 6;
	CONSTEXPR INT32	GROUP_NB		= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(		start_trig )
		PARAM_DEF_INT32_ZERO(	iter_max )
		PARAM_DEF_REAL(			weight_min,		.50, 0.1, .0, 99. )
		PARAM_DEF_REAL(			weight_max,		.50, 5.0, .0, 99. )
		PARAM_DEF_REAL(			weight_power,	.50, 2.0, .0, 10. )
		PARAM_DEF_REF(			bdd_name_symbo )
	};
}

CONSTRUCTOR_CREATE(c_def_cartogram)
{
	_bdd_target		= nullptr;
	_bdd_tri		= nullptr;
	//_tab_country	= nullptr;
	_mod_point		= nullptr;
	_iter_current	= 0;
	_iter_max		= 5;
	_file_min		= 0;
	_file_max		= 100;

	init_name_with( "Cartogram" );
	param_init_with( n_def_cartogram::param, n_def_cartogram::PARAM_NB_MAX);
}

// ===========================================================================================

void c_def_cartogram::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt(				h, _b_start_trig	);
	param_set_pt(				h, _iter_max		);
	param_set_pt(				h, _weight_max		);
	param_set_pt(				h, _weight_min		);
	param_set_pt(				h, _weight_power	);
	param_set_pt_attach_obj(	h, _target_name_symbo, _bdd_target );

	err_param_init_pt( h );
}

// ===========================================================================================

c_def_cartogram::~c_def_cartogram()
{
	if ( _tab_country.size() ) _tab_country.clear();
	IF_FREE_AND_NULL( _mod_point );
}

// =======================
void	c_def_cartogram::update()
{
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target && !_bdd_target->is_name_symbo( _target_name_symbo )  || !_bdd_target->get_root() )
			_bdd_target = (c_bdd*) find_by_class_start_and_name_symbo( "bdd_tri", 7, _target_name_symbo );
	}
	else
	{
		_bdd_target = nullptr;
	}

	if( _bdd_target /*&& c_bdd_tri::is_instance( _bdd_target )*/ &&  (void *)_bdd_tri != (void *)_bdd_target )
	{
		_bdd_tri=( c_bdd_tri * )_bdd_target; 
		init(); // creation du tableau de points
	}

	set_deforming( _bdd_tri && _bdd_tri->get_point_nb() != 0 );

	if( _bdd_tri && !_mod_point ) // init appelé trop tot
	{
		init(); // creation du tableau de points
	}

	if( _b_start_trig )
		start();
}
// ===========================================================================================

void	c_def_cartogram::start() // trigger 
{
	//load_weight("d:\\AAASeed\\svg\\monde.csv");
	_iter_current  = 0;
	_b_start_trig = false;
}

// ===========================================================================================

void	c_def_cartogram::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( !_mod_point )  // securite
	{
		MEMCPY( dst, src, sizeof(REAL) * 3 * nb, __FUNCTION__ );
		return;
	}

	// --- tests 

	if( nb != _bdd_target->get_point_nb() )
		return; 

	// --- copy la source au depart

	if( _iter_current == 0 )
	{
		MEMCPY( _mod_point, src, sizeof(REAL) * 3 * nb, __FUNCTION__ );
	}
	// --- iteration

	if(_iter_current < _iter_max ) 
	{	  
		iterate_one();
		//if (_iter_current) iterate_one();
		//else ++_iter_current;              // pour affichage initial 
	}

	// --- copy dans la destination 
	MEMCPY( dst, _mod_point, sizeof(REAL) * 3 * nb, __FUNCTION__ );
}

// ==========================================================================

bool c_def_cartogram::load_weight(  C_PCHAR_C fileName )
{
	FILE *file = c_file::FOPEN( fileName, "rt" );
	if ( !file )
		return false;

	for ( INT32 i = 0; i < get_nb_country(); ++i )
		_tab_country[ i ]._b_process = false;

	_file_min = 100;
	_file_max = 0;

	INT32	id, index;
	REAL	weight;

	while ( !feof( file ) )
	{
		fscanf( file, "%d %f\n", &id, &weight );

		if ( ( index = get_country_by_country_id( id ) ) != -1 )
		{
			_file_min = MAX( _file_min, weight );
			_file_max = MIN( _file_max, weight );
			_tab_country[ index ]._weight = weight;
			_tab_country[ index ]._b_process = true;
		}
	}
	c_file::FCLOSE( file );
	//printf("%f %f\n",_file_min,_file_max);

	//// --- test debug ---

	//REAL p;
	//for( INT32 i = 0; i < get_nb_country(); ++i )
	//{
	//	p = _tab_country[i]._weight;           // pas de constructeur car tableau
	//}
	return true;
}

// =============================================================================================
// allocation du buffer de points et du tableau des objets
// a rappeler à chaque changement de bdd_tri (par ex)
void	c_def_cartogram::init()
{
	INT32 aa = _bdd_tri->get_obj_nb();
	INT32 bb = _bdd_tri->get_point_nb();

	if ( !aa || !bb ) return;
	// --- allocations 		
	//_point_for_tri = new REAL[ nb_real * 3 ];

	if( _tab_country.size() )
		_tab_country.clear();
	if( _mod_point )
		FREE_ALIGNED( _mod_point );  

	_tab_country.resize( _bdd_tri->get_obj_nb() );

	_mod_point = (REAL*) MALLOC_ALIGNED(sizeof(REAL) * 3 * _bdd_tri->get_point_nb(), 0 );

	c_obj_info*	info;

	// --- copie obj_info

	char id[ 16 ];

	for( INT32 i = 0; i < _bdd_tri->get_obj_nb(); ++i )
	{
		info = _bdd_tri->get_info( i + 1 ); // commence à 1 ( 0=donnees generales )
		_tab_country[ i ].init();
		_tab_country[ i ]._point_tab		= _bdd_tri->get_points() ;
		_tab_country[ i ]._point_mod_tab	= _mod_point;
		_tab_country[ i ]._tri_tab			= _bdd_tri->get_tri_tab() + info->_tri_start * 3;
		_tab_country[ i ]._point_nb			= info->_point_nb;
		_tab_country[ i ]._tri_nb			= info->_tri_nb;
		_tab_country[ i ]._point_start		= info->_point_start;

		strcpy( id, info->_name.get() + info->_name.get_len() - 5 );
		id[ 4 ] = 0;
		_tab_country[ i ]._country_id = atol( id );
	}
	//load_weight("c:\\monde.csv");
}

// =============================================================================================
// --- calcule la surface idéale d'un pays ---

REAL c_def_cartogram::get_expected_area( c_country c )
{
	REAL	tmp = c._weight;

	tmp = ( tmp - _file_min )/(REAL)( _file_max - _file_min );
	tmp = powf( tmp,_weight_power );
	tmp = _weight_min + tmp * ( _weight_max - _weight_min );
	return c.get_area( false ) * tmp; // une maniere de calculer.
}

// =============================================================================================

INT32 c_def_cartogram::get_nb_country()
{
	return _bdd_tri->get_obj_nb();
}

// =============================================================================================

INT32 c_def_cartogram::get_country_by_country_id( INT32 id )
{
	for ( INT32 i = 0; i < get_nb_country(); ++i )
	{
		if ( _tab_country[ i ]._country_id == id ) 	return i;
	}
	return -1;
}

// =============================================================================================

// Input: A map with n cells and a value for each cell, cell _area percent error tolerance ,
// Output:New coordinates for cell vertices that make them form a value-by-_area cartogram
// http://cse.unl.edu/~revesz/publications/IDEAS00.pdf - code source utilisé 

void c_def_cartogram::iterate_one()
{
//	INT32	i, j, k;
	REAL	x, y, aireC, aireD;
	REAL	errorMax  =  REAL(0.1);	// en %
	REAL	error;
	REAL	distReff, dist;
	CPointF	point;

	if ( !_mod_point ) return; // securite

	if ( _iter_current == _iter_max ) return; // ben oui

	// --- compute the desired Area (aireD) of each cell ---

	if ( _iter_current == 0 )
		for ( INT32 i = 0; i < get_nb_country(); ++i )
			_tab_country[ i ]._expected_area  = get_expected_area( _tab_country[ i ] );

	// --- boucle sur les pays
	for ( INT32 i = 0; i <get_nb_country(); ++i )
	{
		if( _tab_country[ i ]._b_process )	// skip country we don't want to process
		{
			aireC = _tab_country[ i ].get_area( true );
			aireD = _tab_country[ i ]._expected_area;

			error = fabs( aireC - aireD ) / aireD;  // calcul de l'erreur pour ce pays

			if ( error > errorMax ) // --- si différence trop forte
			{
				distReff = REAL(100) * ABS( aireD - aireC ) / SQRT( aireC * REAL(PI) ); // distance d'influence

				REAL SQRT_AD_AC = SQRT ( aireD / aireC );

				// ---boucle sur les pays ---

				for ( INT32 j = 0; j < get_nb_country(); ++j )
				{
					if ( _tab_country[ i ]._b_process )
					{
						dist = get_dist( _tab_country[ i ].get_center(), _tab_country[ j ].get_center() );

						if (dist < distReff )  // dans la zone d'influence 
						{
							// pour chaque point de j 

							INT32 xi = INT32(_tab_country[ i ].get_center().x);
							INT32 yi = INT32(_tab_country[ i ].get_center().y);

							REAL rayonMoyen = SQRT( aireC / REAL(PI) );

							//_tab_country[j]._mod_center=CPointF(0,0); // reinit _center car modif points

							// --- boucle sur les points --- 

							REAL* pt = _tab_country[ j ]._point_mod_tab + _tab_country[ j ]._point_start * 3 ;

							for( INT32 k = 0 ; k < _tab_country[ j ]._point_nb ; ++k ) // --- pour chaque point 
							{
								point.x = *pt;
								point.y = *(pt + 1);

								REAL distxy= get_dist( point, _tab_country[ i ].get_center() );

								REAL SQRT_AD_PI = SQRT( ( aireD - aireC ) / ( REAL(PI) * POW_R( distxy, 2 ) ) + REAL(1) );   

								if (distxy < distReff )  // dans la zone d'influence 
								{
									x = point.x;
									y = point.y;

									if ( distxy < SQRT( aireC / PI ) ) // si dist < rayon du pays => influence fixe
									{
										//SQRT_AD_AC = SQRT ( aireD / aireC );
										x = xi + (x - xi) * SQRT_AD_AC;
										y = yi + (y - yi) * SQRT_AD_AC;
									}
									else // dist > rayon du pays => influence degressive
									{		
										//SQRT_AD_PI =  (aireD - aireC) / (2*PI*dist*dist);
										x = xi + (x - xi) * SQRT_AD_PI;
										y = yi + (y - yi) * SQRT_AD_PI;
									}

									*pt = x;  // ecriture des nouvelles valeurs
									*(pt + 1) = y;

								} // if (distxy < distReff )
								pt += 3;
							} // for(k=0 ;
							_tab_country[ j ].set_changed( true ); // on a modifié les frontieres de ce pays

						} // if (dist < distReff )
					}
				} // for (j = 0;
			}
		}
	} // for

	++_iter_current; // ben oui.

	// --- test finalite ---
	// --- recalcul de l'aire pour chaque pays ---
	bool b_ok = true;
	for( INT32 i = 0; i < get_nb_country(); ++i )
	{
		aireC = _tab_country[ i ].get_area( true );

		aireD = _tab_country[ i ]._expected_area;
		error = fabs( aireC - aireD ) / aireD;  // calcul de l'erreur pour ce pays
		if( error > errorMax )
			b_ok = false; // --- si différence trop forte
	}
	if( b_ok )
		_iter_current = _iter_max; // on force l'arret
}


// ==================================================================================
