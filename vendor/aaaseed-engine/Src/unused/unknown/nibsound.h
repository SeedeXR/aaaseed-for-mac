
#ifdef AAA_NIBSOUND_H
#error "NIBSOUND_H included more than once."
#endif
#define AAA_NIBSOUND_H 1


#ifndef AAA_OBJ_UI_H
#	include    "obj_ui.h"
#endif

class c_serial;

class	c_nibsound final : public c_obj_ui
	{
public:
	c_serial* max_serial;
private:
	BOOL	b_send_only_change;
	REAL	scaling_factor;
	BOOL	b_ring_send;
	BOOL	b_position_send;
	BOOL	b_rgba_send;
	REAL	rgba_dist;
	REAL	rgba_angle;
	INT32	rgba_bind;
	REAL	rgba_size_u;
	REAL	rgba_size_v;
	BOOL	b_verbose;	//todoq put it in obj_ui ?

	INT32	refresh_index;

	char	str_out[128];
	char	*str_cur;

	void	print_str_out();

	void	send_char( unsigned char value_in);
	void	send_int8( UINT32 value_in);
	void	send_real( REAL value_in);
	void	send_begin();
	void	send_end();

	void	send_value( INT32 symbol, INT32	value_in);
	void	send_value( INT32 symbol, REAL	value_in);
	void	send_refresh();
	void	send_ring();
	void	send_position();
	void	send_rgba_low( REAL u, REAL v, INT32 index);
	void	send_rgba();

public:
	inline	virtual	const char*	const	get_class_name()	{ return "nibsound"; };
	inline	virtual	const TYPE_IO		get_type_io()		{ return TYPE_IO(-1); };
	inline	virtual	const char*	const	get_fname_ext()		{ return "nibsound"; };
	c_nibsound();
	virtual	~c_nibsound();

	virtual	void update() {};

	REAL	get_rgba_dist() { return rgba_dist; }
	REAL	get_rgba_angle() { return rgba_angle; }
	void	send();
	void	init( INT32 port_nb = 5);
	void	param_init_pt();
	};

extern	c_nibsound *	nibsound;

#endif
