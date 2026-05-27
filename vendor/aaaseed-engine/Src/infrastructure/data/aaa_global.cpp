#include "aaa_global.h"
#include "obj_ui/com/net.h"
#include "obj_ui/com/osc/OscPrintReceivedElements.h"


namespace aaa
{

//todo deal with strings
//todo deal with 3d
//todo it with c_registry ?
//todo	keep type in struct ?
c_var_map<DOUBLE>	vars_double;

void	osc_process_message_variable_set( CONST osc::ReceivedMessage& msg )
{
	osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
	osc::ReceivedMessageArgumentIterator it_end = msg.ArgumentsEnd();
	bool b_verbose = net->is_osc_variable_set_verbose();
	o_str& o = o_str::push_name();
	if( it != it_end )
	{
		o.set( it->AsString() );
		if( ++it != it_end )
		{
			DOUBLE val = it->get_as_double();
			if( b_verbose )
				VERBOSE_PRINT_STRING( "OSC receive variable set %.480s = %f", o.get(), val );
			if( !o.is_empty() )
				vars_double.set( o.get(), val );
		}
	}
	o_str::pop_name();
}

}	//namespace aaa