#include "bdd_tri.h"
#include <GL/glu.h>
#include "obj_ui/tessellation/triangle.h" 
#include "tinyxml/tinyxml.h"
#include "aaa_os.h"

typedef	void	(__stdcall *GluTessCallbackType)();

enum	SVG_PATH : INT32
{
	SVG_PATH_NONE = 0,
	SVG_PATH_CLOSE,
	SVG_PATH_MOVE_TO_A,
	SVG_PATH_MOVE_TO_R,
	SVG_PATH_LINE_TO_A,
	SVG_PATH_LINE_TO_R,
	SVG_PATH_H_LINE_TO_A,
	SVG_PATH_H_LINE_TO_R,
	SVG_PATH_V_LINE_TO_A,
	SVG_PATH_V_LINE_TO_R,
	SVG_PATH_CURVE_TO_A,
	SVG_PATH_CURVE_TO_R,
	SVG_PATH_SMOOTH_CURVE_TO_A,
	SVG_PATH_SMOOTH_CURVE_TO_R,
	SVG_PATH_QUADRATIC_A,
	SVG_PATH_QUADRATIC_R,
	SVG_PATH_SMOOTH_QUADRATIC_A,
	SVG_PATH_SMOOTH_QUADRATIC_R,
	SVG_PATH_ARC_A,
	SVG_PATH_ARC_R,
	SVG_PATH_DOT,
	SVG_PATH_COMMA,
	SVG_PATH_SPACE,
	SVG_PATH_MINUS,
};

AAA_ERR	c_bdd_tri::svg_tesselate( INT32 index )
{
	GLUtesselator*	tess = 0;

	// create tesselation object
	tess = gluNewTess();
	if( !tess )
	{
		ERR_PRINT_STRING( "%s() : creation of tesselation object failed.", __FUNCTION__ );
		return ERR_ANY;
	}

	// register tesselation callbacks
	gluTessCallback( tess, GLU_TESS_BEGIN_DATA, reinterpret_cast<GluTessCallbackType>(tess_begin_data_cb) );
	//	gluTessCallback( tess, GLU_TESS_EDGE_FLAG_DATA, reinterpret_cast<GluTessCallbackType>(tess_edge_flag_data_cb) );
	gluTessCallback( tess, GLU_TESS_VERTEX_DATA, reinterpret_cast<GluTessCallbackType>(tess_vertex_data_cb) );
	gluTessCallback( tess, GLU_TESS_END_DATA, reinterpret_cast<GluTessCallbackType>(tess_end_data_cb) );
	gluTessCallback( tess, GLU_TESS_COMBINE_DATA, reinterpret_cast<GluTessCallbackType>(tess_combine_data_cb) );
	gluTessCallback( tess, GLU_TESS_ERROR_DATA, reinterpret_cast<GluTessCallbackType>(tess_error_data_cb) );

	//UINT32	vertex_num = 0;
	gluTessProperty( tess, GLU_TESS_WINDING_RULE , GLU_TESS_WINDING_ODD );
	gluTessProperty( tess, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );

	//// copy all points to vertices vector
	//size_t	contours_size = _svg_contour[ index ]._contours.size();
	//for ( size_t i = 0; i < contours_size; ++i )
	//{
	//	// go through the points in the contour
	//	size_t	contour_size = _svg_contour[ index ]._contours[ i ].size();
	//	for ( size_t j = 0; j < contour_size; ++j )
	//	{
	//		_svg_contour[index]._vertices.push_back( _svg_contour[ index ]._contours[ i ][ j ] );
	//	}
	//}
	// begin polygon
	gluTessBeginPolygon( tess, reinterpret_cast<void*>(this) );

	//	std::vector<contour_obj>	_svg_contour;
	_svg_callback_info._index_obj = index;
	INT32	vertex_num = 0;
	INT32	contours_size = INT32(_svg_contour[ index ]._contours.size());
	for( INT32 i = 0; i < contours_size; ++i )
	{
		gluTessBeginContour(tess);
		_svg_callback_info._index_contour = i;

		// go through the points in the contour
		INT32	contour_size = INT32(_svg_contour[ index ]._contours[ i ].size());
		for( INT32 j = 0; j < contour_size; ++j )
		{
			//		DBG_PRINT_STRING( "%f %f %f\n", vec[0], vec[1], vec[2] );
			_svg_contour[index]._vertices.push_back( _svg_contour[ index ]._contours[ i ][ j ] );
			gluTessVertex( tess, reinterpret_cast<double*>(&_svg_contour[ index ]._contours[ i ][ j ]), reinterpret_cast<HANDLE>(static_cast<intptr_t>(vertex_num)) );
			++vertex_num;
		}

		gluTessEndContour( tess );
	}

	// end polygon
	gluTessEndPolygon( tess );


	// destroy the tesselation object
	gluDeleteTess( tess );
	tess = 0;

	return	AAA_OK;
}

void	c_bdd_tri::svg_convert( c_obj3d_file_reader* reader )
{
	INT32	nb_point_in = 0;
	INT32	nb_tri_in = 0;
	c_obj_info*	info;
//	INT32	*p;
	UINT32	*pi;

	INT32	nb_obj_in = INT32(_svg_contour.size());

	// count number of vertices and triangles
	for ( INT32 i = 0; i < nb_obj_in; ++i )
	{
		nb_point_in += INT32(_svg_contour[i]._vertices.size());
		nb_tri_in += INT32(_svg_contour[i]._triangles.size()) / 3;
	}

	geo_alloc( nb_obj_in, nb_point_in, 0, 0, nb_tri_in, 0 );
	//todo we have something strange in number of poly
#if AAA_POLY_COMPILE()
	if ( nb_point_in != _nb_point || _nb_normal != 0 || _nb_uv != 0 || nb_tri_in != _nb_tri || ( _b_poly_in_file && ( _nb_poly != 0 ) ) )
	{
		box_err( "Can't allocate %d points %d normals %d uv %d triangles and %d polys.", nb_point_in, 0, 0, nb_tri_in, 0 );
		//		retcode = ERR_MEM_BASE;
		goto exit;
	}
#else
	if ( nb_point_in != _nb_point || _nb_normal != 0 || _nb_uv != 0 || nb_tri_in != _nb_tri )
	{
		box_err( "Can't allocate %d points %d normals %d uv and %d triangles.", nb_point_in, 0, 0, nb_tri_in );
		//		retcode = ERR_MEM_BASE;
		goto exit;
	}
#endif


	info = _obj_info;
	if( nb_obj_in > 1 )
		++info;

//	p = _tri_from_file_x3;
	pi = _tri_index_point;
	for( INT32 obj_index = 0; obj_index < nb_obj_in; ++obj_index )
	{
		info->_point_start		= reader->_point_index_offset;
		info->_tri_start		= reader->_tri_index_offset;
#if AAA_POLY_COMPILE()
		info->_poly_start		= reader->_poly_count_offset;
		info->_poly_index_start	= reader->_poly_index_offset;
#endif
		info->_mtl_index		= -1;	
		info->_obj_index		= _obj_nb;
		info->_point_nb			= INT32(_svg_contour[ obj_index ]._vertices.size());
		info->_obj_info_id		= _obj_nb + 1;

		{
			REAL * pf = _vertex_data[0].get_point_original() - 1;	//maa check transformed... in these cases
			for( INT32 i = 0; i < info->_point_nb; ++i )
			{
				Vertex CONST & v = _svg_contour[ obj_index ]._vertices[i];
				*++pf = REAL( v.x ); // / max_xy;
				*++pf = REAL( v.y ); // / max_xy;
				*++pf = REAL( v.z );
			}
		}

		INT32	tri_size = INT32(_svg_contour[ obj_index ]._triangles.size());
		for ( INT32 i = 0; i < tri_size; ++i )
		{
			UINT32	tmp = _svg_contour[ obj_index ]._triangles[i] + reader->_point_index_offset;
			*pi = tmp;
			//*p = (INT32) tmp * 3;
			//	printf( "triangles *p = %d\n", *p );
			++pi;
//			++p;		
		}

		reader->_point_index_offset += info->_point_nb;
		reader->_tri_index_offset += tri_size / 3;
		info->_tri_nb = tri_size / 3;
		info->_poly_nb = 0;

		//printf( " point start = %d, tri start = %d, point size = %d\n", info->_point_start, info->_tri_start, info->_point_nb );
		//printf( " pio = %d, tio = %d, _tri_nb %d\n", reader->_point_index_offset, reader->_tri_index_offset, info->_tri_nb );
		clear_v3( info->_legend_tra );
		info->_legend_scale = 1;

		//todo make a fn for that
		info->_name = _svg_contour[obj_index]._name;

		++info;
		++_obj_nb;
	}

exit:
	return;
}

o_str	c_bdd_tri::svg_layer_name()
{
	o_str	buf;
	size_t	layer_name_size = _layer_name.size();
	for ( UINT32 i = 0; i < layer_name_size; ++i )
	{
		buf.add( _layer_name[i].get() );
		buf.add_char( '_' );
	}
	return buf;
}

std::vector<Vertex>	c_bdd_tri::svg_process_polygon( C_PCHAR attribute_value )
{
	std::vector<Vertex> points;

	std::vector<o_str>	split;
	o_str	str_temp;
	str_temp.set( attribute_value );
	//	str_temp.strip( "\t\n" );
	str_temp.split( split, ", \t\n" );

	// todoqq : check to see if we have the right number of points => %2
	if( !split.empty() )
	{
		size_t	split_size = split.size();
		for ( size_t i = 0; i < split_size; i = i + 2 )
		{
			Vertex	coord;
			coord.x = atof( split[ i ].get() );
			coord.y = atof( split[ i + 1 ].get() );

			points.push_back( coord );
			//			printf( "%f\n", atof( split[ i ].get() ) );
		}
	}
	return points;
}

std::vector<DOUBLE>	c_bdd_tri::svg_split_path( C_PCHAR values )
{
	std::vector<DOUBLE>	points;
	o_str	tmp;
	tmp.set( values );
	std::vector<o_str>	split1;
	std::vector<o_str>	split3;
	//	points.push_back( .0 );
	tmp.split( split1, ", \t\n" );

	// todoqqq : missing one case : 0.6.5 => 0.6 0.5
	if( !split1.empty() )
	{
		size_t	split1_size = split1.size();
		for ( size_t i = 0; i < split1_size; ++i )
		{
			std::vector<o_str>	split2;
			split1[i].split( split2,  '-' );
			if( !split2.empty() )
			{
				size_t	split2_size = split2.size();
				for ( size_t j = 0; j < split2_size; ++j )
				{
					if( j == 0 )
					{
						if ( split1[i].get_char( 0 ) == '-' )
						{
							o_str	temp2;
							temp2.set( "-" );
							temp2.add( split2[j].get() );
							split3.push_back( temp2 );
						}
						else
						{
							o_str	temp2;
							temp2.add( split2[j].get() );
							split3.push_back( temp2 );
						}
					}
					else
					{
						o_str	temp2;
						temp2.set( "-" );
						temp2.add( split2[j].get() );
						split3.push_back( temp2 );
					}
				}
			}
			//get_char
			//count_char

		}
		size_t	split3_size = split3.size();
		for ( size_t i = 0; i < split3_size; ++i )
		{
			points.push_back( atof( split3[i].get() ) );
		}
	}
	return points;
}

std::vector<Vertex>	c_bdd_tri::svg_process_path( C_PCHAR attribute_value )
{
	std::vector<Vertex>	contour;

	C_PCHAR	pt = attribute_value;
	C_PCHAR	start = pt;
	C_PCHAR	pt_debut = pt;
	INT32	len = INT32(strlen( attribute_value ));
	//INT32	index_debut = 0;
	//INT32	index_fin = 0;
	SVG_PATH	last_command = SVG_PATH_NONE;
	SVG_PATH	new_command = SVG_PATH_NONE;
	Vertex	last_point;
	std::vector<DOUBLE>	points;

	while ( *pt != 0 && ( ( pt - start ) < len ) )
	{
		switch( *pt )
		{
		case 'M' :	// Move to absolute (x y)+
			new_command = SVG_PATH_MOVE_TO_A;
			break;
		case 'm' :	// Move to relative (x y)+
			new_command = SVG_PATH_MOVE_TO_R;
			break;
		case 'Z' :	// Close path (none)
		case 'z' :
			new_command = SVG_PATH_CLOSE;
			break;
		case 'L' :	// lineto absolute (x y)+
			new_command = SVG_PATH_LINE_TO_A;
			break;
		case 'l' :	// lineto relative (x y)+
			new_command = SVG_PATH_LINE_TO_R;
			break;
		case 'H' :	// Horizontal lineto absolute x+
			new_command = SVG_PATH_H_LINE_TO_A;
			break;
		case 'h' :	// Horizontal lineto relative x+
			new_command = SVG_PATH_H_LINE_TO_R;
			break;
		case 'V' :	// Vertical lineto absolute y+
			new_command = SVG_PATH_V_LINE_TO_A;
			break;
		case 'v' :	// Vertical lineto relative y+
			new_command = SVG_PATH_V_LINE_TO_R;
			break;
		case 'C' :	// curveto absolute (x1 y1 x2 y2 x y)+
			new_command = SVG_PATH_CURVE_TO_A;
			break;
		case 'c' :	// curveto relative (x1 y1 x2 y2 x y)+
			new_command = SVG_PATH_CURVE_TO_R;
			break;
		case 'S' :	// shorthand/smooth curveto absolute (x2 y2 x y)+
			new_command = SVG_PATH_SMOOTH_CURVE_TO_A;
			break;
		case 's' :	// shorthand/smooth curveto relative (x2 y2 x y)+
			new_command = SVG_PATH_SMOOTH_CURVE_TO_R;
			break;
		case 'Q' :	// quadratic Bézier curveto absolute (x1 y1 x y)+
			new_command = SVG_PATH_QUADRATIC_A;
			break;
		case 'q' :	// quadratic Bézier curveto relative (x1 y1 x y)+
			new_command = SVG_PATH_QUADRATIC_R;
			break;
		case 'T' :	// Shorthand/smooth quadratic Bézier curveto absolute (x y)+
			new_command = SVG_PATH_SMOOTH_QUADRATIC_A;
			break;
		case 't' :	// Shorthand/smooth quadratic Bézier curveto relative (x y)+
			new_command = SVG_PATH_SMOOTH_QUADRATIC_R;
			break;
		case 'A' :	// elliptical arc absolute (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
			new_command = SVG_PATH_ARC_A;
			break;
		case 'a' :	// elliptical arc relative (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
			new_command = SVG_PATH_ARC_R;
			break;
			//case '.' :	// decimal point
			//	new_command = SVG_PATH_DOT;
			//	break;
			//case ',' :	// separation
			//	new_command = SVG_PATH_COMMA;
			//	break;
			//case ' ' :  // separation
			//	new_command = SVG_PATH_SPACE;
			//	break;
			//case '-' :	// minus
			//	new_command = SVG_PATH_MINUS;
			//	break;
		default:
			break;
		}
		if ( new_command != SVG_PATH_NONE )
		{
			// process data
			if ( pt != pt_debut )
			{
				CHAR	buf[1024];
				size_t	len_path = pt - pt_debut - 1;
				strncpy( buf, ( pt_debut + 1 ), len_path );
				buf[len_path] = 0;
				points = svg_split_path( buf );
				pt_debut = pt;
				//for ( INT32 i = 0; i< points.size(); ++i )
				//{
				//	printf( "%f\t", points[i] );
				//}
			}

			bool	b_clear_points = true;
			// we have a new command, process the last command
			switch( last_command )
			{
			case SVG_PATH_CLOSE :
				break;
			case SVG_PATH_MOVE_TO_A :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ];
						last_point.y = coord.y = points[ i + 1 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_MOVE_TO_R :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ] + last_point.x;
						last_point.y = coord.y = points[ i + 1 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_LINE_TO_A :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ];
						last_point.y = coord.y = points[ i + 1 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_LINE_TO_R :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ] + last_point.x;
						last_point.y = coord.y = points[ i + 1 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_H_LINE_TO_A :
				if ( points.size() > 0 )
				{
					// need at least 1 points
					for ( size_t i = 0; i < points.size(); ++i )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ];
						coord.y = last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_H_LINE_TO_R :
				if ( points.size() > 0 )
				{
					// need at least 1 points
					for ( size_t i = 0; i < points.size(); ++i )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ] + last_point.x;
						coord.y = last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_V_LINE_TO_A :
				if ( points.size() > 0 )
				{
					// need at least 1 point
					for ( size_t i = 0; i < points.size(); ++i )
					{
						Vertex	coord;
						coord.x = last_point.x;
						last_point.y = coord.y = points[ i ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_V_LINE_TO_R :
				if ( points.size() > 0 )
				{
					// need at least 1 point
					for ( size_t i = 0; i < points.size(); ++i )
					{
						Vertex	coord;
						coord.x = last_point.x;
						last_point.y = coord.y = points[ i ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_CURVE_TO_A :
				if ( points.size() > 5 && !( points.size() % 6 ) )
				{
					// need at least 6 points
					for ( size_t i = 0; i < points.size(); i = i + 6 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 4 ];
						last_point.y = coord.y = points[ i + 5 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_CURVE_TO_R :
				if ( points.size() > 5 && !( points.size() % 6 ) )
				{
					// need at least 6 points
					for ( size_t i = 0; i < points.size(); i += 6 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 4 ] + last_point.x;
						last_point.y = coord.y = points[ i + 5 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_SMOOTH_CURVE_TO_A :
				if ( points.size() > 3 && !( points.size() % 4 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 4 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 2 ];
						last_point.y = coord.y = points[ i + 3 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_SMOOTH_CURVE_TO_R :
				if ( points.size() > 3 && !( points.size() % 4 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 4 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 2 ] + last_point.x;
						last_point.y = coord.y = points[ i + 3 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_QUADRATIC_A :
				if ( points.size() > 3 && !( points.size() % 4 ) )
				{
					// need at least 4 points
					for ( size_t i = 0; i < points.size(); i += 4 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 2 ];
						last_point.y = coord.y = points[ i + 3 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_QUADRATIC_R :
				if ( points.size() > 3 && !( points.size() % 4 ) )
				{
					// need at least 4 points
					for ( size_t i = 0; i < points.size(); i += 4 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i + 2 ] + last_point.x;
						last_point.y = coord.y = points[ i + 3 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_SMOOTH_QUADRATIC_A :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ];
						last_point.y = coord.y = points[ i + 1 ];
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_SMOOTH_QUADRATIC_R :
				if ( points.size() > 1 && !( points.size() % 2 ) )
				{
					// need at least 2 points
					for ( size_t i = 0; i < points.size(); i += 2 )
					{
						Vertex	coord;
						last_point.x = coord.x = points[ i ] + last_point.x;
						last_point.y = coord.y = points[ i + 1 ] + last_point.y;
						contour.push_back( coord );
					}
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_ARC_A :
				if ( points.size() > 8 && !( points.size() % 9 ) )
				{
					// need at least 9 points
					ERR_PRINT_STRING( "SVG Import : Elliptical Arc not supported yet" );
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			case SVG_PATH_ARC_R :
				if ( points.size() > 8 && !( points.size() % 9 ) )
				{
					// need at least 9 points
					ERR_PRINT_STRING( "SVG Import : Elliptical Arc not supported yet" );
				}
				else
				{
					ERR_PRINT_STRING( "Import SVG : wrong number of points" );
				}
				break;
			default:
				break;
			}
			last_command = new_command;
			new_command = SVG_PATH_NONE;
			if( b_clear_points )
			{
				// clears points, new command
				points.clear();
			}
		}
		++pt;
	}
	return contour;
}

void	c_bdd_tri::svg_process_recursive( TiXmlNode *node, INT32 depth )
{
	bool const b_layer = ( strcmp( node->Value(), "g" ) == 0 );
	
	_b_svg_layer_hidden = false;

	svg_process( node, depth ); // do code logic with the node

	if ( _b_svg_layer_hidden && _b_svg_ignore_hidden_layer )
		return;

	node = node->FirstChild();

	while( node )
	{
		if ( node->NoChildren() )
		{
			svg_process( node, depth ); // do code logic with the node
		}
		else
		{
			svg_process_recursive( node, depth + 1 ); // recurse
		}
		node = node->NextSibling();
	}
	if ( !_contour_cur.empty() )
	{
		//	printf( "Contour size %d\n", _contour_cur.size() );
		contour_obj	tmp;
		tmp._contours = _contour_cur;
		tmp._name = svg_layer_name();
		if( _b_svg_export_layer_name )
		{
			// save layers name in csv for database
			size_t	layer_name_size = _layer_name.size();
			for ( UINT32 i = 0; i < layer_name_size; ++i )
			{
				c_file::FWRITE( _layer_name[i].get(), _layer_name[i].get_len(), 1, _csv_layer_export );
				c_file::FWRITE( ";", 1, 1, _csv_layer_export );
			}
			c_file::FWRITE( "\n", 1, 1, _csv_layer_export );		
		}

		PRINT_STRING( "Layer Name : %s\n", tmp._name.get() );
		_svg_contour.push_back( tmp );
		_contour_cur.clear();
	}

	// FH08 : maybe we should check is node->value = "g", so we know it's a layer
	if ( b_layer )
	{
		if( _layer_name.size() > 0 )
			// hack, if group as no name, layer name will not be correct, base test on depth
			_layer_name.pop_back();
	}
}

void	c_bdd_tri::svg_process( TiXmlNode *node, INT32 depth )
{
	CONST char *value = node->Value();
	svg_process_node( node->Type(), value, depth );
	TiXmlElement *element = node->ToElement(); // is there an element?  Yes, traverse it's attribute key-pair values.
	if ( element )
	{
		TiXmlAttribute *atr = element->FirstAttribute();
		while ( atr )
		{
			CONST char *atr_name  = atr->Name();
			CONST char *atr_value = atr->Value();
			svg_process_attribute( node->Type(), value, depth, atr_name, atr_value );
			atr = atr->Next();
		}
	}
	if( strcmp( value, "text" ) == 0 )
		_b_svg_text = true;
}

void	c_bdd_tri::svg_process_node( INT32 ntype, CONST char* svalue, INT32 depth )
{
	CHAR	value[43];
	value[39] = '.';
	value[40] = '.';
	value[41] = '.';
	value[42] = 0;

	strncpy( value, svalue, 39 );

	switch ( ntype )
	{
	case TiXmlNode::TINYXML_ELEMENT:
		svg_display( depth, "Node(ELEMENT): %s\n", value );
		break;
	case TiXmlNode::TINYXML_DOCUMENT:
		svg_display( depth, "Node(DOCUMENT): %s\n", value );
		break;
	case TiXmlNode::TINYXML_TEXT:
		svg_display( depth, "Node(TEXT): %s\n", value );
		if ( _b_svg_text )
		{
			// text body for text
			VERBOSE_PRINT_STRING( "%s", value );
			svg_text	tmp;
			tmp._text = value;
			o_str	tmp_str = svg_layer_name();
			//	_svg_text[ _last_id ] = tmp;
		}
		break;
	case TiXmlNode::TINYXML_COMMENT:
		svg_display( depth, "Node(COMMENT): %s\n", value );
		break;
	case TiXmlNode::TINYXML_DECLARATION:
		svg_display( depth, "Node(DECLARATION): %s\n", value );
		break;
	case TiXmlNode::TINYXML_UNKNOWN:
		svg_display( depth, "Node(UNKNOWN): %s\n", value );
		break;
	default:
		svg_display( depth, "Node(?????): %s\n", value );
		break;
	}
}

void	c_bdd_tri::svg_process_attribute(	CONST INT32  node_type,			// enumerated type of the node
											CONST char*	node_value,			// The node value / key
											CONST INT32	depth,				// how deeply nested we are in the XML hierachy
											CONST char*	attribute_name,		// the name of the attribute
											CONST char*	attribute_value )	// the value of the attribute
{
	if ( strcmp( node_value, "g" ) == 0 && strcmp( attribute_name, "id" ) == 0 )
	{
		// we got an id
		o_str	tmp;
		tmp.set( attribute_value );
		_layer_name.push_back( tmp );
		_last_id.set( attribute_value );
	}
	else if( strcmp( node_value, "polygon" ) == 0 && strcmp( attribute_name, "points" ) == 0 )
	{
		// parse polygon points
		Contour	contour_current;
		contour_current = svg_process_polygon( attribute_value );
		// add contour to current obj
		if ( !contour_current.empty() )
			_contour_cur.push_back( contour_current );
	}
	else if( strcmp( node_value, "polyline" ) == 0 && strcmp( attribute_name, "points" ) == 0 )
	{
		// parse polyline points, // we don't close the contour but that's what the tesselation need
		Contour	contour_current;
		contour_current = svg_process_polygon( attribute_value );
		// add contour to current obj
		if ( !contour_current.empty() )
			_contour_cur.push_back( contour_current );
	}
	else if( strcmp( node_value, "path" ) == 0 && strcmp( attribute_name, "d" ) == 0 )
	{
		// parse path points
		Contour	contour_current;
		contour_current = svg_process_path( attribute_value );
		// add contour to current obj
		if ( !contour_current.empty() )
			_contour_cur.push_back( contour_current );
	}
	else if( strcmp( node_value, "text" ) == 0 )
	{
		// text
		o_str tmp = svg_layer_name();
		GOOD_PRINT_STRING( "layer name : %s", tmp.get() );
	}
	else if( strcmp( node_value, "g" ) == 0 && strcmp( attribute_name, "display" ) == 0 )
	{
		if( strcmp( attribute_value, "none" ) == 0 )
			_b_svg_layer_hidden = true;
	}
	svg_display( depth,"  ### Attribute(%s,%s)\n", attribute_name, attribute_value );

}

void	c_bdd_tri::svg_display( INT32 depth,CONST char* fmt, ... )
{
	//for ( INT32 i = 0; i < depth; ++i )
	//{
	//	printf("  ");
	//}
	//char wbuff[8192];
	//vsprintf( wbuff, fmt, (char *)(&fmt+1) );
	//printf( "%s", wbuff );
}



void	c_bdd_tri::svg_parse_linear( TiXmlNode *begin )
{
	TiXmlNode	*node = begin;
	INT32		depth = 0;

	while ( node )
	{
		svg_process( node, depth );
		if ( node->NoChildren() )
		{
			while( !node->NextSibling() && node != begin )
			{
				--depth;
				node = node->Parent();
			}
			if ( node == begin )
			{
				break;
			}
			node = node->NextSibling();
		}
		else
		{
			++depth;
			node = node->FirstChild();
		}
	}
}

// read data from SVG file
AAA_ERR	c_bdd_tri::read_points_from_svg( C_PCHAR_C filename )
{
	TiXmlDocument	svg_file;

	// try to load file
	if( !svg_file.LoadFile( filename ) )
	{
		// check if error
		if( svg_file.Error() )
		{
			ERR_PRINT_STRING( "3D : Error reading SVG file %s", svg_file.ErrorDesc() );
		}
		return ERR_ANY;
	}
	else
	{
		// svg file loaded
		// now parse data
		//svg_parse_linear(&svg_file);
		_layer_name.clear();
		_contour_cur.clear();
		_svg_contour.clear();
		//		_b_svg_polygon = false;
		//		_b_svg_path = false;
		_b_svg_text = false;
		CHAR	filename_dst[AAA_PATH_MAX()];
		fname::cpy_and_replace_ext( filename_dst, filename, "csv" );
		_csv_layer_export = c_file::FOPEN( filename_dst, "wt" );
		svg_process_recursive( &svg_file, 0 );
		c_file::FCLOSE( _csv_layer_export );
		//		printf("toto");
	}
	return AAA_OK;
}

void	__stdcall	c_bdd_tri::tess_begin_data_cb( GLenum type, c_bdd_tri* caller )
{
	//	DBG_PRINT_STRING( "tess_begin_data_cb" );
	switch( type )
	{
	case GL_TRIANGLES :
		//			DBG_PRINT_STRING( "GL_TRIANGLES" );
		break;
	case GL_TRIANGLE_STRIP :
		//			DBG_PRINT_STRING( "GL_TRIANGLE_STRIP" );
		break;
	case GL_TRIANGLE_FAN :
		//			DBG_PRINT_STRING( "GL_TRIANGLE_FAN" );
		break;
		//case GL_QUADS :
		//	DBG_PRINT_STRING( "GL_QUADS" );
		//	break;
		//case GL_QUAD_STRIP :
		//	DBG_PRINT_STRING( "GL_QUAD_STRIP" );
		//	break;
		//case GL_POLYGON :
		//	DBG_PRINT_STRING( "GL_POLYGON" );
		//	break;
	default:
		DBG_PRINT_STRING( "other %d", type );
		break;
	}
	caller->_tess_triangle_type = type;
	caller->_tess_tri_index = -1;
	caller->_tess_tri_size = 0;
}

void	__stdcall	c_bdd_tri::tess_edge_flag_data_cb( GLboolean flag, c_bdd_tri* caller )
{
	//	DBG_PRINT_STRING( "tess_edge_flag_data_cb" );
}

void	__stdcall	c_bdd_tri::tess_vertex_data_cb( unsigned int vertex_index, c_bdd_tri* caller )
{
	//	DBG_PRINT_STRING( "tess_vertex_data_cb" );

	UINT32	index_obj = caller->_svg_callback_info._index_obj;

	switch( caller->_tess_triangle_type )
	{
	case GL_TRIANGLES :
		caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
		break;
	case GL_TRIANGLE_STRIP :
		if ( caller->_tess_tri_size > 2 )
		{
			if( ( caller->_tess_tri_size % 2 ) == 0 )
			{
				caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_svg_contour[ index_obj ]._triangles.size() - 3 ] );
				caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_svg_contour[ index_obj ]._triangles.size() - 2 ] );
				caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
			}
			else
			{
				caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_svg_contour[ index_obj ]._triangles.size() - 1 ] );
				caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_svg_contour[ index_obj ]._triangles.size() - 3 ] );
				caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
			}
			caller->_tess_tri_size += 3;
		}
		else
		{
			caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
			caller->_tess_tri_size++;
		}
		break;
	case GL_TRIANGLE_FAN :
		if ( caller->_tess_tri_size > 2 )
		{
			caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_tess_tri_index ] );
			caller->_svg_contour[ index_obj ]._triangles.push_back( caller->_svg_contour[ index_obj ]._triangles[ caller->_svg_contour[ index_obj ]._triangles.size() - 2 ] );
			caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
			caller->_tess_tri_size += 3;
		}
		else
		{
			caller->_svg_contour[ index_obj ]._triangles.push_back( vertex_index );
			if( caller->_tess_tri_size == 0 )
			{
				// need new reference point
				caller->_tess_tri_index = INT32(caller->_svg_contour[ index_obj ]._triangles.size()) - 1;
			}
			caller->_tess_tri_size++;
		}
		break;
	}
}


//void __stdcall c_bdd_tri::tess_vertex_data_cb( unsigned int vertex_index, c_bdd_tri* caller )
//{
//	//	DBG_PRINT_STRING( "tess_vertex_data_cb" );
//	//	caller->Indices.push_back(vertexIndex);
//
//	switch( caller->_tess_triangle_type )
//	{
//	case GL_TRIANGLES :
//		caller->_tess_tri.push_back( vertex_index );
//		break;
//	case GL_TRIANGLE_STRIP :
//		if ( caller->_tess_tri_size > 2 )
//		{
//			if( ( caller->_tess_tri_size % 2 ) == 0 )
//			{
//				caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri.size() - 3 ] );
//				caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri.size() - 2 ] );
//				caller->_tess_tri.push_back( vertex_index );
//				//					caller->tess_tri.push_back( caller->tess_tri[ caller->tess_tri.size() - 3 ] );
//				//					caller->tess_tri.push_back( caller->tess_tri[ caller->tess_tri.size() - 2 ] );
//				//					caller->tess_tri.push_back( vertexIndex );
//			}
//			else
//			{
//				caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri.size() - 1 ] );
//				caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri.size() - 3 ] );
//				caller->_tess_tri.push_back( vertex_index );
//				//					caller->tess_tri.push_back( caller->tess_tri[ caller->tess_tri.size() - 2 ] );
//				//					caller->tess_tri.push_back( caller->tess_tri[ caller->tess_tri.size() - 2 ] );
//				//					caller->tess_tri.push_back( vertexIndex );
//			}
//			caller->_tess_tri_size += 3;
//		}
//		else
//		{
//			caller->_tess_tri.push_back( vertex_index );
//			caller->_tess_tri_size++;
//		}
//		break;
//	case GL_TRIANGLE_FAN :
//		if ( caller->_tess_tri_size > 2 )
//		{
//			caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri_index ] );
//			caller->_tess_tri.push_back( caller->_tess_tri[ caller->_tess_tri.size() - 2 ] );
//			caller->_tess_tri.push_back( vertex_index );
//			caller->_tess_tri_size += 3;
//		}
//		else
//		{
//			caller->_tess_tri.push_back( vertex_index );
//			if( caller->_tess_tri_size == 0 )
//			{
//				// need new reference point
//				caller->_tess_tri_index = caller->_tess_tri.size() - 1;
//			}
//			caller->_tess_tri_size++;
//		}
//		break;
//	}
//}
//
void	__stdcall	c_bdd_tri::tess_end_data_cb( c_bdd_tri* caller )
{
	//	DBG_PRINT_STRING( "tess_end_data_cb" );
}

void	__stdcall	c_bdd_tri::tess_combine_data_cb( GLdouble coords[3], unsigned int vertex_data[4], GLfloat weight[4], unsigned int* out_data, c_bdd_tri* caller )
{
	//	DBG_PRINT_STRING( "tess_combine_data_cb %f %f %f", coords[0], coords[1], coords[2]  );
	// add new vertex

	Vertex	vec;
	vec.x = coords[0];
	vec.y = coords[1];
	vec.z = coords[2];

	UINT32	index_obj = caller->_svg_callback_info._index_obj;
	caller->_svg_contour[ index_obj ]._vertices.push_back( vec ); 
	*out_data = INT32(caller->_svg_contour[ index_obj ]._vertices.size()) - 1;
}

//void __stdcall c_bdd_tri::tess_combine_data_cb( GLdouble coords[3], unsigned int vertex_data[4], GLfloat weight[4], unsigned int* out_data, c_bdd_tri* caller )
//{
//	//	DBG_PRINT_STRING( "tess_combine_data_cb %f %f %f", coords[0], coords[1], coords[2]  );
//	// add new vertex
//
//	Vertex	vec;
//	vec.x = coords[0];
//	vec.y = coords[1];
//	vec.z = coords[2];
//
//	//caller->add_vertex_current_obj( )
//	//REAL*	pf;
//	//pf = _vertex_data._point + _point_index_offset * 3 + hack_nb_point_read * 3;
//	//pf[0] = vertex_data[0];
//	//pf[1] = vertex_data[1];
//	//pf[2] = vertex_data[2];
//
//}


void	__stdcall	c_bdd_tri::tess_error_data_cb( GLenum error_nb, c_bdd_tri* caller )
{
	ERR_PRINT_STRING( "Tessellation Error : %s", gluErrorString( error_nb ) );
}


// === SCOMPA SC10 === 

// Use Jonathan Richard Shewchuk Triangle: a 2D Quality Mesh Generator and Delaunay Triangulator
// http://www.cs.cmu.edu/~quake/triangle.html
// http://www.cs.cmu.edu/~quake/showme.html

// =========================


//#define REAL double	// Must be the same in triangle.c

AAA_ERR	c_bdd_tri::svg_tessellate_mesh( size_t index )
{
	struct triangulateio in, out;

	MEMCLEAR( &in,  sizeof(triangulateio) );
	MEMCLEAR( &out, sizeof(triangulateio) );

	// --- calcule le nombre de points
	INT32 nbOfPoints = 0; 
	for( UINT32 i = 0; i < _svg_contour[ index ]._contours.size(); ++i )
		nbOfPoints += INT32(_svg_contour[ index ]._contours[ i ].size());

	/* Define input points. */
	in.numberofpoints = nbOfPoints;
	in.numberofpointattributes = 0;

	in.pointlist = (double *) MALLOC_ALIGNED( in.numberofpoints * 2 * sizeof(double), 0 );
	in.numberofsegments = nbOfPoints;
	in.segmentlist = (int *) MALLOC_ALIGNED( in.numberofsegments * sizeof(int) * 2, 0 );

	double xx,yy;

	// --- stoque les points ---
	INT32 k = 0; // compteur de points
	for( UINT32 i = 0; i < _svg_contour[ index ]._contours.size(); ++i )
	{
		INT32 firstNode = k; // premier point du pol
		// go through the points in the contour
		for( UINT32 j = 0; j < _svg_contour[ index ]._contours[ i ].size(); ++j )
		{
			xx = in.pointlist[ k * 2 ] = _svg_contour[ index ]._contours[ i ][ j ].x;
			yy = in.pointlist[ k * 2 + 1 ] = _svg_contour[ index ]._contours[ i ][ j ].y;

			in.segmentlist[ k * 2 ] = k;
			in.segmentlist[ k * 2 + 1] = k + 1;

			// --- tactique pour virer les doublons ( très lent ) ---
			// --- la tessellation plante si doublons
			bool b_doublon = false;
			for( INT32 zz = 0; zz < k; ++zz ) // recherche doublon depuis le 1er pol
			{
				if( (in.pointlist[zz * 2 ] == xx ) && ( in.pointlist[ zz * 2 + 1 ] == yy ) )
				{
					b_doublon = true;
					break;
				}
			}
			if( b_doublon )
			{
				in.numberofpoints--;
				in.numberofsegments--;
			}
			else k++; // point ok -> next one
		}
		if( k > 0 )
			in.segmentlist[ (k - 1) * 2 + 1 ] = firstNode; // loop last segment
	}

	// --- tableau des markers
	//in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
	//for (int i=0;i<in.numberofpoints; ++i ) in.pointmarkerlist[i]=0;

	//in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));
	//for (int i=0;i<in.numberofsegments; ++i ) in.segmentmarkerlist[i]=0;

	in.numberofholes	= 0;
	in.numberofregions	= 0;

	out.pointlist				= nullptr;
	out.pointattributelist		= nullptr;
	out.pointmarkerlist			= nullptr;
	out.trianglelist			= nullptr;
	out.triangleattributelist	= nullptr;
	out.neighborlist			= nullptr;
	out.segmentlist				= nullptr;
	out.segmentmarkerlist		= nullptr;
	out.edgelist				= nullptr;
	out.edgemarkerlist			= nullptr;

	//   Triangulate the points.  
	//   Switches are chosen to read and write a  PSLG (p) 
	//   preserve the convex hull (c)
	//   number everything from zero (z)
	//   produce an edge _list (e) 
	//   Max size of triangle (a) EX: a10 

	// "pczAevn"
	// "pzena10"
	// "pzena400"

	CHAR	strCommandLine[16];
	CHAR	tmpStr[16];
	sprintf( strCommandLine,"pze%s%s", _tessellate_tri_size ? "a" : "" , _tessellate_tri_size ? itoa( _tessellate_tri_size, tmpStr , 10 ) : "" );

	triangulate( strCommandLine, &in, &out, nullptr ); 

	// --- ecriture du resultat dans _svg_contour

	// --- les points
	Vertex	vec;
	for ( INT32 i = 0; i < out.numberofpoints; ++i ) 
	{
		vec.x = out.pointlist[ i * 2 ];
		vec.y = out.pointlist[ i * 2 + 1 ];
		vec.u = vec.v = 0;
		_svg_contour[ index ]._vertices.push_back( vec );
	}

	// --- les triangles
	for ( INT32 i = 0; i < out.numberoftriangles; ++i ) 
	{
		//printf("Triangle %4d points:", i);
		for (int j = 0; j < out.numberofcorners; ++j ) 
		{
			_svg_contour[ index ]._triangles.push_back( out.trianglelist[ i * out.numberofcorners + j ] ); // corner should be 3 
		}
	}

	// Free all allocated arrays, including those allocated by Triangle. 
	IF_FREE_ALIGNED_AND_NULL( in.pointlist );
	IF_FREE_AND_NULL( in.pointattributelist );
	IF_FREE_ALIGNED_AND_NULL( in.segmentlist );
	IF_FREE_AND_NULL( in.pointmarkerlist );
	IF_FREE_AND_NULL( in.segmentmarkerlist );
	IF_FREE_AND_NULL( out.pointlist );
	IF_FREE_AND_NULL( out.pointattributelist );
	IF_FREE_AND_NULL( out.triangleattributelist );


	////  if (in.pointmarkerlist)			free(in.pointmarkerlist);
	////  if (in.regionlist)				free(in.regionlist);

	//if ( out.trianglelist )				FREE( out.trianglelist );

	return AAA_OK;
}
