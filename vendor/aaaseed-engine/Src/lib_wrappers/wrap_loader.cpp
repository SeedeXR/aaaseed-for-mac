//#define WRAP_API_H "wrap_loader_api_Macro.h"

#include "wrap_loader.h"
#include <vector>
#include "system/shared/SystemUtils.h"

//todo check if it is better recompiling Poco with v145 toolset
#	if	AAA_VSTOOL() >= 143
#		define AAA_LOCAL_LIB_VSTOOL_STR()  "v143"
#	else
#		define AAA_LOCAL_LIB_VSTOOL_STR()  AAA_LIB_VSTOOL_STR()
#	endif	//AAA_VSTOOL() >= 143

#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
#	ifndef Foundation_Exception_INCLUDED
#		include "Poco/Exception.h"
#	endif
#	include <lib_use.h>
	AAA_LIB_USE_D( "Poco/PocoFoundationmd_"AAA_LOCAL_LIB_VSTOOL_STR() ) 
#else
// POCO used for events base class
#	include <lib_use.h>
	AAA_LIB_USE_D( "Poco/PocoFoundationmd_"AAA_LOCAL_LIB_VSTOOL_STR() ) 
//	#include <winbase.h>
#endif	//AAA_LIB_USE_POCO_TO_LOAD_DLL


//AAA_LIB_USE32( "IPHLPAPI" )

c_dll_linker::c_dll_linker()
	:_b_loaded(false)
{
}

c_dll_linker::~c_dll_linker()
{
}

bool	c_dll_linker::init()
{
	if( !_b_loaded )
	{
		_b_loaded = get_pointers( true );
	}
	return _b_loaded;
}

bool	c_dll_linker::deinit()
{
	return true;
}



#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
#else
	c_library_shared::c_library_shared()
	{
		_handle = 0;
	}
	c_library_shared::~c_library_shared()
	{
	}


	bool c_library_shared::load( C_PCHAR_C path )	//, int /*flags*/)
	{
	//	FastMutex::ScopedLock lock(_mutex);

	//	if (_handle) throw LibraryAlreadyLoadedException(_path);
	//	DWORD flags(0);
	//#if !defined(_WIN32_WCE)
	//	Path p(path);
	//	if (p.isAbsolute()) flags |= LOAD_WITH_ALTERED_SEARCH_PATH;
	//#endif
	//	std::wstring upath;
	//	UnicodeConverter::toUTF16(path, upath);
		_handle = LoadLibraryA( path );
		if( !_handle )
		{
			ERR_PRINT_STRING( "LoadLibrary error : %s", aaa::system::get_err_message().c_str() );
			return false;
		}
	//	_path = path;
		return true;
	}


	void c_library_shared::unload()
	{
	//	FastMutex::ScopedLock lock(_mutex);

		if( _handle )
		{
			FreeLibrary( (HMODULE) _handle );
			_handle = 0;
		}
	//	_path.clear();
	}


	bool c_library_shared::isLoaded() const
	{
		return _handle != 0;
	}


	void* c_library_shared::get_symbol( C_PCHAR_C name )
	{
	//	FastMutex::ScopedLock lock(_mutex);

		if( _handle )
		{
	//#if defined(_WIN32_WCE)
	//		std::wstring uname;
	//		UnicodeConverter::toUTF16(name, uname);
	//		return (void*) GetProcAddressW((HMODULE) _handle, uname.c_str());
	//#else
			return (void*) GetProcAddress((HMODULE) _handle, name );
	//#endif
		}
		else
			return nullptr;
	}


	//const std::string& c_library_shared::get_path() const
	//{
	//	return _path;
	//}


	//std::string SharedLibraryImpl::suffixImpl()
	//{
	//#if defined(_DEBUG) && !defined(POCO_NO_SHARED_LIBRARY_DEBUG_SUFFIX)
	//	return "d.dll";
	//#else
	//	return ".dll";
	//#endif
	//}


	//bool SharedLibraryImpl::setSearchPathImpl(const std::string& path)
	//{
	//#if _WIN32_WINNT >= 0x0502
	//	std::wstring wpath;
	//	Poco::UnicodeConverter::toUTF16(path, wpath);
	//	return SetDllDirectoryW(wpath.c_str()) != 0;
	//#else
	//	return false;
	//#endif
	//}
	//
	//
	//} // namespace Poco
#endif	// AAA_LIB_USE_POCO_DLL()


bool	c_dll_loader::b_do_unload = true;
static	std::vector<c_dll_loader*>	loaders;

void	c_dll_loader::unload_all()
{
	for( auto const & pt : loaders )
		pt->deinit();
	loaders.clear();
}

c_dll_loader::c_dll_loader()
{
}
c_dll_loader::~c_dll_loader()
{
	deinit();
}

bool	c_dll_loader::init()
{
	if( is_loaded() )
	{
		DBG_PRINT_STRING( "Library %s() already loaded", _dll_name );
		++_ref_nb;
		return true;
	}

	_b_loaded = false;
	GOOD_PRINT_STRING( "Trying to load library %s", _dll_name );
	bool b_loaded = false;

#if AAA_LIB_USE_POCO_TO_LOAD_DLL()
	try
	{
		load( _dll_name );
		b_loaded = true;
	}

	catch( Poco::LibraryLoadException &e )
	{
		ERR_PRINT_STRING( "Error loading library %s, error %s", _dll_name, e.displayText().c_str() );
		b_loaded = false;
	}
	catch( Poco::Exception &e )
	{
		ERR_PRINT_STRING( "Other Error loading library %s, error %s", _dll_name, e.displayText().c_str() );
		b_loaded = false;
	}
#else
	b_loaded = load( _dll_name ) )
#endif	//AAA_LIB_USE_POCO_TO_LOAD_DLL()
	_b_loaded = b_loaded;

	if( b_loaded )
		loaders.push_back( this );
	else
	{
		ERR_PRINT_STRING( "Failked to load Library %s", _dll_name );
		return false;
	}

	++_ref_nb;
	GOOD_PRINT_STRING( "Library %s loaded", _dll_name );

	_b_loaded = get_pointers( true );
	if( !_b_loaded )
	{
		deinit();
		return false;
	}
	return true;
}

bool	c_dll_loader::deinit()
{
	--_ref_nb;
	if( b_do_unload && _ref_nb == 0 )
	{
		get_pointers( false );
		if( is_loaded() )
		{
			unload();
			GOOD_PRINT_STRING( "Library %s unloaded", _dll_name );
		}
		_b_loaded = false;
	}
	return true;
}
