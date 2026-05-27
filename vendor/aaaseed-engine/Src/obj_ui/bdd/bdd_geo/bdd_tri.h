
#ifdef AAA_BDD_TRI_H
#error "BDD_TRI_H included more than once."
#endif
#define AAA_BDD_TRI_H 1


#ifndef AAA_OBJ3D_FILE_READER_H
#	include "obj3d_file_reader.h"
#endif
#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_AAA_CONST_H
#	include "infrastructure/aaa_const.h"
#endif
#ifndef	AAA_VERTEX_DATA_H
#	include "draw/vertex_data.h"
#endif
#ifndef AAA_BBOX_H
#	include "obj_ui/bdd/util/bbox.h"
#endif
#ifndef AAA_GOL_H
#	include "gol/gol.h"
#endif

class TiXmlNode;

//todo init member and struct member
// todoqqq this should be a class, generalize vertex, 2d vertex used in mulitouch
struct	Vertex
{
	double	x,	y,	z;	// coordinates
	double	nx, ny, nz;	// normal vector
	double	u,	v;		// texture coordinates

	Vertex()
	{
		x = y = z = 0.0;
		nx = ny = 0.0;
		nz = 1.0;
	}
//unused
	//Vertex( FP32 CONST x_in, FP32 CONST y_in )
	//{
	//	x = x_in;
	//	y = y_in;
	//	z = 0.0;
	//	nx = ny = 0.;
	//	nz = 1.;
	//}
};


typedef std::vector<Vertex>	Contour;

class	c_obj_info
{
public:
	INT32	_point_start;
	INT32	_point_nb;

	INT32	_tri_start;
	INT32	_tri_nb;

	INT32	_poly_start;
	INT32	_poly_nb;
	INT32	_poly_index_start;

	INT32	_mtl_index;
	INT32	_obj_index;
	INT32	_obj_info_id;
	
	REAL	_barycenter[3];
	REAL	_legend_tra[3];
	REAL	_legend_scale;

	std::vector<Contour>	_contours;
	c_bbox	_bbox;
	o_str	_name;
	c_obj_info();
};

class	c_obj_cluster
{
friend	class	c_bdd_tri;
private:
	std::vector<c_obj_info*>	_list;
	c_bbox						_bbox;
public:
	c_obj_cluster();
	~c_obj_cluster();
	FINLINE	void	add_obj_info_pt(c_obj_info* pt) { _list.push_back(pt); }
			void	compute_bbox();
			void	sort_by_mtl();
};

struct	svg_text
{
	o_str	_text;
	REAL	_pos[3];
};

class c_bdd_tri;



class	c_def_cartogram;

class	c_bdd_tri final : public c_bdd_multiple 
{
	FACTORY_DECLARE(c_bdd_tri,c_bdd_multiple);
	friend	c_def_cartogram;
	friend	c_obj3d_file_reader;
public:
	static	CONST	INT32	BDD_TRI_CHANNEL_NB_MAX	= CHANNEL_NB_MAX;
	static	CONST	INT32	MORPH_NB_MAX			= 4;

private:
	static	c_instance_by_channel< c_bdd_tri, BDD_TRI_CHANNEL_NB_MAX > inst_by_channel;

	REAL*	alloc_point_data_for_triangle(	REAL*& buf, INT32 nb_tri, INT32 dim, CHAR* str_verbose=nullptr );
	void	dealloc_buffer_real(	REAL*& buf );
public:
	static	c_bdd_tri*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}

private:

	static	bool	_b_verbose;

	c_vertex_data	_vertex_data_out[ 1 + MORPH_NB_MAX ];
	c_vertex_data	_vertex_data[ 1 + MORPH_NB_MAX ];	//	main + MORPH_NB_MAX
	UINT32			_unique_point_nb;

	bool	_b_vbo_use;
	bool	_b_vbo_stream_ui;
	bool	_b_vbo_transfered;
	GLuint	_vbo_vertex;
	GLuint	_vbo_normal;
	GLuint	_vbo_tex;
	GLuint	_vbo_index;
	GLuint	_vbo_attr[MORPH_NB_MAX*2];
	GLuint	_attrib_location[MORPH_NB_MAX*2];
	GLuint	_attrib_vbo[MORPH_NB_MAX*2];
	INT32	_attr_nb;
	bool	_b_vbo_use_ui;

	GLuint	_vao;
	bool	_vbo_in_vao;
	UINT32	_shading_unique_id;
	bool	_b_index_short;

	//	index read from file
	UINT32*	_tri_index_point;
	UINT32*	_tri_index_normal;
	UINT32*	_tri_index_uv;

	REAL*	_attrib_for_tri[MORPH_NB_MAX*2];

	REAL*	_p_point_to_drawing;
	REAL*	_p_normal_to_draw;
	REAL*	_p_uv_to_draw;
	GLvoid*	_p_tri_to_draw;

	INT32	_nb_poly;
#if AAA_POLY_COMPILE()
	UINT32*	_p_poly_index_to_draw;
	UINT32*	_poly_count;
	UINT32*	_poly_index_from_file;
	UINT32*	_poly_index_for_normal;
#endif

	bool	_b_morph_pre_process_ui;
//	REAL*	_point_interpolated;
//	INT32*	_tri_from_file_x3;

	REAL*	_normal_by_point;
//	REAL*	_normal_by_point_interpolated;
	GLvoid*	_tri_index;

//	REAL*	_normal_for_tri_interpolated;
// 
//	REAL*	p_normal_point_to_draw;	//for the normal in multiple

	INT32	_nb_point;
	INT32	_nb_normal;
	INT32	_nb_uv;
	INT32	_nb_tri;


	bool	_b_to_make_normal;
	bool	_b_valid_data;
	bool	_b_draw_by_obj_ui;
	bool	_b_draw_by_id_ui;
	bool	_b_draw_by_name_ui;
	bool	_b_random_color_by_id_ui;
	bool	_b_random_color_by_id;


	bool	_b_uv_in_file;
	bool	_b_draw_uv;
//	REAL	_uv_factor;
//	REAL	_uv_factor_last;
	INT32	_s_tex_change;
	INT32	_mtl_offset;

	bool	_b_normal_do;
	bool	_b_normal_in_file;
	bool	_b_normal_from_file_ui;	//unused but there for compatibilty for now (Maa Sept 2019)
	bool	_b_normal_force_ui;		//todo	don't load normal if we force it

#if AAA_POLY_COMPILE()
	bool	_b_poly_in_file;
	bool	_b_poly_draw;
#endif
	bool	_b_poly_draw_ui;

	bool	_b_need_build;

	REAL	_scale_factor_ui[4];
	REAL	_scale_factor[4];
	bool	_b_scale_to_unit_trig_ui;
	bool	_b_scale_by_object_ui;
	REAL	_obj_scale;

	REAL	_tra_ui[3];
	REAL	_tra[3];
	bool	_b_center_trig_ui;
	bool	_b_flip_x_at_read_ui;
	bool	_b_flip_yz_at_read_ui;
	bool	_b_flip_x_at_read;
	bool	_b_flip_yz_at_read;
	bool	_b_unit_center_at_read_ui;

	INT32	_skip;

	o_str	_model_filename;
	bool	_b_model_load_needed;
	bool	_b_model_load_trig_ui;

//	INT32	index_point_for_ui;

	c_obj_info*					_obj_info;
	std::vector<c_obj_info*>	_obj_by_index;
	std::vector<c_obj_info*>	_obj_by_id;

	o_str	_name_selector;
	INT32	_obj_nb;
	INT32	_obj_start;
	INT32	_obj_stop;
	INT32	_obj_offset_id;
	REAL	_bbox_origin[3];
	REAL	_bbox_min[3];
	REAL	_bbox_max[3];

//CLUSTER
	c_obj_cluster*				_cluster;
	std::vector<c_obj_cluster*>	_cluster_list;
	INT32						_cluster_nb_ui[3];
	INT32						_cluster_nb[3];
	INT32						_cluster_nb_total;
	INT32						_cluster_nb_out;
	INT32						_cluster_debug;

	INT32	_prim_drawn;
	
//	bool	_b_interpolation_ui;
//	REAL	_interpolation_value_ui;
	bool	_b_clipping_ui;
	INT32	_s_clipping_type;

	bool	_b_skip_clipping;

	bool	_b_tex_object_rot_ui;
	bool	_b_tex_object_stick_bbox_ui;

	FP32 CONST *	_cam_pos_pt;
	REAL			_clip_dist;
	REAL			_clip_dist_alpha;

	INT32	_channel_id;
	INT32	_legend_id;

	bool	_b_transform_point_ui;			// transform point at every update ?
	bool	_b_point_need_transform;
	//	bool	_b_point_for_normal_to_place;

	bool	_b_draw_contour;
	REAL	_contour_line_size;
	FP32	_color_contour[4];

	REAL	_weight;		// for cartogram
private:
//SVG

	bool	_b_svg_ignore_hidden_layer;
//	bool	_b_svg_recursive;
	bool	_b_verbose_svg;

	bool	_b_tessellate_mesh;  // Off=>gluTessellation On=> TriangleTessellation
	INT32	_tessellate_tri_size; // Size of triangle if (Triangle Tessellation) 0=> No max size

	// SVG import vars
	//bool	_b_svg_polygon;
	//bool	_b_svg_path;
	bool	_b_svg_text;

	o_str	_last_id;
//	std::map< o_str, svg_text>	_svg_text;


	struct	contour_obj
	{
		std::vector<Contour>		_contours;
		std::vector<UINT32>			_triangles;
		std::vector<Vertex>			_vertices;
		o_str						_name;
	};
	struct	svg_callback
	{
		UINT32	_index_obj;
		UINT32	_index_contour;
		//		UINT32	_index_point;
	};
	svg_callback				_svg_callback_info;
	std::vector<Contour>		_contour_cur;
	std::vector<contour_obj>	_svg_contour;

	bool	_b_svg_layer_hidden;
	bool	_b_svg_export_layer_name;
	FILE*	_csv_layer_export;
	o_str	_contour_name;

	std::vector<o_str>	_layer_name;

	INT32	_tess_triangle_type;
	INT32	_tess_tri_size;		// used to know the size of current object in vertex_data_callback
	INT32	_tess_tri_index;		// used to know the index of current point reference for strips and fans in vertex_data_callback

private:
	static	c_bdd_tri*	cur;
	INT32	get_obj_nb()		{	return _obj_nb;									}	//SC08 
	UINT32*	get_tri_tab()		{	return _tri_index_point;						}	//SC08
public:
			void			init();
			void			init_pt_data();
			void			alloc_obj(		INT32 nb );
			void			alloc_point(	INT32 nb );
			void			alloc_normal(	INT32 nb );
			void			alloc_uv(		INT32 nb );
			void			alloc_tri(		INT32 nb, INT32 nb_normal, INT32 nb_uv );
#if AAA_POLY_COMPILE()
			void			alloc_poly( INT32 nb_tri_in, INT32 nb_poly_in );
#endif
			void			vbo_vao_init();
			void			vbo_vao_deinit();
			void			vbo_define_buffer();
			void			vbo_set_buffer_for_obj(	c_obj_info* info );
			void			vbo_draw(				c_obj_info* info );

			void			center();
			void			center(					REAL* min, REAL* max );
			void			scale_to_unit();
			void			init_transfo();
			void			transform_point(		c_vertex_data* vd, c_obj_info* info );
			void			transform_point(		REAL* dst, REAL* src, INT32 nb );
			void			transform();
			void			transform_contour();

			bool			build_exp();
			void			build();
			void			build_index_tri(		INT32 nb );
			void			build_point_for_tri(	REAL* dst, REAL* src );
			void			build_normal_for_tri(	REAL* dst, REAL* src  );
			void			build_uv_for_tri(		REAL* dst, REAL* src );
#if AAA_POLY_COMPILE()
			void			build_point_for_poly();
#endif

private:
			template< class T >
			void			draw_old_school(		c_obj_info* info );
protected:
			void			obj_draw_face(			c_obj_info* info );
	FINLINE	void			obj_draw_bbox(			c_obj_info* info );
	FINLINE	bool			is_selected_to_draw(	c_obj_info* info );
			void			obj_do_sorts();

typedef void (c_bdd_tri::*P_FN)(  c_obj_info* info ); 
	FINLINE	void			draw_low( P_FN fn, bool b_skip_clip );
			void			load_obj_info();
			void			save_obj_info();

public:	
//	FINLINE	void			alloc()	{	alloc( _nb_point, _nb_normal, _nb_tri);	}
			bool			geo_alloc(	INT32 nb_obj_in,
										INT32 nb_point_in, INT32 nb_normal_in, INT32 nb_uv_in,
										INT32 nb_tri_in, INT32 nb_poly_in ); 
			void			geo_dealloc();
	virtual	void			param_init_pt();

	virtual	void			update();
	virtual	void			draw_before_low();

	virtual	void			draw();
	virtual	void			draw_bbox();

	virtual	void			draw_normal_point( REAL len );
	virtual bool			can_implicit() final override { return _b_draw_uv; }

//	void	get_next_line_starting_with( CHAR * start);
//	void	get_next_line_starting_with_vt_or_vn();

			void			compute_bbox();
			void			compute_bbox( c_obj_info* info );
			void			compute_barycenter_and_co( c_obj_info* info );
//	void	compute_bsphere( c_obj_info* info );
//	void	measure_bbox(REAL* min, REAL* max);

			AAA_ERR			skip_tri_from_stream_obj(		c_obj3d_file_reader* reader );
			AAA_ERR			read_tri_from_stream_obj(		c_obj3d_file_reader* reader );
	virtual	AAA_ERR			load_data();
			AAA_ERR			load_data_obj_alias(			c_obj3d_file_reader* reader, INT32 obj_nb_to_read, c_obj_info* info );
			void			do_load_data_success(			c_obj3d_file_reader* reader );
			AAA_ERR			load_data(						o_str CONST & filename );
	virtual	AAA_ERR			load_data_from_filename(		o_str CONST & filename, INT32 type_io );

	FINLINE	INT32			get_nb_poly()	{	return _nb_poly; };
	FINLINE	INT32			get_nb_tri()	{	return _nb_tri; };
	

	//virtual	INT32		get_point_dataset_nb();
	//virtual	bool		set_point_dataset( INT32 dataset_id );

	virtual	INT32			get_point_nb()	{	return _nb_point; };

	//virtual	void			 set_point_nb( INT32 nb );

	virtual	REAL*			get_points()	{	return _vertex_data[0].get_point_original();	}	//todo check it work with deformer
	
	//virtual void			erase_points();
	virtual	REAL*			get_point_pt( INT32 CONST index ) final override;

	//virtual	INT32			get_point_and_id( REAL* dst, INT32 CONST index );
	//virtual	bool			get_points_3d( REAL* dst, INT32 nb );

	//virtual	REAL*			get_normals();
	//virtual	bool			set_point( INT32 index, REAL CONST * src );

	//virtual	INT32			get_segment_nb();
	//virtual	REAL*			get_segments();
	//virtual	void			get_segment( REAL* a, REAL* b, INT32 index );


	virtual	AAA_ERR			load_do_after( o_str CONST & filename );
	virtual	AAA_ERR			save_do_after( o_str CONST & filename );

	virtual	void			save_obj_file( FILE* file );

			void			make_normal();
			void			prepare_uv_for_tri();

	FINLINE	c_obj_info*	get_info(									INT32 control_id );
			C_PCHAR_C		get_name_from_control(						INT32 control_id );
			template<typename T>
			void			get_origin_from_control(		T* pt,		INT32 control_id )	{	get_info( control_id )->_bbox.get_origin( pt );		}

//hack the calculation have to be cleaned up
			template<typename T>
			void			get_barycenter_from_control(	T* pt,		INT32 control_id )	{	cpy_v3( pt, get_info( control_id )->_barycenter );	}
			template<typename T>
			void			get_min_from_control(			T* pt,		INT32 control_id )	{	get_info( control_id )->_bbox.get_min( pt );		}
			template<typename T>
			void			get_max_from_control(			T* pt,		INT32 control_id )	{	get_info( control_id )->_bbox.get_max( pt );		}
			template<typename T>
			void			get_size_from_control(			T* pt,		INT32 control_id )	{	get_info( control_id )->_bbox.get_size( pt );		}
			void			get_tex_center_from_control(	DOUBLE* pt,	INT32 control_id );
			void			get_tex_size_from_control(		DOUBLE* pt,	INT32 control_id );

//CLUSTER
			void	cluster_init();
			void	cluster_alloc();
			void	cluster_dealloc();
			void	cluster_update();
			void	cluster_build();

			REAL	get_weight()	{ return _weight; }
//	SVG
private:
			o_str	svg_layer_name();
			std::vector<Vertex>	svg_process_polygon( C_PCHAR attribute_value );
			std::vector<Vertex>	svg_process_path( C_PCHAR attribute_value );
			std::vector<DOUBLE>	svg_split_path( C_PCHAR values );
			void	svg_traverse( TiXmlNode* node,INT32 depth );
			void	svg_process( TiXmlNode* node, INT32 depth );
			void	svg_display( INT32 depth, CONST char* fmt, ... );
			void	svg_process_node( INT32 ntype, CONST char* svalue, INT32 depth );
			void	svg_process_recursive( TiXmlNode* node, INT32 depth );
			void	svg_parse_linear( TiXmlNode* begin );
			void	svg_process_attribute(	CONST INT32	ntype,		// enumerated type of the node
											CONST char*	nvalue,		// The node value / key
											CONST INT32	depth,		// how deeply nested we are in the XML hierachy
											CONST char*	aname,		// the name of the attribute
											CONST char*	savalue);	// the value of the attribute

			AAA_ERR	svg_tesselate( INT32 index );
			AAA_ERR	svg_tessellate_mesh( size_t index );
			void	svg_convert( c_obj3d_file_reader* reader );

			AAA_ERR	read_points_from_svg( C_PCHAR_C filename );
			AAA_ERR	tesselate_contour( INT32 offset );
			AAA_ERR	tesselate_convert();

		// tesselation callbacks
			//  GLU_TESS_BEGIN_DATA callback
			static void __stdcall tess_begin_data_cb( GLenum type, c_bdd_tri* caller );
			//  GLU_TESS_EDGE_FLAG_DATA callback
			static void __stdcall tess_edge_flag_data_cb( GLboolean flag, c_bdd_tri* caller );
			//  GLU_TESS_VERTEX_DATA callback
			static void __stdcall tess_vertex_data_cb( unsigned int vertexIndex, c_bdd_tri* caller );
			//  GLU_TESS_END_DATA callback
			static void __stdcall tess_end_data_cb( c_bdd_tri* caller );
			//  GLU_TESS_COMBINE_DATA callback
			static void __stdcall tess_combine_data_cb( GLdouble coords[3], unsigned int vertexData[4], GLfloat weight[4], unsigned int* outData, c_bdd_tri* caller );
			//  GLU_TESS_ERROR_DATA callback
			static void __stdcall tess_error_data_cb( GLenum error_nb, c_bdd_tri* caller );

};
