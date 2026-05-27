
#ifdef AAA_TRAXS_H
#error "TRAXS_H included more than once."
#endif
#define AAA_TRAXS_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_OBJ_UI_ARRAY_H
#	include "infrastructure/obj/obj_ui_array.h"
#endif

class	c_param;
class	c_trax;

class	c_traxs final : public c_obj_ui
{
	FACTORY_DECLARE(c_traxs,c_obj_ui);
public:
	static	bool	gb_update;
	static	INT32	g_channel_offset;
	static	INT32	g_control_offset;
private:
	c_obj_ui_array_pt<c_trax>	_array_pt;
	INT32						_channel_offset;	
	INT32						_control_offset;

	FINLINE bool	check_trax_index( CHAR* str, INT32 index );
public:

//constructor linked
			void	set_nb( INT32 nb);
	virtual	void	alloc( INT32 nb); 
	virtual	void	dealloc();
			void	trax_free_all();
	virtual	void	cell_draw_obj( REAL CONST size );

	virtual	void	update( INT32 channel_offset, INT32 control_offset );
	virtual	void	draw();

	virtual	void	param_init_pt();
	virtual void	prepare_for_ui();

	FINLINE	INT32	get_channel_offset()	{	return _channel_offset; }
	FINLINE	INT32	get_control_offset()	{	return _control_offset; }

			bool	swap_trax( INT32 id_src, INT32 id_dst );

private:
			c_trax*	get_trax_always( INT32 index);
			c_trax*	load_one_from_existing_file( INT32 index, o_str CONST & filename );
public:
			c_trax*	get_trax( INT32 index );

			c_trax*	trax_plug_out( c_obj_ui* obj, p_param param );
			c_trax*	trax_plug_out( INT32 index, c_obj_ui* obj, p_param param );
			c_trax*	trax_plug_in( INT32 index, c_obj_ui* obj, p_param param );

			void	start_loop( bool CONST b_record_in );
			void	stop_loop();
			void	start_preroll();

			void	make_slave(); 
			void	disconnect_all();

	virtual	AAA_ERR	load_do_before(	o_str CONST & filename_in );	
	virtual	AAA_ERR	load_do_after(	o_str CONST & filename_in );	
	virtual	AAA_ERR	save_do_before(	o_str CONST & filename_in );
	virtual	AAA_ERR	save_do_after(	o_str CONST & filename_in );
};

extern	void	traxs_init();
extern	void	traxs_deinit();
extern	void	traxs_before();
extern	void	traxs_after();

