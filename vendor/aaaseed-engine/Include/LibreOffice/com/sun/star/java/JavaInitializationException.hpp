#ifndef INCLUDED_COM_SUN_STAR_JAVA_JAVAINITIALIZATIONEXCEPTION_HPP
#define INCLUDED_COM_SUN_STAR_JAVA_JAVAINITIALIZATIONEXCEPTION_HPP

#include "sal/config.h"

#include "com/sun/star/java/JavaInitializationException.hdl"

#include "com/sun/star/uno/DeploymentException.hpp"
#include "com/sun/star/uno/Type.hxx"
#include "cppu/unotype.hxx"

namespace com { namespace sun { namespace star { namespace java {

inline JavaInitializationException::JavaInitializationException()
    : ::css::uno::DeploymentException()
{ }

inline JavaInitializationException::JavaInitializationException(const ::rtl::OUString& Message_, const ::css::uno::Reference< ::css::uno::XInterface >& Context_)
    : ::css::uno::DeploymentException(Message_, Context_)
{ }

#if !defined LIBO_INTERNAL_ONLY
JavaInitializationException::JavaInitializationException(JavaInitializationException const & the_other): ::css::uno::DeploymentException(the_other) {}

JavaInitializationException::~JavaInitializationException() {}

JavaInitializationException & JavaInitializationException::operator =(JavaInitializationException const & the_other) {
    //TODO: Just like its implicitly-defined counterpart, this function definition is not exception-safe
    ::css::uno::DeploymentException::operator =(the_other);
    return *this;
}
#endif

} } } }

namespace com { namespace sun { namespace star { namespace java {

inline ::css::uno::Type const & cppu_detail_getUnoType(SAL_UNUSED_PARAMETER ::css::java::JavaInitializationException const *) {
    static typelib_TypeDescriptionReference * the_type = 0;
    if ( !the_type )
    {
        const ::css::uno::Type& rBaseType = ::cppu::UnoType< const ::css::uno::DeploymentException >::get();

        typelib_static_compound_type_init( &the_type, typelib_TypeClass_EXCEPTION, "com.sun.star.java.JavaInitializationException", rBaseType.getTypeLibType(), 0,  0 );
    }
    return * reinterpret_cast< const ::css::uno::Type * >( &the_type );
}

} } } }

SAL_DEPRECATED("use cppu::UnoType") inline ::css::uno::Type const & SAL_CALL getCppuType(SAL_UNUSED_PARAMETER ::css::java::JavaInitializationException const *) {
    return ::cppu::UnoType< ::css::java::JavaInitializationException >::get();
}

#endif // INCLUDED_COM_SUN_STAR_JAVA_JAVAINITIALIZATIONEXCEPTION_HPP
