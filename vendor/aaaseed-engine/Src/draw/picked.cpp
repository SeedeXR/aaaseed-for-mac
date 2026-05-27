#include "picked.h"
#include "err.h"

#define	PICKED_HEADER  "# PICKED : "

void	c_picked::dealloc()
{
	clear_picked();
}

void	c_picked::alloc()
{
	dealloc();
	_number = 0;
}

c_picked::c_picked()
{
	alloc();
}

c_picked::~c_picked()
{
	dealloc();
}

void	c_picked::clear_picked()
{
	// clear vector
	if( _picked.size() > 0 )
	{
		for( auto & pick : _picked )
		{
			if( pick.sub_id.size() > 0 )
				pick.sub_id.clear();
		}
		_picked.clear();
	}
}

void	c_picked::get_highest_by_id( st_picked* picked, UINT32 obj_id )
{
	st_picked	temp_pick;
	INT32	priority = -1;

	for( auto const & temp_pick : _picked )
	{
		if( temp_pick.obj_id == obj_id )
		{
			if( temp_pick.priority >= priority )
			{
				priority = temp_pick.priority;
				picked->priority = temp_pick.priority;
				picked->nb = temp_pick.nb;
				picked->z1 = temp_pick.z1;
				picked->z2 = temp_pick.z2;
				picked->sub_id.clear();
				if( temp_pick.nb > 0 )
				{
					picked->sub_id.reserve( temp_pick.sub_id.size() );
					//todo there fn to move all at once
					for( auto const & pick_sub : temp_pick.sub_id )
					{
						picked->sub_id.push_back( pick_sub );
					}
				}
			}
		}
	}
}

void	c_picked::remove_by_id( UINT32 obj_id )
{
	std::vector<st_picked>::iterator	it_picked;

	// remove all reference to obj_id, it means obj have processed picking so it's no longer needed
	if( _picked.size() > 0 )
	{
		it_picked = _picked.begin();
		while( it_picked != _picked.end() )
		{
			if( (*it_picked ).obj_id == obj_id )
			{
				//remove this picking
				it_picked = _picked.erase( it_picked );
			}
			else
				++it_picked;
		}
	}
}

bool	c_picked::picked_by_id( UINT32 obj_id )
{
	// check to see if this obj is picked
	if( _picked.size() > 0 )
	{
		for( auto const & pick : _picked )
		{
			if( pick.obj_id == obj_id )
				return true;
		}
	}
	return false;
}

INT32	c_picked::get_hits_nb()
{
	return _number;
}

void	c_picked::analyse_hits( INT32 hits, UINT32* table )
{
	// Analyze picked object, move information to a vector
	// first clear picking info from previous frame
	clear_picked();
	_number = hits;
	if( hits )
	{
		UINT32*	pt;

		pt = table;
		_picked.resize( hits );
		for( INT32 i = 0; i < hits; ++i )
		{
			st_picked	temp_sub_id;
			temp_sub_id.sub_id.clear();
			temp_sub_id.priority = 0;
			temp_sub_id.nb = *pt++ - 1;
			temp_sub_id.z1 = (REAL) *pt++ / 0x7fffffff;
			temp_sub_id.z2 = (REAL) *pt++ / 0x7fffffff;
			temp_sub_id.obj_id = *pt++;
			if( temp_sub_id.nb > 1 )
			{
				temp_sub_id.sub_id.resize( temp_sub_id.nb - 1 );
				for( INT32 j = temp_sub_id.nb - 2; j >= 0; --j )
				{
		//			temp_sub_id.sub_id.push_back( *pt++ );
					temp_sub_id.sub_id[j] = *pt++;
				}
				temp_sub_id.priority = *pt++;
				--temp_sub_id.nb;
			}
			else if( temp_sub_id.nb == 1 )
			{
				if( *pt != -1 )
					temp_sub_id.sub_id.push_back( *pt );
				else
					temp_sub_id.nb = 0;
				temp_sub_id.priority = 0;
				++pt;
			}
			_picked[ i ] = temp_sub_id;
			//_picked.push_back( temp_sub_id );
		}
	}
}

void	c_picked::print()
{
	if( _picked.size() > 0 )
	{
		HEADER_PRINT_STRING( PICKED_HEADER, " %d objects picked", _number );
		for( auto const & info_pick : _picked )
		{
			HEADER_PRINT_STRING( PICKED_HEADER, " %d picked with z1 %g z2 %g priority %d", info_pick.obj_id, info_pick.z1, info_pick.z2, info_pick.priority );
			if( info_pick.nb > 0 )
			{
				for( auto const & sub_index : info_pick.sub_id )
				{
					HEADER_PRINT_STRING( PICKED_HEADER, " \t%d picked ", sub_index );
				}
			}
		}
	}
}


c_picked	g_picked_def;
