#include "boid_universe.h"
#include <unordered_map>

static std::unordered_map< INT32, c_boid_universe* >	universes;

INT32				c_boid_universe::cur_id	= -42;
c_boid_universe*	c_boid_universe::cur = nullptr;

void c_boid_universe::c_init()
{
	set_cur_null();
}

void c_boid_universe::c_deinit()
{
	for( auto const & elt : universes )
		delete elt.second;
	universes.clear();
}

c_boid_universe::c_boid_universe()
{
	_boids		= new std::set<c_boids*>;
	_boids_next	= new std::set<c_boids*>;
}

c_boid_universe::~c_boid_universe()
{
//	auto it = universes.find( id );
//	if( it==universes.end() )
//		universes.erase( it );
	if( get_cur() == this )
		set_cur_null();
	delete _boids;
	delete _boids_next;
}


c_boid_universe*	c_boid_universe::get_universe( INT32 id )
{
	if( id==0 )
		return cur;
	//todo 0 will be a special case current
	const auto it = universes.find( id );
	return it==universes.end() ? nullptr : it->second;
}

void	c_boid_universe::set_cur_null()
{
	cur_id	= -42;
	cur = nullptr;
}

c_boid_universe*	c_boid_universe::set_cur( INT32 id )
{	
	if( id == 0 )	// 0 means current
	{	
		if( cur )
			return cur;
		// so the first time we force the universe to be id 1 and avoid a nullptr cur
		id = 1;
	}

	if( cur_id != id )
	{
		cur_id = id;
		cur = get_universe( id );
		if( !cur )
		{
			cur = new c_boid_universe;
			universes[id] = cur;
		}
	}
	
	return cur;
}

//member
FINLINE	void	c_boid_universe::add( c_boids* boid )
{
	_boids_next->insert(boid);
}
//static
void	c_boid_universe::add_boid( c_boids* boids )
{
	if( cur )
		cur->add( boids );
}

//member
void	c_boid_universe::remove( c_boids* boids )
{
	auto f = _boids->find( boids );
	if( f != _boids->end() )
		_boids->erase( boids );
	f = _boids_next->find( boids );
	if( f != _boids_next->end() )
		_boids_next->erase( boids );
}
//static
void	c_boid_universe::remove_boid( c_boids* boids )
{
	for( auto & elt : universes )
		elt.second->remove( boids );
}

FINLINE	void c_boid_universe::swap()
{
	SWAP( _boids, _boids_next );
	_boids_next->clear();
}

void	c_boid_universe::switch_next_frame()
{
	for( auto const & elt : universes )
	{
		elt.second->swap();
		//c_boid_universe* p_boid_universe = elt.second;
	}
}
