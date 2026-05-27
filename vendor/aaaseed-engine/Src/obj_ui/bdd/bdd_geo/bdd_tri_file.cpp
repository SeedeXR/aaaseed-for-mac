#include "bdd_tri.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "file/file_csv.h"
#include "infrastructure/seedfile.h"
#include "file/file_io.h"
#include "spy.h"


void	c_bdd_tri::do_load_data_success( c_obj3d_file_reader* reader )
{
	if( _obj_nb != 1 )
	{
		c_obj_info*	info		= _obj_info;
		info->_point_start		= 0;
		info->_tri_start		= 0;
		info->_poly_start		= 0;
		info->_poly_index_start	= 0;
		info->_point_nb			= reader->_point_index_offset;
		info->_tri_nb			= reader->_tri_index_offset;
#if AAA_POLY_COMPILE()
		info->_poly_nb			= reader->_poly_count_offset;
#endif
	}
	_model_filename.set_fname_relative( reader->_filename );
	_b_valid_data = true;

	obj_do_sorts();
	load_obj_info();

	//			init_transfo();
	//			transform();
	//			prepare_uv_for_tri();
#if AAA_POLY_COMPILE()
	GOOD_PRINT_STRING( "loaded %d obj : %d pt, %d nor, %d uv, %d tri, %d poly.",
		_obj_nb,
		reader->_point_index_offset, reader->_normal_index_offset, reader->_uv_index_offset,
		reader->_tri_index_offset, reader->_poly_count_offset );
#else
	GOOD_PRINT_STRING( "loaded %d obj : %d pt, %d nor, %d uv, %d tri.",
		_obj_nb, reader->_point_index_offset, reader->_normal_index_offset, reader->_uv_index_offset, reader->_tri_index_offset );
#endif
	PRINT_CR();
	_b_need_build = true;
}

namespace{
	c_file_io			file_reader;
}

AAA_ERR	c_bdd_tri::load_data( o_str CONST & filename )
{
	//	if( !_b_model_load_needed )
	//		return AAA_OK;
	_b_model_load_needed = false;
	_b_point_need_transform = true;
#if AAA_POLY_COMPILE()
	_b_poly_in_file = false;
#endif

	AAA_ERR retcode = ERR_FILE_BASE;
	if( filename.is_empty() )
		return retcode;

	c_obj3d_file_info	found;
	c_obj3d_file_reader	reader;

	reader.begin( this, filename.get() );
	if( reader._file_type == c_obj3d_file_reader::F3D_UNKNOWN )
		goto exit;

	if( reader._file_type == c_obj3d_file_reader::F3D_SVG )
	{
		geo_dealloc();
		_b_normal_in_file = false; 
		_b_uv_in_file = false; 
		_obj_nb = 0;
		// read svg file
		retcode = read_points_from_svg( filename.get() );
		//tesselate data
		if( !_svg_contour.empty() )
		{
			for( INT32 i = 0; i < (INT32)_svg_contour.size(); ++i )
			{
				if( _b_tessellate_mesh )  // SC10
					svg_tessellate_mesh( i );
				else
					svg_tesselate( i );
			}
			// convert vector of vertices and triangles to aaaseed internal format
			svg_convert( &reader );
			do_load_data_success( &reader );
		}
		return retcode;
	}

	SPY_PUSH_RANGE( "load_data_from_file", spy::FILE_HIGH );

	reader._b_flip_yz_at_read = _b_flip_yz_at_read;
	reader._b_flip_x_at_read = _b_flip_x_at_read;
	reader._b_read_tri = true;

	if( ERR( file_reader.read_file(filename) ) )
	{
		box_err( "Can't read %s file.", filename.get() );
		goto exit;
	}
	reader.set_file_reader( &file_reader );


//
//	Read file (obj alias wavefront for now)
//
//	first we just count the different elements by parsing the whole file once
	_b_normal_in_file	= false; 
	_b_uv_in_file		= false;
	_b_verbose			= false;
	if( ERR( reader.count_elt_from_stream( &found ) ) )
		goto exit;
	_b_normal_in_file	=	found._normal > 0;
	_b_uv_in_file		=	found._uv > 0;
#if AAA_POLY_COMPILE()
	_b_poly_in_file		=	found._b_poly;
#endif

	GOOD_PRINT_STRING( "\tFound %d Obj  %d pt  %d nor  %d uv  %d tri  %d face.", 
		found._obj, found._point, found._normal, found._uv, found._tri, found._face );
	_b_verbose = true;

// DEALLOC
	geo_dealloc();
// ALLOC
//todo regroup both in a data realloc
#if AAA_POLY_COMPILE()
	INT32 nb_poly = _b_poly_in_file ? found._face : 0;

	if( !geo_alloc( found._obj, found._point, found._normal, found._uv, found._tri, nb_poly ) )
	{
		//todo move this in geo alloc
		box_err( "Can't allocate %d points %d normals %d uv %d triangles and %d polys.",
					found._point, found._normal, found._uv, found._tri, nb_poly );
		retcode = ERR_MEM_BASE;
		goto exit;
	}
#else
	if( !geo_alloc( found._obj, found._point, found._normal, found._uv, found._tri, 0 ) )
	{
		//todo move this in geo alloc
		box_err( "Can't allocate %d points %d normals %d uv %d triangles.",
					found._point, found._normal, found._uv, found._tri );
		retcode = ERR_MEM_BASE;
		goto exit;
	}
#endif

	clear_v3( reader._offset_at_load );	//todo not really use, could do it
	reader.set_dst_buffer(	_vertex_data[0].get_point_original(),
							_b_uv_in_file ? _vertex_data[0].get_uv() : nullptr,
							_vertex_data[0].get_normal()
							);

	{
		c_obj_info* info = _obj_info;
		if( found._obj > 1 )
			++info;
		retcode = reader.load_data( found._obj, info );
	}
	if( ERR(retcode) )
		goto exit;

	_obj_nb = reader._obj_nb;
	do_load_data_success( &reader );

	//todo add a check by object
	if( found._obj		!= _obj_nb )
		ERR_PRINT_STRING( "\tlost %d objects",		found._obj		- _obj_nb						);
	if( found._point	!= reader._point_index_offset )
		ERR_PRINT_STRING( "\tlost %d points",		found._point	- reader._point_index_offset	);
	if( found._normal	!= reader._normal_index_offset )
		ERR_PRINT_STRING( "\tlost %d normal",		found._normal	- reader._normal_index_offset	);
	if( found._uv		!= reader._uv_index_offset )
		ERR_PRINT_STRING( "\tlost %d uv coords",	found._uv		- reader._uv_index_offset		);
	//hack
	if( found._tri		!= reader._tri_index_offset )
		ERR_PRINT_STRING( "\tlost %d triangles",	found._tri		- reader._tri_index_offset		);
	_nb_tri =  MIN( reader._tri_index_offset, found._tri );
#if AAA_POLY_COMPILE()
	if( found._face		!= reader._poly_count_offset )		ERR_PRINT_STRING( "\tlost %d polygons",		found._face		- reader._poly_count_offset		);
	_nb_poly =  MIN( reader._poly_count_offset, found._face );
#endif

	//	read morph targets
	//
	{
		o_str tmp_filename(filename);
		tmp_filename.drop_ext();
		tmp_filename.add( "_morph_x.obj" );
		CHAR* pt_letter = tmp_filename.get_changeable(-5);	//	point to the letter now

		for( INT32 i=1; i<=MORPH_NB_MAX; ++i )
		{
			*pt_letter = 'a'+i-1; 
			if( !c_file::is_exist(tmp_filename) )
				continue;
			GOOD_PRINT_STRING( "find morph target %s", tmp_filename.get() );
			if( ERR(file_reader.read_file( tmp_filename )) )
			{
				ERR_PRINT_STRING( "can't open it, so %s skip %s", get_class_name(), tmp_filename.get() );
				continue;
			}

			c_obj3d_file_info	found_bis;
			if( ERR( reader.count_elt_from_stream( &found_bis ) ) )
				goto exit;

			bool b_ok = true;
			if( found._point != found_bis._point )
			{
				ERR_PRINT_STRING( "try to read %s as morph target, but %d points here",		tmp_filename.get(), found_bis._point	);
				b_ok = false;
			}
			if( found._normal != found_bis._normal )
			{
				ERR_PRINT_STRING( "try to read %s as morph target, but %d normals here",	tmp_filename.get(), found_bis._normal	);
				b_ok = false;
			}
			if( found._uv != found_bis._uv )
			{
				ERR_PRINT_STRING( "try to read %s as morph target, but %d uvs here",		tmp_filename.get(), found_bis._uv		);
				b_ok = false;
			}
			//todo use eventually
			//_b_normal_in_file	=	found.normal > 0;
			//_b_uv_in_file		=	found.uv > 0;
			//_b_poly			=	found.b_poly;

			if( !b_ok )
				continue;

			GOOD_PRINT_STRING( "will read %s as morph target",		tmp_filename.get(), found_bis._point	);

			reader.init();
			reader.set_file_reader( &file_reader );
			reader._b_read_tri = false;

			c_vertex_data&	vd = _vertex_data[i];
			vd.dealloc();
			vd.alloc_point(		found._point	);
			vd.alloc_normal(	found._normal	);

			reader.set_dst_buffer(	vd.get_point_original(),
									nullptr,
									vd.get_normal()
									);

			//todo check result
			reader.load_data( found._obj, nullptr  );

			do_load_data_success( &reader );
		}
	}

exit:
	//	HEAP_IS_CORRUPT();

	if( NOERR(retcode) )
	{
		if( _b_unit_center_at_read_ui )
		{
			center();
			scale_to_unit();
		}
	}
	else
	{
		geo_dealloc();
		_b_valid_data = false;
		box_err( "Could not read 3D model %s.", filename.get() );
	}
	//	param_set_nax_no_inc( index_point_for_ui+3, _nb_tri);
	SPY_POP_RANGE();
	return retcode;
}

AAA_ERR	c_bdd_tri::load_data()
{
	AAA_ERR	retcode = AAA_OK;
	if( _b_model_load_needed )
		retcode = load_data( _model_filename );
	return retcode;
}

AAA_ERR	c_bdd_tri::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	switch( type_io )
	{
	case aaa::file::TYPE_IO_OBJ_WAVEFRONT:
		return load_data( filename );
	case aaa::file::TYPE_IO_BDD_TRI:
		return load_from_file( filename );
	}
	return ERR_TYPE_UNUSED;
}

//todoqq do better and generalize this dynamic extension to an object
void	c_bdd_tri::load_obj_info()
{
	if( _b_valid_data && _obj_nb > 1 )
	{
		REAL*	data;
		//INT32	i;
		INT32	nb;
		CHAR	fname[AAA_PATH_MAX()];

		strcpy( fname, get_my_filename() );
		strcat( fname, ".more.csv");

		if( c_file::is_exist( fname ) )
		{
			nb = 5*_obj_nb;
			data = (REAL*) MALLOC_ALIGNED( sizeof(REAL)*nb, 0 );
			if( data )
			{
				INT32	nb_read = file_csv::read_float_grid( fname, data, 5, _obj_nb );
				if( nb_read <= nb )
				{
					c_obj_info*	info;
					REAL*	pt = data;
					INT32	id;

					nb_read /= 5;
					for( INT32 i = 1; i <= nb_read; ++i )
					{
						id = (INT32) *pt++;
						info = get_info( id );
						if( info )
						{
							info->_legend_tra[0] = *pt++;
							info->_legend_tra[1] = *pt++;
							info->_legend_tra[2] = *pt++;
							info->_legend_scale = *pt++;
						}
						else
							pt += 4;
					}
				}
				FREE_ALIGNED(data);
			}
		}
	}
}

void	c_bdd_tri::save_obj_info()
{
	if( _b_valid_data && _obj_nb > 1 )
	{
		REAL*	data;
		//INT32	i;
		INT32	nb;
		nb = 5 * _obj_nb;
		data = (REAL*) MALLOC_ALIGNED( sizeof(REAL)*nb, 0 );
		if( data )
		{
			CHAR	fname[ AAA_PATH_MAX() ];
			bool	b_need_save = false;

			strcpy( fname, get_my_filename() );
			strcat( fname, ".more.csv" );

			c_obj_info*	info;
			REAL	test;
			REAL*	pt = data - 1;

			for( INT32 i = 1; i <= _obj_nb; ++i )
			{
				info = get_info( i );
				*++pt = (REAL)info->_obj_info_id;
				if( info )
				{
					*++pt = test = info->_legend_tra[0];
					if( test != 0. )
						b_need_save = true;
					*++pt = test = info->_legend_tra[1];
					if( test != 0. )
						b_need_save = true;
					*++pt = test = info->_legend_tra[2];
					if( test != 0. )
						b_need_save = true;
					*++pt = test = info->_legend_scale;
					if( test != 1. )
						b_need_save = true;
				}
				else
					pt += 4;
			}
			if( b_need_save )
				file_csv::write_real_grid( fname, data, 5, _obj_nb );
			FREE_ALIGNED( data );
		}
	}
}

void	c_bdd_tri::save_obj_file( FILE* file )
{
	//save 3d as Wavefront obj
	if( _b_valid_data && _obj_nb > 1)
	{
		c_obj_info*	info;
		REAL*	pt = nullptr;
		UINT32*	pf = nullptr;

		for( INT32 i = 1; i <= _obj_nb; ++i )
		{
			info = get_info( i );

			// save points
			fprintf( file, "o %s\n", info->_name.get() );
			fprintf( file, "g %s\n", info->_name.get() );
			pt = _vertex_data[0].get_point() + info->_point_start * 3;
			for( INT32 j = 0; j < info->_point_nb; ++j )
			{
				fprintf( file, "v %f %f %f\n", *pt, *(pt+1), *(pt+2) );
				pt += 3;
			}
			fprintf( file, "# %d vertices\n\n", info->_point_nb );
			// save normal
			if( _b_normal_in_file )
			{
				pt = _normal_by_point + info->_point_start * 3;
				for( INT32 j = 0; j < info->_point_nb; ++j )
				{
					fprintf( file, "vn %f %f %f\n", *pt, *(pt+1), *(pt+2) );
					pt += 3;
				}
				fprintf( file, "# %d vertex normal\n\n", info->_point_nb );
			}
			// save object name
			// hack about the names, because \ is messing with data in obj
			fprintf( file, "g\n" );
			//			fprintf( file, "o %c%s%c\n\n", '"',info->_name.get(), '"' );

			pf = _tri_index_point + info->_tri_start * 3;
			for( INT32 j = 0; j < info->_tri_nb; ++j )
			{
				fprintf( file, "f %d %d %d\n", (*pf) + 1, *(pf+1) + 1, *(pf+2) + 1 );
				pf += 3;
			}
			fprintf( file, "# %d faces\n\n", info->_tri_nb );
		}
	}
}
