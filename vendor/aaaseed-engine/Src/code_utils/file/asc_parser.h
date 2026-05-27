
#ifdef AAA_ASC_PARSER_H
#error "ASC_PARSER_H included more than once."
#endif
#define AAA_ASC_PARSER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

/*!
 * \class c_dir_parser
 * \brief Directory Parser class
 * todo clean and extend
 * be careful this is destructive on the string
 */
class c_asc_parser
{
protected:
	CHAR*	_cur;
	CHAR*	_word;
	bool	_b_word;

			bool	search_word_begin();
	virtual	bool	search_word_end();
public:

	c_asc_parser()		{};
	~c_asc_parser()		{};

	void	set_start( CHAR* start );
	CHAR*	get_next_word();
	bool	get_next_int(		INT32&	i );
	bool	get_next_double(	DOUBLE&	r );

	CHAR*	get_word()	CONST;	
	CHAR*	get_rest()	CONST;
	bool	is_word()	CONST;
};

class c_asc_parser_cstring : public c_asc_parser
{
	virtual	bool	search_word_end();
};

