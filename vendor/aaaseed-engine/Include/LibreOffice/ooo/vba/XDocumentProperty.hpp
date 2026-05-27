#ifndef INCLUDED_OOO_VBA_XDOCUMENTPROPERTY_HPP
#define INCLUDED_OOO_VBA_XDOCUMENTPROPERTY_HPP

#include "sal/config.h"

#include "ooo/vba/XDocumentProperty.hdl"

#include "com/sun/star/script/XDefaultProperty.hpp"
#include "ooo/vba/XHelperInterface.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::XDocumentProperty const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[2];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XDefaultProperty > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XHelperInterface > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.XDocumentProperty", 2, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::XDocumentProperty > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::XDocumentProperty > >::get();
}

::css::uno::Type const & ::ooo::vba::XDocumentProperty::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::XDocumentProperty >::get();
}

#endif // INCLUDED_OOO_VBA_XDOCUMENTPROPERTY_HPP
