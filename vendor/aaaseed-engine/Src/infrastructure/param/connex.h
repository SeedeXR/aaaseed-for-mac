
#ifdef AAA_CONNEX_H
#error "CONNEX_H included more than once."
#endif
#define AAA_CONNEX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_LIST_H
#	include "infrastructure/obj/aaa_list.h"
#endif


class c_param;
class c_trax;
class c_obj_ui;

class	c_connex final
{
public:
	typedef c_list_pt<c_connex>	LIST;

	static	bool	b_verbose;
private:
	bool		_b_trax_out;
	c_trax*		_trax;
	c_param*	_param;
	c_obj_ui*	_param_obj;

public:
	c_connex(	c_obj_ui* o_in, c_param* p_in,
				c_obj_ui* o_out, c_param* p_out	);
	~c_connex();

	AAA_ERR				plug();
	void				unplug();
	FINLINE	c_param*	get_param_in_set()	{	return _param;	}
	FINLINE	c_param*	get_param_out_set()	{	return _param;	}

			c_param*	get_in_param();
			c_param*	get_out_param();
	FINLINE	c_obj_ui*	get_in_obj()		{	 return _b_trax_out ? _param_obj		: (c_obj_ui*)_trax ;	}
	FINLINE	c_obj_ui*	get_out_obj()		{	 return _b_trax_out ? (c_obj_ui*)_trax	: _param_obj ;			}
	FINLINE	c_obj_ui*	get_param_obj()		{	 return _param_obj ;	}
	FINLINE	c_trax*		get_trax()			{	 return _trax ;			}

	static	AAA_ERR		add(	c_obj_ui* o_in,	c_param* p_in,	c_obj_ui* o_out,	c_param* p_out );
	static	void		draw();
	static	AAA_ERR		remove(	c_connex* con	);
};




