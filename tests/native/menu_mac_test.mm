// tests/native/menu_mac_test.mm
//
// second_todo.md S7 (c153) : coverage for the native NSMenu context-menu
// builder. An NSMenu builds + introspects without a running NSApplication, so
// the model->NSMenu mapping (counts, nesting, separators, enabled, ids) is
// fully tested headless. The live popup path needs a real view/event and is
// exercised in-app, not here.

#import <gtest/gtest.h>

#include "src/menu/menu_mac.h"

#include <string>
#include <vector>

namespace
{

//	File { New ; --- ; Open ; Recent { a.lua ; b.lua } }  Edit { Undo(disabled) }
std::vector<aaa::menu::Item> sample_model()
{
    using aaa::menu::Item;

    Item recent;
    recent.label = "Recent";
    recent.id    = 12;
    {
        Item a; a.label = "a.lua"; a.id = 20;
        Item b; b.label = "b.lua"; b.id = 21;
        recent.children = { a, b };
    }

    Item file;
    file.label = "File";
    file.id    = 1;
    {
        Item new_it;  new_it.label = "New";  new_it.id = 10;
        Item open_it; open_it.label = "Open"; open_it.id = 11;
        file.children = { new_it, Item::make_separator(), open_it, recent };
    }

    Item edit;
    edit.label = "Edit";
    edit.id    = 2;
    {
        Item undo; undo.label = "Undo"; undo.id = 30; undo.enabled = false;
        edit.children = { undo };
    }

    return { file, edit };
}

}   //	anonymous namespace

TEST( MenuMac, PureCommandItemCount )
{
    //	Non-separator command items : File New Open Recent a.lua b.lua Edit Undo
    EXPECT_EQ( aaa::menu::command_item_count( sample_model() ), (std::size_t)8 );
}

TEST( MenuMac, TopLevelStructure )
{
    aaa::menu::ContextMenu m;
    ASSERT_TRUE( m.set_items( sample_model() ) );
    ASSERT_TRUE( m.is_built() );

    EXPECT_EQ( m.item_count_at( {} ), (std::size_t)2 );
    EXPECT_EQ( m.label_at( { 0 } ), std::string( "File" ) );
    EXPECT_EQ( m.label_at( { 1 } ), std::string( "Edit" ) );
    EXPECT_TRUE( m.has_submenu_at( { 0 } ) );
    EXPECT_TRUE( m.has_submenu_at( { 1 } ) );
}

TEST( MenuMac, SubmenuAndSeparator )
{
    aaa::menu::ContextMenu m;
    ASSERT_TRUE( m.set_items( sample_model() ) );

    //	File submenu : New, ---, Open, Recent
    EXPECT_EQ( m.item_count_at( { 0 } ), (std::size_t)4 );
    EXPECT_EQ( m.label_at( { 0, 0 } ), std::string( "New" ) );
    EXPECT_EQ( m.id_at( { 0, 0 } ), 10 );
    EXPECT_TRUE( m.is_separator_at( { 0, 1 } ) );
    EXPECT_FALSE( m.is_separator_at( { 0, 0 } ) );
    EXPECT_EQ( m.label_at( { 0, 2 } ), std::string( "Open" ) );
    EXPECT_EQ( m.id_at( { 0, 2 } ), 11 );
    EXPECT_TRUE( m.has_submenu_at( { 0, 3 } ) );
}

TEST( MenuMac, NestedSubmenuRecursion )
{
    aaa::menu::ContextMenu m;
    ASSERT_TRUE( m.set_items( sample_model() ) );

    //	File > Recent > { a.lua, b.lua } -- the recursion-bug-prone path.
    EXPECT_EQ( m.item_count_at( { 0, 3 } ), (std::size_t)2 );
    EXPECT_EQ( m.label_at( { 0, 3, 0 } ), std::string( "a.lua" ) );
    EXPECT_EQ( m.id_at( { 0, 3, 0 } ), 20 );
    EXPECT_EQ( m.label_at( { 0, 3, 1 } ), std::string( "b.lua" ) );
    EXPECT_EQ( m.id_at( { 0, 3, 1 } ), 21 );
}

TEST( MenuMac, EnabledFlagHonoured )
{
    aaa::menu::ContextMenu m;
    ASSERT_TRUE( m.set_items( sample_model() ) );

    EXPECT_TRUE( m.is_enabled_at( { 0, 0 } ) );    //	New
    EXPECT_FALSE( m.is_enabled_at( { 1, 0 } ) );   //	Undo (disabled)
}

TEST( MenuMac, OutOfRangePathsAreSafe )
{
    aaa::menu::ContextMenu m;
    ASSERT_TRUE( m.set_items( sample_model() ) );

    EXPECT_EQ( m.item_count_at( { 99 } ), (std::size_t)0 );
    EXPECT_EQ( m.label_at( { 99 } ), std::string() );
    EXPECT_EQ( m.id_at( { 0, 99 } ), 0 );
    EXPECT_FALSE( m.has_submenu_at( { 5, 5, 5 } ) );
    EXPECT_EQ( m.last_selected_id(), 0 );   //	nothing selected yet
}
