
#ifdef AAA_AAA_GLOBAL_H
#error "AAA_GLOBAL_H included more than once."
#endif
#define AAA_AAA_GLOBAL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif
#ifndef _STRING_
#	include <string>
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

namespace osc
{
	class	ReceivedMessage;
}

/*!
 * \class c_var_map
 * \brief c_var_map class
 * todonow
 * todoqq change names (file/class)
 * todo	regroup adapt with registry
 * todo remove unused temp stuff (while typing in dialog)
 */
template<class T>
class	c_var_map final : public c_obj
{
private:
	std::unordered_map< std::string, T>	_vars;
public:
//	c_var_map();
//	virtual ~c_var_map();

	template<class S>
	FINLINE	void	set( std::string key, S& val )
					{
						_vars[key] = val;
					}
	template<class S>
	FINLINE	bool	get( std::string key, S& val )
					{	
						const auto it = _vars.find( key );
						if( it != _vars.end() )
						{
							val = S(it->second);
							return true;
						}
						return false;
					}
			void	clear()
					{
						_vars.clear();
					}

			void	print_keys()
					{
						GOOD_PRINT_STRING( "%d keys in", _vars.size() );
						for( auto const & elt : _vars )
							GOOD_PRINT_STRING( "\t%s", elt.first.c_str() );

					}
};

namespace aaa
{
	extern	c_var_map<DOUBLE>	vars_double;
	void	osc_process_message_variable_set( CONST osc::ReceivedMessage& msg );
}
