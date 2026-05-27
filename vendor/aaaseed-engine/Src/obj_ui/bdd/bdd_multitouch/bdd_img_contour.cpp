#include "bdd_img_contour.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "gol/gol_color.h"
#include "draw/model.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
#include "image/img_compo.h"
#include "spy.h"	 



c_bdd_img_contour*	c_bdd_img_contour::cur = nullptr;


static	INT32	contour_mode[4] =
{
	cv::RETR_EXTERNAL,	//	only the extreme outer contours
	cv::RETR_LIST,		//	all the contours and puts them in the list
	cv::RETR_CCOMP,		//	all the contours and organizes them into two-level hierarchy: top level are external boundaries of the components, second level are bounda boundaries of the holes
	cv::RETR_TREE		//	all the contours and reconstructs the full hierarchy of nested contours 
};

FACTORY_CREATE_PROP_V1( c_bdd_img_contour, bdd_img_contour, Image Contour, bdd_img_contour, sub_menu="Image"; );


namespace n_bdd_img_contour
{
	C_PCHAR_C	contour_mode_str[4] =
	{
		"External",
		"All",
		"All_then_two_level",
		"All_then_full_hierarchy"
	};

	CONSTEXPR INT32 BASE_PARAM_NB		= 11 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32 HOW_PARAM_NB		= 21;
	CONSTEXPR INT32 DRAW_PARAM_NB		= 13;
	CONSTEXPR INT32 FIDUCIAL_PARAM_NB	= 5;
	CONSTEXPR INT32 TRANSFER_PARAM_NB	= 8;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 4;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	HOW_PARAM_NB
								+	DRAW_PARAM_NB
								+	FIDUCIAL_PARAM_NB
								+	TRANSFER_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_NONE(		Bdd_ui_pref )

		PARAM_DEF_BOOL_OFF(	draw_in_texture )
		PARAM_DEF_IMG_DST(	contour_dst )
//		PARAM_DEF_BOOL_OFF(	contour_dst_mono )
//		PARAM_DEF_BOOCL_OFF(	contour_dst_alpha )
		PARAM_DEF_BOOL_ON(	contour_dst_clear )

		PARAM_DEF_GROUP_CLOSED(	How, HOW_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO(	band_nb )
			PARAM_DEF_REAL_POS_ZERO(	threshold )
			
			PARAM_DEF_SYMBO_PSTR_ONE(	contour_mode,			contour_mode_str	)
			PARAM_DEF_BOOL_OFF(			blob_point_nb_filter	)
			PARAM_DEF_INT32(			blob_point_nb_min,		2, 1,		1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(			blob_point_nb_max,		12, 10,		1, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_OFF(			blob_area_filter )
			PARAM_DEF_REAL_POS_ZERO(	blob_area_min )
			PARAM_DEF_REAL_POS_ONE(		blob_area_max )
			PARAM_DEF_BOOL_OFF(			blob_size_x_filter )
			PARAM_DEF_REAL_POS_ZERO(	blob_size_x_min )
			PARAM_DEF_REAL_POS_ONE(		blob_size_x_max )
			PARAM_DEF_BOOL_OFF(			blob_size_y_filter )
			PARAM_DEF_REAL_POS_ZERO(	blob_size_y_min )
			PARAM_DEF_REAL_POS_ONE(		blob_size_y_max )

			PARAM_DEF_BOOL_ON(	contour_wanted )
			PARAM_DEF_BOOL_OFF(	polygon_approximation )
			PARAM_DEF_REAL_POS(	polygon_approximation_factor,		1, .02	)
			PARAM_DEF_BOOL_OFF(	bounding_find_angle )
			PARAM_DEF_BOOL_OFF(	bounding_find_y_max )
			PARAM_DEF_REAL_POS(	bounding_find_y_max_distance,		1., .1	)

		PARAM_DEF_INT32_LOCKED(	contour_discarded_nb )
		PARAM_DEF_INT32_LOCKED(	contour_nb )
		PARAM_DEF_INT32_LOCKED(	contour_point_nb )
		
		PARAM_DEF_POINT_XYZ(	center)
		BDD_BASE_MODEL_PARAM

		PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		center_draw_discarded	)
			PARAM_DEF_BOOL_OFF(		center_draw				)
			PARAM_DEF_REAL_ONE(		center_draw_size		)
			PARAM_DEF_BOOL_OFF(		contour_draw_discarded	)
			PARAM_DEF_BOOL_OFF(		contour_draw			)
			PARAM_DEF_REAL_ONE(		contour_draw_alpha		)
			PARAM_DEF_BOOL_OFF(		contour_draw_as_quad	)
			PARAM_DEF_BOOL_OFF(		bounding_draw			)
			PARAM_DEF_REAL_ONE(		bounding_draw_alpha		)
			PARAM_DEF_SCALE_UVF(	bounding_draw_size		)
			PARAM_DEF_BOOL_OFF(		bounding_draw_ellipse	)

		PARAM_DEF_GROUP_CLOSED( Fiducial, FIDUCIAL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		fiducial_active )
			PARAM_DEF_REAL_POS(		fiducial_size_min,		1000, 200	)
			PARAM_DEF_INT32_LOCKED(	fiducial_discarded_nb )
			PARAM_DEF_INT32_LOCKED(	fiducial_kept_nb )
			PARAM_DEF_BOOL_OFF(		fiducial_draw )

		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB )
			PARAM_DEF_REF(			transfer_bdd_target )
			PARAM_DEF_POINT_XYZ(	transfer_center )
			PARAM_DEF_SCALE_XYZF(	transfer_size )
	};
}

void	c_bdd_img_contour::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_attach_obj( h, c_bdd_ui_pref::cur );

	param_set_pt( h, _b_draw_in_texture );
	param_set_pt( h, _contour_img_dst );
//	param_set_pt( h, _b_dst_mono );		//todo deal with it
//	param_set_pt( h, _b_dst_alpha );	//todo deal with it
	param_set_pt( h, _b_dst_clear );

	++h;
		param_set_pt( h, _band_nb );
		param_set_pt( h, _threshold );
		param_set_pt( h, _s_contour_mode );
		param_set_pt( h, _b_blob_point_nb_filter );
			param_set_pt( h, _blob_point_nb_min );
			param_set_pt( h, _blob_point_nb_max );
		param_set_pt( h, _b_blob_area_filter );
			param_set_pt( h, _blob_area_min );
			param_set_pt( h, _blob_area_max );
		param_set_pt( h, _b_blob_sx_filter );
			param_set_pt( h, _blob_sx_min );
			param_set_pt( h, _blob_sx_max );
		param_set_pt( h, _b_blob_sy_filter );
			param_set_pt( h, _blob_sy_min );
			param_set_pt( h, _blob_sy_max );

		param_set_pt( h, _b_contour_wanted );
		param_set_pt( h, _b_poly_approx_ui );
		param_set_pt( h, _poly_approx_factor_ui );
		param_set_pt( h, _b_bounding_find_angle );
		param_set_pt( h, _b_find_y_max );
		param_set_pt( h, _find_y_max_distance );

	param_set_pt( h, _contour_discarded_nb );
	param_set_pt( h, _contour_nb );
	param_set_pt( h, _contour_point_nb );

	param_set_pt_3( h, _center_ui );
	param_init_add_model( h );

	++h;
		param_set_pt(	h, _b_center_draw_discarded_ui	);
		param_set_pt(	h, _b_center_draw_ui			);
		param_set_pt(	h, _center_size_ui				);
		param_set_pt(	h, _b_contour_draw_discarded_ui );
		param_set_pt(	h, _b_contour_draw_ui			);
		param_set_pt(	h, _contour_draw_alpha_ui		);
		param_set_pt(	h, _b_contour_draw_as_quad_ui	);
		param_set_pt(	h, _b_bounding_draw_ui			);
		param_set_pt(	h, _bounding_draw_alpha_ui		);
		param_set_pt_3(	h, _bounding_draw_size_ui		);
		param_set_pt(	h, _b_bounding_draw_ellipse_ui  );

	++h;
		param_set_pt( h, _b_fiducial );
		param_set_pt( h, _fiducial_size_min );
		param_set_pt( h, _fiducial_discarded_nb );
		param_set_pt( h, _fiducial_kept_nb );
		param_set_pt( h, _b_fiducial_draw );

	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo,	_bdd_target );
		param_set_pt_3(			h, _transfer_translate	);
		param_set_pt_4(			h, _transfer_scale_ui	);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_contour )
	,_aaa_cv					( nullptr )
	//,_ipl_contour_draw			( nullptr )
	,_contour_nb				( 0 )
	,_contour_point_nb			( 0 )
	,_contour_discarded_nb		( 0 )
	,_fiducial_discarded_nb		( 0 )
	,_fiducial_kept_nb			( 0 )
	,_bdd_target				( nullptr )
{
	param_init_with( n_bdd_img_contour::param, n_bdd_img_contour::PARAM_NB );
	force_mono();
}

void	c_bdd_img_contour::release()
{
	IF_THIS_NULL_RETURN();
	//_ipl_contour_draw.release();
	_contour_dst.release();
	obj_delete( _aaa_cv );
}

c_bdd_img_contour::~c_bdd_img_contour()
{
	if( cur == this )
		cur = nullptr;

	release();
}

void	c_bdd_img_contour::update()
{
	//if( !update_img_src() )
	bool b_change_it;
	if( !update_mat_src(b_change_it) )
	{
		//	printf( "no update_img_src\n" );
		return;
	}

	if( is_size_changed() )
		release();
	if( is_content_changed() )
	{
		c_model::cur->get_size_v3( _size );
		add_scale_v2( _center, _center_ui, _size, -.5 );
		_center[2] = _center_ui[2];
		do_process();
	}
//		else
//			printf( "no content change \n" );
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
			_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo );
	}
	else
		_bdd_target = nullptr;
	if( _bdd_target )
		((c_bdd_blob*)_bdd_target)->register_as_src( this );
}

void	c_bdd_img_contour::do_process()
{
//	TBUF_ADD( tbuf::CH_OPEN_CV, 1., "bdd_img_contour", this );
	//c_img_ipl*		ipl_mono = get_in_mono_to_use();
	//if( ipl_mono )
	{
		//if( _band_nb )
		//{
		//	UINT8*	src	=	ipl_mono->get_data_pt_hack() - 1;
		//	INT32	i32	=	ipl_mono->get_size_x() * ipl_mono->get_size_y();
		//	while( --i32>=0 )
		//	{	
		//		INT32 v = (INT32(*++src) * _band_nb) >> 8;
		//		*src = (v&1) ? 255 : 0;
		//	}
		//}
		if( _b_draw_in_texture )
		{
			if ( _contour_dst.empty() )
				_contour_dst.create( _src_cur->size(), _src_cur->type() );

			find_blob_contour( _s_contour_mode, &_contour_dst );

			_src_cur = &_contour_dst;
			update_dst_index( _contour_img_dst, "Contour Destination" );
			/*if( build_dst_image( _ipl_contour_draw, _contour_img_dst, *ipl_mono, "Contour image", _b_dst_mono?1:(_b_dst_alpha?4:3) ) )
			//if( _ipl_contour_draw.build( _contour_img_dst, *ipl_mono, "Contour image", 3 ) )
			{
				find_blob_contour( ipl_mono, &_ipl_contour_draw, _s_contour_mode );
				_ipl_contour_draw.set_changed();
				validate_dst_change( _contour_img_dst );
			}*/
		}
		else
			find_blob_contour( _s_contour_mode, nullptr );
			//find_blob_contour( ipl_mono, nullptr, _s_contour_mode );
		cur = this;
	}
//	else
//		printf( "no mono\n" );
//	TBUF_ADD( tbuf::CH_OPEN_CV, 0., nullptr, this );
}

void	c_bdd_img_contour::transfer_blobs_to( BLOBS_CONT& blobs )
{
	REAL	sca[3];

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );

	//todoopt by avoiding copy

//was but it erase
//	blobs = _blob;
//	c_blob::scale_translate( blobs, sca, _transfer_translate );

	for( auto const & blob : _blob )
	{
		blobs.push_back( blob );
		blobs.back().scale_translate( sca, _transfer_translate );
	}
}

void	c_bdd_img_contour::draw()
{	
	//if( !is_active() )
	//	return;

	if( _contour_draw_alpha_ui != 0. )
	{
		bool b_as_quad = _b_contour_draw_as_quad_ui;
// 2023 March was there because we had a bug
//		if( b_as_quad )
//		{
//			b_as_quad = false;
//			DBG_PRINT_STRING( "Maa force contour_draw_as_quad to FALSE to avoid crash : to be solved" );
//		}

		if( _b_contour_draw_discarded_ui || _b_center_draw_discarded_ui )
		{
			c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_CONSTRUCTION );
			if( _b_contour_draw_discarded_ui )
				c_blob::draw_contour( _blob_discarded, b_as_quad, _center[2] );
			if( _b_center_draw_discarded_ui )
				c_blob::draw_center( _blob_discarded, _center_size_ui, _center[2] );
		}
		if( _b_contour_draw_ui || _b_center_draw_ui )
		{
			c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_OUTPUT, _contour_draw_alpha_ui );
			if( _b_contour_draw_ui )
				c_blob::draw_contour( _blob, b_as_quad, _center[2] );
			if( _b_center_draw_ui )
				c_blob::draw_center( _blob, _center_size_ui, _center[2] );
		}
	}
	if( _b_bounding_draw_ui && _bounding_draw_alpha_ui!=0. )
	{
		REAL size[2];
		scale_v2( size, _bounding_draw_size_ui, _bounding_draw_size_ui[2] );
		GOL::color4( 1,0,0, _bounding_draw_alpha_ui );
		c_blob::draw_bounding( _blob,
			size, 0.,
			_b_bounding_draw_ellipse_ui,
			_center[2]
			);
	}
	if( _b_fiducial_draw )
	{
		GOL::color_red();
		c_blob::draw_fiducial( _blob );
	}	
}

// stolen from opencv - squares.c sample
// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2 
REAL cos_angle( cv::Point* pt1, cv::Point* pt2, cv::Point* pt0 )
{
	double xa = pt1->x - pt0->x;
	double ya = pt1->y - pt0->y;
	double xb = pt2->x - pt0->x;
	double yb = pt2->y - pt0->y;

	return REAL( (xa * xb + ya * yb) / sqrt((xa * xa + ya * ya) * (xb * xb + yb * yb) + 1e-10) );
}

CONST	INT32	TAG_LEVEL_MAX = 6;
CONST	INT32	power_10[TAG_LEVEL_MAX] = { 1, 10, 100, 1000, 10000, 100000 };

// check out the number of children at each level some how to calculate a blob tag.
// there can be a max of 9 squares at each level..
INT32 getTag( CvSeq *curCont, INT32 level=0 )
{
	INT32 num = 0;
	INT32 sum = 0;

	for( ; curCont != 0; curCont = curCont->h_next )
	{
		++num;
		if( curCont->v_next && level < TAG_LEVEL_MAX )
			sum += getTag( curCont->v_next, level+1 );
		if( num > 9 )
			return 9;
	}
	sum += num * power_10[level];
	return sum;
}

FINLINE	void fill_blob_from_box( c_blob& blob, CvBox2D& box, REAL x_factor, REAL y_factor )
{
	blob._area = box.size.width * box.size.height;
//todo check this
//#ifdef AAA_OPENCV2
	blob._angle = - box.angle * REAL(1./360.) + REAL(.25);
//#else
//	blob._angle =   box.angle / 360.;
//#endif
	blob._center.set( ( box.center.x + REAL(.5) ) * x_factor, ( box.center.y + REAL(.5) ) * y_factor );

	//bug opencv invert ?
	//todo deal with asymetrie
	REAL	half_x = ( box.size.height + REAL(1) ) * x_factor * REAL(.5);
	REAL	half_y = ( box.size.width  + REAL(1) ) * y_factor * REAL(.5);

	blob._box.set_bottom_left(	blob._center.get_x() - half_x, blob._center.get_y() - half_y );
	blob._box.set_top_right(	blob._center.get_x() + half_x, blob._center.get_y() + half_y );
// FIXME: it might be nice if we could get the actual weight..
// It also might be nice to find the weighted center..

	blob._weight = 0;
}

FINLINE	void fill_blob_from_rect( c_blob& blob, cv::Rect& rect, REAL x_factor, REAL y_factor )
{
	REAL	x = rect.x * x_factor;
	REAL	y = rect.y * y_factor;
	REAL	half_x = rect.width  * x_factor * REAL(.5);
	REAL	half_y = rect.height * y_factor * REAL(.5);

	x += half_x;
	y += half_y;

	blob._area = REAL(rect.width * rect.height);
	blob._angle = 0.;

	blob._center.set( x, y );

	blob._box.set_bottom_left( x - half_x, y - half_y );
	blob._box.set_top_right(   x + half_x, y + half_y );

	// FIXME: it might be nice if we could get the actual weight..
	// It also might be nice to find the weighted center..

	blob._weight = 0;
}

#define CV_RGBA( r, g, b, a )  cvScalar( (b), (g), (r), (a) )
bool c_bdd_img_contour::find_blob_contour( INT32 s_how, cv::UMat* dst ) //opencv version of this fn with modern cv::Mat instead of ipl_image
{
	std::vector< std::vector < cv::Point > >	contours;
	std::vector< std::vector < cv::Point > >	result;
	std::vector< cv::Vec4i >					hierarchy;

	c_blob			blob;

	_fiducial_kept_nb = 0;
	_fiducial_discarded_nb = 0;

	_blob.clear();
	_blob_discarded.clear();
	_contour_point_nb  = 0;

	// the polyline in blob are in a [0,1] space
	//	and the blob should be too but only at the end
	_x_factor = OVER_ONE_AS_REAL( _src_cur->size().width  );
	_y_factor = OVER_ONE_AS_REAL( _src_cur->size().height );

	if( _src_cur->type() != CV_8UC1 )
	{	
		//todo
		//_src_cur->convertTo( _distrans_convert_src, CV_8UC1,  1., 0. );
		//_src_cur = &_distrans_convert_src;
		ERR_PRINT_STRING( "%s() only support CV_8UC1 type (8bits)", __FUNCTION__ );
		_contour_discarded_nb = 0;
		_contour_nb = 0;
		return false;
	}
	else
	{
		SPY_PUSH_RANGE( "src cv::convertTo 8bits", spy::IMG_LOW );
			//try
			//{
				if( _threshold!=0 )
					cv::threshold( *_src_cur, *_src_cur, double(_threshold * aaa::img::REAL_NEARLY_256), double(1. * aaa::img::REAL_NEARLY_256), cv::THRESH_TOZERO );
				//perhaps move it to start while(next) end version	
				// todo : check is *_src_cur 8bit 1channel (CV_8UC1 only?)
				cv::findContours( *_src_cur, contours, hierarchy, /*sizeof(CvContour),*/ contour_mode[s_how], cv::CHAIN_APPROX_NONE );
			//}
			//catch( cv::Exception& e )
			//{
			//	ERR_PRINT_STRING( "Open CV Exception : %s", e.what() );
			//	ERR_PRINT_STRING( "\tIn %s()", __FUNCTION__ );
			//}
		SPY_POP_RANGE();
	}
		
	if( !_aaa_cv )
		_aaa_cv = new aaa::c_cv;

	double col_external_discarded[4];
	double col_hole_discarded[4];
	double col_external[4];
	double col_hole[4];
	
	if( dst )
	{
		INT32	col4[4];
		c_bdd_ui_pref::cur->get_color_255( col4, c_bdd_ui_pref::COLOR_BACK );
		if( _b_dst_clear )
			dst->setTo( cv::Scalar( col4[0], col4[1], col4[2], col4[3] ) );
		_aaa_cv->set_line_dst( dst );

		cv::Mat mat = _contour_dst.getMat(cv::ACCESS_READ);

		{
			c_bdd_ui_pref::cur->get_color_255( col4, c_bdd_ui_pref::COLOR_CONSTRUCTION );

			CvScalar color;
			color = CV_RGBA(	col4[0],	col4[1],	col4[2],	col4[3] );
			cvScalarToRawData( &color,		col_external_discarded,	mat.type(), 0 );

			color = CV_RGBA(	col4[0]*.5,	col4[1]*.5,	col4[2]*.5,	col4[3] );
			cvScalarToRawData( &color,		col_hole_discarded,		mat.type(), 0 );

			c_bdd_ui_pref::cur->get_color_255( col4, c_bdd_ui_pref::COLOR_OUTPUT );
			color = CV_RGBA(	col4[0],	col4[1],	col4[2],	col4[3] );
			cvScalarToRawData( &color,		col_external,			mat.type(), 0 );

			color = CV_RGBA(	col4[0]*.5, col4[1]*.5,	col4[2]*.5,	col4[3] );
			cvScalarToRawData( &color,		col_hole,				mat.type(), 0 );
		}
	}

	_aaa_cv->set_xy_factor( _x_factor, _y_factor );
//	printf( "find_blob_contour\n");

	REAL	poly_approx = _b_poly_approx_ui ? _poly_approx_factor_ui * REAL(.02) : REAL(0);
	REAL	x,y;
	bool	b_contour_wanted = _b_contour_wanted || _b_find_y_max;
	size_t	nb_contours = contours.size();
	result.resize( nb_contours );
	for( INT32 i = 0; i < nb_contours; ++i )
	{
		//todo calling draw_contours like this is far from optimum
		//		draw_contours should be split in init / do / release

		size_t count = contours[i].size();	// This is number of point in contour
		if( _b_blob_point_nb_filter && ( count < _blob_point_nb_min || _blob_point_nb_max < count ) )
		{
			if( _b_contour_draw_discarded_ui )
			{			
				if( poly_approx > .0 )
					cv::approxPolyDP( contours[i], result[i], poly_approx*cv::arcLength( contours[i], true ), true );
				else
					result[i] = contours[i];
				if( b_contour_wanted )
				{
					if( dst )
						_aaa_cv->process_contour( &blob._polyline, result[i], col_external, col_hole, 1, 8 );
					else
						_aaa_cv->extract_contour( &blob._polyline, result[i] );
						//_aaa_cv->draw_contours( &blob._polyline, &_contour_dst.getMat(cv::ACCESS_RW), &result, col_external_discarded, col_hole_discarded, -1, 1, 8 );	//CV_FILLED, 8 );
					//else
					//	_aaa_cv->extract_contours( &blob._polyline, result );
					if( _b_find_y_max )
						blob._polyline.find_max_y_average( x,y, _find_y_max_distance );
				}
				else
					blob._polyline.clear();
				blob._b_size_valid = false;

				blob.scale_translate( _size, _center );
				_blob_discarded.push_back( blob );
			}
			continue;
		}

		if( poly_approx > .0 )
			cv::approxPolyDP( contours[i], result[i], poly_approx*cv::arcLength( contours[i], true ), true );
		else
			result[i] = contours[i];
		if( b_contour_wanted )
		{
			if( dst )
				_aaa_cv->process_contour( &blob._polyline,  result[i], col_external, col_hole, 1, 8 );
			else
				_aaa_cv->extract_contour( &blob._polyline, result[i] );
				//_aaa_cv->draw_contours( &blob._polyline, _src_cur, result, col_external, col_hole, -1, 1, 8 );	//CV_FILLED, 8 );
			//else
			//	_aaa_cv->extract_contours( &blob._polyline, result );
			if( _b_find_y_max )
				blob._polyline.find_max_y_average( x, y, _find_y_max_distance );
		}
		else
			blob._polyline.clear();
		blob._b_size_valid = true;

		// First we check to see if this contour looks like a square.. if we search for fiducial
		bool	b_blob_filled = false;
		if( _b_fiducial )
		{
			if( poly_approx <= .0 )	//not already done
				cv::approxPolyDP( contours[i], result[i], _poly_approx_factor_ui * .02 * cv::arcLength( contours[i], true ), true );

			if( result[i].size() == 4
				&&  fabs( cv::contourArea( result, false ) > _fiducial_size_min
				&&  cv::isContourConvex( result ) ) )
			{
				// find minimum angle between joint
				// edges (maximum of cosine)
				cv::Point	pt0 = cv::Point( cv::Mat( result[i] ).row(0) );
				cv::Point	pt1 = cv::Point( cv::Mat( result[i] ).row(1) );
				cv::Point	pt2 = cv::Point( cv::Mat( result[i] ).row(2) );
				cv::Point	pt3 = cv::Point( cv::Mat( result[i] ).row(3) );

				REAL s;
				s = ABS( cos_angle( &pt2, &pt0, &pt1 ) );
				s = MAX( s, ABS( cos_angle( &pt3, &pt1, &pt2 ) ) );
				s = MAX( s, ABS( cos_angle( &pt0, &pt2, &pt3 ) ) );

				// if cosines of all angles are small
				// (all angles are ~90 degree) then this is a square..
				if( s < REAL(.5) )
				{
					b_blob_filled = true;
					if( _b_bounding_find_angle )
					{
						//CvRect			rect;
						//rect = cvBoundingRect( cont );
						//fill_blob_from_rect( blob, rect, _x_factor, _y_factor );
						//was cv::minAreaRect( contours[i] );
						CvBox2D box = cvBox2D( cv::minAreaRect(contours[i]) );
						fill_blob_from_box( blob, box, _x_factor, _y_factor );
					}
					else
					{						
						cv::Rect rect = cv::boundingRect(contours[i]);
						//CvRect			rect;
						//rect = { 
						//	tmp.height,
						//	tmp.width,
						//	tmp.x,
						//	tmp.y 
						//};
						fill_blob_from_rect( blob, rect, _x_factor, _y_factor );
					}

					// use v_next..
					//opencv
					//if(cont->v_next)
					//{
					//	blob._id_tag = getTag(cont->v_next);
					////				printf("Square Detected %d\n", blob.tagID);
					//}
					//else
					//	blob._id_tag = 0;
				}
			}
		}

		// fallback, if it's a regular blob.
		if( !b_blob_filled )
		{
			//	seq sequence that contains input point set.
			//	Only points with integer coordinates (CvPoint) are supported now.
			if( _b_bounding_find_angle )
			{
				CvBox2D box = cvBox2D(cv::minAreaRect( contours[i] ));
				fill_blob_from_box( blob, box, _x_factor, _y_factor );
			}
			else
			{
				cv::Rect rect = cv::boundingRect(contours[i]);
				//CvRect			rect;
				//rect = {
				//	tmp.height,
				//	tmp.width,
				//	tmp.x,
				//	tmp.y
				//};
				fill_blob_from_rect( blob, rect, _x_factor, _y_factor );
			}
			blob._id_tag = 0;

			/*	bug try in opencv post beta 5
			// Number point must be more than or equal to 6 to use cvFitEllipse_32f)
			//	but there is bug so if the first try (cvMinAreaRect2) is smaller we use it
			if( count > 6  )
			{
			//todo deal with the x and y factor and rotationb
			//todo the area is just an aproximation
			// Fits ellipse to current contour.
			box = cvFitEllipse2( cont );
			REAL	area;

			fill_blob_from_box( blob, box, _x_factor, _y_factor );
			blob._id_tag = 0;
			}
			*/
		}
	
		blob.scale_translate( _size, _center );
		if	(	( _b_blob_area_filter	&&	OUTSIDE( blob._area,				_blob_area_min,	_blob_area_max	) )
			||	( _b_blob_sx_filter		&&	OUTSIDE( blob._box.get_size_x(),	_blob_sx_min,	_blob_sx_max	) )
			||	( _b_blob_sy_filter		&&	OUTSIDE( blob._box.get_size_y(),	_blob_sy_min,	_blob_sy_max	) )
			)
		{
			//todo opt don't store the rejected when not needed
			if( _b_find_y_max )
				blob._center.set(  x * _size[0] + _center[0], y * _size[1] + _center[1] );
			if( blob._id_tag )
				++_fiducial_discarded_nb;
			_blob_discarded.push_back( blob );
		}
		else
		{
			if( _b_find_y_max )
				blob._center.set(  x * _size[0] + _center[0], y * _size[1] + _center[1] );
			if( blob._id_tag )
				++_fiducial_kept_nb;
			_contour_point_nb += blob._polyline.get_point_nb();
			_blob.push_back( blob );
		}
	}
	//if( b_contour_wanted && !_contour_dst.empty() )
		//cv::drawContours(_contour_dst, contours, -1, cv::Scalar( col4[0], col4[1], col4[2], col4[3] ), cv::FILLED, 8, hierarchy, 3 );
	//	_aaa_cv->draw_contours( &blob._polyline, &_contour_dst, result, col_external, col_hole, 2147483647, cv::FILLED, 8 );	//CV_FILLED, 8 );
	//else
	//	_aaa_cv->extract_contours( &blob._polyline, result );
	//todo create ST_PATAM for UINT32
	_contour_discarded_nb = (UINT32)_blob_discarded.size();
	_contour_nb = (INT32)_blob.size();
	return true;
}

#if 0
//	code to test to detect finger
#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

std::vector<cv::Point2i> detectFingertips(cv::Mat1f z, float zMin = 0.0f, float zMax = 0.75f, cv::Mat1f& debugFrame = cv::Mat1f())
{
	using namespace cv;
	using namespace std;
	bool debug = !debugFrame.empty();

	vector<Point2i> fingerTips;

	Mat handMask = z < zMax & z > zMin;

	std::vector<std::vector<cv::Point> > contours;
	findContours(handMask.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); // we are cloning here since method will destruct the image

	if (contours.size()) {
		for (int i=0; i<contours.size(); i++) {
			vector<Point> contour = contours[i];
			Mat contourMat = Mat(contour);
			double area = cv::contourArea(contourMat);

			if (area > 3000)  { // possible hand
				Scalar center = mean(contourMat);
				Point centerPoint = Point(center.val[0], center.val[1]);

				vector<Point> approxCurve;
				cv::approxPolyDP(contourMat, approxCurve, 20, true);

				vector<int> hull;
				cv::convexHull(Mat(approxCurve), hull);

				// find upper and lower bounds of the hand and define cutoff threshold (don't consider lower vertices as fingers)
				int upper = 640, lower = 0;
				for (int j=0; j<hull.size(); j++) {
					int idx = hull[j]; // corner index
					if (approxCurve[idx].y < upper) upper = approxCurve[idx].y;
					if (approxCurve[idx].y > lower) lower = approxCurve[idx].y;
				}
				float cutoff = lower - (lower - upper) * 0.1f;

				// find interior angles of hull corners
				for (int j=0; j<hull.size(); j++) {
					int idx = hull[j]; // corner index
					int pdx = idx == 0 ? approxCurve.size() - 1 : idx - 1; //  predecessor of idx
					int sdx = idx == approxCurve.size() - 1 ? 0 : idx + 1; // successor of idx

					Point v1 = approxCurve[sdx] - approxCurve[idx];
					Point v2 = approxCurve[pdx] - approxCurve[idx];

					float angle = acos( (v1.x*v2.x + v1.y*v2.y) / (norm(v1) * norm(v2)) );

					// low interior angle + within upper 90% of region -> we got a finger
					if (angle < 1 && approxCurve[idx].y < cutoff) {
						int u = approxCurve[idx].x;
						int v = approxCurve[idx].y;

						fingerTips.push_back(Point2i(u,v));

						if (debug) {
							cv::circle(debugFrame, approxCurve[idx], 10, Scalar(1), -1);
						}
					}
				}

				if (debug) {
					// draw cutoff threshold
					cv::line(debugFrame, Point(center.val[0]-100, cutoff), Point(center.val[0]+100, cutoff), Scalar(1.0f));

					// draw approxCurve
					for (int j=0; j<approxCurve.size(); j++) {
						cv::circle(debugFrame, approxCurve[j], 10, Scalar(1.0f));
						if (j != 0) {
							cv::line(debugFrame, approxCurve[j], approxCurve[j-1], Scalar(1.0f));
						} else {
							cv::line(debugFrame, approxCurve[0], approxCurve[approxCurve.size()-1], Scalar(1.0f));
						}
					}

					// draw approxCurve hull
					for (int j=0; j<hull.size(); j++) {
						cv::circle(debugFrame, approxCurve[hull[j]], 10, Scalar(1.0f), 3);
						if(j == 0) {
							cv::line(debugFrame, approxCurve[hull[j]], approxCurve[hull[hull.size()-1]], Scalar(1.0f));
						} else {
							cv::line(debugFrame, approxCurve[hull[j]], approxCurve[hull[j-1]], Scalar(1.0f));
						}
					}
				}
			}
		}
	}

	return fingerTips;
}

#endif

INT32	c_bdd_img_contour::get_point_nb()
{
	return _contour_point_nb;
}
bool	c_bdd_img_contour::get_points_3d( REAL* dst, INT32 nb )
{
	if( nb <= 0 )
		return true;	//todo true ?
	INT32 blob_index = -1;
	while( nb > 0 )
	{
		c_polyline_2d&	pl = _blob[++blob_index]._polyline;
		INT32 nb_here = pl.get_point_nb();
		nb_here = MIN( nb_here, nb );
		for( INT32 i=0; i<nb_here; ++i )
		{
			const st_point_2d& st = pl.get_point(i);
			set_v3( dst, st.x, st.y, REAL(0) );
			dst += 3;
		}
		nb -= nb_here;
	}
	return true;
}