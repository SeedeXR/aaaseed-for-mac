#ifndef INCLUDED_OOO_VBA_MSFORMS_XOVAL_HPP
#define INCLUDED_OOO_VBA_MSFORMS_XOVAL_HPP

#include "sal/config.h"

#include "ooo/vba/msforms/XOval.hdl"

#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace msforms {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::msforms::XOval const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.msforms.XOval", 0, 0 );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::msforms::XOval > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::msforms::XOval > >::get();
}

::css::uno::Type const & ::ooo::vba::msforms::XOval::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::msforms::XOval >::get();
}

#endif // INCLUDED_OOO_VBA_MSFORMS_XOVAL_HPP
