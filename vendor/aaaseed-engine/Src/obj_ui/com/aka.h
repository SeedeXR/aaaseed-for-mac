
#ifdef AAA_AKA_H
#error "AKA_H included more than once."
#endif
#define AAA_AKA_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

//	aka is an iphone app which send osc 
namespace osc
{
	class	ReceivedMessage;
};

class	c_aka final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_aka, c_obj_active_ui );
private:
	static	void	c_init();
public:
	static	void	c_osc_receive( CONST osc::ReceivedMessage& msg );
	static	INT32	aka_nb;
private:
	BOOL	_b_verbose;
	INT32	_iphone_id_to_receive;
	INT32	_page;
	INT32	_sliders_but_dst_channel;
	INT32	_sliders_but_dst_control;
	INT32	_sliders_dst_channel;
	INT32	_sliders_dst_control;
	INT32	_grid_but_dst_channel;
	INT32	_grid_but_dst_control;
	INT32	_grid_dst_channel;
	INT32	_grid_dst_control;
	BOOL	_b_xy_centered;
	REAL	_xy_center_size;
	INT32	_xy_dst_channel;
	INT32	_xy_dst_control;
	INT32	_xy_but_dst_channel;
	INT32	_xy_but_dst_control;
	INT32	_matrix_but_dst_channel;
	INT32	_matrix_but_dst_control;
	INT32	_matrix_dst_channel;
	INT32	_matrix_dst_control;

public:
	static	void	verbose_set( BOOL in );
	static	void	verbose_flip();

public:
	virtual	void	param_init_pt();
	virtual	void	update();

//	virtual	AAA_ERR	save_do_after( CONST CHAR* CONST filename );
//	virtual	AAA_ERR	load_do_after( CONST CHAR* CONST filename );

			BOOL	osc_receive( CONST osc::ReceivedMessage& msg );
};

static	CONST	INT32	AKA_OBJ_MAX_NB = 26;
//#define	MIDI_OBJ_MAX_NB	26	
extern	c_aka*	aka_array[AKA_OBJ_MAX_NB];

