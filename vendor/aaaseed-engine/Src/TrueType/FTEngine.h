
#ifdef AAA_FTENGINE_H
#error "FTENGINE_H included more than once."
#endif
#define AAA_FTENGINE_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct TT_Engine_;
typedef struct TT_Engine_ TT_Engine;

class FTEngine
{
protected:
	static FTEngine*	_static_engine;
	TT_Engine*			_engine;
public:
	FTEngine();
	virtual ~FTEngine();

	bool				init();
	TT_Engine*			getEngine() CONST
	{
		return _engine;
	}
	static FTEngine*	getStaticEngine();
};

#endif	//#if AAA_TRUETYPE1_USE()

