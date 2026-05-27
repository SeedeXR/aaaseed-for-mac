#include "ui/obj_value.h"
#include "infrastructure/param/param_declare.h"
#include <algorithm>

FACTORY_CREATE_V1( c_obj_value, values, Values, values );

namespace	n_obj_value
{
	CONSTEXPR INT32 BASE_PARAM_NB	= ( c_obj_value::ITEM_NB + 1 ) * c_obj_value::GROUP_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP_CLOSED( value_00_to_31, c_obj_value::ITEM_NB )
			PARAM_DEF_REAL_ZERO( value_00 )
			PARAM_DEF_REAL_ZERO( value_01 )
			PARAM_DEF_REAL_ZERO( value_02 )
			PARAM_DEF_REAL_ZERO( value_03 )
			PARAM_DEF_REAL_ZERO( value_04 )
			PARAM_DEF_REAL_ZERO( value_05 )
			PARAM_DEF_REAL_ZERO( value_06 )
			PARAM_DEF_REAL_ZERO( value_07 )
			PARAM_DEF_REAL_ZERO( value_08 )
			PARAM_DEF_REAL_ZERO( value_09 )
			PARAM_DEF_REAL_ZERO( value_10 )
			PARAM_DEF_REAL_ZERO( value_11 )
			PARAM_DEF_REAL_ZERO( value_12 )
			PARAM_DEF_REAL_ZERO( value_13 )
			PARAM_DEF_REAL_ZERO( value_14 )
			PARAM_DEF_REAL_ZERO( value_15 )
			PARAM_DEF_REAL_ZERO( value_16 )
			PARAM_DEF_REAL_ZERO( value_17 )
			PARAM_DEF_REAL_ZERO( value_18 )
			PARAM_DEF_REAL_ZERO( value_19 )
			PARAM_DEF_REAL_ZERO( value_20 )
			PARAM_DEF_REAL_ZERO( value_21 )
			PARAM_DEF_REAL_ZERO( value_22 )
			PARAM_DEF_REAL_ZERO( value_23 )
			PARAM_DEF_REAL_ZERO( value_24 )
			PARAM_DEF_REAL_ZERO( value_25 )
			PARAM_DEF_REAL_ZERO( value_26 )
			PARAM_DEF_REAL_ZERO( value_27 )
			PARAM_DEF_REAL_ZERO( value_28 )
			PARAM_DEF_REAL_ZERO( value_29 )
			PARAM_DEF_REAL_ZERO( value_30 )
			PARAM_DEF_REAL_ZERO( value_31 )
		PARAM_DEF_GROUP_CLOSED( value_32_to_63, c_obj_value::ITEM_NB )
			PARAM_DEF_REAL_ZERO( value_32 )
			PARAM_DEF_REAL_ZERO( value_33 )
			PARAM_DEF_REAL_ZERO( value_34 )
			PARAM_DEF_REAL_ZERO( value_35 )
			PARAM_DEF_REAL_ZERO( value_36 )
			PARAM_DEF_REAL_ZERO( value_37 )
			PARAM_DEF_REAL_ZERO( value_38 )
			PARAM_DEF_REAL_ZERO( value_39 )
			PARAM_DEF_REAL_ZERO( value_40 )
			PARAM_DEF_REAL_ZERO( value_41 )
			PARAM_DEF_REAL_ZERO( value_42 )
			PARAM_DEF_REAL_ZERO( value_43 )
			PARAM_DEF_REAL_ZERO( value_44 )
			PARAM_DEF_REAL_ZERO( value_45 )
			PARAM_DEF_REAL_ZERO( value_46 )
			PARAM_DEF_REAL_ZERO( value_47 )
			PARAM_DEF_REAL_ZERO( value_48 )
			PARAM_DEF_REAL_ZERO( value_49 )
			PARAM_DEF_REAL_ZERO( value_50 )
			PARAM_DEF_REAL_ZERO( value_51 )
			PARAM_DEF_REAL_ZERO( value_52 )
			PARAM_DEF_REAL_ZERO( value_53 )
			PARAM_DEF_REAL_ZERO( value_54 )
			PARAM_DEF_REAL_ZERO( value_55 )
			PARAM_DEF_REAL_ZERO( value_56 )
			PARAM_DEF_REAL_ZERO( value_57 )
			PARAM_DEF_REAL_ZERO( value_58 )
			PARAM_DEF_REAL_ZERO( value_59 )
			PARAM_DEF_REAL_ZERO( value_60 )
			PARAM_DEF_REAL_ZERO( value_61 )
			PARAM_DEF_REAL_ZERO( value_62 )
			PARAM_DEF_REAL_ZERO( value_63 )
		PARAM_DEF_GROUP_CLOSED( value_64_to_95, c_obj_value::ITEM_NB )
			PARAM_DEF_REAL_ZERO( value_64 )
			PARAM_DEF_REAL_ZERO( value_65 )
			PARAM_DEF_REAL_ZERO( value_66 )
			PARAM_DEF_REAL_ZERO( value_67 )
			PARAM_DEF_REAL_ZERO( value_68 )
			PARAM_DEF_REAL_ZERO( value_69 )
			PARAM_DEF_REAL_ZERO( value_70 )
			PARAM_DEF_REAL_ZERO( value_71 )
			PARAM_DEF_REAL_ZERO( value_72 )
			PARAM_DEF_REAL_ZERO( value_73 )
			PARAM_DEF_REAL_ZERO( value_74 )
			PARAM_DEF_REAL_ZERO( value_75 )
			PARAM_DEF_REAL_ZERO( value_76 )
			PARAM_DEF_REAL_ZERO( value_77 )
			PARAM_DEF_REAL_ZERO( value_78 )
			PARAM_DEF_REAL_ZERO( value_79 )
			PARAM_DEF_REAL_ZERO( value_80 )
			PARAM_DEF_REAL_ZERO( value_81 )
			PARAM_DEF_REAL_ZERO( value_82 )
			PARAM_DEF_REAL_ZERO( value_83 )
			PARAM_DEF_REAL_ZERO( value_84 )
			PARAM_DEF_REAL_ZERO( value_85 )
			PARAM_DEF_REAL_ZERO( value_86 )
			PARAM_DEF_REAL_ZERO( value_87 )
			PARAM_DEF_REAL_ZERO( value_88 )
			PARAM_DEF_REAL_ZERO( value_89 )
			PARAM_DEF_REAL_ZERO( value_90 )
			PARAM_DEF_REAL_ZERO( value_91 )
			PARAM_DEF_REAL_ZERO( value_92 )
			PARAM_DEF_REAL_ZERO( value_93 )
			PARAM_DEF_REAL_ZERO( value_94 )
			PARAM_DEF_REAL_ZERO( value_95 )
		PARAM_DEF_GROUP_CLOSED( value_96_to_127, c_obj_value::ITEM_NB )
			PARAM_DEF_REAL_ZERO( value_96 )
			PARAM_DEF_REAL_ZERO( value_97 )
			PARAM_DEF_REAL_ZERO( value_98 )
			PARAM_DEF_REAL_ZERO( value_99 )
			PARAM_DEF_REAL_ZERO( value_100 )
			PARAM_DEF_REAL_ZERO( value_101 )
			PARAM_DEF_REAL_ZERO( value_102 )
			PARAM_DEF_REAL_ZERO( value_103 )
			PARAM_DEF_REAL_ZERO( value_104 )
			PARAM_DEF_REAL_ZERO( value_105 )
			PARAM_DEF_REAL_ZERO( value_106 )
			PARAM_DEF_REAL_ZERO( value_107 )
			PARAM_DEF_REAL_ZERO( value_108 )
			PARAM_DEF_REAL_ZERO( value_109 )
			PARAM_DEF_REAL_ZERO( value_110 )
			PARAM_DEF_REAL_ZERO( value_111 )
			PARAM_DEF_REAL_ZERO( value_112 )
			PARAM_DEF_REAL_ZERO( value_113 )
			PARAM_DEF_REAL_ZERO( value_114 )
			PARAM_DEF_REAL_ZERO( value_115 )
			PARAM_DEF_REAL_ZERO( value_116 )
			PARAM_DEF_REAL_ZERO( value_117 )
			PARAM_DEF_REAL_ZERO( value_118 )
			PARAM_DEF_REAL_ZERO( value_119 )
			PARAM_DEF_REAL_ZERO( value_120 )
			PARAM_DEF_REAL_ZERO( value_121 )
			PARAM_DEF_REAL_ZERO( value_122 )
			PARAM_DEF_REAL_ZERO( value_123 )
			PARAM_DEF_REAL_ZERO( value_124 )
			PARAM_DEF_REAL_ZERO( value_125 )
			PARAM_DEF_REAL_ZERO( value_126 )
			PARAM_DEF_REAL_ZERO( value_127 )
	};
}

p_param	c_obj_value::get_value_param( INT32 index )
{	
	index = get_clean_index(index);
	if( index <= ITEM_NB )
		index = index + 1;
	else if( index <= ITEM_NB*2 )
		index = index + 2;
	else if( index <= ITEM_NB*3 )
		index = index + 3;
	else
		index = index + 4;
	return get_param( index );
}

void	c_obj_value::param_init_pt()
{
	INT32	h = 0;
	for( INT32 j=0; j<GROUP_NB; ++j )
	{
		++h;
		for( INT32 i = 0; i < ITEM_NB; ++i )
			param_set_pt( h, _values[ j*32 + i ] );
	}
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_obj_value)
{
	param_init_with( n_obj_value::param, n_obj_value::PARAM_NB_MAX );// obj_value_param, OBJ_VALUE_PARAM_NB_MAX);
}
EMPTY_DESTRUCTOR(c_obj_value)

/*
struct	maapair
{
	INT32	ind;
	REAL	val;
};

class maapairless { 
	public: 
		bool operator() ( maapair a, maapair b ) 
			{ 
			return (a.val < b.val);
			} 
 };

INT32	c_obj_value::get_sort_index_from_min( INT32 start, INT32 stop, INT32 order_index )
{
vector<struct maapair>	list;
INT32			i;
struct	maapair onepair;

	if( start > stop)
		SWAP( start, stop);
	if( order_index < 0 )
		order_index = 0;
	else if( order_index > (stop-start) )
		order_index = (stop-start);
	for( i=start; i<=stop; ++i )
		{
		onepair.ind = i;
		onepair.val = values[i];
		list.push_back( onepair );
		}
	sort( list.begin(), list.end(), maapairless() );
	return list[order_index].ind-start;
}
*/

class	pt_real_less
{
	public:
		bool	operator() ( REAL* a, REAL* b )
		{
			return ( *a < *b );
		}
};

INT32	c_obj_value::get_sort_index_from_min( INT32 start, INT32 stop, INT32 order_index )
{
	std::vector<REAL*>	list;

	ORDER( start, stop );
	if( order_index < 0 )
		order_index = 0;
	else if( order_index > (stop-start) )
		order_index = (stop-start);

	//list.reserve( stop - start + 1 );
	//for( INT32 i = start; i <= stop; ++i )
	//	list.push_back( &_values[i] );
	list.resize( stop - start + 1 );
	for ( INT32 i = start; i <= stop; ++i )
		list[ i - start ] = &_values[i];

	nth_element( list.begin(), list.begin() + order_index, list.end(), pt_real_less() );

	return INT32( list[order_index] - &_values[start] );
}

//todoopt todo
INT32	c_obj_value::get_sort_index_from_max( INT32 start, INT32 stop, INT32 order_index )
{
	ORDER( start, stop );
	if( order_index < 0 )
		order_index = 0;
	else if( order_index > (stop-start) )
		order_index = (stop-start);

	return get_sort_index_from_min( start, stop, (stop-start) - order_index );
}


