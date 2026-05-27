#ifndef INCLUDED_OOO_VBA_EXCEL_XAPPLICATION_HPP
#define INCLUDED_OOO_VBA_EXCEL_XAPPLICATION_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XApplication.hdl"

#include "com/sun/star/beans/XExactName.hpp"
#include "com/sun/star/script/XInvocation.hpp"
#include "ooo/vba/XAssistant.hpp"
#include "ooo/vba/XConnectable.hpp"
#include "ooo/vba/excel/XRange.hpp"
#include "ooo/vba/excel/XWindow.hpp"
#include "ooo/vba/excel/XWorkbook.hpp"
#include "ooo/vba/excel/XWorksheet.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XApplication const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[3];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::beans::XExactName > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XInvocation > >::get().getTypeLibType();
        aSuperTypes[2] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XConnectable > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XApplication", 3, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XApplication > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XApplication > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XApplication::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XApplication >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XAPPLICATION_HPP
