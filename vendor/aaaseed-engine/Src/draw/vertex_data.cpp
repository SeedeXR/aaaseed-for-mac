#include "vertex_data.h"
#include "err.h"
#include "aaa_mem.h"


void	c_vertex_data::set_transformed( bool CONST b_in )
{
	_b_transformed = b_in;
	_point = (b_in && _point_transformed) ? _point_transformed : _point_original ;
}

c_vertex_data::c_vertex_data()
	:_point(nullptr)
	,_point_original(nullptr)
	,_point_transformed(nullptr)
	,_point_deformed(nullptr)
	,_nb_point(0)
	,_normal(nullptr)
	,_nb_normal(0)
	,_uv(nullptr)
	,_nb_uv(0)
	,_b_transformed(false)
{
}

c_vertex_data::~c_vertex_data()
{
	dealloc();
}

void c_vertex_data::dealloc_point()
{
	SAFE_DELETE_ARRAY( _point_original		);
	SAFE_DELETE_ARRAY( _point_transformed	);
	SAFE_DELETE_ARRAY( _point_deformed		);
	_point = nullptr;
	_nb_point = 0;
	set_transformed( false );
}
void c_vertex_data::dealloc_normal()
{
	SAFE_DELETE_ARRAY( _normal		);
	_nb_normal = 0;
}
void c_vertex_data::dealloc_uv()
{
	SAFE_DELETE_ARRAY( _uv );
	_nb_uv = 0;
}
void c_vertex_data::dealloc()
{
	dealloc_point();
	dealloc_normal();
	dealloc_uv();
}

AAA_ERR c_vertex_data::alloc_point( INT32 CONST nb )
{
	dealloc_point();
	INT32	nb_real = nb * 3;
	_point_original = new REAL[ nb_real ];
	if( _point_original )
	{
		_point = _point_original;
		_nb_point = nb;
		return AAA_OK;
	}
	return ERR_MEM_BASE;
}
AAA_ERR c_vertex_data::alloc_point_transformed()
{
	if( _point_transformed )
		return AAA_OK;
	if( _nb_point==0 )
		return ERR_ANY;
	_point_transformed = new REAL[ _nb_point * 3 ];
	return _point_transformed ? AAA_OK : ERR_MEM_BASE;
}
AAA_ERR c_vertex_data::alloc_point_deformed()
{
	if( _point_deformed )
		return AAA_OK;
	if( _nb_point==0 )
		return ERR_ANY;
	_point_deformed = new REAL[ _nb_point * 3 ];
	return _point_deformed ? AAA_OK : ERR_MEM_BASE;
}

AAA_ERR c_vertex_data::alloc_normal( INT32 CONST nb )
{
	dealloc_normal();
	if( nb == 0 )
		return AAA_OK;
	INT32	nb_real = nb * 3;
	_normal = new REAL[ nb_real ];
	if( _normal )
	{
		_nb_normal = nb;
		return AAA_OK;
	}
	return ERR_MEM_BASE;
}

AAA_ERR c_vertex_data::alloc_uv( INT32 CONST nb )
{
	dealloc_uv();
	if( nb == 0 )
		return AAA_OK;
	INT32	nb_real = nb * 2;
	_uv = new REAL[ nb_real ];
	if( _uv )
	{
		_nb_uv = nb;
		return AAA_OK;
	}
	return ERR_MEM_BASE;
}

AAA_ERR c_vertex_data::alloc( INT32 CONST nb, bool CONST b_normal, bool CONST b_uv )
{
	AAA_ERR	retcode;
	retcode = alloc_point( nb );
	if( ERR(retcode) )
		return retcode;

	if( b_normal )
	{
		retcode = alloc_normal( nb );
		if( ERR(retcode) )
			return retcode;
	}

	if( b_uv )
		retcode = alloc_uv( nb );
	return retcode;
}