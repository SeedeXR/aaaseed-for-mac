

#ifdef AAA_WRAP_LOADER_H
#error "WRAP_LOADER_H included more than once."
#endif
#define AAA_WRAP_LOADER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

#define AAA_LIB_USE_POCO_TO_LOAD_DLL()	1

#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
#	ifndef Foundation_SharedLibrary_INCLUDED
#		include "Poco/SharedLibrary.h"
#	endif
#else
	class c_library_shared
		/// The SharedLibrary class dynamically
		/// loads shared libraries at run-time.
	{
	//	std::string _path;
		//HANDLE
		void*		_handle;

	public:

		c_library_shared();
	//	c_library_shared(const std::string& path);
			/// Creates a SharedLibrary object and loads a library
			/// from the given path.

	//	SharedLibrary(const std::string& path, int flags);
			/// Creates a SharedLibrary object and loads a library
			/// from the given path, using the given flags.
			/// See the Flags enumeration for valid values.

		virtual ~c_library_shared();
			/// Destroys the SharedLibrary. The actual library
			/// remains loaded.

		bool load( C_PCHAR_C path);
			/// Loads a shared library from the given path.
			/// Throws a LibraryAlreadyLoadedException if
			/// a library has already been loaded.
			/// Throws a LibraryLoadException if the library
			/// cannot be loaded.

	//	void load(const std::string& path, int flags);
			/// Loads a shared library from the given path,
			/// using the given flags. See the Flags enumeration
			/// for valid values.
			/// Throws a LibraryAlreadyLoadedException if
			/// a library has already been loaded.
			/// Throws a LibraryLoadException if the library
			/// cannot be loaded.

		void unload();
			/// Unloads a shared library.

		bool isLoaded() const;
			/// Returns true if a library has been loaded.

	//	bool hasSymbol(const std::string& name);
			/// Returns true iff the loaded library contains
			/// a symbol with the given name.

		void* get_symbol( C_PCHAR_C name );
			/// Returns the address of the symbol with
			/// the given name. For functions, this
			/// is the entry point of the function.
			/// Throws a NotFoundException if the symbol
			/// does not exist.

	//	const std::string& get_path() const;
			/// Returns the path of the library, as
			/// specified in a call to load() or the
			/// constructor.

	//	static std::string suffix();
			/// Returns the platform-specific filename suffix
			/// for shared libraries (including the period).
			/// In debug mode, the suffix also includes a
			/// "d" to specify the debug version of a library
			/// (e.g., "d.so", "d.dll") unless the library has
			/// been compiled with -DPOCO_NO_SHARED_LIBRARY_DEBUG_SUFFIX.

	//	static bool setSearchPath(const std::string& path);
			/// Adds the given path to the list of paths shared libraries
			/// are searched in.
			///
			/// Returns true if the path was set, otherwise false.
			///
			/// Currently only supported on Windows, where it calls
			/// SetDllDirectory(). On all other platforms, does not
			/// do anything and returns false.

	private:
		c_library_shared( const c_library_shared& );
		c_library_shared& operator= ( const c_library_shared& );
	};
#endif

#ifndef AAA_ERR_H
#	include "err.h"
#endif

class c_dll_linker : public c_obj
{
protected:
	bool	_b_loaded;
public:
	c_dll_linker();
	virtual ~c_dll_linker();

	virtual	bool	init();
	virtual	bool	deinit();
			bool	is_loaded()		CONST			{ return _b_loaded;	}
protected:
//	std::string		_dll_name;
	C_PCHAR			_dll_name;
	virtual	bool	get_pointers( bool b_load )		{ return false; }
};

#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
class c_dll_loader : public c_dll_linker, public Poco::SharedLibrary
#else
class c_dll_loader : public c_dll_linker, public c_library_shared
#endif
{
	//		std::string	_dll_name;
	UINT32	_ref_nb	{0};
public:
	static	bool	b_do_unload;
	c_dll_loader();
	virtual ~c_dll_loader();

	static	void	unload_all();
	virtual	bool	init();
	virtual	bool	deinit();

protected:
	virtual	bool	get_pointers( bool b_load )		{ return false; }
};

#define WRAP_GET_POINTERS_BEGIN()					\
	virtual	bool	get_pointers( bool b_load )		\
	{												\
		UINT32	nb_found	= 0;					\
		UINT32	nb_link		= 0;

#if AAA_DEBUG()
#	define AAA_LOCAL_DBG_PRINT( format, name ) DBG_PRINT_STRING( format, name )
#else
#	define AAA_LOCAL_DBG_PRINT( format, name )
#endif

#define WRAP_GET_LINKS_DO( _classname, _name, _type, _pars )		\
	++nb_link;														\
	_classname::_name = (DLL__##_name)&::_name;						\
	if( _classname::_name )											\
	{																\
		++nb_found;													\
		AAA_LOCAL_DBG_PRINT( "Found method %s", #_name );			\
	}																\
	else															\
		ERR_PRINT_STRING( "Could not found method %s", #_name );


#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
#	define WRAP_GET_POINTERS_DO( _name, _type, _pars )					\
		AAA_LOCAL_DBG_PRINT( "Search method %s", #_name );				\
		++nb_link;														\
		if( !b_load )													\
		{																\
			++nb_found;													\
			_name = nullptr;											\
		}																\
		else															\
		{																\
			if( hasSymbol(#_name) )										\
			{															\
				_name = (DLL__##_name)getSymbol( #_name );				\
				++nb_found;												\
				AAA_LOCAL_DBG_PRINT( "Found method %s", #_name );		\
			}															\
			else														\
				ERR_PRINT_STRING( "Could not find method %s", #_name );	\
		}
#else
#	define WRAP_GET_POINTERS_DO( _name, _type, _pars )					\
		AAA_LOCAL_DBG_PRINT( "Search method %s", #_name );				\
		++nb_link;														\
		if( !b_load )													\
		{																\
			++nb_found;													\
			_name = nullptr;											\
		}																\
		else															\
		{																\
			_name = (DLL__##_name)get_symbol( #_name );					\
			if( _name )													\
			{															\
				++nb_found;												\
				AAA_LOCAL_DBG_PRINT( "Found method %s", #_name );		\
			}															\
			else														\
				ERR_PRINT_STRING( "Could not find method %s", #_name );	\
		}
#endif

#define WRAP_GET_POINTERS_END()																			\
		if( b_load )																					\
			GOOD_PRINT_STRING( "DLL %.64s Found %d link(s) of %d", WRAP_DLL_NAME, nb_found, nb_link );	\
		else																							\
			GOOD_PRINT_STRING( "DLL %.64s Removed %d link(s) of %d", WRAP_DLL_NAME, nb_found, nb_link );\
		return nb_link == nb_found;													\
	}

