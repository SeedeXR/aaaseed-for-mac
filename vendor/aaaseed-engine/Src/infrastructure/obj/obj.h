
#ifdef AAA_OBJ_H
#error "OBJ_H included more than once."
#endif
#define AAA_OBJ_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#define AAA_OBJ_USE_ID() 0

class	c_obj
{
public:
	typedef	UINT32	OBJ_ID;
private:
	static	UINT32	obj_nb_created;
	static	UINT32	obj_nb;

#if AAA_OBJ_USE_ID()
			OBJ_ID	_obj_id;
#endif

public:
#if AAA_OBJ_USE_ID()
	FINLINE	OBJ_ID	get_obj_id() CONST { return _obj_id; } 
#endif

	static	UINT32*	get_nb_pt()			{ return &obj_nb; }
	static	UINT32*	get_nb_created_pt()	{ return &obj_nb_created; }

private:
	C_NO_CPY_MOVE( c_obj )
	//	we forbid these this way
	//c_obj( const c_obj& rhs );
	//c_obj& operator=( const c_obj& rhs );

public:
	c_obj();
	virtual ~c_obj();
};
