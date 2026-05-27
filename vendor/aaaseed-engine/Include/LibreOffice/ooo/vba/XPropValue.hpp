#ifndef INCLUDED_OOO_VBA_XPROPVALUE_HPP
#define INCLUDED_OOO_VBA_XPROPVALUE_HPP

#include "sal/config.h"

#include "ooo/vba/XPropValue.hdl"

#include "com/sun/star/script/XDefaultProperty.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XPropValue const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XDefaultProperty > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XPropValue", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XPropValue > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XPropValue > >::get();
}

::css::uno::Type const & ::ooo::vba::XPropValue::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XPropValue >::get();
}

#endif // INCLUDED_OOO_VBA_XPROPVALUE_HPP
