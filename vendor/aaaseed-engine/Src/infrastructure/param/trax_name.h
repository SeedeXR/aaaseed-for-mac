
#ifdef AAA_TRAX_NAME_H
#error "TRAX_NAME_H included more than once."
#endif
#define AAA_TRAX_NAME_H 1


#ifndef _MAP_
#	ifdef _MSC_VER
#		pragma warning(disable:4786)
#	endif
#	include <map>
#endif
#ifndef _STRING_
#	ifdef _MSC_VER
#		pragma warning(disable:4786)
#	endif
#	include <string>
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
//todonow
//todoqq change names (file/class)
//todo	regroup adapt with registry
//todo remove unused temp stuff (while typing in dialog)

template<class T>
class	c_var_map final : public c_obj
{
private:
	std::map< std::string, T>	vars;
public:
//	c_var_map();
//	virtual ~c_var_map();
//	virtual	CONST char*	CONST	get_class_name()	{ return "var_map"; };

			void	set( std::string key, T& val );
	FINLINE	BOOL	get( std::string key, T& val );
			void	clear();

	void	print_keys();
};

template<class T>	 
void c_var_map<T>::set( std::string key, T& val )
{
	vars[key] = val;
}

template<class T>
FINLINE	BOOL c_var_map<T>::get( std::string key, T& val )
{
map< std::string, T >::iterator it;
	it = vars.find( key );
	if ( it != vars.end() )
	{
		val = it->second;
		return TRUE;
	}
	return FALSE;
}

template<class T>
void c_var_map<T>::print_keys()
{
map< std::string, T >::iterator it;

	GOOD_PRINT_STRING( "%d keys in", vars.size() );
	for ( it = vars.begin(); it != vars.end(); ++it )
		GOOD_PRINT_STRING( "\t%s", it->first.c_str() );
}

template<class T>
void c_var_map<T>::clear()
{
	vars.clear();
}


