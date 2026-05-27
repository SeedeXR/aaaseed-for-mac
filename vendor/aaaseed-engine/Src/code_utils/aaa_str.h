


#ifdef AAA_AAA_STR_H
#error "AAA_STR_H included more than once."
#endif
#define AAA_AAA_STR_H 1


#ifndef AAA_STR_UTILS_H
#	include "str_utils.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _STRING_
#	include <string>
#endif

class	o_str final
{
public:
	typedef CHAR char_t;

public:
	static o_str	CONST o_empty;
	static UINT32*	get_nb_pt();
	static UINT32*	get_nb_created_pt();

	// we have a stack to avoid dynamic allocation
	static void		dealloc_name();
	static o_str&	push_name(					 );
	static o_str&	push_name( o_str CONST & src );
	static o_str&	push_name( C_PCHAR_C     src );
	static void		pop_name();
	static INT32	get_stack_index();

protected:
	CONSTEXPR static INT32 LINE_NB_UNDEFINED = -1;
private:
	CHAR*	_data;	//first position make it more easy for debugging with visual
	UINT32	_len;
	INT32	_line_nb;
	UINT32	_size_allocated;

			void		sprintf_at( INT32 index, C_PCHAR_C format, ... );
	FINLINE	UINT32		count_char( C_PCHAR_C data )	CONST
						{
							if( data )
							{
								C_PCHAR pt = data - 1;
								while( *++pt );
								return (INT32)(pt - data);
							}
							return 0;
						}
	FINLINE	UINT32		count_line( C_PCHAR_C data )	CONST
						{
							if( data )
							{
								C_PCHAR	pt = data - 1;
								INT32	line_nb = 1;	//todo default to 1 ?
								while( *++pt )
								{
									if( *pt == 0xa )
										++line_nb;
								}
								return line_nb;
							}
							return 0;
						}
	FINLINE	void		count( C_PCHAR_C data, INT32& len, INT32& line_nb )	CONST
						{
							if( data )
							{
								C_PCHAR	pt = data - 1;
								line_nb = 1;	//todo default to 1 ?
								while( *++pt )
								{
									if( *pt == 0xa )
										++line_nb;
								}
								len = (INT32)(pt - data);
							}
							else
							{
								line_nb = 0;
								len = 0;
							}
						}
			void		__alloc_low( INT32 size_in );
			void		get_fname_pure_common( C_PCHAR& start, INT32& len ) CONST;
public:
	FINLINE	void		alloc( UINT32 size_in )
						{
							//	we allocate by block of 16 with a +1 minimum for the trailing /0 char
							size_in = (size_in + 16) & ~0xf;
							if( size_in > _size_allocated )
								__alloc_low( size_in );
						}
	FINLINE	void		__set_len_low( UINT32 len )
						{
							_len = len;
							_line_nb = LINE_NB_UNDEFINED;
						}
	FINLINE	void		__set_len_terminate( UINT32 len )
						{
							__set_len_low( len );
							*(_data+_len) = 0x0;
						}
	FINLINE	void		erase()
						{
							_len = 0;
							_line_nb = 0;
							if( _data )
								*_data = 0;
						}

			void		set( C_PCHAR_C				str_in	);
			void		set( C_PWCHAR_C				str_in	);
	FINLINE	void		sync()
						{
							__set_len_low( count_char( _data ) );
						}
			void		set( C_PCHAR_C				src,	INT32 CONST len,	INT32 CONST line_nb = LINE_NB_UNDEFINED );
			void		set( o_str CONST * CONST	o		);

			void		set( UINT32 CONST			in		);
			void		set( INT32 CONST			in		);
			void		set( REAL CONST				in		);

			void		add( C_PWCHAR_C				str_in	);
			void		add( C_PCHAR_C				str		,INT32 len );
			void		add( C_PCHAR_C				str_in	);
			void		add( o_str CONST&			o		);
			void		add( o_str CONST * CONST	o		);
			void		add( UINT32	CONST			in		,INT32 CONST len );
			void		add( UINT32	CONST			in		);
			void		add( INT32					in		);
			void		add_fp32_2decimals(		FP32 CONST in	);
			void		add_fp32_10_4decimals(	FP32 CONST in	);
			void		add( REAL	CONST			in		);
			void		add_v2( REAL CONST *		in		);
			void		add_v3( REAL CONST *		in		);
			void		add( std::string CONST&		str_in	);

			void		add_int16(	INT16			val		);
			void		add_uint16(	UINT16			val		);

			void		keep( UINT32 CONST			size	);
			void 		strip( C_PCHAR				c		);

			void		convert_from_to( CHAR CONST from, CHAR CONST to );
			void		convert_line_feed_to_carriage_return();
			void		convert_carriage_return_line_feed();
			void		convert_to_backslash();
			void		convert_to_slash();

			void		convert_to_lowercase();
			void		convert_to_uppercase();

			void		remove_control_at_end();
			bool		remove_control();
			bool		remove_control_and_space();
			bool		remove_char( CHAR CONST to_remove );
			bool		remove_char_at_end( CHAR CONST to_remove );

			AAA_ERR		read_file(	C_PCHAR_C		filename );
	FINLINE AAA_ERR		read_file(	o_str CONST &	filename )			{ return 	read_file( filename.get() ); }
			AAA_ERR		write_file(	C_PCHAR_C		filename )	CONST; 
	FINLINE AAA_ERR		write_file(	o_str CONST &	filename )	CONST	{ return 	write_file( filename.get() ); }

//	CONSTEXPR	C_PCHAR_C	get_data()							CONST	{ return _data; }
	CONSTEXPR	C_PCHAR_C	get()								CONST	{ return _data ? _data : ""; }
	FINLINE	C_PCHAR_C	get( INT32 index )	CONST
						{
							if( index >= 0 )
							{
								if( UINT32(index) >= _len )
									return nullptr;
							}
							else
							{
								index += _len;
								if( index < 0 )
									return nullptr;
							}
							return _data + index;
						}
	FINLINE	C_PCHAR_C	get( INT32 index, INT32 CONST nb )	CONST
						{
							if( index >= 0 )
							{
								if( UINT32(index) >= _len )
									return nullptr;
							}
							else
							{
								index += _len;
								if( index < 0 )
									return nullptr;
							}
							if( UINT32(index + nb) > _len )
								return nullptr;
							return _data + index;
						}
	FINLINE	CHAR*		get_changeable()					CONST	{ return _data; }
	FINLINE	CHAR*		get_changeable( INT32 CONST index )	CONST
						{
							if( index >= 0 )
							{
								if( UINT32(index) >= _len )
									return nullptr;
								return _data + index;
							}
							INT32 i = _len + index;
							if( i < 0 )
								return nullptr;
							return _data + i;
						}
	FINLINE	CHAR*		get_changeable( INT32 index, INT32 CONST nb )	CONST
						{
							if( index >= 0 )
							{
								if( UINT32(index) >= _len )
									return nullptr;
							}
							else
							{
								index += _len;
								if( index < 0 )
									return nullptr;
							}
							if( UINT32(index + nb) > _len )
								return nullptr;
							return _data + index;
						}
	FINLINE	C_PCHAR_C	get_if_not_empty()				CONST		{ return (_data && *_data) ? _data : nullptr; }

	FINLINE C_PCHAR_C	get_end()						CONST		{ return _data ? _data + _len : ""; }	//todo secure this access
	FINLINE	INT32		get_len()						CONST		{ return _len; }
	FINLINE	INT32		get_size_allocated()			CONST		{ return _size_allocated; }
			INT32		get_line_nb();
	FINLINE	CHAR		get_char( INT32 CONST index )	CONST
						{
							CHAR* pt = get_changeable( index );
							return pt ? *pt : 0;
						}
			void		add_char( CONST CHAR c );
	FINLINE void		add_space()							{ add_char(' '); }
	FINLINE void		add_newline()						{ add_char('\n'); }
			void		add_slash();
			void		set_char( CONST CHAR c );
	FINLINE	AAA_ERR		set_char( INT32 CONST index, CONST CHAR c )
						{
							CHAR* pt = get_changeable( index );
							if( pt )
							{
								*pt = c;
								_line_nb = LINE_NB_UNDEFINED;	//todo opt
								return AAA_OK;
							}
							return ERR_OUT_OF_BOUND;
						}

	FINLINE	AAA_ERR		drop_at(	 INT32 index )
						{
							CHAR* pt = get_changeable( index );
							if( pt )
							{
								*pt = 0;
								_line_nb = - 1;	//todo opt
								_len = (INT32)(pt - _data);
								return AAA_OK;
							}
							return ERR_OUT_OF_BOUND;
						}
			AAA_ERR		set_digits( INT32 CONST index, INT32 digit_nb, INT32 val );

	FINLINE	bool		is_empty()						CONST		{ return _len == 0; }


// EQUAL
	//todo handles empty cases
	FINLINE	bool		is_str_equal(				C_PCHAR_C str )	CONST
						{
							if( !str )
								return _len == 0;	//beton
							if( _len == 0 )
								return *str == 0;	//beton
							return strcmp( str, _data ) == 0;
						}
	FINLINE	bool		is_str_equal(				C_PCHAR_C str,	INT32 len ) CONST
						{
							if( len != _len )
								return false;
							return is_str_equal( str );
						}
	FINLINE	bool		is_equal(					o_str CONST & o )	CONST
						{
						//	if( _len != obj._len )
						//		return false;
						//		return strcmp( _data, obj._data ) == 0;
							INT32	len = _len;
							if( len != o._len )
								return false;
							if( len == 0 )
								return o._len == 0;	//beton
							return strcmp( o._data, _data ) == 0;	//beton
						//	if( (o._len) != _len )
						//		return false;
						//	return is_equal( o._data );
						}
	FINLINE	bool		is_equal_nocase(			C_PCHAR_C str )	CONST
						{
							if( !str )
								return _len == 0;	//beton
							if( _len == 0 )
								return *str == 0;	//beton
							return str_is_equal_bothnocase( str, _data );
						}
	FINLINE	bool		is_equal_nocase(			C_PCHAR_C str,	UINT32 len ) CONST
						{
							if( len != _len )
								return false;
							return is_equal_nocase( str );
						}
// BEGIN
	FINLINE	bool		is_starting_with(			C_PCHAR_C str,	UINT32 len ) CONST
						{
							if( len > _len )
								return false;
							return strncmp( str, _data, len ) == 0;	//beton
						}
	FINLINE	bool		is_starting_with(			o_str& o ) CONST
						{
							if( o._len > _len )
								return false;
							return strncmp( o._data, _data, o._len ) == 0;	//beton
						}
	FINLINE	bool		is_starting_with_nocase(	C_PCHAR_C str,	UINT32 len ) CONST
						{
							if( len > _len )
								return false;
							return str_is_equal_bothnocase( str, _data, len );
						}
	FINLINE	bool		is_starting_with_nocase(	o_str& o ) CONST
						{
							if( o._len > _len )
								return false;
							return str_is_equal_bothnocase( o._data, _data, o._len );
						}
	FINLINE	bool		is_beginning_of_nocase(		C_PCHAR_C str ) CONST
						{
							return str_is_equal_bothnocase( str, _data, _len );
						}
// END
	FINLINE	bool		is_ending_with(				CHAR CONST c ) CONST
						{
							return _len > 0 && c == *(_data + _len - 1);
						}
	FINLINE	bool		is_ending_with(				C_PCHAR_C str,	UINT32 len ) CONST
						{
							if( len > _len )
								return false;
							return strncmp( str, _data+_len-len, len ) == 0;	//beton
						}
	FINLINE	bool		is_ending_with_nocase(		C_PCHAR_C str,	UINT32 len ) CONST
						{
							if( len > _len )
								return false;
							return str_is_equal_bothnocase( str, _data+_len-len, len );
						}
	FINLINE	bool		is_ending_with(				C_PCHAR_C str ) CONST
						{
							return is_ending_with( str, UINT32(strlen(str)) );
						}
	FINLINE	bool		is_ending_with(				o_str CONST & o ) CONST
						{
							if( o._len > _len )
								return false;
							return strncmp( o._data, _data+_len-o._len, o._len ) == 0;	//beton
						}
	//			void	change_backslash_to_slash();
	void				split( std::vector<o_str>& dst, CONST CHAR	separator );
	void				split( std::vector<o_str>& dst, C_PCHAR_C	separator );

//direct access should be for friends only or done better
	FINLINE	void		alloc_to_add( INT32 size )
						{
							if( size > 0 )
							{
								size += get_len();
								alloc( size );
							}
						}
			void		inc_internal( INT32 CONST len );

// FILENAME
	//todo there is a problem with path beginning by . like ./dir/file
			C_PCHAR_C	get_ext()	CONST;
			bool		have_ext()	CONST ;
			void		drop_ext();
			void		add_ext(					C_PCHAR_C		ext		);
			void		add_ext(					o_str CONST &	ext		);
			void		replace_ext(				C_PCHAR_C		ext		);

			void		set_dir_name(				C_PCHAR_C		fname	);
			void		set_dir_name(				o_str CONST &	fname	);
			void		add_dir_name(				C_PCHAR_C		fname	);


			C_PCHAR_C	get_fname()		CONST;
			void		set_fname(					o_str CONST &	fname	);

			void		set_fname_pure(				C_PCHAR_C		fname	);
			void		set_fname_pure(				o_str CONST &	fname	);

			void		set_fname_relative(			C_PCHAR_C		fname,	o_str CONST & dir );
			void		set_fname_relative_ext_no(	C_PCHAR_C		fname,	o_str CONST & dir );
			void		set_fname_relative(			C_PCHAR_C		fname	);
			void		set_fname_relative_ext_no(	C_PCHAR_C		fname	);

	FINLINE	void		set_fname_relative(			o_str CONST &	fname,	o_str CONST & dir )	{ return set_fname_relative( fname.get(), dir ); }
	FINLINE	void		set_fname_relative_ext_no(	o_str CONST &	fname,	o_str CONST & dir )	{ return set_fname_relative_ext_no( fname.get(), dir ); }
	FINLINE	void		set_fname_relative(			o_str CONST &	fname	)					{ return set_fname_relative( fname.get() ); }
	FINLINE	void		set_fname_relative_ext_no(	o_str CONST &	fname	)					{ return set_fname_relative_ext_no( fname.get() ); }

			void		set_fname_absolute(			C_PCHAR_C		fname,	C_PCHAR_C dir );
			bool		is_fname_absolute() CONST;	

			void		drop_fname();
			void		replace_fname(				C_PCHAR_C		fname	);
			void		replace_fname(				o_str CONST &	fname	);

			void		expand_fname(				o_str CONST &src	);
			void		expand_fname(				C_PCHAR_C src		);
			void		compact_fname(				o_str CONST &src	);
			void		compact_fname(				C_PCHAR_C src		);
								
	FINLINE	void		set( o_str&& o ) NOEXCEPT
						{
							if( this != &o )
							{
								free_data();
								_data				= o._data;
								_size_allocated		= o._size_allocated;
								_len				= o._len;
								_line_nb			= o._line_nb;

								o._data				= nullptr;
								o._size_allocated	= 0;
								o._len				= 0;
								o._line_nb			= 0;
							}
						}
	FINLINE	void		set( o_str CONST & o )
						{
							set( o._data, o._len, o._line_nb );
						}
	FINLINE	o_str&		operator=( o_str&& o ) NOEXCEPT
						{
							if( this != &o )
								set( static_cast<o_str&&>( o ) );
							return *this;
						}
	~o_str();
	o_str();

	o_str( C_PCHAR_C pt, UINT32 len );
	o_str( C_PCHAR_C pt );
	o_str( UINT32 size_in );

//	copy constructor
	o_str( o_str CONST & obj );
	o_str( o_str&& o ) NOEXCEPT;
	//	copy assignment operator
	o_str&	operator=(	o_str	CONST & obj ) NOEXCEPT;

//	o_str&	operator=( CONST std::string& str_in );
//	o_str&	operator=( C_PCHAR_C str_in );
	void	operator=(	std::string CONST & str_in );
	void	operator=(	C_PCHAR_C str_in );

	void	free_data() NOEXCEPT;

	bool	operator==(	o_str	CONST & obj )	CONST	{	return is_equal( obj );			}
	bool	operator==(	C_PCHAR_C str_in )		CONST	{	return is_str_equal( str_in );	}
	bool	operator!=(	o_str	CONST & obj )	CONST	{	return !this->operator==(obj);	}

	o_str&	operator+=(	C_PCHAR_C str_in );
	o_str&	operator+=(	wchar_t CONST * CONST str_in );
	o_str&	operator+=(	o_str	CONST & o );
	o_str&	operator+=(	o_str	CONST * CONST o );
	o_str&	operator+=(	CHAR	CONST	c );
	o_str&	operator+=(	UINT32	CONST in );
	o_str&	operator+=(	INT32	CONST in );

	std::string		to_string() CONST
	{
		return _data ? std::string( _data, _len ) : std::string();
	}
};

