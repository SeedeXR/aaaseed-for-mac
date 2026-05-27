#include "net_requests.h"
#include "language/lua/aaalua_glue.h"


//2025 October in Debug version v143 still doesn't work: we get an exception (see below)

#if AAA_WIN64()
#	include "code_utils/lib_use.h"
	AAA_LIB_USE( "cpr" )
#endif


namespace aaa
{
	std::map< UINT32, cpr::AsyncResponse > net_requests::request_map;
	UINT32 net_requests::request_id		= 0;
	net_requests::AuthMode net_requests::s_auth_mode = AuthMode::NONE;
	bool net_requests::b_auth_enabled	= false;
	std::string net_requests::username	= "";
	std::string net_requests::password	= "";
	std::string net_requests::token		= "";
	
	UINT32 net_requests::request_get(C_PCHAR_C url)
	{
#if 1	//try this version to isolate problem 
		auto future = [&]()
		{
			//2025 October in Debug version v143 we get an exception here
			if( !b_auth_enabled || s_auth_mode == AuthMode::NONE )
				return cpr::GetAsync( cpr::Url{url} );
			else if( s_auth_mode == AuthMode::OAUTH )
				return cpr::GetAsync( cpr::Url{url}, cpr::Bearer(token) );
			else
				return cpr::GetAsync( cpr::Url{url}, get_user_pw_auth_parameters() );
		}();
		++request_id;
		//request_map.emplace( request_id, std::move(future) );
#	if 1
		try
		{
			request_map.emplace( request_id, std::move(future) );
		}
		catch( const std::exception& e )
		{
			ERR_PRINT_STRING( "%s() on request_id %s failed with error %s.", __FUNCTION__, request_id, e.what() );
		}
#	else
		request_map.emplace( request_id, std::move(future) );
#	endif
		return request_id;
#else
		++request_id;
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE )
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert( { request_id, cpr::GetAsync(cpr::Url{url}, cpr::Bearer(token)) } );
			else
				request_map.insert( { request_id, cpr::GetAsync(cpr::Url{url}, get_user_pw_auth_parameters()) } );
		}
		else
			request_map.insert( { request_id, cpr::GetAsync(cpr::Url{url}) } );
		return request_id;
#endif
	}

	static auto header_def = cpr::Header{ {"accept", "application/json"} };
	//static auto header_def = cpr::Header{ {"accept", "application/json"}, {"Content-Type", "application/json"} };
	// Function to trim whitespace from both ends of a string
	std::string trim( const std::string& str )
	{
		size_t first = str.find_first_not_of(" \t");
		if( std::string::npos == first )
			return "";
		size_t last = str.find_last_not_of(" \t");
		return str.substr( first, (last - first + 1) );
	}

	// Function to parse the header string into cpr::Header
	// make it better (split by comma will cause problem with propmts ?)
	cpr::Header make_header( C_PCHAR_C str_in )
	{
		if( str_in && *str_in )
		{
			cpr::Header header;
			std::string input( str_in );
    
			// Remove all curly braces first
			input.erase(std::remove( input.begin(), input.end(), '{'), input.end() );
			input.erase(std::remove( input.begin(), input.end(), '}'), input.end() );
    
			// Split by commas to get individual key-value pairs
			std::istringstream iss(input);
			std::string pair;
    
			while( std::getline( iss, pair, ',' ) )
			{
				// Split each pair by colon
				size_t colonPos = pair.find( ':' );
				if( colonPos != std::string::npos )
				{
					std::string key   = trim( pair.substr( 0, colonPos  ) );
					std::string value = trim( pair.substr( colonPos + 1 ) );
					if( !key.empty() && !value.empty() )
						header[key] = value;
				}
			}
			return header;
		}
		else
			return header_def;
	}

	UINT32 net_requests::request_post( C_PCHAR_C url, C_PCHAR_C payload, C_PCHAR_C header_str )
	{
		auto make_request = [](	const cpr::Url& cpr_url, 
								cpr::Body&& cpr_payload,  // Use `&&` to allow moving the payload
								const cpr::Header& cpr_header)
			-> cpr::AsyncResponse
			{  // Explicit return type for clarity
				if( b_auth_enabled && s_auth_mode != AuthMode::NONE )
					if( s_auth_mode == AuthMode::OAUTH )
						return cpr::PostAsync( cpr_url, std::move(cpr_payload), cpr_header, cpr::Bearer(token) );
					else
						return cpr::PostAsync( cpr_url, std::move(cpr_payload), cpr_header, get_user_pw_auth_parameters() );
				else
					return cpr::PostAsync( cpr_url, std::move(cpr_payload), cpr_header );
			};
		// Insert into the map with move semantics (avoid copying payload/header).
		// If multi-threaded, use `std::atomic<UINT32>`.
		request_map.insert( { ++request_id,  make_request(	cpr::Url{url}, 
															cpr::Body{payload},		// If `payload` is large, wrap in `std::move`.
															make_header(header_str)  // Temporary `cpr::Header` is safe here (lives long enough).
															)
							} );
		return request_id;
	}
#if 0 
	UINT32 net_requests::request_post( C_PCHAR_C url, C_PCHAR_C payload, C_PCHAR_C header_str )
	{
		cpr::Header cpr_header = make_header( header_str );
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE )
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert({ ++request_id, cpr::PostAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, cpr::Bearer(token)) });
			else 
				request_map.insert({ ++request_id, cpr::PostAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, get_user_pw_auth_parameters() ) });
		}
		else
			request_map.insert({ ++request_id, cpr::PostAsync(cpr::Url{++request_id, cpr::PostAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header ) }url}, cpr::Body{payload}, cpr_header ) });
		return request_id;
	}
#endif

	UINT32 net_requests::request_put( C_PCHAR_C url, C_PCHAR_C payload, C_PCHAR_C header_str )
	{
		cpr::Header cpr_header = make_header( header_str );
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE )
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert({ ++request_id, cpr::PutAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, cpr::Bearer(token) ) });
			else
				request_map.insert({ ++request_id, cpr::PutAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, get_user_pw_auth_parameters() ) });
		}
		else
			request_map.insert({ ++request_id, cpr::PutAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header ) });
		return request_id;
	}

	UINT32 net_requests::request_patch( C_PCHAR_C url, C_PCHAR_C payload, C_PCHAR_C header_str )
	{
		cpr::Header cpr_header = make_header( header_str );
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE)
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert({ ++request_id, cpr::PatchAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, cpr::Bearer(token)) });
			else
				request_map.insert({ ++request_id, cpr::PatchAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header, get_user_pw_auth_parameters()) });
		}
		else
			request_map.insert({ ++request_id, cpr::PatchAsync(cpr::Url{url}, cpr::Body{payload}, cpr_header ) });
		return request_id;
	}

	UINT32 net_requests::request_delete(C_PCHAR_C url)
	{
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE)
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert({ ++request_id, cpr::DeleteAsync(cpr::Url{url}, cpr::Bearer(token)) });
			else
				request_map.insert({ ++request_id, cpr::DeleteAsync(cpr::Url{url}, get_user_pw_auth_parameters()) });
		}
		else
			request_map.insert({ ++request_id, cpr::DeleteAsync(cpr::Url{url}) });		
		return request_id;
	}

	UINT32 net_requests::request_download(C_PCHAR_C url, C_PCHAR_C path)
	{
		if( b_auth_enabled && s_auth_mode != AuthMode::NONE)
		{
			if (s_auth_mode == AuthMode::OAUTH)
				request_map.insert({ ++request_id, cpr::DownloadAsync(cpr::fs::path(path), cpr::Url{url}, cpr::Bearer(token)) });
			else
				request_map.insert({ ++request_id, cpr::DownloadAsync(cpr::fs::path(path), cpr::Url{url}, get_user_pw_auth_parameters()) });
		}
		else
			request_map.insert({ ++request_id, cpr::DownloadAsync(cpr::fs::path(path), cpr::Url{url}) });
		return request_id;
	}

	cpr::Authentication net_requests::get_user_pw_auth_parameters()
	{
		switch( s_auth_mode )
		{
			case AuthMode::BASIC:
				return cpr::Authentication( username, password, cpr::AuthMode::BASIC );
			case AuthMode::DIGEST:
				return cpr::Authentication( username, password, cpr::AuthMode::DIGEST );
			case AuthMode::NTLM:
				return cpr::Authentication( username, password, cpr::AuthMode::NTLM );
			default:
				return cpr::Authentication( username, password, cpr::AuthMode::BASIC );
		}
	}

	// Sets username and password to use for Basic authentication and enables it for all requests made after this call.
	void net_requests::set_basic_auth(C_PCHAR_C user, C_PCHAR_C pw)
	{
		username = user;
		password = pw;
		s_auth_mode = AuthMode::BASIC;
		b_auth_enabled = true;
	}

	// Sets username and password to use for NTLM authentication and enables it for all requests made after this call.
	void net_requests::set_digest_auth(C_PCHAR_C user, C_PCHAR_C pw)
	{
		username = user;
		password = pw;
		s_auth_mode = AuthMode::DIGEST;
		b_auth_enabled = true;
	}

	// Sets username and password to use for NTLM authentication and enables it for all requests made after this call.
	void net_requests::set_ntlm_auth(C_PCHAR_C user, C_PCHAR_C pw)
	{	
		username = user;
		password = pw;
		s_auth_mode = AuthMode::NTLM;
		b_auth_enabled = true;
	}

	// Sets bearer token to use for OAuth authentication and enables it for all requests made after this call.
	void net_requests::set_oauth(C_PCHAR_C bearer_token)
	{
		token = bearer_token;
		s_auth_mode = AuthMode::OAUTH;
		b_auth_enabled = true;
	}

	// Clears all variables related to authentication and disables it for subsequent requests.
	void net_requests::clear_auth()
	{
		b_auth_enabled = false;
		s_auth_mode = AuthMode::NONE;
		username = "";
		password = "";
		token = "";
	}

	// Enables use of existing authentication credentials and mode. Returns 0 if no credentials have been set previously, 1 if successful.
	UINT32 net_requests::enable_auth()
	{

		if( s_auth_mode == AuthMode::NONE )
			return 0;

		b_auth_enabled = true;
		return 1;
	}

	// Disables authentication for subsequent requests while leaving credentials for future use.
	void net_requests::disable_auth()
	{
		b_auth_enabled = false;
	}

	cpr::Response net_requests::get_request_result_if_done( UINT32 CONST id )
	{
		// Check if key (id) is in the map
		if( request_map.count(id) )
		{
			// Check if request is complete
			if( request_map.at(id).wait_for( std::chrono::seconds(0)) == std::future_status::ready )
			{
				// Get the result of the request
				cpr::Response r = request_map.at(id).get();
				// Erase the completed request from the map
				request_map.erase(id);
				// Return the response
				return r;
			}
		}
		// Return a default response if the request with the given id isn't complete or doesn't exist
		return cpr::Response();
	}

	bool net_requests::request_is_done( UINT32 CONST id )
	{
		// Check if key (id) is in the map
		if( request_map.count(id) )
		{
			// Check if request is complete
			if( request_map.at(id).wait_for(std::chrono::seconds(0)) == std::future_status::ready )
				return true;
		}
		return false;
	}


	void net_requests::parse_json_to_table( c_lua_state* l, rapidjson::GenericObject<false, rapidjson::Value> obj)
	{	// For when we encounter a JSON object
		l->new_table();
		for( auto& m : obj )
		{
			const char* key = m.name.GetString();
			const rapidjson::Type type = m.value.GetType();
			switch( type )
			{
				case rapidjson::Type::kNullType:
					l->push_string(key);
					l->push_nil();
					break;
				case rapidjson::Type::kNumberType:
					//todo reorder tests by most likely used
					if(      m.value.IsUint() )
						l->set_field( key, m.value.GetUint() );
					else if( m.value.IsUint64() )
						l->set_field( key, m.value.GetUint64() );
					else if( m.value.IsInt() )
						l->set_field( key, m.value.GetInt() );
					else if( m.value.IsInt64() )
						l->set_field( key, m.value.GetInt64() );
					else if( m.value.IsDouble() )
						l->set_field( key, m.value.GetDouble() );
					break;
				case rapidjson::Type::kStringType:
					l->set_field( key, m.value.GetString() );
					break;
				case rapidjson::Type::kFalseType:
					l->set_field( key, m.value.GetBool() );
					break;
				case rapidjson::Type::kTrueType:
					l->set_field( key, m.value.GetBool() );
					break;
				case rapidjson::Type::kArrayType:
					l->push_string(key);
					parse_json_to_table( l, m.value.GetArray() );
					l->set_table(-3);
					break;
				case rapidjson::Type::kObjectType:
					l->push_string(key);
					parse_json_to_table( l, m.value.GetObject() );
					l->set_table(-3);
					break;
			};
		}
	}

	void net_requests::parse_json_to_table( c_lua_state* l, rapidjson::GenericArray<false, rapidjson::Value> obj )
	{	// For when we encounter an array in the JSON object. Needs to be handled slightly differently due to indexing for Lua table.
		l->new_table();
		INT32 idx = 1;
		for (auto& m : obj) {
			const rapidjson::Type type = m.GetType();
			switch (type)
			{
				case rapidjson::Type::kNullType:
					l->push_int(idx);
					l->push_nil();
					break;
				case rapidjson::Type::kNumberType:
					//todo reorder tests by most likely used
					if (m.IsUint())
						l->set_field(idx, m.GetUint());
					else if (m.IsUint64())
						l->set_field(idx, m.GetUint64());
					else if (m.IsInt())
						l->set_field(idx, m.GetInt());
					else if (m.IsInt64())
						l->set_field(idx, m.GetInt64());
					else if (m.IsDouble())
						l->set_field(idx, m.GetDouble());
					break;
				case rapidjson::Type::kStringType:
					l->set_field(idx, m.GetString());
					break;
				case rapidjson::Type::kFalseType:
					l->set_field(idx, m.GetBool());
					break;
				case rapidjson::Type::kTrueType:
					l->set_field(idx, m.GetBool());
					break;
				case rapidjson::Type::kArrayType:
					l->push_int(idx);
					parse_json_to_table(l, m.GetArray());
					l->set_table(-3);
					break;
				case rapidjson::Type::kObjectType:
					l->push_int(idx);
					parse_json_to_table(l, m.GetObject());
					l->set_table(-3);
					break;
			};
			idx++;
		}
	}

	void net_requests::c_init()
	{
		DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
		DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
	}

	void net_requests::c_deinit()
	{
	}
}