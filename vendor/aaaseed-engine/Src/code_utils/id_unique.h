
#ifdef AAA_ID_UNIQUE_H
#error "ID_UNIQUE_H included more than once."
#endif
#define AAA_ID_UNIQUE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace id_unique {
	template< typename T >
	class	c_positif
	{
	private:
		T	_id	{0};
	protected:
	public:
		c_positif()	{	change();	}

		void	change();

		FINLINE	T			get()			CONST	{ return _id; }
		FINLINE	T CONST *	get_const_pt()	CONST	{ return &_id; }
		FINLINE	T *			get_pt()		CONST	{ return &_id; }
	};

	typedef	c_positif<UINT32>	c_u32;
	typedef	c_positif<UINT64>	c_u64;
};

