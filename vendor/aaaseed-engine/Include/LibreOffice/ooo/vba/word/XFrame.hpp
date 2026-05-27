#ifndef INCLUDED_OOO_VBA_WORD_XFRAME_HPP
#define INCLUDED_OOO_VBA_WORD_XFRAME_HPP

#include "sal/config.h"

#include "ooo/vba/word/XFrame.hdl"

#include "ooo/vba/XHelperInterface.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace word {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::word::XFrame const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XHelperInterface > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.word.XFrame", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::word::XFrame > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::word::XFrame > >::get();
}

::css::uno::Type const & ::ooo::vba::word::XFrame::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::word::XFrame >::get();
}

#endif // INCLUDED_OOO_VBA_WORD_XFRAME_HPP
