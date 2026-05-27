
#ifdef AAA_JSONHELPER_H
#error "JSONHELPER_H included more than once."
#endif
#define AAA_JSONHELPER_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "aaa_type.h"
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

#include <string>


namespace aaa::json
{
std::string	get_pretty_string( rapidjson::Document& document );
std::string	get_string( rapidjson::Document& document );

inline static std::string get_string( rapidjson::Document CONST& js, C_PCHAR_C key, std::string CONST& default_value )
{
	if( js.HasMember( key ) )
		return js[ key ].IsString() ? js[ key ].GetString() : default_value;
	return default_value;
}

inline FP32 get_fp32( rapidjson::Value CONST& js, C_PCHAR_C key, FP32 CONST default_value )
{
	if ( js.HasMember( key ) )
		return js[ key ].IsFloat() ? static_cast<float>( js[ key ].GetFloat() ) : default_value;
	return default_value;
}

inline DOUBLE get_double( rapidjson::Value CONST& js, C_PCHAR_C key, DOUBLE CONST default_value )
{
	if ( js.HasMember( key ) )
		return js[ key ].IsDouble() ? static_cast<float>( js[ key ].GetDouble() ) : default_value;
	return default_value;
}

inline INT32 get_int( rapidjson::Value CONST& js, C_PCHAR_C key, INT32 CONST default_value )
{
	if ( js.HasMember( key ) )
		return js[ key ].IsInt() ? js[ key ].GetInt() : default_value;
	return default_value;
}

inline bool get_bool( rapidjson::Value CONST& js, C_PCHAR_C key, bool CONST b_default_value )
{
	if( js.HasMember( key ) )
		return js[ key ].IsBool() ? js[ key ].GetBool() : b_default_value;
	return b_default_value;
}

inline static std::string get_string( rapidjson::Value CONST& js, C_PCHAR_C key, std::string CONST& default_value )
{
	if ( js.HasMember( key ) )
		return js[ key ].IsString() ? js[ key ].GetString() : default_value;
	return default_value;
}

/*
inline static glm::vec2 get_vec( rapidjson::Value CONST& js, C_PCHAR_C key, glm::vec2 CONST& default_value )
{
	glm::vec2 data;
	if ( js.HasMember( key ) )
	{
		const auto& js2 = js[ key ];
		data.x = get_float( js2, "x", default_value.x );
		data.y = get_float( js2, "y", default_value.y );
	}
	else
	{
		data = default_value;
	}
	return data;
}

inline static glm::vec3 get_vec( rapidjson::Value CONST& js, C_PCHAR_C key, glm::vec3 CONST& default_value )
{
	glm::vec3 data;
	if ( js.HasMember( key ) )
	{
		const auto& js2 = js[ key ];
		data.x = get_float( js2, "x", default_value.x );
		data.y = get_float( js2, "y", default_value.y );
		data.z = get_float( js2, "z", default_value.y );
	}
	else
	{
		data = default_value;
	}
	return data;
}

inline static glm::vec4 get_vec( rapidjson::Value CONST& js, C_PCHAR_C key, glm::vec4 CONST& default_value )
{
	glm::vec4 data;
	if ( js.HasMember( key ) )
	{
		const auto& js2 = js[ key ];
		data.x = get_float( js2, "x", default_value.x );
		data.y = get_float( js2, "y", default_value.y );
		data.z = get_float( js2, "z", default_value.z );
		data.w = get_float( js2, "w", default_value.w );
	}
	else
	{
		data = default_value;
	}
	return data;
}

inline static glm::vec4 get_color( rapidjson::Value CONST & js, C_PCHAR_C key, glm::vec4 CONST& default_value )
{
	glm::vec4 data;
	if ( js.HasMember( key ) )
	{
		const auto& js2 = js[ key ];
		data.x = get_float( js2, "r", default_value.x );
		data.y = get_float( js2, "g", default_value.y );
		data.z = get_float( js2, "b", default_value.z );
		data.w = get_float( js2, "a", default_value.w );
	}
	else
	{
		data = default_value;
	}
	return data;
}

*/
/*
template < typename T >
void	set_value( rapidjson::Value& js, const std::string& key, const T& value, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, value, allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const glm::vec3& val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value object( rapidjson::kObjectType );
	object.AddMember( "x", val.x, allocator );
	object.AddMember( "y", val.y, allocator );
	object.AddMember( "z", val.z, allocator );
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, object, allocator );
}

inline static void set_value( rapidjson::Value& js, const std::string& key, const glm::vec2& val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value object( rapidjson::kObjectType );
	object.AddMember( "x", val.x, allocator );
	object.AddMember( "y", val.y, allocator );
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, object, allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const glm::vec4& val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value object( rapidjson::kObjectType );
	object.AddMember( "x", val.x, allocator );
	object.AddMember( "y", val.y, allocator );
	object.AddMember( "z", val.z, allocator );
	object.AddMember( "w", val.w, allocator );
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, object, allocator );
}

inline static void	set_color( rapidjson::Value& js, const std::string& key, const glm::vec4& val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value object( rapidjson::kObjectType );
	object.AddMember( "r", val.x, allocator );
	object.AddMember( "g", val.y, allocator );
	object.AddMember( "b", val.z, allocator );
	object.AddMember( "a", val.w, allocator );
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, object, allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const int val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, static_cast<int>( val ), allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const bool val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, static_cast<bool>( val ), allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const float val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, static_cast<float>( val ), allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const double val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	js.AddMember( valKey, static_cast<double>( val ), allocator );
}

inline static void	set_value( rapidjson::Value& js, const std::string& key, const std::string& val, rapidjson::Document::AllocatorType& allocator )
{
	rapidjson::Value valKey( key.c_str(), allocator );
	rapidjson::Value valJson( val.c_str(), allocator );
	js.AddMember( valKey, valJson, allocator );
}
*/

}

