#ifndef AAA_VERSION_H
#	include "version.h"
#endif
#include <stdio.h>
#include <string.h>

c_version::c_version()
{
	set_all( 0, 0, 0, 0 );
}

void	c_version::get_all( INT32 &major, INT32 &minor, INT32 &type, INT32 &index )
{
	major	= _major;
	minor	= _minor;
	type	= _type;
	index	= _index;
}

void	c_version::set_all( INT32 major, INT32 minor, INT32 type, INT32 index )
{
	_major	= major;
	_minor	= minor;
	_type	= type;
	_index	= index;
}

void	c_version::sprint_version( CHAR* str )
{
	C_PCHAR name;
	switch( _type )
	{
	case 0:	 name = "Alpha";		break;
	case 1:  name = "Beta";			break;
	case 2:  name = "Release";		break;
	default: name = "Undefined";	break;
	}
	sprintf( str, "Version %1d.%2d %s %d", _major, _minor, name, _index );
}

void	c_version::sprint_version_short( CHAR* str )
{
	sprintf( str, "%02d%02d_%03d", _major, _minor, _index );
	str += 4;
	switch( _type )
	{
	case 0:  *str = 'A'; break;
	case 1:  *str = 'B'; break;
	case 2:  *str = 'R'; break;
	default: *str = 'U'; break;
	}
}

