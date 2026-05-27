
#include "file_xls.h"

#if AAA_LIBREOFFICE_USE()
#include "aaa_util.h"
#include "err.h"
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#include <osl/file.hxx>
#include <osl/process.h>

#include <lib_use.h>
AAA_LIB_USE( "LibreOffice/isal" );
AAA_LIB_USE( "LibreOffice/icppu" );
AAA_LIB_USE( "LibreOffice/icppuhelper" );

using namespace com::sun::star::beans;
using namespace com::sun::star::table;
using namespace com::sun::star::util;

using ::rtl::OUString;



// Tutorial and info
// https://wiki.openoffice.org/wiki/Calc/API/Programming
// https://studiofreya.com/2016/10/11/integrating-libreoffice-into-cpp/
// https://studiofreya.com/2016/10/19/errors-connecting-to-libreoffice-with-cpp-errors-part-2/
// https://studiofreya.com/2016/10/21/open-libreoffice-calc-with-cpp-part-3/
// https://wiki.openoffice.org/wiki/Documentation/DevGuide/OfficeDev/Document_Events
// https://www.openoffice.org/udk/cpp/man/tutorial/unointro.html

// https://web.archive.org/web/20100110090550/http://wiki.services.openoffice.org/wiki/FR/Documentation/OpenOffice_Calc#Intercepter_un_.C3.A9v.C3.A9nement_.28.E2.80.9CEvent_Listener.E2.80.9D.29
// https://web.archive.org/web/20100716011150/http://www.oooforum.org/forum/viewtopic.phtml?t=14697%7CUse

// https://wiki.openoffice.org/wiki/Documentation/BASIC_Guide/StarDesktop
// https://erack.de/bookmarks/O.html#LibO_Scripting


#if AAA_LIBREOFFICE_USE()
Reference<XComponentContext>			c_file_xls::_x_component_context = nullptr;
Reference<XMultiComponentFactory>		c_file_xls::_x_service_manager = nullptr;
Reference<XDesktop2>					c_file_xls::_x_component_loader = nullptr;
#endif
bool									c_file_xls::_b_init = false;
bool									c_file_xls::_b_hidden = true;

//typedef ::cppu::WeakImplHelper1< ::com::sun::star::view::XSelectionChangeListener >
//SelectionChangeListenerHelper;
//
//class SelectionChangeListener : public SelectionChangeListenerHelper
//{
//public:
//	SelectionChangeListener()
//	{
//		printf( "EventListener installed\n" ); fflush( stdout );
//	}
//
//	~SelectionChangeListener()
//	{
//		printf( "EventListener released\n" ); fflush( stdout );
//	}
//
//	virtual void SAL_CALL selectionChanged( const com::sun::star::lang::EventObject& aEvent )
//		throw ( RuntimeException )
//	{
//		printf( "Selection changed \n" ); fflush( stdout );
//	}
//
//	virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& aEventObj )
//		throw( ::com::sun::star::uno::RuntimeException )
//	{
//		printf( "object listened to will be disposed\n" );
//	}
//};


//typedef ::cppu::WeakImplHelper1< XModifyListener > listenerHelper;
//
//class listener : public listenerHelper
//{
//public:
//	listener();
//	~listener()
//	{
//		printf( "EventListener released\n" );
//	}
//	// Methods
//	virtual void SAL_CALL modified( const ::css::lang::EventObject& aEvent );
//	virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& aEventObj )
//	{
//		printf( "object listened to will be disposed\n" );
//	}
//};
//
//listener::listener()
//{
//	DBG_PRINT_STRING( "listener" );
//};
//
//
//Reference<XModifyBroadcaster> messageHost;
//Reference<XModifyBroadcaster> messageHost2;
//
//void SAL_CALL listener::modified( const ::css::lang::EventObject& aEvent )
//{
//	//DBG_PRINT_STRING( "Event modified", rtl::OUStringToOString( aEvent.Source, RTL_TEXTENCODING_ISO_8859_1 ).getStr() ) );
//	auto str = aEvent.Source;
//	//str.is()
//	DBG_PRINT_STRING( "Event modified" );
//}


AAA_ERR c_file_xls::init_libreoffice()
{
	AAA_ERR	retcode = ERR_ANY;
	if( !_b_init )
	{
		try
		{
			// Get the remote office component context
			_x_component_context = Reference<XComponentContext>( ::cppu::bootstrap() );
			if( !_x_component_context.is() )
			{
				ERR_PRINT_STRING( "Libreoffice : No component context!" );
			}
			else
			{
				// get the remote office service manager
				_x_service_manager = Reference<XMultiComponentFactory>( _x_component_context->getServiceManager() );
				if( !_x_service_manager.is() )
				{
					ERR_PRINT_STRING( "Libreoffice : No service manager!" );
				}
				else
				{
					// get an instance of the remote office desktop UNO service
					// and query the XComponentLoader interface
					_x_component_loader = Desktop::create( _x_component_context );
					_b_init = true;
					retcode = AAA_OK;
				}
			}
		}
		catch( ::cppu::BootstrapException& e )
		{
			ERR_PRINT_STRING( "Caught BootstrapException: %s", OUStringToOString( e.getMessage(), RTL_TEXTENCODING_UTF8 ).getStr() );
		}
		catch( Exception& e )
		{
			ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
		}
	}
	return retcode;
}

AAA_ERR c_file_xls::deinit_libreoffice()
{
	AAA_ERR	retcode = ERR_ANY;
	try
	{
		if( _x_component_loader.is() )
		{
			_x_component_loader->dispose();
			_x_component_loader = nullptr;
		}
		if( _x_service_manager.is() )
		{
			_x_service_manager = nullptr;
		}
		if( _x_component_context.is() )
		{
			_x_component_context = nullptr;
		}
	}
	catch( Exception& e )
	{
		ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
	}
	_b_init = false;
	return retcode;
}

c_file_xls::c_file_xls()
:_b_opened {false}
{
}

c_file_xls::~c_file_xls()
{
	close();
}

AAA_ERR		c_file_xls::open( o_str CONST& filename, o_str CONST& sheetname )
{
	AAA_ERR	retcode = ERR_ANY;
	if( !_b_init )
	{
		retcode = init_libreoffice();
		if( retcode != AAA_OK )
			return retcode;
	}
	if( !_b_opened )
	{
		try
		{
			//Global Const acFormatPDF = writer_pdf_Export
			//Global Const acFormatODT = writer8
			//Global Const acFormatDOC = MS Word 97
			//Global Const acFormatHTML = HTML
			//Global Const acFormatODS = calc8
			//Global Const acFormatXLS = MS Excel 97
			//Global Const acFormatXLSX = Calc MS Excel 2007 XML
			//Global Const acFormatTXT = Text - txt - csv( StarCalc )
			OUString filter_name;
			auto CONST ext = filename.get_ext();
			if( str_is_equal_nocase( ext, "ods" ) )
			{
				filter_name = OUString::createFromAscii( "calc8" );
			}
			else if( str_is_equal_nocase( ext, "xls" ) || str_is_equal_nocase( ext, "xlsx" ) )
			{
				filter_name = OUString::createFromAscii( "Calc MS Excel 2007 XML" );
			}
			else
			{
				ERR_PRINT_STRING( "Libreoffice Unsupported format : %s", ext );
				return retcode;
			}
			Sequence < PropertyValue > args1( 2 );
			args1[ 0 ].Name = OUString::createFromAscii( "Hidden" );
			args1[ 0 ].Value = ::css::uno::Any( _b_hidden );
			args1[ 1 ].Name = OUString::createFromAscii( "FilterName" );
			args1[ 1 ].Value = ::css::uno::Any( filter_name );

			// Transform path to fileURL
			OUString absolute_doc_url, working_dir, doc_path_url;
			osl_getProcessWorkingDir( &working_dir.pData );
			osl::FileBase::getFileURLFromSystemPath( OUString::createFromAscii( filename.get() ), doc_path_url );
			osl::FileBase::getAbsoluteFileURL( working_dir, doc_path_url, absolute_doc_url );

		//	auto strPath = OUString::createFromAscii( "file:///C:/FranzDev/AAADev/AAAGaBuZo/GaBuData/APP_CUR/APP_LV_FP/AAA/Powow_1/Kinect.ods" );
		//	auto CONST file_path = OUString::createFromAscii( filename.get() );

			// open a spreadsheet document
			_x_component = Reference<XComponent>( _x_component_loader->loadComponentFromURL(
															absolute_doc_url,
															OUString( "_blank" ), 0,
															args1 )
												);
			// to create a spreadsheet
			//xComponent = Reference<XComponent>( xComponentLoader->loadComponentFromURL(
			//	OUString( "private:factory/scalc" ),
			//	OUString( "_blank" ), 0,
			//	args1 ) );
			if( !_x_component.is() )
			{
				ERR_PRINT_STRING( "Libreoffice : opening spreadsheet document failed!" );
				return ERR_ANY;
			}
			// Reference to the document
			_x_spreadsheet = Reference <XSpreadsheetDocument>( _x_component, UNO_QUERY );
			if( !_x_spreadsheet.is() )
			{
				ERR_PRINT_STRING( "Libreoffice : opening spreadsheet document failed!" );
				return ERR_ANY;
			}
			// Get the sheets
			_x_sheets = _x_spreadsheet->getSheets();
			if( !_x_sheets.is() )
			{
				ERR_PRINT_STRING( "Libreoffice : couldn't get a ref to the sheets" );
			}
			_b_opened = true;
			retcode = AAA_OK;
			set_current_sheet( sheetname );

		}
		catch( Exception& e )
		{
			ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
			//return 1;
		}
	}
	return retcode;
}

void		c_file_xls::close()
{
	try
	{
		if( _b_opened )
		{
			_x_sheet_current = nullptr;
			_x_sheets = nullptr;
			_x_spreadsheet = nullptr;

			//Close the document
			_x_component->dispose();
			_x_component = nullptr;
			_b_opened = false;
		}
	}
	catch( Exception& e )
	{
		ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
	}
}

void		c_file_xls::set_current_sheet( INT32 CONST index )
{
	try
	{
		if( _b_opened )
		{
			auto CONST sheet_names = _x_sheets->getElementNames();
			_sheet_count = sheet_names.getLength();
			_sheet_cur_index = index < sheet_names.getLength() ? index : 0;
			_x_sheet_current = Reference<XSpreadsheet>( _x_sheets->getByName( sheet_names[ _sheet_cur_index ] ), UNO_QUERY );
			if( _x_sheet_current.is() )
			{
				update_cell_range();
			}
			else
			{
				ERR_PRINT_STRING( "Libreoffice : couldn't get a ref to the sheet" );
			}
		}
	}
	catch( Exception& e )
	{
		ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
	}
}

void		c_file_xls::set_current_sheet( o_str CONST& name )
{
	try
	{
		if( _b_opened )
		{
			_sheet_cur_index = 0;
			auto CONST sheet_names = _x_sheets->getElementNames();
			_sheet_count = sheet_names.getLength();
			auto CONST sheet_name_str = OUString::createFromAscii( name.get() );
			if( sheet_names.getLength() > 1 && name.get_len() != 0 )
			{
				for( INT32 i = 0; i < sheet_names.getLength(); ++i )
				{
					if( sheet_name_str == sheet_names[ i ] )
					{
						_sheet_cur_index = i;
						continue;
					}
				}
			}
			_x_sheet_current = Reference<XSpreadsheet>( _x_sheets->getByName( sheet_names[ _sheet_cur_index ] ), UNO_QUERY );
			if( _x_sheet_current.is() )
			{
				update_cell_range();
			}
			else
			{
				ERR_PRINT_STRING( "Libreoffice : couldn't get a ref to the sheet" );
			}
		}
	}
	catch( Exception& e )
	{
		ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
	}
}

void		c_file_xls::update_cell_range()
{
	try
	{
		if( _b_opened && _x_sheet_current.is() )
		{
			Reference<XSheetCellCursor> sheet_cell_cursor = _x_sheet_current->createCursor();
			Reference<XUsedAreaCursor> used_area_cursor( sheet_cell_cursor, UNO_QUERY );
			used_area_cursor->gotoEndOfUsedArea( false );

			Reference<XCellRangeAddressable> cell_range_addressable( used_area_cursor, UNO_QUERY );
			CellRangeAddress cell_range_address = cell_range_addressable->getRangeAddress();

			_row_count = cell_range_address.EndRow + 1;
			_col_count = cell_range_address.EndColumn + 1;
		//	DBG_PRINT_STRING( "sheet : %d, start column : %d start row: %d ...\n", cell_range_address.Sheet, _start_col, _start_row );
			DBG_PRINT_STRING( "Sheet : %d, %d columns x %d row", cell_range_address.Sheet, _col_count, _row_count );
		}
	}
	catch( Exception& e )
	{
		ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
	}
}

void	c_file_xls::get_data_line( INT32 CONST sheet_index, INT32 CONST row, c_data_line* dataline )
{
	if( _b_opened )
	{
		if( _sheet_cur_index != sheet_index )
		{
			set_current_sheet( sheet_index );
		}
		if( INSIDE_MIN_MAX( row, 0, _row_count - 1 ) && dataline )
		{
			try
			{
				if( _x_sheet_current.is() )
				{
					for( INT32 i = 0; i < _col_count; ++i )
					{
						Reference<XCell> x_cell = _x_sheet_current->getCellByPosition( i, row );
						if( x_cell.is() )
						{
							switch( x_cell->getType() )
							{
							case CellContentType::CellContentType_EMPTY:
								{
									dataline->insert_str( i, "" );
									break;
								}
							case CellContentType::CellContentType_VALUE:
								{
									auto CONST val = x_cell->getValue();
									dataline->insert_double( i, val );
									break;
								}
							case CellContentType::CellContentType_TEXT:
								{
									auto CONST tmp = x_cell->getFormula();
									// todo : conversion is not properly done might need a code page conversion
									rtl::OString CONST o = OUStringToOString( tmp, RTL_TEXTENCODING_UTF8 );
									dataline->insert_str( i, o.pData->buffer );
									break;
								}
							case CellContentType::CellContentType_FORMULA:
								{
									//auto tmp = xCell->getFormula();
									auto CONST val = x_cell->getValue();
									dataline->insert_double( i, val );
									break;
								}
							}
						}
					}
				}
			}
			catch( Exception& e )
			{
				ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
			}
		}
	}
}

AAA_ERR c_file_xls::write_int32( INT32 data, INT32 nb_x, INT32 nb_y )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_opened )
	{
		try
		{
			if( _x_sheet_current.is() )
			{
				Reference<XCell> x_cell = _x_sheet_current->getCellByPosition( nb_x, nb_y );
				if( x_cell.is() )
				{
					x_cell->setValue( data );
					retcode = AAA_OK;
				}
				else
				{
					ERR_PRINT_STRING( "%s() Couldn't get cell (%d,%d)", __FUNCTION__, nb_x, nb_y );
				}
			}
		}
		catch( Exception& e )
		{
			ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
		}
	}
	return retcode;
}

AAA_ERR c_file_xls::write_real( REAL data, INT32 nb_x, INT32 nb_y )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_opened )
	{
		try
		{
			if( _x_sheet_current.is() )
			{
				Reference<XCell> x_cell = _x_sheet_current->getCellByPosition( nb_x, nb_y );
				if( x_cell.is() )
				{
					x_cell->setValue( data );
					retcode = AAA_OK;
				}
				else
				{
					ERR_PRINT_STRING( "%s() Couldn't get cell (%d,%d)", __FUNCTION__, nb_x, nb_y );
				}
			}
		}
		catch( Exception& e )
		{
			ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
		}
	}
	return retcode;
}

AAA_ERR c_file_xls::write_str( C_PCHAR_C str, INT32 nb_x, INT32 nb_y )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_opened )
	{
		try
		{
			if( _x_sheet_current.is() )
			{
				Reference<XCell> x_cell = _x_sheet_current->getCellByPosition( nb_x, nb_y );
				if( x_cell.is() )
				{

					auto CONST formula = OUString::fromUtf8( rtl::OString( str, strlen( str ) ) );
					x_cell->setFormula( formula );
					retcode = AAA_OK;
				}
				else
				{
					ERR_PRINT_STRING( "%s() Couldn't get cell (%d,%d)", __FUNCTION__, nb_x, nb_y );
				}
			}
		}
		catch ( Exception& e)
		{
			ERR_PRINT_STRING( "%s() Caught UNO exception: %s", __FUNCTION__, OUStringToOString( e.Message, RTL_TEXTENCODING_UTF8 ).getStr() );
		}
	}
	return retcode;
}

AAA_ERR c_file_xls::write_str( o_str CONST& str, INT32 nb_x, INT32 nb_y )
{
	return write_str( str.get(), nb_x, nb_y );
}

#endif	//#if AAA_LIBREOFFICE_USE()

