#pragma once


#ifdef AAA_IMG_NET_H
#error "IMG_NET_H included more than once."
#endif
#define AAA_IMG_NET_H 1


#ifndef AAA_AAA_RECT_H
#	include "aaa/aaa_rect.h"
#endif


// we set some field in this structure using param in ui at least for now (June 2020)
//	and get back some information
struct st_img_send
{
//	bool			_b_difference;
//	INT32			_bind_difference;
	INT32			_net_link_index;
	INT32			_channel_id;
	INT32			_bind_dst;
	bool			_b_compress;
	bool			_b_crop;
	aaa::rect::lbrt	_crop;
	INT32			_blk_nb;	
	UINT64_SOON		_sent_size;
};

