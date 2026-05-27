#ifndef INCLUDED_OOO_VBA_MSFORMS_XTOGGLEBUTTON_HPP
#define INCLUDED_OOO_VBA_MSFORMS_XTOGGLEBUTTON_HPP

#include "sal/config.h"

#include "ooo/vba/msforms/XToggleButton.hdl"

#include "ooo/vba/msforms/XCommandButton.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace msforms {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::msforms::XToggleButton const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::msforms::XCommandButton > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.msforms.XToggleButton", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::msforms::XToggleButton > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::msforms::XToggleButton > >::get();
}

::css::uno::Type const & ::ooo::vba::msforms::XToggleButton::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::msforms::XToggleButton >::get();
}

#endif // INCLUDED_OOO_VBA_MSFORMS_XTOGGLEBUTTON_HPP
