
#ifdef AAA_PBO_H
#error "PBO_H included more than once."
#endif
#define AAA_PBO_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif

class c_img_2d;

class c_pbo final : public c_obj
{
private:
	std::vector<INT32>	_pbo_ids;
	size_t				_index;
	UINT64				_data_size;
	UINT32				_bind;
//	UINT32				_channel_nb;
	aaa::PIXEL_FORMAT	_format;
	GLenum				_gl_format;
	INT32				_sx;
	INT32				_sy;
	bool				_b_update_needed;
public:
	c_pbo();
	virtual ~c_pbo();

	void	dealloc();
	void	allocate(		c_img_2d* CONST img, UINT32 CONST tex );
	void	load_data(		c_img_2d* CONST img, UINT32 CONST bind );
	void	move_to_gpu(	c_img_2d* CONST img, bool CONST b_mipmap_generate );

};

