/*
	oscpack -- Open Sound Control (OSC) packet manipulation library
	http://www.rossbencina.com/code/oscpack

	Copyright (c) 2004-2013 Ross Bencina <rossb@audiomulch.com>

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "OscOutboundPacketStream.h"
#include "err.h"
#include <cassert>
#include <cstring>
#include <cstddef>

/*maa
#if defined(__WIN32__) || defined(WIN32)
#include <malloc.h> // for alloca
#endif
*/
#include "OscHostEndianness.h"
#include "aaa_mem.h"


namespace	osc
{

static	void	FromInt32( char *p, int32 x )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::int32 i;
		char c[4];
	} u;

	u.i = x;

	p[3] = u.c[0];
	p[2] = u.c[1];
	p[1] = u.c[2];
	p[0] = u.c[3];
#else
	*reinterpret_cast<int32*>(p) = x;
#endif
}

static	void	FromUInt32( char *p, uint32 x )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::uint32 i;
		char c[4];
	} u;

	u.i = x;

	p[3] = u.c[0];
	p[2] = u.c[1];
	p[1] = u.c[2];
	p[0] = u.c[3];
#else
	*reinterpret_cast<uint32*>(p) = x;
#endif
}

static	void	FromInt64( char *p, int64 x )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::int64 i;
		char c[8];
	} u;

	u.i = x;

	p[7] = u.c[0];
	p[6] = u.c[1];
	p[5] = u.c[2];
	p[4] = u.c[3];
	p[3] = u.c[4];
	p[2] = u.c[5];
	p[1] = u.c[6];
	p[0] = u.c[7];
#else
	*reinterpret_cast<int64*>(p) = x;
#endif
}

static	void	FromUInt64( char *p, uint64 x )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::uint64 i;
		char c[8];
	} u;

	u.i = x;

	p[7] = u.c[0];
	p[6] = u.c[1];
	p[5] = u.c[2];
	p[4] = u.c[3];
	p[3] = u.c[4];
	p[2] = u.c[5];
	p[1] = u.c[6];
	p[0] = u.c[7];
#else
	*reinterpret_cast<uint64*>(p) = x;
#endif
}


OutboundPacketStream::OutboundPacketStream( char *buffer, unsigned long capacity )
:_data(						buffer )
,_end(						_data + capacity )
,typeTagsCurrent_(			_end )
,messageCursor_(			_data )
,argumentCurrent_(			_data )
,elementSizePtr_(			0 )
,_b_message_is_in_progress( false )	
{
}

OutboundPacketStream::~OutboundPacketStream()
{
}

char*	OutboundPacketStream::BeginElement( char *beginPtr )
{
	if( elementSizePtr_ == 0 )
	{
		elementSizePtr_ = reinterpret_cast<uint32*>(_data);
		return beginPtr;
	}
	else
	{
		// store an offset to the old element size ptr in the element size slot
		// we store an offset rather than the actual pointer to be 64 bit clean.
		*reinterpret_cast<uint32*>(beginPtr) = (uint32)(reinterpret_cast<char*>(elementSizePtr_) - _data);
		elementSizePtr_ = reinterpret_cast<uint32*>(beginPtr);
		return beginPtr + 4;
	}
}

void	OutboundPacketStream::EndElement( char *endPtr )
{
	if( elementSizePtr_ != 0 )
	{
		if( elementSizePtr_ == reinterpret_cast<uint32*>(_data) )
		{
			elementSizePtr_ = 0;
		}
		else
		{
			// while building an element, an offset to the containing element's
			// size slot is stored in the elements size slot (or a ptr to data_
			// if there is no containing element). We retrieve that here
			uint32 *previousElementSizePtr = (uint32*)(_data + *reinterpret_cast<uint32*>(elementSizePtr_));

			// then we store the element size in the slot, note that the element
			// size does not include the size slot, hence the - 4 below.
			uint32 elementSize = uint32(endPtr - reinterpret_cast<char*>(elementSizePtr_)) - 4;
			FromUInt32( reinterpret_cast<char*>(elementSizePtr_), elementSize );

			// finally, we reset the element size ptr to the containing element
			elementSizePtr_ = previousElementSizePtr;
		}
	}
}


bool	OutboundPacketStream::ElementSizeSlotRequired() const
{
	return (elementSizePtr_ != 0);
}

bool	OutboundPacketStream::check_for_available_space( CONST UINT32 asked )
{
	if( asked > get_capacity() )
	{
		ERR_PRINT_STRING( "OSC : Out of Memory" );
		return false;
	}
	return true;
}

bool	OutboundPacketStream::check_for_available_bundle_space()
{
	return check_for_available_space( get_size() + ((ElementSizeSlotRequired())?4:0) + 16 );
}

INT32	OutboundPacketStream::get_nil_space()
{
	return 4;
}

INT32	OutboundPacketStream::get_real_space()
{
	return 4;
}

INT32	OutboundPacketStream::get_bool_space()
{
	 return 4;
}

INT32	OutboundPacketStream::get_string_space( C_PCHAR_C str )
{
	return RoundUp4( INT32(strlen(str)) + 1 );
}

INT32	OutboundPacketStream::get_int_space()
{
	return 4;
}

INT32	OutboundPacketStream::get_bundle_space()
{
//	unsigned long required = ((ElementSizeSlotRequired())?4:0) + 16;
	return 16;
}

INT32	OutboundPacketStream::get_message_space( const char *addressPattern )
{
	// plus 4 for at least four bytes of type tag
//	unsigned long required = ((ElementSizeSlotRequired())?4:0) + RoundUp4(strlen(addressPattern) + 1) + 4;
	return RoundUp4((INT32)strlen(addressPattern) + 1) + 4;
}

bool	OutboundPacketStream::check_for_available_message_space( const char *addressPattern )
{
	// plus 4 for at least four bytes of type tag
	return check_for_available_space( get_size() + (ElementSizeSlotRequired()?4:0) + RoundUp4( INT32(strlen(addressPattern))+1) + 4 );
}

bool	OutboundPacketStream::check_for_available_argument_space( INT32 argumentLength )
{
	// plus three for extra type tag, comma and null terminator
	return check_for_available_space( (unsigned long)(argumentCurrent_ - _data) + argumentLength + RoundUp4( long(_end - typeTagsCurrent_) + 3) );
}

INT32	OutboundPacketStream::get_argument_space_size( INT32 arg_nb )
{
	return RoundUp4( arg_nb + 2 );	//todo check
}

void	OutboundPacketStream::clear()
{
	typeTagsCurrent_ = _end;
	messageCursor_ = _data;
	argumentCurrent_ = _data;
	elementSizePtr_ = 0;
	_b_message_is_in_progress = false;
}

UINT32	OutboundPacketStream::get_capacity() CONST
{
	return (UINT32)(_end - _data);
}

UINT32	OutboundPacketStream::get_size() const
{
	unsigned int result = (unsigned int)(argumentCurrent_ - _data);
	if( is_message_in_progress() )
	{
		// account for the length of the type tag string. the total type tag
		// includes an initial comma, plus at least one terminating \0
		result += RoundUp4( INT32(_end - typeTagsCurrent_) + 2 );
	}
	return result;
}

const	char*	OutboundPacketStream::get_data() const
{
	return _data;
}

bool	OutboundPacketStream::is_ready() const
{
	return !is_message_in_progress() && !is_bundle_in_progress();
}

bool	OutboundPacketStream::is_message_in_progress() const
{
	return _b_message_is_in_progress;
}

bool	OutboundPacketStream::is_bundle_in_progress() const
{
	return elementSizePtr_ != 0;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const BundleInitiator& rhs )
{
	if( is_message_in_progress() )
	{
		ERR_PRINT_STRING( "OSC : opening or closing bundle or message while message is in progress" );
	}
	else
	{
		if( check_for_available_bundle_space() )
		{
			messageCursor_ = BeginElement( messageCursor_ );

			MEMCPY( messageCursor_, "#bundle\0", 8, __FUNCTION__ );
			FromUInt64( messageCursor_ + 8, rhs.timeTag );

			messageCursor_ += 16;
			argumentCurrent_ = messageCursor_;
		}
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const BundleTerminator& rhs )
{
	(void) rhs;

	if( !is_bundle_in_progress() )
	{
		ERR_PRINT_STRING( "OSC : call to EndBundle when bundle is not in progress" );
	}
	else if( is_message_in_progress() )
	{
		ERR_PRINT_STRING( "OSC : opening or closing bundle or message while message is in progress" );
	}
	else
		EndElement( messageCursor_ );

	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const BeginMessage& rhs )
{
	if( is_message_in_progress() )
	{
		ERR_PRINT_STRING( "OSC : opening or closing bundle or message while message is in progress" );
	}
	else
	{
		if( check_for_available_message_space( rhs.addressPattern ) )
		{
			messageCursor_ = BeginElement( messageCursor_ );

			strcpy( messageCursor_, rhs.addressPattern );
			unsigned long rhsLength = (unsigned long)strlen(rhs.addressPattern);
			messageCursor_ += rhsLength + 1;

			// zero pad to 4-byte boundary
			unsigned long i = rhsLength + 1;
			while( i & 0x3 )
			{
				*messageCursor_++ = '\0';
				++i;
			}

			argumentCurrent_ = messageCursor_;
			typeTagsCurrent_ = _end;

			_b_message_is_in_progress = true;
		}
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const MessageTerminator& rhs )
{
	(void) rhs;

	if( !is_message_in_progress() )
	{
		ERR_PRINT_STRING( "OSC : call to EndMessage when message is not in progress" );
	}
	else
	{
		int typeTagsCount = int(_end - typeTagsCurrent_);
		if( typeTagsCount )
		{
			char *tempTypeTags = (char*)alloca(typeTagsCount);
			MEMCPY( tempTypeTags, typeTagsCurrent_, typeTagsCount, __FUNCTION__ );

			// slot size includes comma and null terminator
			int typeTagSlotSize = RoundUp4( typeTagsCount + 2 );

			uint32 argumentsSize = uint32(argumentCurrent_ - messageCursor_);

			MEMMOVE( messageCursor_ + typeTagSlotSize, messageCursor_, argumentsSize );

			messageCursor_[0] = ',';
			//todo change the architecture to avoid this horrible reverse copy after a first copy
			// copy type tags in reverse (really forward) order
			for( int i=0; i < typeTagsCount; ++i )
				messageCursor_[i+1] = tempTypeTags[ (typeTagsCount-1) - i ];

			char *p = messageCursor_ + 1 + typeTagsCount;
			for( int i=0; i < (typeTagSlotSize - (typeTagsCount + 1)); ++i )
				*p++ = '\0';

			typeTagsCurrent_ = _end;

			// advance messageCursor_ for next message
			messageCursor_ += typeTagSlotSize + argumentsSize;
		}
		else
		{
			// send an empty type tags string
			MEMCPY( messageCursor_, ",\0\0\0", 4, __FUNCTION__ );

			// advance messageCursor_ for next message
			messageCursor_ += 4;
		}

		argumentCurrent_ = messageCursor_;

		EndElement( messageCursor_ );

		_b_message_is_in_progress = false;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( bool rhs )
{
	if( check_for_available_argument_space(0) )
		*(--typeTagsCurrent_) = (char)((rhs) ? TRUE_TYPE_TAG : FALSE_TYPE_TAG);
	return *this;
}


OutboundPacketStream&	OutboundPacketStream::operator<<( const NilType& rhs )
{
	(void) rhs;
	if( check_for_available_argument_space(0) )
		*(--typeTagsCurrent_) = NIL_TYPE_TAG;

	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const InfinitumType& rhs )
{
	(void) rhs;
	if ( check_for_available_argument_space(0) )
		*(--typeTagsCurrent_) = INFINITUM_TYPE_TAG;

	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( int32 rhs )
{
	if( check_for_available_argument_space(4) )
	{
		*(--typeTagsCurrent_) = INT32_TYPE_TAG;
		FromInt32( argumentCurrent_, rhs );
		argumentCurrent_ += 4;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( float rhs )
{
	if( check_for_available_argument_space(4) )
	{
		*(--typeTagsCurrent_) = FLOAT_TYPE_TAG;
#ifdef OSC_HOST_LITTLE_ENDIAN
		union{
			float f;
			char c[4];
		} u;

		u.f = rhs;

		argumentCurrent_[3] = u.c[0];
		argumentCurrent_[2] = u.c[1];
		argumentCurrent_[1] = u.c[2];
		argumentCurrent_[0] = u.c[3];
#else
		*reinterpret_cast<float*>(argumentCurrent_) = rhs;
#endif
		argumentCurrent_ += 4;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( char rhs )
{
	if( check_for_available_argument_space(4) )
	{
		*(--typeTagsCurrent_) = CHAR_TYPE_TAG;
		FromInt32( argumentCurrent_, rhs );
		argumentCurrent_ += 4;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const RgbaColor& rhs )
{
	if( check_for_available_argument_space(4) )
	{
		*(--typeTagsCurrent_) = RGBA_COLOR_TYPE_TAG;
		FromUInt32( argumentCurrent_, rhs );
		argumentCurrent_ += 4;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const MidiMessage& rhs )
{
	if( check_for_available_argument_space(4) )
	{
		*(--typeTagsCurrent_) = MIDI_MESSAGE_TYPE_TAG;
		FromUInt32( argumentCurrent_, rhs );
		argumentCurrent_ += 4;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( int64 rhs )
{
	if( check_for_available_argument_space(8) )
	{
		*(--typeTagsCurrent_) = INT64_TYPE_TAG;
		FromInt64( argumentCurrent_, rhs );
		argumentCurrent_ += 8;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const TimeTag& rhs )
{
	if( check_for_available_argument_space(8) )
	{
		*(--typeTagsCurrent_) = TIME_TAG_TYPE_TAG;
		FromUInt64( argumentCurrent_, rhs );
		argumentCurrent_ += 8;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( double rhs )
{
	if( check_for_available_argument_space(8) )
	{
		*(--typeTagsCurrent_) = DOUBLE_TYPE_TAG;

#ifdef OSC_HOST_LITTLE_ENDIAN
		union{
			double f;
			char c[8];
		} u;

		u.f = rhs;

		argumentCurrent_[7] = u.c[0];
		argumentCurrent_[6] = u.c[1];
		argumentCurrent_[5] = u.c[2];
		argumentCurrent_[4] = u.c[3];
		argumentCurrent_[3] = u.c[4];
		argumentCurrent_[2] = u.c[5];
		argumentCurrent_[1] = u.c[6];
		argumentCurrent_[0] = u.c[7];
#else
		*reinterpret_cast<double*>(argumentCurrent_) = rhs;
#endif
		argumentCurrent_ += 8;
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const char *rhs )
{
	if( check_for_available_argument_space( RoundUp4((INT32)strlen(rhs) + 1) ) )
	{
		*(--typeTagsCurrent_) = STRING_TYPE_TAG;
		strcpy( argumentCurrent_, rhs );
		unsigned long rhsLength = (unsigned long)strlen(rhs);
		argumentCurrent_ += rhsLength + 1;

		// zero pad to 4-byte boundary
		unsigned long i = rhsLength + 1;
		while( i & 0x3 )
		{
			*argumentCurrent_++ = '\0';
			++i;
		}
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const Symbol& rhs )
{
	if( check_for_available_argument_space( RoundUp4((INT32)strlen(rhs) + 1) ) )
	{
		*(--typeTagsCurrent_) = SYMBOL_TYPE_TAG;
		strcpy( argumentCurrent_, rhs );
		unsigned long rhsLength = (unsigned long)strlen(rhs);
		argumentCurrent_ += rhsLength + 1;

		// zero pad to 4-byte boundary
		unsigned long i = rhsLength + 1;
		while( i & 0x3 )
		{
			*argumentCurrent_++ = '\0';
			++i;
		}
	}
	return *this;
}

OutboundPacketStream&	OutboundPacketStream::operator<<( const Blob& rhs )
{
	if( check_for_available_argument_space( 4 + RoundUp4(rhs.size) ) )
	{
		*(--typeTagsCurrent_) = BLOB_TYPE_TAG;
		FromUInt32( argumentCurrent_, rhs.size );
		argumentCurrent_ += 4;
		MEMCPY( (void*)argumentCurrent_, (void*)(rhs.data), (size_t)(rhs.size), __FUNCTION__ );
		argumentCurrent_ += rhs.size;

		// zero pad to 4-byte boundary
		unsigned long i = rhs.size;
		while( i & 0x3 )
		{
			*argumentCurrent_++ = '\0';
			++i;
		}
	}
	return *this;
}

OutboundPacketStream& OutboundPacketStream::operator<<( const ArrayInitiator& rhs )
{
	(void) rhs;
	if( !check_for_available_argument_space( 0 ) )
		return *this;
	*(--typeTagsCurrent_) = ARRAY_BEGIN_TYPE_TAG;
	return *this;
}

OutboundPacketStream& OutboundPacketStream::operator<<( const ArrayTerminator& rhs )
{
	(void) rhs;
	if( !check_for_available_argument_space( 0 ) )
		return *this;
	*(--typeTagsCurrent_) = ARRAY_END_TYPE_TAG;
	return *this;
}

} // namespace osc
