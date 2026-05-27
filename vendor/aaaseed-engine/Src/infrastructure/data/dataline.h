
#ifdef AAA_DATALINE_H
#error "DATALINE_H included more than once."
#endif
#define AAA_DATALINE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef AAA_STR_UTILS_H
#	include "str_utils.h"
#endif

class c_data_elt final
{
private:
	o_str*	_str;
	bool	_b_number;
	DOUBLE	_d;
public:
	c_data_elt();
	~c_data_elt();

	void	clear();

	FINLINE	bool	is_number()	CONST	{	return _b_number;	}
	FINLINE	bool	has_data()	CONST	{	return _b_number || ( _str && !_str->is_empty() ); }
	FINLINE	FP32	get_fp32() CONST
					{
						if( _b_number )
							return FP32(_d);
						return FP32(0);
					}
	FINLINE	DOUBLE	get_double() CONST
					{
						if( _b_number )
							return _d;
						return .0;
					}
	FINLINE	o_str CONST &	get_str() CONST
					{
						return _str ? *_str : o_str::o_empty;
					}
	FINLINE	CHAR	get_char( INT32 CONST index ) CONST
					{
						if( _str )
							return _str->get_char( index );
						return	0;
					}
	FINLINE	void	set_double( DOUBLE CONST d )
					{
						_b_number = true;
						_d = d;
						if( _str )
							_str->erase();
					}
	FINLINE	void	set_str( C_PCHAR_C str )
			{
				_b_number = false;
				if( _str == nullptr )
					//		DBG_PRINT_STRING( "c_data_elt::set_str()");
					_str = new o_str( str );
				else
					_str->set( str );
				_d = 0.;
			}

	FINLINE	void	set_str_and_double(	C_PCHAR_C str,	DOUBLE CONST d	)
			{
				_b_number = true;
				if( _str == nullptr )
					//		DBG_PRINT_STRING( "c_data_elt::set_str()");
					_str = new o_str( str );
				else
					_str->set( str );
				_d = d;
			}
};

class c_data_line final
{
protected:
	typedef	std::deque<c_data_elt*>		cont_elt;
	typedef	cont_elt::iterator			iter_elt;
	cont_elt							_elt;
	INT32								_size;

private:
	FINLINE	void			grow( INT32 size )
							{	//todo faster
								do 
								{
									++_size;
									_elt.push_back( nullptr );
								}
								while( _size < size );
							}

	FINLINE	c_data_elt*		get_elt_pt( INT32 CONST index ) CONST
							{
								if( index < _size )
									return _elt[index];
								return nullptr;
							}
public:
	FINLINE	c_data_elt**	get_elt_hd( INT32 CONST  index )
							{
								if( _size <= index )
									grow( index+1 );
								return &_elt[index];
							}
	FINLINE	INT32			get_size()	CONST	{	return _size;	}

public:
					c_data_line();
					~c_data_line();

			void	clear();


	FINLINE	bool	is_number( INT32 CONST index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						if( pt )
							return pt->is_number(); 
						return false;
					}
	FINLINE	bool	has_data( INT32 CONST index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						if( !pt )
							return false;
						return pt->has_data(); 
					}
	FINLINE	bool	has_data() CONST
					{
						INT32	size	=	_size;
						if( size == 0 )
							return false;
						for( INT32 x = 0; x < size; ++x )
						{
							c_data_elt* elt = _elt[x];
							if( elt && elt->has_data() )
								return true;
						}
						return false;
					}
	FINLINE	FP32	get_fp32( INT32 CONST index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						if( pt )
							return pt->get_fp32();
						return 0;
					}
	FINLINE	DOUBLE	get_double( INT32 CONST index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						if( pt )
							return pt->get_double(); 
						return 0;
					}
	FINLINE	o_str CONST & get_str( INT32 CONST index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						return pt ? pt->get_str() : o_str::o_empty;
					}
	FINLINE	CHAR	get_char( INT32 CONST index, INT32 CONST char_index ) CONST
					{
						c_data_elt*	pt = get_elt_pt( index );
						if( pt )
							return pt->get_char( char_index );
						return 0;
					}

			void	set_double(			INT32 CONST index, DOUBLE CONST in );
			void	set_str(			INT32 CONST index, C_PCHAR_C in );
			void	set_str_and_double(	INT32 CONST index, C_PCHAR_C str,	DOUBLE CONST d	); 
	private:
			void	insert(			INT32 CONST index);
	public:
			void	insert_double(	INT32 CONST index, DOUBLE CONST in );
			void	insert_str(		INT32 CONST index, C_PCHAR_C in );
};

