
#ifdef AAA_BDD_BLOB_H
#error "BDD_BLOB_H included more than once."
#endif
#define AAA_BDD_BLOB_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_BLOB_H
#	include "obj_ui/bdd/bdd_multitouch/blob.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _SET_
#	include <set>
#endif
#ifndef AAA_NET_UTIL_OBJ_H
#	include "obj_ui/com/net_util_obj.h"
#endif

class	c_bdd_blob_base  : public c_bdd_multiple
{
	FACTORY_ABSTRACT_DECLARE( c_bdd_blob_base, c_bdd_multiple );
public:
protected:
private:

public:
	virtual	UINT32				get_blob_nb()			= 0;
	virtual	c_blob CONST &		get_blob_index(INT32 i)	= 0;
	//todo
	//		BLOBS_CONT CONST &	get_blobs()				{	return _blob;			}
	//		void				clear_blob();
};

class	c_bdd_blob final : public c_bdd_blob_base
{
	FACTORY_DECLARE( c_bdd_blob, c_bdd_blob_base );
public:
	static	c_bdd_blob*		cur;
protected:
	BLOBS_CONT			_blob;
	std::set<c_bdd*>	_bdd_src;

	REAL				_center[3];
	REAL				_center_ui[3];
	REAL				_size[3];

	INT32				_blob_nb;
	bool				_blob_reset_trig_ui;
//	INT32				_blob_point_nb_min;
//	INT32				_blob_point_nb_max;
//	REAL				_blob_area_min;
//	REAL				_blob_area_max
//	bool				_b_contour_draw_discarded_ui;
	bool				_b_contour_draw_ui;
//	REAL				_contour_draw_alpha_ui;
	bool				_b_contour_draw_as_quad_ui;
	bool				_b_bounding_draw_ui;
	bool				_b_bounding_draw_ellipse_ui;
	INT32				_ellipse_seg_nb;
//	REAL				_bounding_draw_alpha_ui;
	REAL				_bounding_draw_size_factor;
	REAL				_bounding_draw_size_min;
	bool				_b_multiply_blob;
	INT32				_multiply_nb;
	bool				_b_multiply_unstable;
	REAL				_multiply_range_x;
	REAL				_multiply_range_y;


	INT32				_fiducial_nb;
	bool				_b_fiducial_draw;
	bool				_b_blob_inserted;

	c_net_buf_obj		_net_buf;
	typedef	c_map_receiver< BLOBS_CONT, c_blob >	TYPE_MAP_RECEIVER;
	TYPE_MAP_RECEIVER	_map_receiver;
private:
	//	void	do_process();
public:
	void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
			void	draw_blobs( BLOBS_CONT& blobs );
	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual bool	can_implicit() final override { return true; }

	virtual	UINT32				get_blob_nb()			{	return (UINT32)_blob.size();	}
	virtual	c_blob CONST &		get_blob_index(INT32 i)	{	return _blob[i];		}
			BLOBS_CONT CONST &	get_blobs()				{	return _blob;			}
			void				clear_blob();
			void				add_blob( INT32 id, REAL x, REAL y, REAL sx, REAL sy );

	//hack don't work with receive stuff 
	virtual	INT32				get_point_nb();	
	virtual	INT32				get_point_and_id( REAL* dst, INT32 CONST index );

			void				register_as_src( c_bdd* src );
			REAL				get_field_at( REAL* pos );
			void				get_field_gradient_at( REAL* grad,  REAL* pos );
};
