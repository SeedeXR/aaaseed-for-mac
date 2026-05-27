#ifndef INCLUDED_OOO_VBA_XCOMMANDBARCONTROLS_HPP
#define INCLUDED_OOO_VBA_XCOMMANDBARCONTROLS_HPP

#include "sal/config.h"

#include "ooo/vba/XCommandBarControls.hdl"

#include "ooo/vba/XCollection.hpp"
#include "ooo/vba/XCommandBarControl.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XCommandBarControls const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XCollection > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XCommandBarControls", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XCommandBarControls > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XCommandBarControls > >::get();
}

::css::uno::Type const & ::ooo::vba::XCommandBarControls::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XCommandBarControls >::get();
}

#endif // INCLUDED_OOO_VBA_XCOMMANDBARCONTROLS_HPP
