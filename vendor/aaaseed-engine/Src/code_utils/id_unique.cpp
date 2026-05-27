#include "id_unique.h"
#include "err.h"

namespace id_unique
{
	UINT32 seed_32 = 0;
	FINLINE UINT32 get_new_positif_32()
	{
		//todo make several generator (use _template ?)
		if( seed_32 == aaa::BIGGEST<UINT32> )
		{
			ERR_PRINT_STRING( "id_unique::positif_32 looped, can not be used for time test now" );
			seed_32 = 1;
		}
		else
			++seed_32;
		return seed_32;
	}
	template<>
	void	c_u32::change()	{	_id = get_new_positif_32();	}

	UINT64 seed_64 = 0;
	FINLINE UINT64 get_new_positif_64()
	{
		//todo make several generator (use _template ?)
		if( seed_64 == aaa::BIGGEST<UINT64> )
		{
			ERR_PRINT_STRING( "id_unique::positif_64 looped, can not be used for time test now" );
			seed_64 = 1;
		}
		else
			++seed_64;
		return seed_64;
	}
	template<>
	void	c_u64::change()	{	_id = get_new_positif_64();	}

};




