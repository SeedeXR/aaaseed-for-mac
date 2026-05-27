
#ifdef AAA_BOID_BASE_H
#error "BOID_BASE_H included more than once."
#endif
#define AAA_BOID_BASE_H 1


#ifndef AAA_POID_H
#	include "poid.h"
#endif

#define BOID_USE_LOCK()	1

class	c_boid;
class	c_boid_info_lua
{
public:
	c_bdd_boid*	_bdd;
	c_bdd_boid*	_bdd_a;
	c_bdd_boid*	_bdd_b;
	c_boid*		_boid;
	c_boid*		_boid_a;
	c_boid*		_boid_b;
public:
	void	get_pos( REAL* vec );
};
extern	c_boid_info_lua boid_info_lua;

