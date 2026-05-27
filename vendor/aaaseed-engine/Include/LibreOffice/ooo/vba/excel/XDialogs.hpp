#ifndef INCLUDED_OOO_VBA_EXCEL_XDIALOGS_HPP
#define INCLUDED_OOO_VBA_EXCEL_XDIALOGS_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XDialogs.hdl"

#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XDialogs const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XDialogs", 0, 0 );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XDialogs > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XDialogs > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XDialogs::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XDialogs >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XDIALOGS_HPP
