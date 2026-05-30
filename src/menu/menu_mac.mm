// src/menu/menu_mac.mm
//
// second_todo.md S7 (c153) : native NSMenu context menu. See menu_mac.h for
// the API + doctrine. Pure ObjC++, manual reference counting (-fno-objc-arc).

#import "src/menu/menu_mac.h"

#import <AppKit/AppKit.h>

//	------------------------------------------------------------------
//	Target object : receives the menu action and records the command id
//	(the NSMenuItem.tag) of the chosen item.
//	------------------------------------------------------------------
@interface AAAMenuTarget : NSObject
{
@public
    int selected_id_;
}
- (void)onMenuItem:(id)sender;
@end

@implementation AAAMenuTarget
- (void)onMenuItem:(id)sender
{
    if( [sender isKindOfClass:[NSMenuItem class]] )
        selected_id_ = (int) [(NSMenuItem *) sender tag];
}
@end

namespace aaa
{
namespace menu
{

std::size_t command_item_count( std::vector<Item> const & items )
{
    std::size_t n = 0;
    for( Item const & it : items )
    {
        if( it.separator )
            continue;
        ++n;                                    //	this command / submenu head
        n += command_item_count( it.children ); //	recurse
    }
    return n;
}

namespace
{

//	Recursively build an NSMenu from the model. `target`/`action` wire every
//	leaf to the selection recorder. autoenablesItems is turned OFF so the
//	model's `enabled` flag is authoritative (otherwise AppKit would disable
//	items whose action has no responder in a headless build).
NSMenu * build_menu( std::vector<Item> const & items,
                     id target, SEL action )
{
    NSMenu * menu = [[NSMenu alloc] initWithTitle:@""];
    [menu setAutoenablesItems:NO];

    for( Item const & it : items )
    {
        if( it.separator )
        {
            [menu addItem:[NSMenuItem separatorItem]];
            continue;
        }

        NSString * title =
            [NSString stringWithUTF8String:it.label.c_str()];
        if( title == nil )
            title = @"";

        NSMenuItem * mi =
            [[NSMenuItem alloc] initWithTitle:title
                                       action:action
                                keyEquivalent:@""];
        [mi setTag:(NSInteger) it.id];
        [mi setEnabled:( it.enabled ? YES : NO )];
        [mi setTarget:target];

        if( !it.children.empty() )
        {
            NSMenu * sub = build_menu( it.children, target, action );
            [mi setSubmenu:sub];
            [sub release];   //	setSubmenu retains
        }

        [menu addItem:mi];
        [mi release];        //	addItem retains
    }
    return menu;   //	caller owns the +1
}

//	Walk to the (sub)menu addressed by all-but-last of `path`, returning the
//	NSMenuItem at the last index, or nil if any hop is out of range.
NSMenuItem * item_for_path( NSMenu * root, std::vector<int> const & path )
{
    if( root == nil || path.empty() )
        return nil;
    NSMenu * m = root;
    for( std::size_t i = 0; i + 1 < path.size(); ++i )
    {
        int idx = path[ i ];
        if( idx < 0 || idx >= (int) m.numberOfItems )
            return nil;
        NSMenuItem * mi = [m itemAtIndex:idx];
        if( mi.submenu == nil )
            return nil;
        m = mi.submenu;
    }
    int last = path.back();
    if( last < 0 || last >= (int) m.numberOfItems )
        return nil;
    return [m itemAtIndex:last];
}

//	The (sub)menu addressed by the full `path` (path = chain of submenu
//	indices). Empty path = root.
NSMenu * menu_for_path( NSMenu * root, std::vector<int> const & path )
{
    NSMenu * m = root;
    for( std::size_t i = 0; i < path.size(); ++i )
    {
        if( m == nil )
            return nil;
        int idx = path[ i ];
        if( idx < 0 || idx >= (int) m.numberOfItems )
            return nil;
        m = [m itemAtIndex:idx].submenu;
    }
    return m;
}

}   //	anonymous namespace

struct ContextMenu::Impl
{
    NSMenu *        menu   = nil;
    AAAMenuTarget * target = nil;

    void teardown()
    {
        [menu release];   menu   = nil;
        [target release]; target = nil;
    }
};

ContextMenu::ContextMenu() : impl_( new Impl() ) {}

ContextMenu::~ContextMenu()
{
    impl_->teardown();
    delete impl_;
}

bool ContextMenu::set_items( std::vector<Item> const & items )
{
    @autoreleasepool
    {
        impl_->teardown();
        impl_->target = [[AAAMenuTarget alloc] init];
        impl_->target->selected_id_ = 0;
        impl_->menu = build_menu( items, impl_->target,
                                  @selector( onMenuItem: ) );
        return impl_->menu != nil;
    }
}

bool ContextMenu::is_built() const { return impl_->menu != nil; }

std::size_t ContextMenu::item_count_at( std::vector<int> const & path ) const
{
    NSMenu * m = menu_for_path( impl_->menu, path );
    return ( m == nil ) ? 0 : (std::size_t) m.numberOfItems;
}

std::string ContextMenu::label_at( std::vector<int> const & path ) const
{
    NSMenuItem * mi = item_for_path( impl_->menu, path );
    if( mi == nil || mi.isSeparatorItem )
        return std::string();
    char const * utf8 = [mi.title UTF8String];
    return utf8 ? std::string( utf8 ) : std::string();
}

bool ContextMenu::is_separator_at( std::vector<int> const & path ) const
{
    NSMenuItem * mi = item_for_path( impl_->menu, path );
    return mi != nil && mi.isSeparatorItem;
}

bool ContextMenu::is_enabled_at( std::vector<int> const & path ) const
{
    NSMenuItem * mi = item_for_path( impl_->menu, path );
    return mi != nil && mi.isEnabled;
}

bool ContextMenu::has_submenu_at( std::vector<int> const & path ) const
{
    NSMenuItem * mi = item_for_path( impl_->menu, path );
    return mi != nil && mi.submenu != nil;
}

int ContextMenu::id_at( std::vector<int> const & path ) const
{
    NSMenuItem * mi = item_for_path( impl_->menu, path );
    return ( mi == nil ) ? 0 : (int) mi.tag;
}

int ContextMenu::last_selected_id() const
{
    return ( impl_->target != nil ) ? impl_->target->selected_id_ : 0;
}

int ContextMenu::popup_at( void * view, void * event )
{
    if( impl_->menu == nil || view == nil || event == nil )
        return 0;
    @autoreleasepool
    {
        NSView *  v = (NSView *)  view;
        NSEvent * e = (NSEvent *) event;
        [NSMenu popUpContextMenu:impl_->menu withEvent:e forView:v];
        return last_selected_id();
    }
}

}   //	namespace menu
}   //	namespace aaa
