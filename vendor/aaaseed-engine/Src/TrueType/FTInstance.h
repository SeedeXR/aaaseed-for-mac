
#ifdef AAA_FTINSTANCE_H
#error "FTINSTANCE_H included more than once."
#endif
#define AAA_FTINSTANCE_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct TT_Instance_;
typedef struct TT_Instance_ TT_Instance;

class AAA_FTFace;

class FTInstance
{
protected:
	AAA_FTFace*			_face;
	TT_Instance*	_instance;

public:
	FTInstance( AAA_FTFace* face );

	virtual ~FTInstance();
	bool create();

	AAA_FTFace * getFace() const
	{
		return _face;
	}

	TT_Instance* getInstance() const
	{
		return _instance;
	}

	bool set_resolutions( int x_resolution, int y_resolution );

	bool set_point_size( int point_size );

	int get_sy() const;
	int getDescender() const;
};

#endif	//#if AAA_TRUETYPE1_USE()

