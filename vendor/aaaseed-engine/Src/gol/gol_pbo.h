
#ifdef AAA_GOL_PBO_H
#error "GOL_PBO_H included more than once."
#endif
#define AAA_GOL_PBO_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif

namespace GOL	{

extern	bool	b_pbo_can;
extern	UINT32	pbo_free_nb_out;
extern	bool	b_pbo_use_several_ui;
extern	DOUBLE	pbo_size_mb_out;
extern	bool	b_pbo_free_dealloc_ui;

extern	void	pbo_init();
extern	void	pbo_deinit();

//todo we never free
extern	void	pbo_alloc_buf(		UINT32 CONST nb );
extern	void	pbo_dealloc_buf(	);
extern	bool	pbo_do_bind(		INT32 CONST index,	bool CONST b_write );
extern	void	pbo_do_data(		UINT64 CONST size,	bool CONST b_write );
extern	void*	pbo_do_map(			bool CONST b_write	);
extern	void*	pbo_do_map_range(	UINT64 CONST size,	bool CONST b_write );
extern	void	pbo_do_unmap(		bool CONST b_write );
extern	void	pbo_dealloc(		INT32 CONST index );
extern	INT32	pbo_alloc(			INT32 CONST index,	UINT64 CONST size, bool CONST b_write );
extern	INT32	pbo_move_to(		void CONST * CONST src,	INT32 CONST size );

}	//namespace GOL


