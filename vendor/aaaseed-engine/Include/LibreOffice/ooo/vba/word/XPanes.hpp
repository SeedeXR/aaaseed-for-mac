#ifndef INCLUDED_OOO_VBA_WORD_XPANES_HPP
#define INCLUDED_OOO_VBA_WORD_XPANES_HPP

#include "sal/config.h"

#include "ooo/vba/word/XPanes.hdl"

#include "ooo/vba/XCollection.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace ooo { namespace vba { namespace word {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::word::XPanes const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XCollection > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.word.XPanes", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::word::XPanes > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::word::XPanes > >::get();
}

::css::uno::Type const & ::ooo::vba::word::XPanes::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::word::XPanes >::get();
}

#endif // INCLUDED_OOO_VBA_WORD_XPANES_HPP
