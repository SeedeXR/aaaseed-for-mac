#ifndef INCLUDED_COM_SUN_STAR_DRAWING_QRCODE_HPP
#define INCLUDED_COM_SUN_STAR_DRAWING_QRCODE_HPP

#include "sal/config.h"

#include "com/sun/star/drawing/QRCode.hdl"

#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "typelib/typeclass.h"
#include "typelib/typedescription.h"

namespace com { namespace sun { namespace star { namespace drawing {

inline QRCode::QRCode()
    : Payload()
    , ErrorCorrection(0)
    , Border(0)
{
}

inline QRCode::QRCode(const ::rtl::OUString& Payload_, const ::sal_Int32& ErrorCorrection_, const ::sal_Int32& Border_)
    : Payload(Payload_)
    , ErrorCorrection(ErrorCorrection_)
    , Border(Border_)
{
}


inline bool operator==(const QRCode& the_lhs, const QRCode& the_rhs)
{
    return the_lhs.Payload == the_rhs.Payload
        && the_lhs.ErrorCorrection == the_rhs.ErrorCorrection
        && the_lhs.Border == the_rhs.Border;
}

inline bool operator!=(const QRCode& the_lhs, const QRCode& the_rhs)
{
return !operator==(the_lhs, the_rhs);
}
} } } }

namespace com { namespace sun { namespace star { namespace drawing {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::css::drawing::QRCode const *) {
    //TODO: On certain platforms with weak memory models, the following code can result in some threads observing that the_type points to garbage
    static ::typelib_TypeDescriptionReference * the_type = 0;
    if (the_type == 0) {
        ::typelib_TypeDescriptionReference * the_members[] = {
            ::cppu::UnoType< ::rtl::OUString >::get().getTypeLibType(),
            ::cppu::UnoType< ::sal_Int32 >::get().getTypeLibType(),
            ::cppu::UnoType< ::sal_Int32 >::get().getTypeLibType() };
        ::typelib_static_struct_type_init(&the_type, "com.sun.star.drawing.QRCode", 0, 3, the_members, 0);
    }
    return *reinterpret_cast< ::css::uno::Type * >(&the_type);
}

} } } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::drawing::QRCode const *) {
    return ::cppu::UnoType< ::css::drawing::QRCode >::get();
}

#endif // INCLUDED_COM_SUN_STAR_DRAWING_QRCODE_HPP
