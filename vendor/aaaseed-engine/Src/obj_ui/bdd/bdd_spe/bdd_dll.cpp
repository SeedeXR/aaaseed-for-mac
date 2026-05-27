
#include "bdd_dll.h"


#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

FACTORY_CREATE_PROP_V1( c_bdd_dll, bdd_dll, External Dll, bdd_dll, sub_menu="Special"; );

namespace n_bdd_dll
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 2 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 BOOL_PARAM_NB	= 16;
	CONSTEXPR INT32 INT_PARAM_NB	= 16;
	CONSTEXPR INT32 REAL_PARAM_NB	= 16;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 3;	
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	BOOL_PARAM_NB
								+	INT_PARAM_NB
								+	REAL_PARAM_NB
								+	GROUP_PARAM_NB;
			
//todo use macro to simplify code
	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_REF( dll_name )
		PARAM_DEF_GROUP_CLOSED( Param_bool, BOOL_PARAM_NB )
			PARAM_DEF_BOOL_OFF( bool_01 )
			PARAM_DEF_BOOL_OFF( bool_02 )
			PARAM_DEF_BOOL_OFF( bool_03 )
			PARAM_DEF_BOOL_OFF( bool_04 )
			PARAM_DEF_BOOL_OFF( bool_05 )
			PARAM_DEF_BOOL_OFF( bool_06 )
			PARAM_DEF_BOOL_OFF( bool_07 )
			PARAM_DEF_BOOL_OFF( bool_08 )
			PARAM_DEF_BOOL_OFF( bool_09 )
			PARAM_DEF_BOOL_OFF( bool_10 )
			PARAM_DEF_BOOL_OFF( bool_11 )
			PARAM_DEF_BOOL_OFF( bool_12 )
			PARAM_DEF_BOOL_OFF( bool_13 )
			PARAM_DEF_BOOL_OFF( bool_14 )
			PARAM_DEF_BOOL_OFF( bool_15 )
			PARAM_DEF_BOOL_OFF( bool_16 )
		PARAM_DEF_GROUP_CLOSED( Param_int, INT_PARAM_NB )
			PARAM_DEF_INT32_ZERO( int_01 )
			PARAM_DEF_INT32_ZERO( int_02 )
			PARAM_DEF_INT32_ZERO( int_03 )
			PARAM_DEF_INT32_ZERO( int_04 )
			PARAM_DEF_INT32_ZERO( int_05 )
			PARAM_DEF_INT32_ZERO( int_06 )
			PARAM_DEF_INT32_ZERO( int_07 )
			PARAM_DEF_INT32_ZERO( int_08 )
			PARAM_DEF_INT32_ZERO( int_09 )
			PARAM_DEF_INT32_ZERO( int_10 )
			PARAM_DEF_INT32_ZERO( int_11 )
			PARAM_DEF_INT32_ZERO( int_12 )
			PARAM_DEF_INT32_ZERO( int_13 )
			PARAM_DEF_INT32_ZERO( int_14 )
			PARAM_DEF_INT32_ZERO( int_15 )
			PARAM_DEF_INT32_ZERO( int_16 )
		PARAM_DEF_GROUP_CLOSED( Param_float, REAL_PARAM_NB )
			PARAM_DEF_REAL_ZERO( real_01 )
			PARAM_DEF_REAL_ZERO( real_02 )
			PARAM_DEF_REAL_ZERO( real_03 )
			PARAM_DEF_REAL_ZERO( real_04 )
			PARAM_DEF_REAL_ZERO( real_05 )
			PARAM_DEF_REAL_ZERO( real_06 )
			PARAM_DEF_REAL_ZERO( real_07 )
			PARAM_DEF_REAL_ZERO( real_08 )
			PARAM_DEF_REAL_ZERO( real_09 )
			PARAM_DEF_REAL_ZERO( real_10 )
			PARAM_DEF_REAL_ZERO( real_11 )
			PARAM_DEF_REAL_ZERO( real_12 )
			PARAM_DEF_REAL_ZERO( real_13 )
			PARAM_DEF_REAL_ZERO( real_14 )
			PARAM_DEF_REAL_ZERO( real_15 )
			PARAM_DEF_REAL_ZERO( real_16 )
	};
}


void	c_bdd_dll::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _o_dll_name );
	++h;
		param_set_pt_n( h, _param_dll.param_bool, 16 );
	++h;
		param_set_pt_n( h, _param_dll.param_int, 16 );
	++h;
		param_set_pt_n( h, _param_dll.param_real, 16 );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_bdd_dll)
{
	_h_lib = 0;
	_pf_alloc = nullptr;
	_pf_dealloc = nullptr;
	_pf_draw = nullptr;
	_pf_update = nullptr;

	_b_loaded = false;
	_b_init = false;
	param_init_with( n_bdd_dll::param, n_bdd_dll::PARAM_NB );
}

c_bdd_dll::~c_bdd_dll()
{
	if( _b_loaded )
	{
		// call deinit function in dll
		_pf_dealloc();
	}
	if( _h_lib )
	{
		::FreeLibrary( _h_lib );
	}
}

void	c_bdd_dll::update()
{
	if( is_active() )
	{
		if( !_b_loaded )
		{
			if( _o_dll_name.get_len() == 0 )
			{
				// empty string don't bother
				return;
			}
			// Load dll library
			wchar_t * wch = sysutils::utf8_to_unicode( _o_dll_name.get() );
			_h_lib = ::LoadLibraryW( wch );
			sysutils::free_str_tmp( wch );
			if( _h_lib == 0 )
			{
				ERR_PRINT_STRING( "Bdd_dll unable to load %s.", _o_dll_name.get() );
				return;
			}

			// Dll exist, get function pointers
			_pf_alloc = (AAA_DLL_FN_ALLOC)::GetProcAddress( _h_lib, "alloc" );
			if( !_pf_alloc )
			{
				ERR_PRINT_STRING( "Bdd_dll unable to get pointer to alloc in %s.", _o_dll_name.get() );
				return;
			}
			_pf_dealloc = (AAA_DLL_FN_VOID )::GetProcAddress( _h_lib, "dealloc" );
			if( !_pf_dealloc )
			{
				ERR_PRINT_STRING( "Bdd_dll unable to get pointer to dealloc() in %s.", _o_dll_name.get() );
				return;
			}
			_pf_draw = (AAA_DLL_FN_INT32)::GetProcAddress( _h_lib, "draw" );
			if( !_pf_draw )
			{
				ERR_PRINT_STRING( "Bdd_dll unable to get pointer to draw() in %s.", _o_dll_name.get() );
				return;
			}
			_pf_update = (AAA_DLL_FN_INT32)::GetProcAddress( _h_lib, "update" );
			if( !_pf_update )
			{
				ERR_PRINT_STRING( "Bdd_dll unable to get pointer to update() in %s.", _o_dll_name.get() );
				return;
			}

			// everything is loaded
			_b_loaded = true;
			if( _pf_alloc( &_param_dll ) )
			{
				_b_init = true;
				_pf_update();
			}
		}
		else
		{
			// update
			if( _b_init )
				_pf_update();
		}
	}
}

void	c_bdd_dll::draw()
{
	if( is_active() && _b_init )
		_pf_draw();
}
