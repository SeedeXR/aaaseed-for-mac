
#ifdef AAA_OBJ_VALUE_H
#error "OBJ_VALUE_H included more than once."
#endif
#define AAA_OBJ_VALUE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_obj_value final : public c_obj_ui
{
	FACTORY_DECLARE(c_obj_value,c_obj_ui);
public:
	static INT32 CONST ITEM_NB = 32;
	static INT32 CONST GROUP_NB = 4;
private:
	REAL	_values[ ITEM_NB * GROUP_NB ];
public:

	virtual	void	param_init_pt();

	FINLINE	INT32	get_clean_index( INT32 index )			{ return IMOD( index, GROUP_NB*ITEM_NB );		}
	FINLINE	REAL	get_from_index( INT32 index )			{ return _values[ get_clean_index(index) ];	}
	FINLINE	void	set_to_index( INT32 index, REAL in )	{ _values[ get_clean_index(index) ] = in;		}

			p_param	get_value_param( INT32 index );

//private:
//	INT32	get_sort_index_low( INT32 start, INT32 stop, INT32 order_index );
public:
	INT32	get_sort_index_from_min( INT32 start, INT32 stop, INT32 order_index );
	INT32	get_sort_index_from_max( INT32 start, INT32 stop, INT32 order_index );
};



