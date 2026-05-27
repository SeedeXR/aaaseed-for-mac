
#ifdef AAA_AAA_GYPSY_H
#error "AAA_GYPSY_H included more than once."
#endif
#define AAA_AAA_GYPSY_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_GYPSY_H
#	include "gypsy/gypsy.h"
#endif


class	c_gypsy_skel final : public c_obj
{
//	FACTORY_DECLARE(c_gypsy_skel,c_obj);
public:
	static	bool	b_dll_loaded;
private:
	Skeleton*	_skel;
	ActorData*	_actor;
	bool		_b_actor_data;
	bool		_b_live;
public:
	c_gypsy_skel();
#if AAA_WIN64()
	~c_gypsy_skel()		{}

	AAA_ERR	read_bvh_from_filename(		C_PCHAR filename )		{ return ERR_ANY; }
	AAA_ERR	read_actor_from_filename(	C_PCHAR filename )		{ return ERR_ANY; }
	bool	set_live( bool in )		{ return false; }	//todo
	void	set_refrence_position()		{}

	AAA_ERR	get_data(		REAL* data, INT32 real_by_node )	{ return ERR_ANY; }
	AAA_ERR	get_data_live(	REAL* data, INT32 real_by_node )	{ return ERR_ANY; }

	INT32	get_node_nb()			{ return 0; }
	INT32	get_frame_nb()			{ return 0; }
	REAL	get_frame_by_sec()		{ return .0f; }

#else //#if AAA_WIN64()
	~c_gypsy_skel();

private:
	REAL*	get_data_for_all_node( Frame* pt_frame, REAL* data, INT32 real_by_node );
public:
	AAA_ERR	read_bvh_from_filename(		C_PCHAR filename );
	AAA_ERR	read_actor_from_filename(	C_PCHAR filename );

	bool	set_live( bool in );
	void	set_refrence_position();

	AAA_ERR	get_data(		REAL* data, INT32 real_by_node );
	AAA_ERR	get_data_live(	REAL* data, INT32 real_by_node );

	INT32	get_node_nb();
	INT32	get_frame_nb();
	REAL	get_frame_by_sec();

#endif //#if AAA_WIN64()
			void	init();
	virtual	void	param_init_pt() {}

};


