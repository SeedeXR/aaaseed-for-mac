
#ifdef AAA_AAA_EVENT_H
#error "AAA_EVENT_H included more than once."
#endif
#define AAA_AAA_EVENT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_event;
typedef c_event* (*CREATE_EVENT_FN)(void);

class	c_event
{
private:
	static	bool	b_verbose_sys;
	static	bool	b_verbose;

	UINT32	_type;
	UINT32	_sub_type;
	INT32	_value_nb;
	INT32	_value[5];

public:
	FINLINE	static	bool*	get_verbose_sys_pt()								{ return &b_verbose_sys; }
	FINLINE	static	bool	is_verbose_sys()									{ return b_verbose_sys; }

	FINLINE	static	bool*	get_verbose_pt()									{ return &b_verbose; }
	FINLINE	static	bool	is_verbose()										{ return b_verbose; }
			static	void	set_verbose(	bool CONST in );
			static	void	flip_verbose();			

			static	UINT8 CONST *	do_from_mem(	UINT8 CONST * pt );
			static	void			register_cod4(	UINT32 cod4, CREATE_EVENT_FN create_fn );

			c_event();
	virtual	~c_event();
		
	FINLINE	UINT32		get_type()							CONST				{ return _type; }
	FINLINE	bool		is_type(		UINT32 CONST type )	CONST				{ return _type == type; }
	FINLINE	UINT32		set_type(		UINT32 CONST type )						{ return _type = type; }
	virtual	C_PCHAR_C	get_type_str() = 0;

	FINLINE	UINT32		get_sub_type()						CONST				{ return _sub_type; }
	FINLINE	bool		is_sub_type(	UINT32 CONST type )	CONST				{ return _sub_type == type; }
	FINLINE	UINT32		set_sub_type(	UINT32 CONST type )						{ return _sub_type = type; }
	virtual	C_PCHAR_C	get_sub_type_str() = 0;

	FINLINE	INT32		get_value_nb()											{ return _value_nb; }
	FINLINE	INT32		set_value_nb(	INT32 CONST nb )						{ return _value_nb = nb; }
	FINLINE	INT32		get_value(		INT32 CONST index )	CONST				{ return _value[index]; }
	FINLINE	INT32		set_value(		INT32 CONST index, INT32 CONST value )	{ return _value[index] = value; }

	virtual UINT8 CONST *	read_from_mem(	UINT8 CONST * pt );
	virtual	UINT8*			write_to_mem(	UINT8* pt );

	FINLINE	void	set_types( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST nb )
	{
		set_type( type );
		set_sub_type( sub_type );
		set_value_nb( nb );
	}
	FINLINE	void	set_value_1( INT32 CONST v0 )
	{
		set_value( 0, v0 );
	}
	FINLINE	void	set_value_2( INT32 CONST v0, INT32 CONST v1 )
	{
		set_value_1( v0 );
		set_value( 1, v1 );
	}
	FINLINE	void	set_value_3( INT32 CONST v0, INT32 CONST v1, INT32 CONST v2 )
	{
		set_value_2( v0, v1 );
		set_value( 2, v2 );
	}
	FINLINE	void	set_value_4( INT32 CONST v0, INT32 CONST v1, INT32 CONST v2, INT32 CONST v3 )
	{
		set_value_3( v0, v1, v2 );
		set_value( 3, v3 );
	}
	FINLINE	void	set_value_5( INT32 CONST v0, INT32 CONST v1, INT32 CONST v2, INT32 CONST v3, INT32 CONST v4 )
	{
		set_value_4( v0, v1, v2, v3 );
		set_value( 4, v4 );
	}

	FINLINE	void	set_event_1( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST v0 )
	{
		set_types( type, sub_type, 1 );
		set_value_1( v0 );
	}
	FINLINE	void	set_event_2( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST v0, INT32 CONST v1 )
	{
		set_types( type, sub_type, 2 );
		set_value_2( v0, v1 );
	}
	FINLINE	void	set_event_3( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST v0, INT32 CONST v1, INT32 CONST v2 )
	{
		set_types( type, sub_type, 3 );
		set_value_3( v0, v1, v2 );
	}
	FINLINE	void	set_event_4( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST v0, INT32 CONST v1, INT32 CONST v2, INT32 CONST v3 )
	{
		set_types( type, sub_type, 4 );
		set_value_4( v0, v1, v2, v3 );
	}
	FINLINE	void	set_event_5( UINT32 CONST type, UINT32 CONST sub_type, INT32 CONST v0, INT32 CONST v1, INT32 CONST v2, INT32 CONST v3, INT32 CONST v4 )
	{
		set_types( type, sub_type, 5 );
		set_value_5( v0, v1, v2, v3, v4 );
	}

	virtual	bool	is_to_send();
			void	process();
private:
	virtual void	process_low();
};


#define CREATE_EVENT_TYPE( name, cod4 )							\
static	UINT32 CONST	COD4 = cod4;							\
bool	register_event_##name()									\
{																\
	c_event_##name instance;									\
	c_event::register_cod4( COD4, c_event_##name::create );		\
	return true;												\
}																\
c_event*	c_event_##name::create()							\
{																\
	return new c_event_##name;									\
}																\
bool b_trick_##name = register_event_##name();

