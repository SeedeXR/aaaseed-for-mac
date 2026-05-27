#ifndef INCLUDED_OOO_VBA_MSFORMS_XSHAPE_HPP
#define INCLUDED_OOO_VBA_MSFORMS_XSHAPE_HPP

#include "sal/config.h"

#include "ooo/vba/msforms/XShape.hdl"

#include "ooo/vba/XHelperInterface.hpp"
#include "ooo/vba/msforms/XFillFormat.hpp"
#include "ooo/vba/msforms/XLineFormat.hpp"
#include "ooo/vba/msforms/XPictureFormat.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba { namespace msforms {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::msforms::XShape const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XHelperInterface > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.msforms.XShape", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::msforms::XShape > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::msforms::XShape > >::get();
}

::css::uno::Type const & ::ooo::vba::msforms::XShape::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::msforms::XShape >::get();
}

#endif // INCLUDED_OOO_VBA_MSFORMS_XSHAPE_HPP
