#ifndef INCLUDED_OOO_VBA_EXCEL_XBORDERS_HPP
#define INCLUDED_OOO_VBA_EXCEL_XBORDERS_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XBorders.hdl"

#include "ooo/vba/XCollection.hpp"
#include "ooo/vba/excel/XBorder.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XBorders const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[2];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::excel::XBorder > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XCollection > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XBorders", 2, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XBorders > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XBorders > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XBorders::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XBorders >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XBORDERS_HPP
