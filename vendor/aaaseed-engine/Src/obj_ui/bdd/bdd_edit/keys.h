
#ifdef AAA_KEYS_H
#error "KEYS_H included more than once."
#endif
#define AAA_KEYS_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef _LIST_
#	include <list>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif

enum KEY_TYPE : INT32
{
	KEY_CONTROL = 0,
	KEY_CONST,
	KEY_LINEAR,
	KEY_SINUS,
	KEY_CATMULL,
	KEY_CATMULL_BIS,	//like CATMULL for now, left to experiment
	KEY_TYPE_NB
};

extern C_PCHAR_C	str_key_type[KEY_TYPE_NB];

class c_control_key;

class	c_key
{
private:
	static	REAL	marker_size[2];
	static	REAL	marker_size_half[2];

	REAL	_phase;
	REAL	_pos[3];
	INT32	_s_after;
	bool	_b_include;
public:
	FINLINE	static	void	set_marker_size( REAL in_u, REAL in_v )
	{
		set_v2(		marker_size,		in_u, in_v );
		scale_v2(	marker_size_half, marker_size, REAL(.5) );
	}
	static	c_key*	get_new();
	static	void	set_free( c_key* key );

	c_key();
	virtual	~c_key();

			AAA_ERR	save_v0( FILE* file );
			AAA_ERR	load_v0( FILE* file );
			void	draw_base(	INT32 type, REAL x, REAL y );
public:
			void	param_set_pt( c_obj_ui*	owner, INT32& count );
	FINLINE	void	draw_2d(	INT32 type );
	FINLINE	void	draw(		INT32 type, REAL phase_factor, REAL phase_offset, INT32 axe );

	FINLINE	void	copy_no_pos_to( c_key* dst )
	{
		dst->_b_include	= _b_include;
		dst->_s_after	= _s_after;
	}
	FINLINE	void	copy_to( c_key* dst )
	{
		copy_no_pos_to( dst );
		dst->_phase	=	_phase;
		cpy_v3( dst->_pos, _pos );
	}
//	ANCESSOR
	FINLINE	REAL	get_phase()						{	return	_phase;		}
	FINLINE	void	set_phase( REAL phase_in )		{	_phase = phase_in;	}
	FINLINE	INT32	get_after()						{	return	_s_after;	}
	FINLINE	REAL*	get_pos()						{	return	_pos;	}
	FINLINE	REAL	get_pos( INT32 axe )			{	return	_pos[axe];	}
	FINLINE	void	set_pos( REAL val, INT32 axe )	{	_pos[axe] = val;	}
};

class	c_control_key final : public c_obj_ui
{
	FACTORY_DECLARE( c_control_key, c_obj_ui );
public:
	typedef	std::vector<c_key*>		cont_key;
	typedef	cont_key::iterator		iter_key;

private:
	cont_key	_keys;
	INT32		_key_cache;
	INT32		_key_id_cur;
	c_key*		_p_key_cur;

	bool		_b_need_sort;
	bool		_b_loop;
	REAL		_phase_start;
	REAL		_phase_stop;
	REAL		_phase_factor;
	REAL		_phase_offset;
	REAL		_phase_cur;
	REAL		_offset;
	REAL		_factor;
	INT32		_s_control_type;
	INT32		_key_nb;
	FP32		_color_ui[4];
	FP32		_color[4];

	o_str		_control_name;	//par use obj_name ?	//todo rename

//	INT32		visu_type_;
	REAL		_default_value[3];

public:
			void		init();
	virtual	void		param_init_pt();
			void		key_dealloc();
private:
	FINLINE	INT32		get_key_type( c_key* key );
	FINLINE	REAL		make_phase( REAL in );
public:
	FINLINE	REAL	loop_phase( REAL in );
private:
	FINLINE	INT32		build_id( INT32 id );	// 1 based
	FINLINE	INT32		get_key_nb()				{ return _key_nb; }
private:
	FINLINE	void		sort_key();

			c_key*		push_key_new(	REAL* val = nullptr	);

			c_key*		find_key_pt_by_id(	INT32 id );
			INT32		find_key_id_by_pt(	c_key* pt_in );
			//INT32		keys_find_id_by_phase(				REAL phase_in );
			INT32		find_key_id_by_phase_low(			REAL phase_in );
			INT32		find_key_id_by_phase(				REAL phase_in );

			void		set_key_cur(			c_key*	key			);
			void		set_key_cur_by_id(		INT32	id			);
public:
			void		set_key_cur_by_phase(	REAL	phase_in	);
private:
			void		insert_key();
			void		insert_key(			REAL phase_in, REAL* pos );
			void		insert_key(			REAL phase_in, REAL x, REAL y, REAL z );
			void		delete_key_cur();
			void		insert_key_interpolate();

			REAL		get_value_inter(					INT32 key_type, c_key* p, c_key* n, REAL inter,	INT32 axe );
			void		get_value_inter_v2r(	REAL* dst,	INT32 key_type, c_key* p, c_key* n, REAL inter );
			void		get_value_inter_v3r(	REAL* dst,	INT32 key_type, c_key* p, c_key* n, REAL inter );

			REAL		get_value_low(						REAL phase_in, INT32 axe );
			void		get_value_v2r_low(		REAL* dst,	REAL phase_in );
public:
			void		set_control_type( INT32 control_type )	{ _s_control_type = control_type; } 

			bool		is_value(		REAL in );
			REAL		get_value(		REAL in, INT32 axe=0 );
			void		get_value_v2r(	REAL* dst, REAL phase_in );

	FINLINE	FP32*	get_color()				{ return _color; }
	FINLINE	C_PCHAR_C	get_control_name()		{ return _control_name.get(); }

//	FINLINE	void	set_name( C_PCHAR_C str_in )	{ loc_name_.set(str_in); }

			//	bool		keys_find_by_phase( c_key* &p, c_key* &n, REAL phase_in );
			void		prev_key();
			void		next_key();

			void		set_phase_start(	REAL phase_in )		{ _phase_start = phase_in; }
			void		set_phase_stop(		REAL phase_in )		{ _phase_stop = phase_in; }
			void		set_phase(			REAL phase_in )		{ _phase_cur = phase_in; }
			REAL		get_phase_start()						{ return _phase_start; }
			REAL		get_phase_stop()						{ return _phase_stop; }

			REAL		get_phase()	{ return _phase_cur; }

	
			REAL		get_phase_next(	REAL phase_in );
			REAL		get_phase_prev(	REAL phase_in );
			bool		add_phase(		REAL& phase_in, REAL& dt, bool b_stop );

			AAA_ERR		save_to_file_v1( FILE*	file );
			AAA_ERR		load_from_file_v1( FILE* file );

	virtual	AAA_ERR		load_do_after( o_str CONST & filename );
	virtual	AAA_ERR		save_do_after( o_str CONST & filename );

public:
			c_key*		get_key_cur();

			void		set_key_phase(	REAL phase_in	);
			void		set_key_value(	REAL val_in,	INT32 axe );

			REAL		get_key_phase();
			REAL		get_key_phase(	INT32 id_in );
			REAL		get_key_value(	INT32 axe	);

			void		update();

			void		draw_ui( bool b_sel, FP32 CONST * color_in, INT32 axe, REAL phase_min = -REAL_BIG_VALUE, REAL phase_max = REAL_BIG_VALUE );
			void		draw_curve_check();
			void		draw_phase( c_key* p, c_key* n );
			void		draw_phase( REAL phase_min = -REAL_BIG_VALUE, REAL phase_max = REAL_BIG_VALUE );
			void		draw_curve( c_key* p, c_key* n, INT32 axe );
			void		draw_curve( FP32 CONST * color_in, INT32 axe, REAL phase_min = -REAL_BIG_VALUE, REAL phase_max = REAL_BIG_VALUE );

//	2d unused and buggy for now
			void		draw_ui_2d( bool b_sel, FP32 CONST * color_in );
			void		draw_curve_2d( FP32 CONST * color_in );
			void		draw_curve_2d( c_key* p, c_key* n );
			void		draw_phase_2d();

			bool		do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );
			void		mouse_down( FP32& u_start, FP32& v_start, INT32 axe );
			void		mouse_up();
			void		mouse_get_start_uv( FP32& u_start, FP32& v_start );
};
