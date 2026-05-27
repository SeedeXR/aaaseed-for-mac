
// Define function pointers
#define WRAP_LOADER( _name, _type, _pars ) typedef _type (WRAP_CONV_CALL* DLL__##_name) _pars;
#	include WRAP_API_MACRO_H
#undef WRAP_LOADER

class WRAP_CLASS_NAME :
#ifdef	USE_LINKED_LIB
	public c_dll_linker
#else
	public c_dll_loader
#endif
{
public:
	WRAP_CLASS_NAME()		{ _dll_name = WRAP_DLL_NAME; }
	// Declare functions pointers
#define WRAP_LOADER( _name, _type, _pars ) DLL__##_name _name;
#	include WRAP_API_MACRO_H
#undef WRAP_LOADER

protected:
	// Start of get_pointers function
	WRAP_GET_POINTERS_BEGIN()

#ifdef	USE_LINKED_LIB
#	define WRAP_LOADER( _name, _type, _pars )	WRAP_GET_LINKS_DO( WRAP_CLASS_NAME, _name, _type, _pars  )
#else
#	define WRAP_LOADER( _name, _type, _pars )	WRAP_GET_POINTERS_DO( _name, _type, _pars  )
#endif
// get function pointers
#	include WRAP_API_MACRO_H
#undef WRAP_LOADER

		// End of get_pointers function
		WRAP_GET_POINTERS_END()
};

#undef WRAP_API_MACRO_H
#undef WRAP_DLL_NAME
#undef USE_LINKED_LIB

