#ifdef AAA_NET_REQUESTS_LUA_H
#error "NET_REQUESTS_LUA_H included more than once."
#endif
#define AAA_NET_REQUESTS_LUA_H 1


struct lua_State;
namespace aaalua
{
	namespace n_net_requests
	{
		extern void register_net_requests(lua_State* L);
	}
}