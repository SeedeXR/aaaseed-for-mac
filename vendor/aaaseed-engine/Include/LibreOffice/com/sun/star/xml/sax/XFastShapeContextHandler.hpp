#ifndef INCLUDED_COM_SUN_STAR_XML_SAX_XFASTSHAPECONTEXTHANDLER_HPP
#define INCLUDED_COM_SUN_STAR_XML_SAX_XFASTSHAPECONTEXTHANDLER_HPP

#include "sal/config.h"

#include "com/sun/star/xml/sax/XFastShapeContextHandler.hdl"

#include "com/sun/star/awt/Point.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/document/XDocumentProperties.hpp"
#include "com/sun/star/drawing/XDrawPage.hpp"
#include "com/sun/star/drawing/XShape.hpp"
#include "com/sun/star/frame/XModel.hpp"
#include "com/sun/star/xml/sax/XFastContextHandler.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace com { namespace sun { namespace star { namespace xml { namespace sax {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::css::xml::sax::XFastShapeContextHandler const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        typelib_TypeDescriptionReference * aSuperTypes[1];
        aSuperTypes[0] = ::cppu::UnoType< const ::css::uno::Reference< ::css::xml::sax::XFastContextHandler > >::get().getTypeLibType();
        typelib_static_mi_interface_type_init( &the_type, "com.sun.star.xml.sax.XFastShapeContextHandler", 1, aSuperTypes );
    }
    return * reinterpret_cast< ::css::uno::Type * >( &the_type );
}

} } } } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::uno::Reference< ::css::xml::sax::XFastShapeContextHandler > const *) {
    return ::cppu::UnoType< ::css::uno::Reference< ::css::xml::sax::XFastShapeContextHandler > >::get();
}

::css::uno::Type const & ::css::xml::sax::XFastShapeContextHandler::static_type(SAL_UNUSED_PARAMETER void *) {
    return ::cppu::UnoType< ::css::xml::sax::XFastShapeContextHandler >::get();
}

#endif // INCLUDED_COM_SUN_STAR_XML_SAX_XFASTSHAPECONTEXTHANDLER_HPP
