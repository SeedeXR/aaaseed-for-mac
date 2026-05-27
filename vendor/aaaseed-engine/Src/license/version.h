
#ifdef AAA_VERSION_H
#error "VERSION_H included more than once."
#endif
#define AAA_VERSION_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

class	c_version final : public c_obj
{
private:
	INT32	_major;
	INT32	_minor;
	INT32	_type;
	INT32	_index;
public:
			c_version();
			~c_version()	{};

	void	get_all( INT32 &major, INT32 &minor, INT32 &type, INT32 &index );
	INT32	get_major()		{ return _major; }
	INT32	get_minor()		{ return _minor; }
	INT32	get_type()		{ return _type; }
	INT32	get_index()		{ return _index; }
	void	set_all( INT32 major, INT32 minor, INT32 type, INT32 index );

	void	sprint_version( CHAR* where );
	void	sprint_version_short( CHAR* where);
};
