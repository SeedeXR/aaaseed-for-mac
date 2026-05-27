
#pragma once

#ifdef AAA_UNICODE_H
#error "UNICODE_H included more than once."
#endif
#define AAA_UNICODE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#if !defined(_STRING_)
#	include <string>
#endif
#if !defined(_FUNCTIONAL_)
#	include <functional>
#endif
#if !defined(_VECTOR_)
#	include <vector>
#endif

namespace aaa::text
{

std::u16string	toUtf16( const char *utf8Str, size_t lengthInBytes = 0 );
std::u16string	toUtf16( const std::string &utf8Str );
std::u32string	toUtf32( const char *utf8Str, size_t lengthInBytes = 0 );
std::u32string	toUtf32( const std::string &utf8Str );

std::string		toUtf8( const char16_t *utf16Str, size_t lengthInBytes = 0 );
std::string		toUtf8( const std::u16string &utf16Str );

std::string		toUtf8( const char32_t *utf32str, size_t lengthInBytes = 0 );
std::string		toUtf8( const std::u32string &utf32Str );

std::u16string	toUtf16( const std::u32string &utf32str );
std::u32string	toUtf32( const std::u16string &utf16str );

std::string		fix_utf8_string( const std::string& str );

//! Returns the number of characters (not bytes) in the UTF-8 string \a str. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
std::size_t		stringLengthUtf8( const char *str, std::size_t lengthInBytes = 0 );
std::size_t		stringLengthUtf8( const std::string& str );
//!  Returns the UTF-32 code point of the next character in \a str, relative to the byte \a inOutByte. Increments \a inOutByte to be the first byte of the next character. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
std::uint32_t	nextCharUtf8( const char *str, std::size_t *inOutByte, std::size_t lengthInBytes = 0 );
//! Returns the index in bytes of the next character in \a str, advanced by \a numChars characters. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
std::size_t		advanceCharUtf8( const char *str, std::size_t numChars, std::size_t lengthInBytes = 0 );

void			lineBreakUtf8( const char *str, const std::function<bool( const char *, std::size_t )> &measureFn, const std::function<void( const char *, std::size_t )> &lineProcessFn );

//! Values returned by calcBreaksUtf8 and calcBreaksUtf16
enum UnicodeBreaks { UNICODE_MUST_BREAK, UNICODE_ALLOW_BREAK, UNICODE_NO_BREAK, UNICODE_INSIDE_CHAR };

//! Sets \a resultBreaks to be of the same length as the null-terminated UTF-8 string \a str with the values enumerated by UnicodeBreaks
void		calcLinebreaksUtf8( const char *str, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the UTF-8 string \a str with the values enumerated by UnicodeBreaks
void		calcLinebreaksUtf8( const char *str, std::size_t strLength, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the null-terminated UTF-16 string \a str with the values enumerated by UnicodeBreaks
void		calcLinebreaksUtf16( const uint16_t *str, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the UTF-16 string \a str with the values enumerated by UnicodeBreaks
void		calcLinebreaksUtf16( const uint16_t *str, std::size_t strLength, std::vector<uint8_t> *resultBreaks );

}	//namespace aaa::text
