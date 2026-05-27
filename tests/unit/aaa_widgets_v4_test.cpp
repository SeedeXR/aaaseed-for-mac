//  tests/unit/aaa_widgets_v4_test.cpp
//
//  c150 v4 : WidgetSystem text_area + marked-text (IME composition) tests.
//  c150-A agent stalled before writing the tests ; written manually after
//  c150-A's WidgetSystem additions (header L180-225 + impl L2178-2247).
//
//  Honest scope : we test the WidgetSystem state machine for text_area +
//  marked-text directly. Actual CJK keyboard input via NSTextInputContext
//  is a separate interactive verification gap (same shape as drag-drop
//  per c149-A and Space-press per c143-C / c145-A).

#include "src/gol/metal/metal_backend.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace
{

GOL::MetalBackend* make_backend()
{
    auto* b = new GOL::MetalBackend();
    if( !b->init() ) { delete b; return nullptr; }
    return b;
}

//  Open one frame, run lambda, close ; minimal harness for tests that
//  only care about widget-state mutations.
template < typename F >
void in_frame( aaa::ui::widgets::WidgetSystem& ws, F f )
{
    ws.begin_frame( 1280, 720, 0.0, 0.0, false, false );
    f();
    ws.end_frame();
}

}   //  namespace

//  =====================================================================
//  Marked-text round-trip (5 tests)
//  =====================================================================

TEST( WidgetsV4Marked, RoundTripCurrentAndHas )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        ws.on_marked_text( "ko", 0, 2 );
        EXPECT_TRUE(  ws.has_marked_text() );
        EXPECT_EQ(    ws.current_marked_text(), "ko" );
    }
    delete b;
}

TEST( WidgetsV4Marked, CommitClearsMarkedState )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        ws.on_marked_text( "ni", 0, 2 );
        ws.on_text_committed( "ni" );
        EXPECT_FALSE( ws.has_marked_text() );
    }
    delete b;
}

TEST( WidgetsV4Marked, EmptyMarkedStringHandled )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        ws.on_marked_text( "", 0, 0 );
        //  Empty marked text is allowed (means "I cleared the composer").
        EXPECT_EQ( ws.current_marked_text(), "" );
    }
    delete b;
}

TEST( WidgetsV4Marked, LongMarkedStringHandled )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        std::string const composing( 200, 'k' );
        ws.on_marked_text( composing, 0, 200 );
        EXPECT_EQ( ws.current_marked_text().size(), 200u );
    }
    delete b;
}

TEST( WidgetsV4Marked, ReplaceMarkedTextDoesNotAppend )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        ws.on_marked_text( "ko", 0, 2 );
        ws.on_marked_text( "kon", 0, 3 );    //  user typed another letter
        EXPECT_EQ( ws.current_marked_text(), "kon" );
    }
    delete b;
}

//  =====================================================================
//  text_area widget basics (5 tests)
//  =====================================================================

TEST( WidgetsV4TextArea, ReturnsCurrentValueWithoutFocus )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        std::string out;
        in_frame( ws, [ & ] {
            ws.begin_panel( "p", 10, 10, 400, 300 );
            out = ws.text_area( "notes",
                                "hello\nworld",
                                4, 32, 256 );
            ws.end_panel();
        } );
        EXPECT_EQ( out, "hello\nworld" );
    }
    delete b;
}

TEST( WidgetsV4TextArea, LineCountReflectsValue )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        in_frame( ws, [ & ] {
            ws.begin_panel( "p", 10, 10, 400, 300 );
            ws.text_area( "notes", "a\nb\nc", 4, 32, 256 );
            ws.end_panel();
        } );
        //  Three lines : "a", "b", "c".
        EXPECT_EQ( ws.text_area_line_count( "notes" ), 3 );
    }
    delete b;
}

TEST( WidgetsV4TextArea, SingleLineHasOneLineCount )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        in_frame( ws, [ & ] {
            ws.begin_panel( "p", 10, 10, 400, 300 );
            ws.text_area( "x", "single", 4, 32, 256 );
            ws.end_panel();
        } );
        EXPECT_EQ( ws.text_area_line_count( "x" ), 1 );
    }
    delete b;
}

TEST( WidgetsV4TextArea, EmptyValueStartsAtOneLine )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        in_frame( ws, [ & ] {
            ws.begin_panel( "p", 10, 10, 400, 300 );
            ws.text_area( "x", "", 4, 32, 256 );
            ws.end_panel();
        } );
        //  Empty text_area starts with a single empty line (cursor row 0).
        EXPECT_GE( ws.text_area_line_count( "x" ), 1 );
        EXPECT_EQ( ws.text_area_value( "x" ), "" );
    }
    delete b;
}

TEST( WidgetsV4TextArea, ValueAccessorReturnsCurrentBuffer )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        in_frame( ws, [ & ] {
            ws.begin_panel( "p", 10, 10, 400, 300 );
            ws.text_area( "notes", "first\nsecond\nthird", 6, 32, 256 );
            ws.end_panel();
        } );
        EXPECT_EQ( ws.text_area_value( "notes" ), "first\nsecond\nthird" );
    }
    delete b;
}

//  =====================================================================
//  Synthetic CJK round-trip (3 tests)
//  =====================================================================

TEST( WidgetsV4Cjk, JapaneseHiraganaCommit )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        //  "ko" composes via Hiragana IME to "こ" (U+3053). The IME
        //  selects the final character ; commit flushes the composed
        //  value into the widget system.
        ws.on_marked_text( "ko", 0, 2 );
        ws.on_text_committed( "\xe3\x81\x93" );   //  UTF-8 こ
        EXPECT_FALSE( ws.has_marked_text() );
    }
    delete b;
}

TEST( WidgetsV4Cjk, ChinesePinyinCommit )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        //  Pinyin "ni" -> 你 (U+4F60).
        ws.on_marked_text( "ni", 0, 2 );
        ws.on_text_committed( "\xe4\xbd\xa0" );   //  UTF-8 你
        EXPECT_FALSE( ws.has_marked_text() );
    }
    delete b;
}

TEST( WidgetsV4Cjk, KoreanHangulCommit )
{
    auto* b = make_backend(); ASSERT_NE( b, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( b );
        //  Hangul jamo "an" -> 안 (U+C548).
        ws.on_marked_text( "an", 0, 2 );
        ws.on_text_committed( "\xec\x95\x88" );   //  UTF-8 안
        EXPECT_FALSE( ws.has_marked_text() );
    }
    delete b;
}
