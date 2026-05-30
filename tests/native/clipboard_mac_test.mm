// tests/native/clipboard_mac_test.mm
//
// second_todo.md S6 (c153) : unit coverage for the native macOS clipboard
// sub-lib (src/clipboard). Round-trips UTF-8 strings (incl. multibyte)
// through the real NSPasteboard, checks the truncation guard on the C-ABI
// paste, and the empty-string / clear path.
//
// NOTE : these tests mutate the SYSTEM general pasteboard (there is no
// per-process pasteboard). They save + restore the prior string so a
// developer running the suite locally does not lose their clipboard. The
// LABELS in CMake do NOT mark this perf ; it is plain unit coverage.

#import <gtest/gtest.h>

#include "src/clipboard/clipboard_mac.h"

#include <string>

namespace
{

//	RAII : snapshot the pasteboard string on construction, restore on
//	destruction so the suite is side-effect-free for the developer.
class PasteboardGuard
{
public:
    PasteboardGuard() { saved_ = aaa::clipboard::paste(); }
    ~PasteboardGuard() { aaa::clipboard::copy( saved_ ); }
private:
    std::string saved_;
};

}   //	anonymous namespace

TEST( ClipboardMac, AsciiRoundTrip )
{
    PasteboardGuard guard;
    ASSERT_TRUE( aaa::clipboard::copy( "hello world" ) );
    EXPECT_EQ( aaa::clipboard::paste(), std::string( "hello world" ) );
}

TEST( ClipboardMac, MultibyteUtf8RoundTrip )
{
    PasteboardGuard guard;
    //	Accented Latin + CJK + emoji : exercises multi-byte UTF-8.
    std::string const s = "h\xC3\xA9llo \xE4\xB8\x96\xE7\x95\x8C \xF0\x9F\x8E\xA8";
    ASSERT_TRUE( aaa::clipboard::copy( s ) );
    EXPECT_EQ( aaa::clipboard::paste(), s );
}

TEST( ClipboardMac, EmptyStringClears )
{
    PasteboardGuard guard;
    ASSERT_TRUE( aaa::clipboard::copy( "non-empty" ) );
    ASSERT_TRUE( aaa::clipboard::copy( "" ) );
    EXPECT_EQ( aaa::clipboard::paste(), std::string() );
}

TEST( ClipboardMac, CAbiPasteTruncatesToBuffer )
{
    PasteboardGuard guard;
    ASSERT_TRUE( mac_clipboard_copy( "abcdefgh", 8 ) );

    char buf[ 4 ] = { 'X', 'X', 'X', 'X' };
    int  out_len  = -1;
    ASSERT_TRUE( mac_clipboard_paste( buf, (int)sizeof( buf ), &out_len ) );
    //	buf_size 4 -> at most 3 bytes + NUL terminator.
    EXPECT_EQ( out_len, 3 );
    EXPECT_EQ( std::string( buf ), std::string( "abc" ) );
    EXPECT_EQ( buf[ 3 ], '\0' );
}

TEST( ClipboardMac, CAbiRejectsBadArguments )
{
    PasteboardGuard guard;
    int out_len = 0;
    EXPECT_FALSE( mac_clipboard_copy( nullptr, 4 ) );
    EXPECT_FALSE( mac_clipboard_paste( nullptr, 16, &out_len ) );
    char buf[ 8 ];
    EXPECT_FALSE( mac_clipboard_paste( buf, 0, &out_len ) );
    EXPECT_FALSE( mac_clipboard_paste( buf, (int)sizeof( buf ), nullptr ) );
}
