
#ifdef AAA_MIDI_LUA_H
#error "MIDI_LUA_H included more than once."
#endif
#define AAA_MIDI_LUA_H 1


namespace aaalua
{
	namespace n_midi
	{
		extern void	register_midi( lua_State* L );
	}
}


