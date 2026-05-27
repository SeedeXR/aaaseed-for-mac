
#ifdef AAA_BOID_UNIVERSE_H
#error "BOID_UNIVERSE_H included more than once."
#endif
#define AAA_BOID_UNIVERSE_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef	_SET_
#	include <set>
#endif

class c_boid_universe final : public c_obj
{
	friend class c_boids;

	static	c_boid_universe*	cur;
	static	INT32				cur_id;
	
public:
	static	void				c_init();
	static	void				c_deinit();

	static	c_boid_universe*	get_universe(	INT32 id );

	static	c_boid_universe*	set_cur(		INT32 id );
	static	void				set_cur_null();
	static	c_boid_universe*	get_cur()		{	return cur;	}

	static	void				add_boid(		c_boids* boids );
	static	void				remove_boid(	c_boids* boids );

	static	void				switch_next_frame();

private:
	std::set<c_boids*>*	_boids;
	std::set<c_boids*>*	_boids_next;

public:
	c_boid_universe();
	~c_boid_universe();
//	void	clear()					{	_boids->clear();			}

			void	remove( c_boids* boid );
	FINLINE	void	add( c_boids* boid );
	FINLINE	void	swap();
	//		INT32	get_size()		{	return _boids.size();		}
};

