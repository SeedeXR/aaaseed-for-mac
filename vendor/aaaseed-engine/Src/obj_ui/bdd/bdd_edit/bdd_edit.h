
#ifdef AAA_BDD_EDIT_H
#error "BDD_EDIT_H included more than once."
#endif
#define AAA_BDD_EDIT_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

//todo extend : regroup more that ui_intercept
class	c_edit
{
public:
	enum ACTION : INT32
	{
		DO_NOTHING = 0,
		//	POINT
		SET_POINT_NEXT,
		SET_POINT_PREV,
		SET_POINT_BEGIN,
		SET_POINT_END,
		INSERT_POINT,
		DELETE_POINT,
		//	CURVE
		SET_CURVE_NEXT,
		SET_CURVE_PREV,
		SET_CURVE_BEGIN,
		SET_CURVE_END,
		INSERT_CURVE,

		CENTER_CAM,
//		FLIP_LOCK,
		ACTION_NB_MAX
	};

private:
	bool	_b_ui_intercept;
public:
	bool*	get_ui_intercept_pt()		{	return &_b_ui_intercept;	}
	bool	set_ui_intercept( bool b )	{	return _b_ui_intercept = b; }
	bool	is_ui_intercept()			{	return _b_ui_intercept;		}

	static	bool	enum_command(			o_str& o );
	static	ACTION	find_action_from_str(	o_str CONST & action_asked ); 
			ACTION	find_action_from_key(	INT32 key_code, bool b_special, INT32* modifiers, INT32* x, INT32* y );
};
