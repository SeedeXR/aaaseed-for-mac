// src/clipboard/clipboard_mac.mm
//
// Native macOS clipboard via AppKit NSPasteboard. See clipboard_mac.h for
// scope + doctrine. Manual reference counting (-fno-objc-arc).

#include "src/clipboard/clipboard_mac.h"

#import <AppKit/AppKit.h>

#include <cstring>

//	-----------------------------------------------------------------------
//	C-ABI
//	-----------------------------------------------------------------------
extern "C" bool mac_clipboard_copy( char const * str, int len )
{
    //	len == 0 with a non-null str is the explicit "clear" path : we clear
    //	the pasteboard and set an empty string, which still reports success.
    if( !str || len < 0 )
        return false;

    @autoreleasepool
    {
        NSPasteboard * pb = [NSPasteboard generalPasteboard];
        [pb clearContents];
        NSString * ns = [[[NSString alloc] initWithBytes:str
                                                  length:(NSUInteger)len
                                                encoding:NSUTF8StringEncoding] autorelease];
        if( !ns )
            return false;
        return [pb setString:ns forType:NSPasteboardTypeString] ? true : false;
    }
}

extern "C" bool mac_clipboard_paste( char * buf, int buf_size, int * out_len )
{
    if( !buf || buf_size <= 0 || !out_len )
        return false;
    *out_len = 0;

    @autoreleasepool
    {
        NSPasteboard * pb  = [NSPasteboard generalPasteboard];
        NSString *     str = [pb stringForType:NSPasteboardTypeString];
        if( !str )
            return false;

        char const * utf8 = [str UTF8String];
        if( !utf8 )
            return false;

        int len = (int)std::strlen( utf8 );
        if( len >= buf_size )
            len = buf_size - 1;          //	leave room for the NUL terminator
        std::memcpy( buf, utf8, (size_t)len );
        buf[len] = '\0';
        *out_len = len;
        return true;
    }
}

//	-----------------------------------------------------------------------
//	C++ std::string surface
//	-----------------------------------------------------------------------
namespace aaa
{
namespace clipboard
{

bool copy( std::string const & text )
{
    return mac_clipboard_copy( text.data(), (int)text.size() );
}

std::string paste()
{
    @autoreleasepool
    {
        NSPasteboard * pb  = [NSPasteboard generalPasteboard];
        NSString *     str = [pb stringForType:NSPasteboardTypeString];
        if( !str )
            return std::string();
        char const * utf8 = [str UTF8String];
        return utf8 ? std::string( utf8 ) : std::string();
    }
}

}   //	namespace clipboard
}   //	namespace aaa
