
#ifdef AAA_DEF_NODE_H
#error "DEF_NODE_H included more than once."
#endif
#define AAA_DEF_NODE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef	AAA_NODE_LIST_UI_H
#	include "infrastructure/obj/node_list_ui.h"
#endif

class	c_def_node final : public	c_node_list_ui<c_deformer>
{
	FACTORY_DECLARE( c_def_node, c_node_list_ui<c_deformer> );

	static c_def_node*	cur;
	static c_def_node*	prev;
	static c_def_node*	ui;
public:
	enum FIELD_TYPE : INT32
	{
		FIELD_TYPE_NO = 0,
		FIELD_TYPE_CURRENT,
		FIELD_TYPE_PREVIOUS,
		FIELD_TYPE_MAX_NB
	};
	static	C_PCHAR_C	field_type_str[FIELD_TYPE_MAX_NB];

	FINLINE	static	void		set_cur( c_def_node* pd )	{	prev = cur; cur = pd;	}
	FINLINE	static	c_def_node*	get_cur()					{	return cur;				}

	FINLINE	static	void		set_ui( c_def_node* pd )	{	ui = pd;				}
	FINLINE	static	c_def_node*	get_ui()					{	return ui;				}

	FINLINE	static	c_def_node*	get_prev()					{	return prev;			}

	static	c_def_node*	get_from_field_type( FIELD_TYPE field_type );


	static	void		c_init();

private:
	bool						_b_src_fixed;
	bool						_b_parallel_ui;
	std::vector<c_deformer*>	_defs;

			void			alloc_data( INT32 nb );

public:
	virtual	INT32			get_param_extra_nb();
	virtual	c_param_def*	build_param_extra( c_param_def* param );
	virtual	void			param_init_pt_extra( INT32& h );
	FINLINE	bool			is_src_fixed()					{	return _b_src_fixed;	};

public:
	virtual	void			update();
	//todo rename to avoid confusion with c_defortmer apply with another param interface 
	virtual	void			apply(		REAL* dst,						INT32 nb );
			void			apply(		REAL* dst,	REAL CONST * src,	INT32 nb );
			void			apply(		REAL* dst,	INT32 dst_stride,
										REAL* src,	INT32 src_stride,	INT32 nb );
			void			get_field(	REAL* dst,	REAL CONST * src,	INT32 nb );

};

