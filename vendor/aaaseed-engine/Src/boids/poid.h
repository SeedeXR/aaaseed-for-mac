
#ifdef AAA_POID_H
#error "POID_H included more than once."
#endif
#define AAA_POID_H 1


#ifndef AAA_MECA_H
#	include "meca.h"
#endif

class	c_poid : public c_meca_obj
{
private:
	enum PARTICLE_MASK_SHIFT : INT8
	{
		FLAG_ACTIVE_SHIFT = 0,
		FLAG_WANT_TO_DIE_SHIFT,
		FLAG_TO_KILL_SHIFT,
		FLAG_FIX_SHIFT,
		FLAG_DRAW_SHIFT,
		FLAG_SHIFT_END
	};

	static	CONSTEXPR	UINT32	FLAG_ACTIVE_MASK		=	1 << FLAG_ACTIVE_SHIFT ;
	static	CONSTEXPR	UINT32	FLAG_WANT_TO_DIE_MASK	=	1 << FLAG_WANT_TO_DIE_SHIFT ;
	static	CONSTEXPR	UINT32	FLAG_TO_KILL_MASK		=	1 << FLAG_TO_KILL_SHIFT ;
	static	CONSTEXPR	UINT32	FLAG_FIX_MASK			=	1 << FLAG_FIX_SHIFT ;
	static	CONSTEXPR	UINT32	FLAG_DRAW_MASK			=	1 << FLAG_DRAW_SHIFT ;
	static	CONSTEXPR	UINT32	FLAG_ALL_MASK			=	(1 << FLAG_SHIFT_END) - 1 ;

	static	CONSTEXPR	UINT32	FLAG_BIRTH_MASK			=	FLAG_ACTIVE_MASK ;

public:
	enum LIVING_BOX_TYPE : INT32
	{
		LIVING_BOX_NO = 0,
		LIVING_BOX_DIE,
		LIVING_BOX_WRAP_RANDOM,
		LIVING_BOX_WRAP,
		LIVING_BOX_BOUNCE,
		LIVING_BOX_REPULSE,
		LIVING_BOX_TYPE_NB
	};
	static	C_PCHAR_C	str_living_box_type[LIVING_BOX_TYPE_NB];

	enum ID_GENERATOR : INT32
	{
		ID_GENERATOR_RAND = 0,
		ID_GENERATOR_UNIQUE,
		ID_GENERATOR_FROM_INDEX,
		ID_GENERATOR_NB
	};
	static	C_PCHAR_C	id_generator_str[ID_GENERATOR_NB];

private:
	UINT32	_flags;	//deal mainly with active stuff
	REAL	_sort;
//	INT32	_draw_count;	//Maa tried interesting but not fullfilling the goal. Could be interesting to blur the notion of edge

	FINLINE	void	init()							{	_flags = 0;		}	//clear_draw();	}

	FINLINE	void	set_birth_flags()				{	_flags = (_flags & ~FLAG_ALL_MASK) | FLAG_BIRTH_MASK;	}
	FINLINE	void	set_active()					{	_flags |=  FLAG_ACTIVE_MASK;							}
	FINLINE	void	clear_active()					{	_flags &=  ~FLAG_ACTIVE_MASK;							}

protected:
	FINLINE	void	set_flags( UINT32 CONST in )	{	_flags = in;	}
	FINLINE	UINT32	get_flags() CONST				{	return _flags;	}
public:
	c_poid( c_meca_world* world_in = nullptr ) : c_meca_obj( world_in )
	{
		init();
	}

	FINLINE	void	set_fix()						{	_flags |=  FLAG_FIX_MASK;						}
	FINLINE	void	clear_fix()						{	_flags &=  ~FLAG_FIX_MASK;						}

	FINLINE	bool	is_active()			CONST		{	return (_flags & FLAG_ACTIVE_MASK) != 0 ;		}
	FINLINE	bool	is_fix()			CONST		{	return (_flags & FLAG_FIX_MASK) != 0 ;			}

	FINLINE	bool	is_want_to_die()	CONST		{	return (_flags & FLAG_WANT_TO_DIE_MASK) != 0 ;	}
	FINLINE	void	set_want_to_die()				{	_flags |=  FLAG_WANT_TO_DIE_MASK;				}
	FINLINE	void	clear_want_to_die()				{	_flags &=  ~FLAG_WANT_TO_DIE_MASK;				}

	FINLINE	bool	is_to_kill()		CONST		{	return (_flags & FLAG_TO_KILL_MASK) != 0 ;		}
	FINLINE	void	mark_to_kill()					{	_flags |=  FLAG_TO_KILL_MASK;					}
	FINLINE	void	unmark_to_kill()				{	_flags &=  ~FLAG_TO_KILL_MASK;					}

	FINLINE	bool	is_draw()			CONST		{	return (_flags & FLAG_DRAW_MASK) != 0 ;			}
	FINLINE	void	set_draw()						{	_flags |=  FLAG_DRAW_MASK;						}
	FINLINE	void	clear_draw()					{	_flags &=  ~FLAG_DRAW_MASK;						}

	FINLINE	void	set_alive( UINT32 CONST id )	{
														set_birth_flags();
														set_id( id );
													}
	FINLINE	void	set_dead()						{	clear_active();	}

	FINLINE	void	set_sort( REAL CONST s )		{	_sort = s;		}
	FINLINE	REAL	get_sort()			CONST		{	return _sort;	}
};


#define LOOPER_VECTOR 0

template< class T >
class	c_poids_looper
{
public:
#if	LOOPER_VECTOR
	vector<T*>	_pp;	// pp for poid pointers

			c_poids_looper()					{														}
			void	dealloc()					{	_pp.clear();										}
			void	clear()						{	_pp.clear();										}
	FINLINE	void	add( T* pa )				{	_pp.push_back( pa );								}
	FINLINE	INT32	get_nb()					{	return _pp.size();									}
			bool	set_capacity( INT32 nb )	{	return true;										}
#else
	T**	_pp;
	T**	_pp_end;
			void	init()						{	_pp = nullptr;		_pp_end = nullptr;				}
			c_poids_looper()					{	init();												}
			void	dealloc()					{	SAFE_DELETE_ARRAY( _pp );	init();					}
			void	clear()						{	_pp_end = _pp;										}
	FINLINE	void	add( T* pa )				{	*_pp_end++ = pa;									}
	FINLINE	INT32	get_nb()					{	return (INT32) (_pp_end - _pp);						}
			bool	set_capacity( INT32 nb )	{	dealloc(); _pp = new T*[nb]; clear(); return _pp != nullptr;	}
#endif
	FINLINE	T*		get( INT32 index )			{	return _pp[index];									}
};

#if	LOOPER_VECTOR
#define	LOOPER_BEGIN()	
	{																		\
		vector<c_particle*>::iterator		it		= _looper._pp.begin();	\
		vector<c_particle*>::const_iterator	it_end	= _looper._pp.end();	\
		while( it != it_end )												\
		{																	\
			c_particle*	CONST pp = *it++;
#else
#define	LOOPER_BEGIN()													\
	{																	\
		c_particle**						it =_looper._pp;			\
		c_particle**						it_end =_looper._pp_end;	\
		while( it < it_end )											\
		{																\
			c_particle*	CONST pp = *it++;

#endif
#define	LOOPER_END()	} }	



class	c_poid_to_create final
{
public:
	bool	_b_pos;
	REAL	_pos[3];
	REAL	_speed[3];
};

struct	lua_State;
namespace aaalua
{
	namespace poid
	{
		extern void	register_poid( lua_State* L );
	}
}

class	c_poid_contact final
{
private:
	c_poid*	_a;
	c_poid*	_b;
	REAL	_dist_ratio;
	bool	_b_va;
	bool	_b_vb;
public:
	FINLINE	REAL		get_dist_ratio() CONST	{ return _dist_ratio; }

	FINLINE	INT32		build_id()		CONST 	{ return (reinterpret_cast<uintptr_t>(_a)&0xff)+(reinterpret_cast<uintptr_t>(_b)&0xff); }
	FINLINE	c_poid*		get_a()			CONST	{ return _a; }
	FINLINE	c_poid*		get_b()			CONST	{ return _b; }

	FINLINE	void		set( c_poid* CONST bi, c_poid* CONST bj, bool CONST b_vi, bool CONST b_vj, REAL CONST dist_ratio )
		{
			_a = bi;
			_b = bj;
			_dist_ratio = dist_ratio;
			_b_va = b_vi;
			_b_vb = b_vj;
		}
};

