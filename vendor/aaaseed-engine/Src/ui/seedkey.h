
#ifdef AAA_SEEDKEY_H
#error "SEEDKEY_H included more than once."
#endif
#define AAA_SEEDKEY_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	c_event_keyboard;

class	c_keyboard
{
	friend	class	c_pref;
	friend	void	modifier_update();
public:
	enum	KEYBOARD_COUNTRY : INT32
	{
		KEYBOARD_FRANCE = 0,
		KEYBOARD_UK,
		KEYBOARD_MAC_FR,
		KEYBOARD_MAC_US,
		KEYBOARD_CUSTOM,
		//	KEYBOARD_SWISS,
		//	KEYBOARD_SPANISH,
		KEYBOARD_MAX_NB,
	};
	static	C_PCHAR_C			str_type[KEYBOARD_MAX_NB];

private:
	static	bool				b_verbose;
	static	KEYBOARD_COUNTRY	s_country;
	static	bool				b_numpad_flying;
	static	bool				b_numpad_flying_sliding;
	static	bool				b_alphabet_for_trax;
	static	bool				b_alphabet_for_ui;
	static	bool				b_ascii_for_trax;
	static	bool				b_key_special_for_trax;

public:
	static	INT32				key_alphabet;
	static	INT32				key_ascii;
	static	INT32				key_special;
	static	INT32				key_double_alt;

private:
			static	void	store_alphabet_for_trax(	INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y );
			static	void	store_ascii_for_trax(		INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y );
			static	void	store_key_special_for_trax( INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y );
			static	void	clear_key_alphabet();
			static	void	clear_key_ascii();
			static	void	clear_key_special();
			static	void	clear_key_double_alt();

			static	void	set_numpad_flying( bool b_in );
			static	void	flip_numpad_flying();

			static	bool	process_key_international(	INT32 key_code, INT32 modifiers, INT32 x, INT32 y );
			static	void	process_for_camera(			INT32 key_code );
			static	bool	process_special_ctrl(		INT32 key_code );

public:
			static	void	set_verbose( bool CONST in );
			static	void	flip_verbose();
	FINLINE	static	bool	is_verbose()	{ return b_verbose; }

			static	void	set_country( KEYBOARD_COUNTRY in );

			static	INT32	get_key_alphabet();
			static	INT32	get_key_ascii();
			static	INT32	get_key_special();
			static	void	clear();

			static	void	process( c_event_keyboard * ev );
			static	void	process_special( INT32 key_code, INT32	modifiers, INT32 x, INT32 y );
			static	void	process_special( c_event_keyboard* ev );
 
			static	INT32	get_country()	{ return s_country; }
};
