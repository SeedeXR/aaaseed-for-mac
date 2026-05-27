#include "infrastructure/bind_text.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/app.h"

c_bind*		bind_text::bind			= nullptr;
INT32		bind_text::bank_nb		= 8;
INT32		bind_text::bank_size	= 32;

void	bind_text::alloc()
{
	if( !bind )
	{
		bind = c_bind::get_new( "text_bind" );
		bind->set( bank_nb * bank_size, "Text Binds", "text_bind", aaa::file::TYPE_IO_NONE, bank_nb, true, true, false );
	}
}

void	bind_text::dealloc()
{
	if( bind )
		SAFE_DELETE( bind );
}
void	bind_text::init()
{
	alloc();
}

void	bind_text::deinit()
{
	dealloc();
}
static	void	MAACALLBACK	text_menu_fn( INT32	in )
{
	bind_text::bind->set_index_cur( in );
}

INT32	bind_text::menu_build()
{
	return bind->menu_build( 0, text_menu_fn );
}

void	bind_text::set_focus()
{
	bind->set_focus();
}
