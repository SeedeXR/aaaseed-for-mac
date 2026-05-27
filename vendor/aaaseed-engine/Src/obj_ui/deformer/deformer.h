
#ifdef AAA_DEFORMER_H
#error "DEFORMER_H included more than once."
#endif
#define AAA_DEFORMER_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_def_node;

class	c_deformer : public c_obj_active_ui
{
	FACTORY_ABSTRACT_DECLARE( c_deformer, c_obj_active_ui );
	friend	c_def_node;
private:
	bool	_b_deforming	{false};	//update should set this
protected:
//	REAL*	_src[2];	//todoq	only index 0 used for now
//	REAL*	_dst[2];	//todoq	only index 0 used for now
//	INT32	_nb;
	bool	_b_add;		//todo now always true, but switch implemented but not used
public:
	virtual void	update()=0;
	virtual	void	draw() {};
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) {};

//	FINLINE	REAL*	get_src_pt( INT32 index=0 )					{ return _src[index]; }
//	FINLINE	REAL*	get_dst_pt( INT32 index=0 )					{ return _dst[index]; }
//	FINLINE	INT32	get_nb()									{ return _nb; }
//	FINLINE	void	set_src_pt( REAL* pt, INT32 index = 0 )		{ _src[index] = pt; }
//	FINLINE	void	set_dst_pt( REAL* pt, INT32 index = 0 )		{ _dst[index] = pt; }
//	FINLINE	void	set_nb( INT32 in )							{ _nb = in; }
private:
//	FINLINE void	apply( REAL CONST * src, REAL* dst, INT32 nb )
//	{
//		set_src_pt( src );
//		set_dst_pt( dst );
//		set_nb( nb );
//		apply();
//	}
public:
//bad practice when src and _dst index will be different
//	FINLINE	void	set_src_dst_pt( REAL* src_in, REAL* dst_in, INT32 index = 0)	{ _src[index] = src_in; _dst[index] = dst_in; }
	FINLINE	bool	is_deforming()	CONST		{ return this && _b_deforming; }
	FINLINE	void	set_deforming( bool in )	{ _b_deforming = in; }	//todoq make sure every deformer check all conditions (eg translate don't do it, scale do it)

			INT32	param_init_pt_start();

#define	DEFORMER_BASE_PARAMS	\
	PARAM_DEF_BOOL_OFF(	active ) \
	PARAM_DEF_REF(		name_symbo )

	static	CONST	INT32	BASE_PARAM_NB = 2;
};

