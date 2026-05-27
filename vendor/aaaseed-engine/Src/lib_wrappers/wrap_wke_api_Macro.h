
WRAP_LOADER(	wkeInit,			void,			( void )				)
WRAP_LOADER(	wkeShutdown,		void,			( void )				)
//WRAP_LOADER(	wkeUpdate,			void,			( void )				)
WRAP_LOADER(	wkeVersion,			unsigned int,	( void )				)
WRAP_LOADER(	wkeVersionString,	const utf8*,	( void )				)
WRAP_LOADER(	wkeCreateWebView,	wkeWebView,		( void )				)
WRAP_LOADER(	wkeDestroyWebView,	const utf8*,	( wkeWebView webView )	)
