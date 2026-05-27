#ifndef INCLUDED_OOO_VBA_WORD_XAPPLICATION_HPP
#define INCLUDED_OOO_VBA_WORD_XAPPLICATION_HPP

#include "sal/config.h"

#include "ooo/vba/word/XApplication.hdl"

#include "ooo/vba/XConnectable.hpp"
#include "ooo/vba/word/XDocument.hpp"
#include "ooo/vba/word/XOptions.hpp"
#include "ooo/vba/word/XSelection.hpp"
#include "ooo/vba/word/XSystem.hpp"
#include "ooo/vba/word/XWindow.hpp"
#include "ooo/vba/word/XWordBasic.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba { namespace word {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::word::XApplication const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::XConnectable > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.word.XApplication", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::word::XApplication > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::word::XApplication > >::get();
}

::css::uno::Type const & ::ooo::vba::word::XApplication::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::word::XApplication >::get();
}

#endif // INCLUDED_OOO_VBA_WORD_XAPPLICATION_HPP
