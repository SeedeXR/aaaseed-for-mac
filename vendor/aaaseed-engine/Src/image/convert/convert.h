//	VirtualDub - Video processing and capture application
//	Copyright (C) 1998-2001 Avery Lee
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifdef AAA_CONVERT_H
#error "CONVERT_H included more than once."
#endif
#define AAA_CONVERT_H 1

extern "C" void DIBconvert_16_to_16(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_24_to_16(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_32_to_16(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_16_to_24(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_24_to_24(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_32_to_24(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_8_to_16(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y, void *palette);
extern "C" void DIBconvert_8_to_24(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y, void *palette);
extern "C" void DIBconvert_8_to_32(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y, void *palette);
extern "C" void DIBconvert_16_to_32(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void __cdecl DIBconvert_24_to_32(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y, long alpha );
extern "C" void __cdecl DIBconvert_32_to_32(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);

extern "C" void DIBconvert_32_to_16_dithered(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_32_to_16_565_dithered(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);
extern "C" void DIBconvert_32_to_16_565(void *dest, long dest_pitch, void *src, long src_pitch, long size_x, long size_y);

extern "C"	void __cdecl asm_convert_yuy2_fullscale_bgr32(void *dst, void *src, int w, int h, ptrdiff_t dstmod, ptrdiff_t srcmod);
extern "C"	void __cdecl asm_convert_yuy2_fullscale_bgr32_MMX(void *dst, void *src, int w, int h, ptrdiff_t dstmod, ptrdiff_t srcmod);
