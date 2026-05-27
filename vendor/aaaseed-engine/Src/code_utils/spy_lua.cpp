#include "spy.h"
#include "language/lua/aaalua_glue.h"


namespace aaalua
{
	//NVIDIA NSight
	namespace n_spy
	{
		//	ARGB
		UINT32 CONSTEXPR colors[] =
		{
			0xff000000, 0xffff0000, 0xff00ff00, 0xffffff00, 0xff0000ff, 0xffff00ff, 0xff00ffff, 0xffffffff,
			0xff000000, 0xffc00000, 0xff00c000, 0xffc0c000, 0xff0000c0, 0xffc000c0, 0xff00c0c0, 0xffc0c0c0,
			0xff000000, 0xff7f0000, 0xff007f00, 0xff7f7f00, 0xff00007f, 0xff7f007f, 0xff007f7f, 0xff7f7f7f,
			0xff000000, 0xff400000, 0xff004000, 0xff404000, 0xff000040, 0xff400040, 0xff004040, 0xff404040,
		};
		CONSTEXPR UINT32 colors_nb		= sizeof(colors) / sizeof(colors[0]);
		CONSTEXPR UINT32 colors_mask	= colors_nb - 1;
		static_assert( (colors_nb & colors_mask) == 0, "colors size must be a power of 2 -- bitmask wrap relies on it" );
		AAALUACALL(add_mark)
		{
			if( SPY_IS_ACTIVE() )
			{
				LUAAAA_START(L, __FUNCTION__ );
				DBG_CHECK_ARG_NB(1);
				C_PCHAR_C	str = l.get_str();
				SPY_MARK_DIRECT(str);
			}
			return 0;
		}
		AAALUACALL(push_range)
		{
			if( SPY_IS_ACTIVE() )
			{
				LUAAAA_START(L, __FUNCTION__);
				DBG_CHECK_ARG_NB(2);
				C_PCHAR_C str = l.get_str(1);
				INT32	col = l.get_int32(2);
				SPY_PUSH_RANGE_DIRECT( str, colors[ UINT32(col - 1) & colors_mask ] );
			}
			return 0;
		}
		AAALUACALL(pop_range)
		{
			if( SPY_IS_ACTIVE() )
			{
				LUAAAA_START(L, __FUNCTION__);
				SPY_POP_RANGE_DIRECT();
			}
			return 0;
		}

		void	register_spy(lua_State* L)
		{
			LUAAAA_START(L, __FUNCTION__);

			l.define_table("spy");

			//	GENERAL
			//
			ADD_FN(add_mark);
			ADD_FN(push_range);
			ADD_FN(pop_range);

			l.pop(1);	//pop new table
		}

	}	//end namespace n_spy
}	//end namespace aaalua
