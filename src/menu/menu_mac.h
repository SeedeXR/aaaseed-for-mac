// src/menu/menu_mac.h
//
// second_todo.md S7 (c153) : native macOS right-click CONTEXT MENU (NSMenu).
// The wave-1 port had no Mac-native context menu (only the Qt QML one). The
// upstream engine author's mac-port routes callback_event_menu ->
// SystemContextMenu_mac ; this is the native NSMenu equivalent, driven by a
// platform-neutral C++ item model so the engine's menu description builds the
// same structure on Mac as the Win32 menu does on Windows.
//
// The upstream branch fixed a recursion bug in nested-submenu lookup
// (commit caca41cd) ; this builder is recursive by construction and the
// nested-submenu path is covered by tests/native/menu_mac_test.mm.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + AppKit only. No engine link, no o_str,
//     no aaa_mem. Manual reference counting (-fno-objc-arc).
//   - This header is C++-clean (PIMPL + a plain std model) : no ObjC type
//     crosses the boundary, so a plain .cpp TU can include it.
//
// Testability : an NSMenu can be built and introspected WITHOUT a running
// NSApplication, so the model->NSMenu mapping (counts, nesting, separators,
// enabled state, item ids) is fully unit-tested headless. Only the live
// pop-up (popUpContextMenu:withEvent:forView:) needs a real event/view and
// is exercised in-app.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace aaa
{
namespace menu
{

//	Platform-neutral menu item. A separator ignores label / id / children.
//	A non-leaf item (children non-empty) becomes a submenu.
struct Item
{
    std::string       label;
    int               id        = 0;        //	caller-defined command id
    bool              enabled   = true;
    bool              separator = false;
    std::vector<Item> children;

    static Item make_separator()
    {
        Item it;
        it.separator = true;
        return it;
    }
};

//	Pure helper : total number of NON-separator command items in the tree
//	(recursive ; submenu headers that carry an id count, separators do not).
//	Framework-free -- unit-testable without AppKit.
std::size_t command_item_count( std::vector<Item> const & items );

//	ContextMenu -- builds an NSMenu from an Item tree and (in-app) pops it up
//	at an event location. Selection sets last_selected_id().
//
//	The build is retained internally ; rebuild() / dtor release it. The
//	path-based introspection accessors below let tests verify the built
//	NSMenu structure without a running app : a `path` is the chain of
//	0-based indices from the root (empty path = the root menu itself).
class ContextMenu
{
public:
    ContextMenu();
    ~ContextMenu();

    ContextMenu( ContextMenu const & )             = delete;
    ContextMenu & operator=( ContextMenu const & ) = delete;

    //	Replace the model and (re)build the backing NSMenu. Returns true on
    //	success. Safe to call repeatedly.
    bool set_items( std::vector<Item> const & items );

    //	True once a menu has been built.
    bool is_built() const;

    //	-- headless-testable introspection over the built NSMenu -----------
    //	Number of NSMenuItems in the (sub)menu at `path`. Root = empty path.
    std::size_t item_count_at( std::vector<int> const & path ) const;
    //	Title of the item at `path` (last index addresses the item). "" if
    //	out of range or a separator.
    std::string label_at( std::vector<int> const & path ) const;
    bool        is_separator_at( std::vector<int> const & path ) const;
    bool        is_enabled_at( std::vector<int> const & path ) const;
    //	True if the item at `path` carries a submenu.
    bool        has_submenu_at( std::vector<int> const & path ) const;
    //	Command id of the item at `path` (NSMenuItem.tag), or 0.
    int         id_at( std::vector<int> const & path ) const;

    //	The command id of the most recently selected item, or 0 if none.
    int last_selected_id() const;

    //	-- in-app live path (needs a real NSView + right-click NSEvent) ----
    //	Pop the menu up at the given view-local point. `view` and `event`
    //	are opaque (NSView* / NSEvent* as void*). No-op headless. Returns the
    //	selected command id (also stored in last_selected_id()).
    int popup_at( void * view, void * event );

private:
    struct Impl;
    Impl * impl_;
};

}   //	namespace menu
}   //	namespace aaa
