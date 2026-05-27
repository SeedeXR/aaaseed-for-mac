#ifndef INCLUDED_OOO_VBA_WORD_XDOCUMENTS_HPP
#define INCLUDED_OOO_VBA_WORD_XDOCUMENTS_HPP

#include "sal/config.h"

#include "ooo/vba/word/XDocuments.hdl"

#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"

namespace ooo { namespace vba { namespace word {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::word::XDocuments const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.word.XDocuments", 0, 0 );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::word::XDocuments > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::word::XDocuments > >::get();
}

::css::uno::Type const & ::ooo::vba::word::XDocuments::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::word::XDocuments >::get();
}

#endif // INCLUDED_OOO_VBA_WORD_XDOCUMENTS_HPP
