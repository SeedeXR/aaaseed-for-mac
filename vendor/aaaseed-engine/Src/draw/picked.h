
#ifdef AAA_PICKED_H
#error "PICKED_H included more than once."
#endif
#define AAA_PICKED_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

struct	st_picked
{
	UINT32				obj_id;	// layer_id
	UINT32				nb;		// number of sub-object, probably not necessary as we can know the size with the vector
	REAL				z1;
	REAL				z2;
	INT32				priority;
	std::vector<INT32>	sub_id;	// others informations pushed by layer
};

class	c_picked final : public c_obj
{
private:
	INT32					_number;
	std::vector<st_picked>	_picked;

	void	clear_picked();
	
public:
	c_picked();
	~c_picked();
	void	init();
	void	alloc();
	void	dealloc();

//	void	update();

public:
		bool		picked_by_id( UINT32 obj_id );
		void		print();
		INT32		get_hits_nb();
		void		get_highest_by_id( st_picked *picked, UINT32 obj_id );
		void		analyse_hits( INT32 hits, UINT32* table );
		INT32		get_number()			{ return _number; }
		void		remove_by_id( UINT32	obj_id );
};

extern	c_picked	g_picked_def;

