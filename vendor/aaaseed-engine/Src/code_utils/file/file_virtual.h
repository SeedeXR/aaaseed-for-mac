
#ifdef AAA_FILE_VIRTUAL_H
#error "FILE_VIRTUAL_H included more than once."
#endif
#define AAA_FILE_VIRTUAL_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

#define AAA_FILE_VIRTUAL_ORDERED() 0
#if AAA_FILE_VIRTUAL_ORDERED()
#	ifndef _MAP_
#		include <map>
#	endif
#else
#	ifndef _UNORDERED_MAP_
#		include <unordered_map>
#	endif
#endif
 
class c_factory_base;

class c_file_virtual final : public c_obj
{
public:
	typedef struct
	{
		UINT8*	data;
		UINT32	size;
	} st_vfile;
private:
#if AAA_FILE_VIRTUAL_ORDERED()
	struct	less
	{
	public: 
		CONST	bool	operator() ( C_PCHAR a, C_PCHAR b )	CONST
			{	return bool( strcmp( a, b ) < 0 );	}
	};
public:
	typedef std::map< C_PCHAR_C, st_vfile, less > V_FILE_MAP;
#else	//#if ORDERED
	struct	equal
	{
	public: 
		CONST	bool	operator() ( C_PCHAR a, C_PCHAR b )	CONST
			{	return bool( strcmp( a, b ) == 0 );	}
	};

	struct hash{
		//BKDR hash algorithm
		int operator() ( C_PCHAR str ) CONST
		{
			int CONST seed = 131;	//	31 131 1313 13131131313 etc
			int hash = 0;
			while( *str )
			{
				hash = hash * seed + *str;
				++str;
			}
			return hash & 0x7FFFFFFF;
		}
	};
public:
	typedef std::unordered_map< C_PCHAR_C, st_vfile, hash, equal > V_FILE_MAP;
#endif	//#if ORDERED
private:
	static bool		b_active;
public:
	static bool		b_filename_relative;	
	static bool		b_reading;
	static o_str	dirname;
	static FILE*	file_cur;

	static	 c_factory_base ** get_bdd_factories( INT32 CONST index );

	static	FINLINE	void	set_active( bool CONST in )	{	b_active = in; }
	static	FINLINE	bool	is_active()					{	return b_active; }

	static st_vfile CONST & get_data( C_PCHAR_C key );
	static void	open_vfile_save( o_str CONST & filename, C_PCHAR_C mode );
	static void	open_vfile_load( o_str CONST & filename, C_PCHAR_C mode );

	static void	close_vfile();
	static void	push_vfile();
	static void	pop_vfile();
};

