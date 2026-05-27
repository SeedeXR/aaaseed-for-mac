#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "layers_att.h"

namespace
{
	CONSTEXPR	INT32	STACK_SIZE = 8;
}

//	stack_index points at the current top of the layers-attribute stack.
//	stack_index == -1 means empty, valid slots are stack[0..STACK_SIZE-1].
//	push pre-increments before writing, so first push lands on stack[0]. See push / pop below.
INT32			c_layers_att::stack_index	= -1;
c_layers_att*	c_layers_att::stack			= nullptr;

void	c_layers_att::c_init()
{
	SAFE_NEW_ARRAY( stack, STACK_SIZE );
}

void	c_layers_att::c_deinit()
{
	SAFE_DELETE_ARRAY( stack );
}

//	push : always increment stack_index. Save the current state into the slot only if the
//	slot is in range. When the stack overflows we drop the save but keep the counter in sync
//	with the push call ; the matching pop will skip the load and just decrement, so the
//	push / pop pair stays balanced through the overflow region.
void	c_layers_att::push()
{
	++stack_index;
	if( stack_index < STACK_SIZE )
		stack[stack_index].get_cur();
	else
		ERR_PRINT_STRING( "%s() stack overflow at %d slots, too many nested multiple-with-layers", __FUNCTION__, STACK_SIZE );
}

void	c_layers_att::pop()
{
	if( stack_index < 0 )
	{
#if AAA_DEBUG()
		debug_break( "%s() pop on empty stack, missing push upstream", __FUNCTION__ );
#endif
		return;
	}
	if( stack_index < STACK_SIZE )
		stack[stack_index].set_cur();
	//	else : stack[stack_index] was never written (push overflowed), skip the load.
	--stack_index;
}