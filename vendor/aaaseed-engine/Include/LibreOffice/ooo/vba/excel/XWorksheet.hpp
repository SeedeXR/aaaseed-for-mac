#ifndef INCLUDED_OOO_VBA_EXCEL_XWORKSHEET_HPP
#define INCLUDED_OOO_VBA_EXCEL_XWORKSHEET_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XWorksheet.hdl"

#include "com/sun/star/container/XNamed.hpp"
#include "com/sun/star/lang/XUnoTunnel.hpp"
#include "com/sun/star/script/XInvocation.hpp"
#include "ooo/vba/XHelperInterface.hpp"
#include "ooo/vba/excel/XOutline.hpp"
#include "ooo/vba/excel/XPageSetup.hpp"
#include "ooo/vba/excel/XRange.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XWorksheet const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[4];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XHelperInterface > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XInvocation > >::get().getTypeLibType();
        aSuperTypes[2] = ::cppu::UnoType< const ::css::uno::Reference< ::css::container::XNamed > >::get().getTypeLibType();
        aSuperTypes[3] = ::cppu::UnoType< const ::css::uno::Reference< ::css::lang::XUnoTunnel > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XWorksheet", 4, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XWorksheet > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XWorksheet > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XWorksheet::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XWorksheet >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XWORKSHEET_HPP
