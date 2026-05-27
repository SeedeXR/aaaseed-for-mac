#ifndef INCLUDED_OOO_VBA_XGLOBALSBASE_HPP
#define INCLUDED_OOO_VBA_XGLOBALSBASE_HPP

#include "sal/config.h"

#include "ooo/vba/XGlobalsBase.hdl"

#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "ooo/vba/XHelperInterface.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XGlobalsBase const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[2];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XHelperInterface > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::css::lang::XMultiServiceFactory > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XGlobalsBase", 2, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XGlobalsBase > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XGlobalsBase > >::get();
}

::css::uno::Type const & ::ooo::vba::XGlobalsBase::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XGlobalsBase >::get();
}

#endif // INCLUDED_OOO_VBA_XGLOBALSBASE_HPP
