
#ifdef AAA_FLATLAND_LUA_H
#error "FLATLAND_LUA_H included more than once."
#endif
#define AAA_FLATLAND_LUA_H 1


namespace aaalua
{
	namespace n_flatland
	{
		extern void	register_flatland( lua_State* L );
	}
}
