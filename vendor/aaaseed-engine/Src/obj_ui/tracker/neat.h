
#ifdef AAA_NEAT_H
#error "NEAT_H included more than once."
#endif
#define AAA_NEAT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_NEAT()
#define	COOKING_OUT_AVERAGER	FALSE

#ifndef AAA_TRACKER_H
#	include "tracker.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class	c_neat final : public c_tracker
{
	FACTORY_DECLARE(c_neat,c_tracker);
friend void	WINAPI	neat_marker_get(DWORD error_code, DWORD nb_byte, LPOVERLAPPED over);
private:
#ifdef	WIN32
//todo should disappear with good tracker and com work
//todoq	serial should be out of here in a specific file and object
#if	COOKING_OUT_AVERAGER
	REAL	last[9];
#endif
	INT32		recu[9];
	INT32		port_nb;
	UINT8		buf[256];
	DWORD		read_nb;	
	DCB			dcb;
	HANDLE		hCom;
	DWORD		dwError;
	INT32		nb;
	INT32		nb_recu;
#endif
	INT32		trame;

	INT32		tng_type;
	UINT32		tng_marker;
	INT32		tng_seq_len;
	OVERLAPPED	gover;
	INT32		raw_data[8];
	friend	void	WINAPI	neat_marker_get( DWORD error_code, DWORD nb_byte, LPOVERLAPPED over );
	c_sensor*	sensor;
public:	
	typedef enum TYPE
	{
		TNG2 = 0,
		TNG3,
		TYPE_NB
	} TYPE;
	static	C_PCHAR_C	str_type[TYPE_NB];

	virtual	void	param_init_pt();
			void	set_port_nb( INT32 port_in)	{ port_nb = port_in; }
	virtual	AAA_ERR	open();
	virtual void	close();
	virtual	void	update_low();

			void	calibrate_default();

			void	set_tng_type( TYPE type_in);
	FINLINE	REAL	control_get( INT32 control_id)					{	return	sensor->control_get( control_id);	}
	FINLINE	void	control_put( INT32 control_id, REAL value_in)	{	sensor->control_put( control_id, value_in);	}
	FINLINE	void	toggle_draw_mode()								{	sensor->toggle_draw_mode();	}
};

extern	c_neat* neat1;
extern	c_neat* neat2;

extern	c_neat::TYPE	neat_tng_type_one;
extern	c_neat::TYPE	neat_tng_type_two;


extern	INT32	neat_port_channel_one;
extern	INT32	neat_port_channel_two;



extern	REAL	neat_control_get( INT32 channel_id, INT32 control_id );
extern	void	neat_control_set( INT32 channel0, INT32 control0, REAL value );
extern	void	neat_draw();
extern	void	neat_init();
extern	void	neat_update();
extern	void	neat_open();
extern	void	neat_close();
extern	void	neat_toggle_draw_mode();

extern bool	b_midi_done_with_neat;
extern bool	b_neat_done_with_midi;

#endif //#if AAA_TRACKER_NEAT()


