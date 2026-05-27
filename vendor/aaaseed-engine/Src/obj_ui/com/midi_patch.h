
#ifdef AAA_MIDI_PATCH_H
#error "MIDI_PATCH_H included more than once."
#endif
#define AAA_MIDI_PATCH_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _LIST_
#	include <list>
#endif

class c_midi_patcher_in final : public c_obj
{
private:
	INT32	_data[128][16][2];
	enum	{
		CH_INDEX = 0,
		CTL_INDEX = 1
	};
public:
	c_midi_patcher_in();
	void	init();
	void	patch( INT32& ch_id, INT32& ctl_id );	//	ch are 1 to 16 controller/note 1 to 128
	AAA_ERR	load_data( C_PCHAR_C filename_in );
};

//MIDI_PATCH
class c_midi_patch
{
public:
	INT32	_ch;
	INT32	_ctl;
	INT32	_ch_dst;
	INT32	_ctl_dst;
public:
	c_midi_patch( INT32 ch_in = 0, INT32 ctl_in = 0, INT32 ch_dst_in = 0, INT32 ctl_dst_in = 0 );
	~c_midi_patch()		{};

	//	Copy constructor
	//	c_midi_patch( CONST c_midi_patch& obj )  { *this = obj; }
	//	Assignment operator
	//	c_midi_patch& operator=( CONST c_midi_patch& obj );

	FINLINE	bool	is_src(		INT32 CONST ch_in,	INT32 CONST ctl_in	)	{	return _ch == ch_in && _ctl == ctl_in;	}
	FINLINE	bool	is_dst(		INT32 CONST ch_in,	INT32 CONST ctl_in	)	{	return _ch_dst == ch_in && _ctl_dst == ctl_in;	}
	FINLINE	bool	is_using(	INT32 CONST ch_in,	INT32 CONST ctl_in	)	{	return (_ch == ch_in && _ctl == ctl_in) || (_ch_dst == ch_in && _ctl_dst == ctl_in);	}
	FINLINE	void	get_src(	INT32& ch_out,		INT32& ctl_out	)		{	ch_out = _ch;		ctl_out = _ctl;		}	
	FINLINE	void	get_dst(	INT32& ch_out,		INT32& ctl_out	)		{	ch_out = _ch_dst;	ctl_out = _ctl_dst;	}	

	CHAR*	print( CHAR* str )	CONST;
	bool	scan( CHAR* str );
};

//MIDI_PATCHS
//todo deal with copy and then derive of c_obj
class c_midi_patchs
{
protected:
	std::list<c_midi_patch>				_patches;
	std::list<c_midi_patch>::iterator	_it;
public:
//	c_midi_patchs();
//	~c_midi_patchs()	{};

	void			add(				c_midi_patch CONST & patch );
	void			add(				INT32 CONST ch, INT32 CONST ctl_index, INT32 CONST ch_dst, INT32 CONST ctl_index_dst );
	bool			remove(				INT32 CONST ch, INT32 CONST ctl_index );

	void			begin_find();
	c_midi_patch*	find(				INT32 CONST ch, INT32 CONST ctl_index, INT32 CONST ch_dst, INT32 CONST ctl_index_dst );
	c_midi_patch*	find_using(			INT32 CONST ch, INT32 CONST ctl_index );
	bool			find_and_get_other(	INT32 CONST ch, INT32 CONST ctl_index, INT32& ch_out, INT32& ctl_index_out );
	void			print();

	void			save(	FILE* file );
	void			add(	CHAR* str );

	//	Assignment operator
	//	c_midi_patchs& operator=( CONST c_midi_patchs& obj );
};


extern	void	midi_trig_auto_patch();
extern	void	midi_trig_auto_depatch();
extern	void	midi_update_auto_patch( INT32 channel_id, INT32 control_id );

