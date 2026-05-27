
#include "JsonHelper.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/writer.h>

namespace aaa::json
{
std::string	get_pretty_string( rapidjson::Document& document )
{
	rapidjson::StringBuffer strbuf;
	strbuf.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer( strbuf );
	writer.SetIndent( ' ', 2 );
	if ( document.Accept( writer ) == false )
	{
		//ERR_PRINT_STRING("Error Prettying json");
	}
	const std::string str = std::string( strbuf.GetString(), strbuf.GetSize() );
	return str;
}

std::string	get_string( rapidjson::Document& document )
{
	rapidjson::StringBuffer strbuf;
	strbuf.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer( strbuf );
	if ( document.Accept( writer ) == false )
	{
		//ERR_PRINT_STRING("Error stringify json");
	}
	const std::string str = std::string( strbuf.GetString(), strbuf.GetSize() );
	return str;
}
}