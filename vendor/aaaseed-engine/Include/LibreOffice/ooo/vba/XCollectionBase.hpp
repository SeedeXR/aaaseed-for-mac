#ifndef INCLUDED_OOO_VBA_XCOLLECTIONBASE_HPP
#define INCLUDED_OOO_VBA_XCOLLECTIONBASE_HPP

#include "sal/config.h"

#include "ooo/vba/XCollectionBase.hdl"

#include "com/sun/star/container/XEnumerationAccess.hpp"
#include "com/sun/star/script/XDefaultMethod.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "sal/types.h"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XCollectionBase const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[2];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::container::XEnumerationAccess > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XDefaultMethod > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XCollectionBase", 2, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XCollectionBase > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XCollectionBase > >::get();
}

::css::uno::Type const & ::ooo::vba::XCollectionBase::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XCollectionBase >::get();
}

#endif // INCLUDED_OOO_VBA_XCOLLECTIONBASE_HPP
