#ifndef INCLUDED_OOO_VBA_EXCEL_XVPAGEBREAK_HPP
#define INCLUDED_OOO_VBA_EXCEL_XVPAGEBREAK_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XVPageBreak.hdl"

#include "ooo/vba/excel/XPageBreak.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XVPageBreak const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::excel::XPageBreak > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XVPageBreak", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XVPageBreak > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XVPageBreak > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XVPageBreak::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XVPageBreak >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XVPAGEBREAK_HPP
