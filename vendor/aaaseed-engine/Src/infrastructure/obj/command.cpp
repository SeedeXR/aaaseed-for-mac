#include "infrastructure/obj/command.h"
#include "obj_ui/com/net.h"
#include "file/asc_parser.h"

void	command_parse( c_asc_parser* parser, CHAR*& str, bool& b_str )
{
	CHAR*	p;
	str = (CHAR*)parser->get_next_word();
	if( !str)
		return;
	if( *str == '\"' )
	{	//	this a string
		++str;
		b_str = true;
	}
	else
		b_str = false;
	p = str + strlen(str)-1;
	if( *p == ',' || *p == ';' )
		--p;
	if( *p == ')' )
		--p;
	if( *p == '\"' )
		--p;
	*++p = 0;
}

void	command_do( C_PCHAR_C str )
{
	c_asc_parser	parser;

	parser.set_start( (CHAR*) str );	//	cast because parser change temporary char value
	DBG_PRINT_STRING( "COMMAND : %s", str );
	if( parser.get_next_word() )
	{
		if( str_is_equal( parser.get_word(), "obj_find(" ) )
		{	
			CHAR*		filename;
			bool		b_str_filename;
			c_obj_ui*	pobj;

			// first we find an object
			//	if( sscanf(  str, "set_param_name_v0( \"%s\", \"%s\", %s);", filename, param_name, param_value) == 3 )
			command_parse( &parser, filename, b_str_filename );
			if( !filename )
				return;
			if( b_str_filename )
				pobj = c_obj_ui::find_from_top_by_name_search( filename );
			else
				pobj = c_obj_ui::get_from_id( atol(filename) );

			if( pobj )
			{	
				CHAR* cmd = (CHAR*)parser.get_next_word();
				if( str_is_equal( cmd,")->set_param(" ) )
				{	//	we search now for a param and a value
					CHAR*	param_name;
					CHAR*	param_value;
					bool	b_str;
					
					command_parse( &parser, param_name, b_str );
					if( !param_name || *param_name==0 )
						return;
					command_parse( &parser, param_value, b_str );
					if( !param_value || *param_value==0 )
						return;

					//	and change the param
					pobj->set_param_value( param_name, param_value, b_str );
				}
				else if( str_is_equal( cmd, ")->do_command(" ) )
				{	//	we take the command to do on the object
					cmd = parser.get_rest();
//					if( str_is_equal( cmd, ")->", 3) )
					{
//						cmd += 3;
						pobj->do_command( cmd );
					}
//					if( str_is_equal( (CHAR*) parser.get_next_word(),")->do_command(") )
				}
			}
		}
		else
		{
			debug_break( "%s() dont understand : %s", __FUNCTION__, parser.get_word() );
		}
	}
}

void	command_send( C_PCHAR_C str )
{
	net->sendto( net->_s_remote_link_dst, 0, c_net::BLK_COMMAND_V0, (UINT8*)str, (INT32)strlen( str ) + 1 );
}

void	command_send( c_obj_ui* pobj, C_PCHAR_C str_in )
{
	if( net->is_remote_send() )
	{
		if( pobj )
		{
			CHAR	str[2048];
			if ( pobj->is_my_filename() )
				sprintf( str, "obj_find( \"%s\" )->%s;", pobj->get_name_search_str(), str_in );
			else
				sprintf( str, "obj_find( %u )->%s;", pobj->get_obj_ui_id(), str_in );
			VERBOSE_PRINT_STRING( str );
			command_send( str );
		}
	}
}

