#ifdef AAA_PARAM_MORE_H
#error "PARAM_MORE_H included more than once."
#endif
#define AAA_PARAM_MORE_H 1


#ifndef AAA_CONNEX_H
#	include "connex.h"
#endif

typedef UINT32 OBJ_UI_ID;
class c_param;
class o_str;
class c_obj_ui;

extern FINLINE	c_obj_ui*	get_obj_ui_from_id(	UINT32 CONST id_in );

//todo add is_needed() so we can remove it when it carries no information
class c_param_more final
{
private:
	OBJ_UI_ID		_obj_ui_id;		// for now (2026 May) we store only the obj_ui_id here in the case of a param header (this replace a c_obj_ui pointer)
	//	_flags_and_nb packed UINT32 layout :
	//	  bit  0..23 : count of attached params (NB_MASK = 0x00ffffff, max ~16M).
	//	  bit 24     : error flag (ERROR_MASK = 0x01000000), set/cleared via set_flag_error / clear_flag_error.
	//	  bit 25..31 : reserved.
	UINT32			_flags_and_nb;
	c_param*		_param_attached;
	o_str*			_comment;		//todo index in a pool will reduce memory

	//todo separate connex to save more space?
	c_connex::LIST*	_out;
	c_connex::LIST*	_in;

	CONSTEXPR static UINT32	NB_MASK  = 0x00ffffff;	// list nb max is 256 x 256 x 256 -1
	CONSTEXPR static UINT32	ERROR_MASK = 0x01000000;

public:
	static	UINT32*			get_nb_pt();
	static	UINT32*			get_nb_created_pt();

	c_param_more();
	~c_param_more();
	//param
	C_NO_CPY_MOVE(c_param_more)

			void			set_obj(		c_obj_ui* CONST obj );
	FINLINE	void			clear_obj()								{	_obj_ui_id = 0;	}
	FINLINE	bool			is_obj()								{	return _obj_ui_id != 0;	}
	FINLINE	c_obj_ui*		get_obj()						CONST	{	return (_obj_ui_id != 0) ? get_obj_ui_from_id( _obj_ui_id ) : nullptr;	}

			c_obj_ui*		find_obj_owner();

	FINLINE	UINT32			get_param_attached_nb()			CONST	{	return  _flags_and_nb & NB_MASK;	}
//	FINLINE	bool			is_param_attached()				CONST	{	return  get_param_attached_nb()!=0;		}
	FINLINE	c_param*		get_param_attached()			CONST	{	return  _param_attached;				}
			void			attach_param(	c_param * CONST pt, INT32 CONST nb );

			c_obj_ui*		get_obj_attached() CONST;

			void			set_comment(		C_PCHAR_C text );
			void			set_comment_int32(	INT32 CONST val );
			void			set_comment_real(	REAL CONST val );
			void			set_comment_double(	DOUBLE CONST val );

			void			clear_comment();
			o_str*			get_comment()					CONST;
			o_str*			get_comment_always();
			C_PCHAR_C		get_comment_str()				CONST;

	//linked to comment for now
	//todo move to c_param ?
	FINLINE	void			set_flag_error()						{	_flags_and_nb |= ERROR_MASK;	}
	FINLINE	void			clear_flag_error()						{	_flags_and_nb &= ~ERROR_MASK;	}
	FINLINE	bool			is_flag_error()					CONST	{	return  _flags_and_nb & ERROR_MASK;	}

	//CONNEXION
	FINLINE	bool			is_out()						CONST	{	return _out && _out->is_item();									}
	FINLINE	INT32			get_out_nb()					CONST	{	return _out ? _out->get_item_nb() : 0;							}
	FINLINE	c_connex*		get_out( INT32 CONST index )	CONST	{	return (index<get_out_nb()) ? _out->get_item(index) : nullptr;	}
	FINLINE	c_connex::LIST*	get_out()								{	return _out;													}

	FINLINE	bool			is_in()							CONST	{	return _in && _in->is_item();									}
	FINLINE	INT32			get_in_nb()						CONST	{	return _in ? _in->get_item_nb() : 0;							}
	FINLINE	c_connex*		get_in( INT32 CONST index )		CONST	{	return (index<get_in_nb()) ? _in->get_item(index) : nullptr;	}
	FINLINE	c_connex::LIST*	get_in()								{	return _in;														}
private:
			AAA_ERR			add_con(	c_connex::LIST*& ref,		c_connex * CONST con );	
			void			remove_con(	c_connex::LIST* CONST ref,	c_connex * CONST con ) CONST;
			void			disconnect(	c_connex::LIST* CONST ref ) CONST;
public:
	FINLINE AAA_ERR			add_in(		c_connex * CONST con ) 			{	return add_con( _in, con );		}	//par move to plug vocabulary
	FINLINE AAA_ERR			add_out(	c_connex * CONST con ) 			{	return add_con( _out, con );	}
	FINLINE void			remove_in(	c_connex * CONST con ) CONST	{	remove_con( _in, con );			}
	FINLINE void			remove_out(	c_connex * CONST con ) CONST	{	remove_con( _out, con );		}
			void			disconnect_in()					CONST		{	disconnect( _in );				}
			void			disconnect_out()				CONST		{	disconnect( _out );				}

	//	BORROW : raw setter for an externally-owned c_connex::LIST*. See c_param::borrow_in /
	//	borrow_out for the borrow contract. Called only by c_param's borrow / unborrow path.
	FINLINE	void			borrow_in(	c_connex::LIST*	CONST in  )		{	_in = in;						}
	FINLINE	void			borrow_out(	c_connex::LIST*	CONST out )		{	_out = out;						}
};	