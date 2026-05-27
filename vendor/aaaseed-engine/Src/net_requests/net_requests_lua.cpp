#include "net_requests_lua.h"
#include "net_requests.h"
#include "language/lua/aaalua_glue.h"

namespace aaalua 
{
namespace n_net_requests
{
	AAALUACALL(get)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		C_PCHAR_C url = l.get_str();
#if AAA_DEBUG()
		DBG_PRINT_STRING( "GET URL: %s",		url );
#endif
		UINT32 id = aaa::net_requests::request_get( url );
		return l.return_uint32(id);
	}

	AAALUACALL(post)
	{
		LUAAAA_START(L, __FUNCTION__);
		INT32 arg_nb = l.get_arg_nb( 2, 3 );
		C_PCHAR_C url = l.get_str(1);
		C_PCHAR_C payload = l.get_str(2);
		C_PCHAR_C header = (arg_nb==3 && !l.is_nil(3)) ? l.get_str(3) : nullptr;
#if AAA_DEBUG()
		DBG_PRINT_STRING( "POST URL: %s",		url );
		DBG_PRINT_STRING( " with Payload: %s",	payload );
		if( header )
			DBG_PRINT_STRING( " with Header: %s",	header );
#endif
		UINT32 id = aaa::net_requests::request_post( url, payload, header );
		return l.return_uint32(id);
	}

	AAALUACALL(put)
	{
		LUAAAA_START(L, __FUNCTION__);
		INT32 arg_nb = l.get_arg_nb( 2, 3 );
		C_PCHAR_C url = l.get_str(1);
		C_PCHAR_C payload = l.get_str(2);
		C_PCHAR_C header = (arg_nb==3 && !l.is_nil(3)) ? l.get_str(3) : nullptr;
#if AAA_DEBUG()
		DBG_PRINT_STRING( "PUT URL: %s",		url );
		DBG_PRINT_STRING( " with Payload: %s",	payload );
		if( header )
			DBG_PRINT_STRING( " with Header: %s",	header );
#endif
		UINT32 id = aaa::net_requests::request_put( url, payload, header );
		return l.return_uint32(id);
	}

	AAALUACALL(patch)
	{
		LUAAAA_START(L, __FUNCTION__);
		INT32 arg_nb = l.get_arg_nb( 2, 3 );
		C_PCHAR_C url = l.get_str(1);
		C_PCHAR_C payload = l.get_str(2);
		C_PCHAR_C header = (arg_nb==3 && !l.is_nil(3)) ? l.get_str(3) : nullptr;
#if AAA_DEBUG()
		DBG_PRINT_STRING( "PATCH URL: %s",		url );
		DBG_PRINT_STRING( " with Payload: %s",	payload );
		if( header )
			DBG_PRINT_STRING( " with Header: %s",	header );
#endif
		UINT32 id = aaa::net_requests::request_patch( url, payload, header );
		return l.return_uint32(id);
	}

	AAALUACALL(fn_delete)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		C_PCHAR_C url = l.get_str();
#if AAA_DEBUG()
		DBG_PRINT_STRING( "DELETE URL: %s",		url );
#endif
		UINT32 id = aaa::net_requests::request_delete(url);
		return l.return_uint32(id);
	}

	AAALUACALL(download)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);
		C_PCHAR_C url = l.get_str(1);
		C_PCHAR_C path = l.get_str(2);
#if AAA_DEBUG()
		DBG_PRINT_STRING( "DOWNLOAD URL: %s",	url );
		DBG_PRINT_STRING( " with Path: %s",		path );
#endif
		UINT32 id = aaa::net_requests::request_download( url, path );
		return l.return_uint32(id);
	}

	AAALUACALL(set_basic_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);
		C_PCHAR_C user = l.get_str(1);
		C_PCHAR_C pw = l.get_str(2);
		aaa::net_requests::set_basic_auth(user, pw);
		return l.return_uint32(1);
	}

	AAALUACALL(set_digest_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);
		C_PCHAR_C user = l.get_str(1);
		C_PCHAR_C pw = l.get_str(2);
		aaa::net_requests::set_digest_auth(user, pw);
		return l.return_uint32(1);
	}

	AAALUACALL(set_ntlm_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);
		C_PCHAR_C user = l.get_str(1);
		C_PCHAR_C pw = l.get_str(2);
		aaa::net_requests::set_ntlm_auth(user, pw);
		return l.return_uint32(1);
	}

	AAALUACALL(set_oauth)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		C_PCHAR_C token = l.get_str(1);
		aaa::net_requests::set_oauth(token);
		return l.return_uint32(1);
	}

	AAALUACALL(clear_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		aaa::net_requests::clear_auth();
		return l.return_uint32(1);
	}

	AAALUACALL(enable_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		UINT32 res = aaa::net_requests::enable_auth();
		return l.return_uint32(res);
	}

	AAALUACALL(disable_auth)
	{
		LUAAAA_START(L, __FUNCTION__);
		aaa::net_requests::disable_auth();
		return l.return_uint32(1);
	}

	AAALUACALL(get_request_result_if_done)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		UINT32 id = l.get_uint32(1);
		cpr::Response r = aaa::net_requests::get_request_result_if_done(id);
		if( r.status_code == 0 )
		{
			return l.return_bool(false);
		}
		else
		{
			l.new_table();
			l.set_field( "status_code", r.status_code);
			l.set_field( "text",        r.text.c_str());
			l.set_field( "url",         r.url.c_str());
			l.set_field( "elapsed",     r.elapsed);
			return l.return_table();
		}
	}

	AAALUACALL(get_request_list)
	{
		LUAAAA_START(L, __FUNCTION__);
		std::map<UINT32, cpr::AsyncResponse>::iterator it = aaa::net_requests::request_map.begin();
		l.new_table();
		while( it != aaa::net_requests::request_map.end() )
		{
			l.set_field( it->first, aaa::net_requests::request_is_done(it->first) );
			++it;
		}
		return l.return_table();
	}

	AAALUACALL(json_to_table)
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		C_PCHAR_C json_str = l.get_str();
		rapidjson::Document d;
		rapidjson::ParseResult ok = d.Parse(json_str);
		if( !ok )
			return l.return_nil();
		aaa::net_requests::parse_json_to_table( &l, d.GetObject() );
		return l.return_table();
	}

	void register_net_requests( lua_State* L )
	{
		LUAAAA_START(L, __FUNCTION__);

		l.define_table("requests");

			ADD_FN(get				);
			ADD_FN(post				);
			ADD_FN(put				);
			ADD_FN(patch);
			l.add_fn_to_table( "delete", fn_delete );
			ADD_FN(download			);
			ADD_FN(set_basic_auth);
			ADD_FN(set_digest_auth);
			ADD_FN(set_ntlm_auth);
			ADD_FN(set_oauth);
			ADD_FN(clear_auth);
			ADD_FN(enable_auth);
			ADD_FN(disable_auth);
			ADD_FN(get_request_result_if_done);
			ADD_FN(get_request_list);
			ADD_FN(json_to_table);

		l.pop( 1 );	//pop new table
	}
}
}