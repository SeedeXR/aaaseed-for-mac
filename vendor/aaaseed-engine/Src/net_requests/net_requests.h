#ifdef AAA_NET_REQUESTS_H
#error "NET_REQUESTS_H included more than once."
#endif
#define AAA_NET_REQUESTS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif


#ifndef CPR_CPR_H
#	include <cpr/cpr.h>
#endif
#if !defined(RAPIDJSON_RAPIDJSON_H_)
#	undef GetObject	// window include replace it with GetObjectW
#	include <rapidjson/rapidjson.h>
#	undef GetObject	// window include replace it with GetObjectW
#endif
#if !defined(RAPIDJSON_DOCUMENT_H_)
#	undef GetObject	// window include replace it with GetObjectW
#	include <rapidjson/document.h>
#	undef GetObject	// window include replace it with GetObjectW
#endif


class c_lua_state;

namespace aaa
{
	class net_requests
	{
	enum class AuthMode { NONE, BASIC, DIGEST, NTLM, OAUTH };
	private:
		static UINT32 request_id;
		static AuthMode s_auth_mode;
		static bool b_auth_enabled;
		static std::string username;
		static std::string password;
		static std::string token;
		static cpr::Authentication get_user_pw_auth_parameters();
	public:
		static std::map<UINT32, cpr::AsyncResponse> request_map;
		static UINT32 request_get(		C_PCHAR_C url	);
		static UINT32 request_post(		C_PCHAR_C url,  C_PCHAR_C payload, C_PCHAR_C header = nullptr );
		static UINT32 request_put(		C_PCHAR_C url,  C_PCHAR_C payload, C_PCHAR_C header = nullptr );
		static UINT32 request_patch(	C_PCHAR_C url,  C_PCHAR_C payload, C_PCHAR_C header = nullptr );
		static UINT32 request_delete(	C_PCHAR_C url	);
		static UINT32 request_download(	C_PCHAR_C url,  C_PCHAR_C path);
		static void set_basic_auth(		C_PCHAR_C user, C_PCHAR_C pw);
		static void set_digest_auth(	C_PCHAR_C user, C_PCHAR_C pw);
		static void set_ntlm_auth(		C_PCHAR_C user, C_PCHAR_C pw);
		static void set_oauth(			C_PCHAR_C bearer_token);
		static void clear_auth();
		static UINT32 enable_auth();
		static void disable_auth();
		static cpr::Response get_request_result_if_done(UINT32 CONST id);
		static bool request_is_done(					UINT32 CONST id);
		static void parse_json_to_table( c_lua_state* l, rapidjson::GenericObject<false, rapidjson::Value> obj );
		static void parse_json_to_table( c_lua_state* l, rapidjson::GenericArray <false, rapidjson::Value> obj );
	public:
		static void c_init();
		static void c_deinit();
	};
}