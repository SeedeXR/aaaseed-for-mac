#ifndef INCLUDED_OOO_VBA_EXCEL_XRANGE_HPP
#define INCLUDED_OOO_VBA_EXCEL_XRANGE_HPP

#include "sal/config.h"

#include "ooo/vba/excel/XRange.hdl"

#include "com/sun/star/container/XEnumerationAccess.hpp"
#include "com/sun/star/script/XDefaultMethod.hpp"
#include "com/sun/star/script/XDefaultProperty.hpp"
#include "com/sun/star/script/XErrorQuery.hpp"
#include "ooo/vba/excel/XCharacters.hpp"
#include "ooo/vba/excel/XComment.hpp"
#include "ooo/vba/excel/XFormat.hpp"
#include "ooo/vba/excel/XName.hpp"
#include "ooo/vba/excel/XValidation.hpp"
#include "ooo/vba/excel/XWorksheet.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace ooo { namespace vba { namespace excel {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::ooo::vba::excel::XRange const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[5];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::container::XEnumerationAccess > >::get().getTypeLibType();
        aSuperTypes[1] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XDefaultMethod > >::get().getTypeLibType();
        aSuperTypes[2] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XDefaultProperty > >::get().getTypeLibType();
        aSuperTypes[3] = ::cppu::UnoType< const ::css::uno::Reference< ::css::script::XErrorQuery > >::get().getTypeLibType();
        aSuperTypes[4] = ::cppu::UnoType< const ::css::uno::Reference< ::ooo::vba::excel::XFormat > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "ooo.vba.excel.XRange", 5, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::ooo::vba::excel::XRange > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::ooo::vba::excel::XRange > >::get();
}

::css::uno::Type const & ::ooo::vba::excel::XRange::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::ooo::vba::excel::XRange >::get();
}

#endif // INCLUDED_OOO_VBA_EXCEL_XRANGE_HPP
