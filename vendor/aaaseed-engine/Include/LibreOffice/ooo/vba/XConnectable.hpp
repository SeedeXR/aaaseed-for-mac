#ifndef INCLUDED_OOO_VBA_XCONNECTABLE_HPP
#define INCLUDED_OOO_VBA_XCONNECTABLE_HPP

#include "sal/config.h"

#include "ooo/vba/XConnectable.hdl"

#include "ooo/vba/TypeAndIID.hpp"
#include "ooo/vba/XConnectionPoint.hpp"
#include "ooo/vba/XInterfaceWithIID.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XConnectable const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XInterfaceWithIID > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XConnectable", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XConnectable > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XConnectable > >::get();
}

::css::uno::Type const & ::ooo::vba::XConnectable::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XConnectable >::get();
}

#endif // INCLUDED_OOO_VBA_XCONNECTABLE_HPP
