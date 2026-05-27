#ifdef AAA_DLL_VERSION_H
#error "DLL_VERSION_H included more than once."
#endif
#define AAA_DLL_VERSION_H 1


#ifndef _INC_SHLWAPI
#	include "shlwapi.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

struct TRANSLATIONS {
	WORD m_wLangID;
	WORD m_wCharSet;
};

class c_module_details
{
	DWORD	get_error()				CONST { return _error; }
	void init();

public :
	c_module_details();
	~c_module_details();

	bool set( HANDLE CONST process_id, HMODULE CONST module_id );

//todo rename aaa
	VS_FIXEDFILEINFO GetVersion();
	C_PCHAR_C	GetComments()			CONST { return _o_comments.get();			}
	C_PCHAR_C	GetCompanyName()		CONST { return _o_company_name.get();		}
	C_PCHAR_C	GetFileDescription()	CONST { return _o_file_description.get();	}
	C_PCHAR_C	GetFileVersion()		CONST { return _o_file_version.get();		}
	C_PCHAR_C	GetInternalName()		CONST { return _o_internal_name.get();		}
	C_PCHAR_C	GetProductName()		CONST { return _o_product_name.get();		}
	C_PCHAR_C	GetProductVersion()		CONST { return _o_product_version.get();	}
	C_PCHAR_C	GetLegalCopyright()		CONST { return _o_legal_copyright.get();	}
	C_PCHAR_C	GetLegalTrademarks()	CONST { return _o_legal_trademarks.get();	}
	C_PCHAR_C	GetOriginalFilename()	CONST { return _o_original_filename.get();	}
	C_PCHAR_C	GetPrivateBuild()		CONST { return _o_private_build.get();		}
	C_PCHAR_C	GetSpecialBuild()		CONST { return _o_special_build.get();		}
	C_PCHAR_C	GetFullFileName()		CONST { return _o_full_file_name.get();		}
	C_PCHAR_C	GetModuleName()			CONST { return _o_module_name.get();		}
	long		GetFileSize()			CONST { return _file_size;					}

 private:
	//c_module_details( c_module_details &rCopy );
	void	GetVersionString( o_str& return_str, wchar_t CONST * CONST pVersionInfo, wchar_t CONST * CONST pKey );

	HANDLE		_h_process;
	HMODULE		_h_module;
	DWORD		_error;

	// File Data
	long		_file_size;

	// Language Block Information
	WORD		_lang_id;
	WORD		_char_set;

	o_str		_o_module_type;
	o_str		_o_module_name;
	o_str		_o_full_file_name;
	o_str		_o_base_name;

	// Version Data
	o_str		_o_comments;
	o_str		_o_company_name;
	o_str		_o_file_description;
	o_str		_o_file_version;
	o_str		_o_internal_name;
	o_str		_o_legal_copyright;
	o_str		_o_legal_trademarks;
	o_str		_o_original_filename;
	o_str		_o_private_build;
	o_str		_o_product_name;
	o_str		_o_product_version;
	o_str		_o_special_build;

};

namespace aaa
{
	extern void	print_dll_version();
}