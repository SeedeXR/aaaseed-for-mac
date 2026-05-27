
#ifdef AAA_FILE_XLS_H
#error "FILE_XLS_H included more than once."
#endif
#define AAA_FILE_XLS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
//todo up`date and make it work. probably untouched since 2020 (By Franz at the time)
#if AAA_WIN64()	
#	define AAA_LIBREOFFICE_USE() 0
#else
#	define AAA_LIBREOFFICE_USE() 0
#endif

#if AAA_LIBREOFFICE_USE()

#	ifndef AAA_AAA_STR_H
#		include "aaa_str.h"
#	endif
#	ifndef AAA_DATALINE_H
#		include "infrastructure/data/dataline.h"
#	endif
#	ifndef INCLUDED_COM_SUN_STAR_FRAME_DESKTOP_HPP
#		include <com/sun/star/frame/Desktop.hpp>
#	endif
#	ifndef INCLUDED_COM_SUN_STAR_SHEET_XSPREADSHEETDOCUMENT_HPP
#		include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#	endif
#	ifndef INCLUDED_COM_SUN_STAR_SHEET_XSPREADSHEET_HPP
#		include <com/sun/star/sheet/XSpreadsheet.hpp>
#	endif

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::sheet;

class c_file_xls
{
public:
	static	AAA_ERR	init_libreoffice(); 
	static	AAA_ERR	deinit_libreoffice();
	static	void	set_hide_calc( bool CONST b_hide )	{ _b_hidden = b_hide; }
	static	bool	is_hiden()	CONST					{ return _b_hidden; }

	c_file_xls();
	~c_file_xls();

//	// a call to open should ALWAYS correspond with a call to close 
	AAA_ERR	open( o_str CONST& filename, o_str CONST& sheetname );
	void	close();

	INT32	get_sheet_count() CONST	{	return _sheet_count;	}
	INT32	get_row_count() CONST	{	return _row_count;	}
	INT32	get_col_count() CONST	{	return _col_count;	}
	void	get_data_line( INT32 CONST sheet_index, INT32 CONST row, c_data_line* dataline );
	AAA_ERR	write_int32( INT32 data, INT32 nb_x, INT32 nb_y );
	AAA_ERR	write_real( REAL data, INT32 nb_x, INT32 nb_y );
	AAA_ERR	write_str( C_PCHAR_C str, INT32 nb_x, INT32 nb_y );
	AAA_ERR	write_str( o_str CONST & str, INT32 nb_x, INT32 nb_y );

protected:
	static Reference<XComponentContext>			_x_component_context;
	static Reference<XMultiComponentFactory>	_x_service_manager;
	static Reference<XDesktop2>					_x_component_loader;
	Reference<XComponent>						_x_component;
	Reference<XSpreadsheetDocument>				_x_spreadsheet;		// ref to spreadsheet
	Reference<XSpreadsheets>					_x_sheets;			// ref to all the sheets
	Reference<XSpreadsheet>						_x_sheet_current;	// ref to the current sheet

	static bool									_b_init;
	static bool									_b_hidden;
	INT32										_sheet_count{};
	INT32										_sheet_cur_index{};
	INT32										_row_count{};
	INT32										_col_count{};
	bool										_b_opened;

	void	set_current_sheet( INT32 CONST index );
	void	set_current_sheet( o_str CONST& filename );
	void	update_cell_range();
private:
};

#endif	//#if AAA_LIBREOFFICE_USE()
