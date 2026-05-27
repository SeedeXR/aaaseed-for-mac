/*
MapManager library for the conversion, manipulation and analysis 
of maps used in Mobile Robotics research.
Copyright (C) 2005 Shane O'Sullivan

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

email: shaneosullivan1@gmail.com
*/

#ifdef AAA_VORONOI_DIAGRAM_GENERATOR_H
#error "VORONOI_DIAGRAM_GENERATOR_H included more than once."
#endif
#define AAA_VORONOI_DIAGRAM_GENERATOR_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#include <algorithm>

template< class T >	class c_struct_server
{
private:
	typedef std::vector< T >		asso_vec;
	asso_vec	_vec;
	INT32		_size_allocated;
	INT32		_size;
	INT32		_cur;
private:

public:
	void	erase()
	{
		_size = 0;
		_cur = -1;
	}
	void	dealloc()
	{
		_vec.clear();
		_size_allocated = 0;
		erase();
	}
	void	dealloc_with_pointed_object()
	{
		for( auto const & elt : _vec )
			delete elt;
		dealloc();
	}

	FINLINE	T	operator[] ( INT32 CONST  );

	c_struct_server()	:	_size(0), _size_allocated(0), _cur(-1)		{}
	virtual	~c_struct_server() {	dealloc();	}

	FINLINE	T*		get(		INT32 index );
	FINLINE	T*		get_always(	INT32 CONST index );
	FINLINE	T*		get_next_free();

	FINLINE	void	reset_parsing()				{	_cur = -1;	}
	FINLINE	T*		get_next();

	FINLINE	INT32	get_size()					{	return _size; }
	FINLINE	void	free_obj_pointed();

};

template< class T >		FINLINE	T	c_struct_server<T>::operator[] ( INT32 CONST index )
{
	return _vec[index];
}
template< class T >		FINLINE	T *	c_struct_server<T>::get( INT32 index )
{
	if( index < 0 )
		index = 0;
	else if( index >= _size )
		index = _size - 1;
	return &_vec[index];
}
template< class T >		FINLINE	T *	c_struct_server<T>::get_always( INT32 CONST index )
{
	if( index >= _size_allocated )
	{
		T t;
		_vec.push_back( t );
		//_vec.reserve( index+1 );
		_size_allocated = index + 1;
	}
	return get( index );
}
template< class T >		FINLINE	T *	c_struct_server<T>::get_next_free()
{
	INT32	index = _size++;
	return get_always( index );
}
template< class T >		FINLINE	T *	c_struct_server<T>::get_next()
{
	if( ++_cur >= _size )
	{
		--_cur;
		return nullptr;
	}
	return &_vec[_cur]; 
}

namespace voronoi
{
	struct	Freenode	
	{
		struct	Freenode *			next_free;
	};
	struct FreeNodeArrayList
	{
		struct	Freenode *			memory;
		struct	FreeNodeArrayList *	next;
	};
	struct	Freelist	
	{
		struct	Freenode*	head;
		INT32				nodesize;
		INT32				nb_used;
		INT32				nb_escape;
	};

	struct point_2d
	{
		DOUBLE x, y;
	};
	struct Point3
	{
		INT32 x, y, z;
		INT32 count;
	};
	struct VertexLink
	{
		point_2d	coord;
		point_2d	v[3];
		INT32		count;
	};

	struct PolygonPoint
	{
		struct Site*	site;
	//	struct			point_2d	coord;
		REAL			angle;
	//	bool			b_boundary;
	};

	// structure used both for sites and for vertices
	struct Site
	{
		//	z ignored in the voronoi process but maa store original z
	//	DOUBLE								xyz[3];	//	z ignored in the voronoi process but maa use it
		DOUBLE								x;
		DOUBLE								y;
		DOUBLE								z;

		bool								b_use;
		bool								b_boundary;
		INT32								ref;
		INT32								site_id;
		std::vector<struct PolygonPoint>*	point_list;
		REAL								bary[3];
		FP32								color[4];
	//	INT32		overallRefcnt;
	};
	struct Edge	
	{
		DOUBLE			a, b, c;
		struct	Site *	reg[2];
		struct	Site *	end_point[2];
		DOUBLE			ce;
		INT32			edge_id;
		bool			b_created_how;
	};
	//this is an output format so REAL is enough
	struct st_segment_voronoi 
	{
		struct	Site *		a;
		struct	Site *		b;
	};
	struct Halfedge 
	{
		struct	Halfedge *	EL_left;
		struct	Halfedge *	EL_right;
		struct	Edge *		EL_edge;
		struct	Site *		vertex;
		struct	Halfedge *	PQ_next;
		INT32				EL_pm;
		DOUBLE				ystar;
		INT32				EL_refcnt;
	};

#define VERTEX_OLD	0
	class generator
	{
	public:
		generator();
		~generator();

		void	dealloc();

		bool	generate_voronoi(	REAL * src, INT32 site_nb, 
									REAL x_min, REAL x_max, REAL y_min, REAL y_max,
									REAL dist_min, bool b_gen_vertex_info = true			);

		//By default, the delaunay triangulation is NOT generated
		void set_generate_delaunay(	bool b );

		//By default, the voronoi diagram IS generated
		void set_generate_voronoi(	bool b );
	public:
		INT32			_site_nb;
		INT32			_site_index_begin;
		INT32			_site_index_end;

	private:
		struct	Site*	_sites;
	//	FINLINE	void	push_point( struct Site* dst, struct Site* s1, struct Site* s2 );
	public:
		FINLINE	struct	Site *		get_sites_src()		{	return _sites; }

	private:
		INT32			_site_nb_allocated;

		INT32			_site_id;
		INT32			_nsite_nb_sqrt;
		INT32			_vertices_id_generator;

		struct Freelist	_site_free_list;
		FINLINE	struct	Site*		site_create(	DOUBLE CONST x, DOUBLE CONST y, bool CONST b_boundary	);
		FINLINE	void				site_inc_ref(	struct Site * CONST v	);
		FINLINE	void				site_dec_ref(	struct Site * CONST v	);

		INT32			_edges_id_generator;
		struct Freelist _edge_free_list;
		FINLINE	void				edge_free(		struct Edge * CONST e );
		FINLINE	struct Edge*		edge_create(	struct Site * CONST s1, struct Site * CONST s2,
													DOUBLE CONST a, DOUBLE CONST b, DOUBLE CONST c, DOUBLE CONST ce );

		FINLINE	struct Halfedge*	half_edge_create(				struct Edge     * CONST e,	INT32 CONST pm	);
		FINLINE	void				half_edge_dec_ref_no_delete(	struct Halfedge * CONST he );
		FINLINE	void				half_edge_dec_ref(				struct Halfedge * CONST he );
		FINLINE	void				half_edge_inc_ref(				struct Halfedge * CONST he );
	private:
		c_struct_server<struct Edge*>	_segments_delaunay;
	public:
		FINLINE	void				reset_segment_delaunay_iterator()		{	_segments_delaunay.reset_parsing();			}
		FINLINE	struct Edge*		get_segment_delaunay_next()				{	
																				struct Edge** hd = _segments_delaunay.get_next();
																				return hd ? *hd : nullptr;
																			}
		FINLINE	INT32				get_segment_delaunay_nb()				{	return _segments_delaunay.get_size();		}
		FINLINE	struct Edge*		get_segment_delaunay( INT32 index )		{	return *_segments_delaunay.get( index );	}
	private:
	#if VERTEX_OLD
		Site**						_vertices;
		INT32						_vertices_nb_allocated;
	#else
		c_struct_server<Site*>		_vertices;
	public:
		FINLINE	void				reset_vertice_iterator()				{	_vertices.reset_parsing();		}
		FINLINE	Site*				get_vertice_next()						{
																				Site** hd = _vertices.get_next();
																				return hd ? *hd : nullptr;
																			}
		FINLINE	INT32				get_vertice_nb()						{	return _vertices.get_size();	}
		FINLINE	Site**				get_vertice( INT32 CONST index )		{	return _vertices.get( index );	}
	#endif
	private:
		c_struct_server<st_segment_voronoi>	_segments_voronoi;
		FINLINE	void				push_segment_voronoi(	struct Edge * CONST e, struct Site * CONST s1, struct Site * CONST s2  );
	public:
		FINLINE	void				reset_segment_voronoi_iterator()			{	_segments_voronoi.reset_parsing();		}
		FINLINE	st_segment_voronoi* get_segment_voronoi_next()					{	return _segments_voronoi.get_next();	}
		FINLINE	INT32				get_segment_voronoi_nb()					{	return _segments_voronoi.get_size();	}
		FINLINE	st_segment_voronoi* get_segment_voronoi( INT32 CONST index )	{	return _segments_voronoi.get( index );	}
	public:
		void reset_iterator_vertex_pair_final()
		{
			_final_vertex_link_cur = 0;
		}
		bool get_next_vertex_pair_final( REAL& x1, REAL& y1, REAL& x2, REAL& y2 );

	private:
		point_2d*	_vertex_final;	
		INT32		_vertex_final_nb;
		INT32		_vertex_final_nb_allocated;	
		INT32 		_vertex_final_index;
	public:
		FINLINE	void reset_iterator_vertex_final()
		{
			_vertex_final_index = 0;
		}

		FINLINE	bool get_next_vertex_final( REAL& x, REAL& y )
		{
			if( _vertex_final == 0 )
				return false;
			if( _vertex_final_index >= _vertex_final_nb )
				return false;

			point_2d&	pt = _vertex_final[_vertex_final_index];
			x = REAL(pt.x);
			y = REAL(pt.y);
			++_vertex_final_index;
			return true;
		}

	private:
		void cleanup();

		FINLINE	char*				get_free(		struct Freelist * CONST fl );

		FINLINE	struct Halfedge*	PQfind();
		FINLINE	bool				PQ_empty() CONST;

		struct Halfedge**	_EL_hash;
		INT32				_EL_hash_nb_allocated;


		FINLINE	void				make_vertex(	struct Site * CONST v	);

		FINLINE	void				out_triple(		struct Site CONST * CONST s1, struct Site CONST * CONST s2, struct Site CONST * CONST s3 );


		FINLINE	struct point_2d		PQ_min();
		FINLINE	struct Halfedge *	PQ_extract_min();	
		void						init_geom();
		bool						voronoi( bool CONST genVectorInfo );



		FINLINE	void end_point( struct Edge * CONST e, INT32 CONST lr, struct Site * CONST s );


		//	EL	mean Edge List
				DOUBLE		_x_factor_hash;
				bool				EL_initialize();
		FINLINE	void				EL_insert(	struct Halfedge * CONST lb, struct Halfedge * CONST he_new );
		FINLINE	struct Halfedge *	EL_get_hash( INT32 CONST b );
		FINLINE	struct Halfedge *	EL_left(	struct Halfedge CONST *	CONST he	) CONST ;
		FINLINE	void				EL_delete(	struct Halfedge       *	CONST he	);
		FINLINE	struct Halfedge *	EL_leftbnd(	struct Site           *	CONST p		);
		FINLINE	struct Halfedge *	EL_right(	struct Halfedge CONST *	CONST he	) CONST;
	public:
		FINLINE	struct Halfedge *	EL_get_left_end()	CONST	{	return _EL_left_end;	}
		FINLINE	struct Halfedge *	EL_get_right_end()	CONST	{	return _EL_right_end;	}
	private:
		FINLINE	struct Site *		leftreg(	struct Halfedge CONST * CONST he	) CONST;
	//	FINLINE	void		out_site(struct Site *s);

		//	PQ	mean Priority Queue
				DOUBLE		_y_factor_hash;
				bool				PQ_initialize();
		FINLINE	void				PQ_insert(	struct Halfedge * CONST he, struct Site* CONST v, DOUBLE CONST offset );
		FINLINE	void				PQ_delete(	struct Halfedge * CONST he );
		FINLINE	INT32				PQ_bucket(	struct Halfedge * CONST he );
		FINLINE	void				clip_line(	struct Edge     * CONST e );
		FINLINE	bool				right_of(	struct Halfedge * CONST el,struct Site* CONST p );

		FINLINE	struct Site *		rightreg(	struct Halfedge CONST * CONST he ) CONST;
		FINLINE	struct Edge *		bisect(		struct Site           * CONST s1, struct Site* CONST s2 );
		FINLINE	struct Site *		intersect(	struct Halfedge CONST * CONST el1, struct Halfedge CONST * CONST el2 );	//, struct point_2d *p=0);

	//	FINLINE	void				out_ep(struct Edge* CONST e);
	//	FINLINE	void				out_vertex( struct Site* CONST v);
		FINLINE	struct Site*		next_one();

	//	void		line( float CONST x1, float CONST y1, float CONST x2, float CONST y2 );
	//	void		circle( float CONST x, float CONST y, float CONST radius );

		void  		insert_vertex_address(	INT32 CONST vertexNum, struct Site * CONST address );
		void		insert_vertex_link(		INT32 CONST vertexNum, INT32 CONST vertexLinkedTo );

		void		generate_final_vertex_link();
	private:
		bool				_b_gen_delaunay;
		bool				_b_gen_voronoi;

		struct Freelist		_half_edge_free_list;
		struct Halfedge *	_EL_left_end;
		struct Halfedge *	_EL_right_end;
		INT32 				_EL_hash_size;

		DOUBLE				_xmin, _xmax, _ymin, _ymax;


		struct Site *		_bottom_site;



		INT32				_PQ_hash_nb;
		INT32				_PQ_hash_nb_allocated;
		struct Halfedge *	_PQ_hash;
		INT32				_PQ_count;
		INT32				_PQ_min;

		INT32				_try_nb, _total_search;
	//	float				cradius;
		INT32				_total_alloc;	//todo init

		DOUBLE				_border_x_min, _border_x_max, _border_y_min, _border_y_max;

		FreeNodeArrayList * _memory_list;
		FreeNodeArrayList * _memory_list_cur;


		INT32*		_vertices_count_1;
		INT32*		_vertices_count_3;
		Point3*		_vertex_link;		//lists all the vectors that each vector is directly connected to	
		INT32		_vertex_link_nb;
		INT32		_vertex_link_nb_allocated;

	//	long		sizeOfVertices ;

		VertexLink* _final_vertex_links;
		INT32 		_final_vertex_link_nb;
		INT32		_final_vertex_link_nb_allocated;
		INT32		_final_vertex_link_cur;

		DOUBLE		_min_distance_between_sites;
		DOUBLE		_min_distance_between_sites_squared;
	public:
		bool		_b_verbose;
		DOUBLE		_precision;
		REAL		_PQ_hash_factor;
		REAL		_EL_hash_factor;
		bool		_b_escape;
		void		set_escape_nb_site(			INT32 nb )	{	_site_free_list.nb_escape		= nb;	}
		void		set_escape_nb_edge(			INT32 nb )	{	_edge_free_list.nb_escape		= nb;	}
		void		set_escape_nb_half_edge(	INT32 nb )	{	_half_edge_free_list.nb_escape	= nb;	}

		struct angle_less	{ 
		public: 
			bool CONST operator() ( CONST struct PolygonPoint a, CONST struct PolygonPoint b ) CONST
			{	return a.angle < b.angle ;	}

		};
		struct angle_more	{ 
		public: 
			bool CONST operator() ( CONST struct PolygonPoint a, CONST struct PolygonPoint b ) CONST
			{	return a.angle > b.angle ;	}
		};
		FINLINE	void sort_voronoi_site( struct Site * s )
		{
			sort( s->point_list->begin(), s->point_list->end(), angle_more() );
		}
	};
}




