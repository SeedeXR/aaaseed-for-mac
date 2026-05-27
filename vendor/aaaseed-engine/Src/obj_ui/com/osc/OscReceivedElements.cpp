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
#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#	include "OscReceivedElements.h"
#endif
#include "OscHostEndianness.h"


namespace osc{

// return the first 4 byte boundary after the end of a str4
// be careful about calling this version if you don't know whether
// the string is terminated correctly.
static inline const char* FindStr4End( const char *p )
{
	if( p[0] == '\0' )    // special case for SuperCollider integer address pattern
		return p + 4;

	p += 3;

	while( *p )
		p += 4;

	return p + 1;
}

// return the first 4 byte boundary after the end of a str4
// returns 0 if p == end or if the string is unterminated
static inline const char* FindStr4End( const char *p, const char *end )
{
	if( p >= end )
		return 0;

	if( p[0] == '\0' )    // special case for SuperCollider integer address pattern
		return p + 4;

	p += 3;
	end -= 1;

	while( p < end && *p )
		p += 4;

	if( *p )
		return 0;
	else
		return p + 1;
}

static inline int32 ToInt32( const char *p )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::int32 i;
		char c[4];
	} u;

	u.c[0] = p[3];
	u.c[1] = p[2];
	u.c[2] = p[1];
	u.c[3] = p[0];

	return u.i;
#else
	return *(int32*)p;
#endif
}

static inline uint32 ToUInt32( const char *p )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::uint32 i;
		char c[4];
	} u;

	u.c[0] = p[3];
	u.c[1] = p[2];
	u.c[2] = p[1];
	u.c[3] = p[0];

	return u.i;
#else
	return *(uint32*)p;
#endif
}

int64 ToInt64( const char *p )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::int64 i;
		char c[8];
	} u;

	u.c[0] = p[7];
	u.c[1] = p[6];
	u.c[2] = p[5];
	u.c[3] = p[4];
	u.c[4] = p[3];
	u.c[5] = p[2];
	u.c[6] = p[1];
	u.c[7] = p[0];

	return u.i;
#else
	return *(int64*)p;
#endif
}

uint64 ToUInt64( const char *p )
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::uint64 i;
		char c[8];
	} u;

	u.c[0] = p[7];
	u.c[1] = p[6];
	u.c[2] = p[5];
	u.c[3] = p[4];
	u.c[4] = p[3];
	u.c[5] = p[2];
	u.c[6] = p[1];
	u.c[7] = p[0];

	return u.i;
#else
	return *(uint64*)p;
#endif
}

//------------------------------------------------------------------------------

bool ReceivedPacket::is_bundle() const
{
	return (get_size() > 0 && Contents()[0] == '#');
}

//------------------------------------------------------------------------------

bool ReceivedBundleElement::is_bundle() const
{
	return (get_size() > 0 && Contents()[0] == '#');
}


int32 ReceivedBundleElement::get_size() const
{
	return ToUInt32( _size );
}

void	osc_err_missing_argument()
{
	debug_break( "OSC received message : Missing Argument" );
}
void	osc_err_wrong_argument()
{
	debug_break( "OSC received message : Wrong Argument" );
}
void	osc_err_excess_argument()
{
	debug_break( "OSC received message : too many arguments" );
}

//------------------------------------------------------------------------------

bool ReceivedMessageArgument::AsBool() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return false;							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == TRUE_TYPE_TAG )
		return true;
	else if( *_type_tag == FALSE_TYPE_TAG )
		return false;
	else
	{
		osc_err_wrong_argument();				//maa
		return false;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


bool ReceivedMessageArgument::AsBoolUnchecked() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return false;							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == TRUE_TYPE_TAG )
		return true;
	else
	{
		osc_err_wrong_argument();				//maa
		return false;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


int32 ReceivedMessageArgument::AsInt32() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return 0;							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == INT32_TYPE_TAG )
		return AsInt32Unchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return 0;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


int32 ReceivedMessageArgument::AsInt32Unchecked() const
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		osc::int32 i;
		char c[4];
	} u;

	u.c[0] = _argument[3];
	u.c[1] = _argument[2];
	u.c[2] = _argument[1];
	u.c[3] = _argument[0];

	return u.i;
#else
	return *(int32*)_argument;
#endif
}


float ReceivedMessageArgument::AsFloat() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return .0f;							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == FLOAT_TYPE_TAG )
		return AsFloatUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return .0f;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


float ReceivedMessageArgument::AsFloatUnchecked() const
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		float f;
		char c[4];
	} u;

	u.c[0] = _argument[3];
	u.c[1] = _argument[2];
	u.c[2] = _argument[1];
	u.c[3] = _argument[0];

	return u.f;
#else
	return *(float*)_argument;
#endif
}


char ReceivedMessageArgument::AsChar() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return (char)0;							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == CHAR_TYPE_TAG )
		return AsCharUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return (char)0;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


char ReceivedMessageArgument::AsCharUnchecked() const
{
	return (char)ToInt32( _argument );
}


uint32 ReceivedMessageArgument::AsRgbaColor() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return 0;								//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == RGBA_COLOR_TYPE_TAG )
		return AsRgbaColorUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return 0;								//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


uint32 ReceivedMessageArgument::AsRgbaColorUnchecked() const
{
	return ToUInt32( _argument );
}


uint32 ReceivedMessageArgument::AsMidiMessage() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return 0;								//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == MIDI_MESSAGE_TYPE_TAG )
		return AsMidiMessageUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return 0;								//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


uint32 ReceivedMessageArgument::AsMidiMessageUnchecked() const
{
	return ToUInt32( _argument );
}

DOUBLE	ReceivedMessageArgument::get_as_double() const
{
	double d;
	switch( get_type_tag() )
	{
	case TRUE_TYPE_TAG:			d = 1.;			break;
	case FALSE_TYPE_TAG:		d = 0.;			break;
//	case NIL_TYPE_TAG:			break;
//	case INFINITUM_TYPE_TAG:	break;
	case INT32_TYPE_TAG:		d = AsInt32Unchecked();				break;
	case FLOAT_TYPE_TAG:		d = double(AsFloatUnchecked());		break;
//	case CHAR_TYPE_TAG:			break;
//	case RGBA_COLOR_TYPE_TAG:	break;					break;
//	case MIDI_MESSAGE_TYPE_TAG:	break;					break;
	case INT64_TYPE_TAG:		d = double(AsInt64Unchecked());		break;
//	case TIME_TAG_TYPE_TAG:		break;				break;
	case DOUBLE_TYPE_TAG:		d = AsDoubleUnchecked();			break;
//	case STRING_TYPE_TAG:		break;
//	case SYMBOL_TYPE_TAG:		break;
//	case BLOB_TYPE_TAG:			break;
	//should we asked a message here
	default:					d=0.;	break;
	}
	return d;
}

int64 ReceivedMessageArgument::AsInt64() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return 0;								//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == INT64_TYPE_TAG )
		return AsInt64Unchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return 0;								//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


int64 ReceivedMessageArgument::AsInt64Unchecked() const
{
	return ToInt64( _argument );
}


uint64 ReceivedMessageArgument::AsTimeTag() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return 0;								//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == TIME_TAG_TYPE_TAG )
		return AsTimeTagUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return 0;								//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


uint64 ReceivedMessageArgument::AsTimeTagUnchecked() const
{
	return ToUInt64( _argument );
}


double ReceivedMessageArgument::AsDouble() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return .0;								//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == DOUBLE_TYPE_TAG )
		return AsDoubleUnchecked();
	else
	{
		osc_err_wrong_argument();				//maa
		return .0;							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


double ReceivedMessageArgument::AsDoubleUnchecked() const
{
#ifdef OSC_HOST_LITTLE_ENDIAN
	union{
		double d;
		char c[8];
	} u;

	u.c[0] = _argument[7];
	u.c[1] = _argument[6];
	u.c[2] = _argument[5];
	u.c[3] = _argument[4];
	u.c[4] = _argument[3];
	u.c[5] = _argument[2];
	u.c[6] = _argument[1];
	u.c[7] = _argument[0];

	return u.d;
#else
	return *(double*)_argument;
#endif
}


const char* ReceivedMessageArgument::AsString() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return "\0";							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == STRING_TYPE_TAG )
		return _argument;
	else
	{
		osc_err_wrong_argument();				//maa
		return "\0";							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


const char* ReceivedMessageArgument::AsSymbol() const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return "\0";							//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == SYMBOL_TYPE_TAG )
		return _argument;
	else
	{
		osc_err_wrong_argument();				//maa
		return "\0";							//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


void ReceivedMessageArgument::AsBlob( const void*& data, osc_bundle_element_size_t& size ) const
{
	if( !_type_tag )
	{
		osc_err_missing_argument();				//maa
		return;									//maa
		//	throw MissingArgumentException();	//maa
	}
	else if( *_type_tag == BLOB_TYPE_TAG )
		AsBlobUnchecked( data, size );
	else
	{
		osc_err_wrong_argument();				//maa
		return;									//maa
		//	throw WrongArgumentTypeException();	//maa
	}
}


void ReceivedMessageArgument::AsBlobUnchecked( const void*& data, osc_bundle_element_size_t& size ) const
{
	osc_bundle_element_size_t sizeResult = (osc_bundle_element_size_t)ToInt32( _argument );
	if( !IsValidElementSizeValue( sizeResult ) )
	{
		osc_err_wrong_argument();				//maa
		return;									//maa
		//	throw MalformedMessageException("invalid blob size");	//maa
	}
	size = sizeResult;
	data = (void*)(_argument + osc::OSC_SIZEOF_INT32);
}


std::size_t ReceivedMessageArgument::ComputeArrayItemCount() const
{
	// it is only valid to call this for array type tags
	if( *_type_tag != ARRAY_BEGIN_TYPE_TAG )
		return 0;

	std::size_t result = 0;
	int level = 0;
	const char *typeTag = _type_tag;

	do{
		switch( *typeTag++ ){
		case ARRAY_BEGIN_TYPE_TAG:
			++level;
			break;
		case ARRAY_END_TYPE_TAG:
			--level;
			break;
		case '\0':
			return result;
		default:
			if( level == 1 )
				++result;
			break;
		}
	} while( level > 0 );

	return result;
}

//------------------------------------------------------------------------------

void ReceivedMessageArgumentIterator::Advance()
{
	if( !value_._type_tag )
		return;
		
	switch( *value_._type_tag++ ){
		case '\0':
			// don't advance past end
			--value_._type_tag;
			break;
			
		case TRUE_TYPE_TAG:
		case FALSE_TYPE_TAG:
		case NIL_TYPE_TAG:
		case INFINITUM_TYPE_TAG:
			// zero length
			break;

		case INT32_TYPE_TAG:
		case FLOAT_TYPE_TAG: 					
		case CHAR_TYPE_TAG:
		case RGBA_COLOR_TYPE_TAG:
		case MIDI_MESSAGE_TYPE_TAG:
			value_._argument += 4;
			break;

		case INT64_TYPE_TAG:
		case TIME_TAG_TYPE_TAG:
		case DOUBLE_TYPE_TAG:
			value_._argument += 8;
			break;

		case STRING_TYPE_TAG: 
		case SYMBOL_TYPE_TAG:
			// we use the unsafe function FindStr4End(char*) here because all of
			// the arguments have already been validated in
			// ReceivedMessage::Init() below.		
			value_._argument = FindStr4End( value_._argument );
			break;

		case BLOB_TYPE_TAG:
			{
				uint32 blobSize = ToUInt32( value_._argument );
				value_._argument = value_._argument + osc::OSC_SIZEOF_INT32 + RoundUp4( blobSize );
			}
			break;

		case ARRAY_BEGIN_TYPE_TAG:
		case ARRAY_END_TYPE_TAG:
			// zero length
			break;

		default:    // unknown type tag
			// don't advance
			--value_._type_tag;
			break;
	}
}

//------------------------------------------------------------------------------

ReceivedMessage::ReceivedMessage( const ReceivedPacket& packet )
	: _address_pattern( packet.Contents() )
{
	Init( packet.Contents(), packet.get_size() );
}


ReceivedMessage::ReceivedMessage( const ReceivedBundleElement& bundleElement )
	: _address_pattern( bundleElement.Contents() )
{
	Init( bundleElement.Contents(), bundleElement.get_size() );
}


bool ReceivedMessage::AddressPatternIsUInt32() const
{
	return (_address_pattern[0] == '\0');
}


uint32 ReceivedMessage::AddressPatternAsUInt32() const
{
	return ToUInt32( _address_pattern );
}

namespace {
	void do_error( C_PCHAR_C str )
	{
		ERR_PRINT_STRING( "OSC received message : %.256s", str );
	}
}
void ReceivedMessage::Init( const char *message, unsigned long size )
{
	_b_valid = false;
	
	if( size == 0 )
	{
		do_error( "zero length messages not permitted" );	//maa
		goto error_exit;	//maa
		//	throw MalformedMessageException( "zero length messages not permitted" );	//maa
	}

	if( (size & 0x03L) != 0 )
	{
		do_error( "size must be multiple of four" );	//maa
		goto error_exit;	//maa
		//	throw MalformedMessageException( "message size must be multiple of four" );	//maa
	}

	{
		const char * pt_end = message + size;

		_type_tags_begin = FindStr4End( _address_pattern, pt_end );
		if( _type_tags_begin == 0 )
		{
			// address pattern was not terminated before end
			do_error( "unterminated address pattern" );	//maa
			goto error_exit;	//maa
			//	throw MalformedMessageException( "unterminated address pattern" );	//maa
		}

		if( _type_tags_begin == pt_end )
		{
			// message consists of only the address pattern - no arguments or type tags.
			_type_tags_begin = 0;
			_type_tags_end = 0;
			_arguments = 0;			
		}
		else
		{
			if( *_type_tags_begin != ',' )
			{
				do_error( "type tags not present" );	//maa
				goto error_exit;	//maa
				//	throw MalformedMessageException( "type tags not present" );	//maa
			}

			if( *(_type_tags_begin + 1) == '\0' )
			{
				// zero length type tags
				_type_tags_begin = 0;
				_type_tags_end = 0;
				_arguments = 0;
			}
			else
			{
				// check that all arguments are present and well formed			
				_arguments = FindStr4End( _type_tags_begin, pt_end );
				if( _arguments == 0 )
				{
					do_error( "type tags were not terminated before end of message" );	//maa
					goto error_exit;	//maa
					//	throw MalformedMessageException( "type tags were not terminated before end of message" );	//maa
				}
				++_type_tags_begin; // advance past initial ','
			
				const char *type_tag = _type_tags_begin;
				const char *argument = _arguments;
						
				do
				{
					switch( *type_tag )
					{
					case TRUE_TYPE_TAG:
					case FALSE_TYPE_TAG:
					case NIL_TYPE_TAG:
					case INFINITUM_TYPE_TAG:
						// zero length
						break;

					case INT32_TYPE_TAG:
					case FLOAT_TYPE_TAG:
					case CHAR_TYPE_TAG:
					case RGBA_COLOR_TYPE_TAG:
					case MIDI_MESSAGE_TYPE_TAG:
						if( argument == pt_end )
						{
							do_error( "arguments exceed message size" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "arguments exceed message size" );	//maa
						}
						argument += 4;
						if( argument > pt_end )
						{
							do_error( "arguments exceed message size" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "arguments exceed message size" );	//maa
						}
						break;

					case INT64_TYPE_TAG:
					case TIME_TAG_TYPE_TAG:
					case DOUBLE_TYPE_TAG:
						if( argument == pt_end )
						{
							do_error( "arguments exceed message size" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "arguments exceed message size" );	//maa
						}

						argument += 8;
						if( argument > pt_end )
						{
							do_error( "arguments exceed message size" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "arguments exceed message size" );	//maa
						}
						break;

					case STRING_TYPE_TAG: 
					case SYMBOL_TYPE_TAG:					
						if( argument == pt_end )
						{
							do_error( "arguments exceed message size" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "arguments exceed message size" );	//maa
						}
						argument = FindStr4End( argument, pt_end );
						if( argument == 0 )
						{
							do_error( "unterminated string argument" );	//maa
							goto error_exit;	//maa
							//throw MalformedMessageException( "unterminated string argument" );	//maa
						}
						break;

					case BLOB_TYPE_TAG:
						{
							if( argument + osc::OSC_SIZEOF_INT32 > pt_end )
							{
								do_error( "arguments exceed message size" );	//maa
								goto error_exit;	//maa
								//throw MalformedMessageException( "arguments exceed message size" );	//maa
							}

							uint32 blobSize = ToUInt32( argument );
							argument = argument + osc::OSC_SIZEOF_INT32 + RoundUp4( blobSize );
							if( argument > pt_end )
							{
								do_error( "arguments exceed message size" );	//maa
								goto error_exit;	//maa
								//throw MalformedMessageException( "arguments exceed message size" );	//maa
							}
						}
						break;

					case ARRAY_BEGIN_TYPE_TAG:
					case ARRAY_END_TYPE_TAG:
						// zero length
						break;

					default:
						do_error( "unknown type tag" );	//maa
						goto error_exit;	//maa
						//throw MalformedMessageException( "unknown type tag" );	//maa
					}

				}
				while( *++type_tag != '\0' );
				_type_tags_end = type_tag;
			}
		}
	}
	_b_valid = true;
	return;
error_exit:
	// message consists of only the address pattern - no arguments or type tags.
	_b_valid = false;
	return;
}

//------------------------------------------------------------------------------

ReceivedBundle::ReceivedBundle( const ReceivedPacket& packet )
	: _element_count( 0 )
{
	Init( packet.Contents(), packet.get_size() );
}


ReceivedBundle::ReceivedBundle( const ReceivedBundleElement& bundleElement )
	: _element_count( 0 )
{
	Init( bundleElement.Contents(), bundleElement.get_size() );
}


void ReceivedBundle::Init( const char *bundle, unsigned long size )
{
	if( size < 16 )
	{
		ERR_PRINT_STRING( "OSC received message : packet too short for bundle" );	//maa
		return;	//maa
		//throw MalformedMessageException( "packet too short for bundle" );	//maa
	}


	if( (size & 0x03L) != 0 )
	{
		ERR_PRINT_STRING( "OSC received message : bundle size must be multiple of four" );	//maa
		return;	//maa
		//throw MalformedMessageException( "bundle size must be multiple of four" );	//maa
	}


	if( bundle[0] != '#'
		|| bundle[1] != 'b'
		|| bundle[2] != 'u'
		|| bundle[3] != 'n'
		|| bundle[4] != 'd'
		|| bundle[5] != 'l'
		|| bundle[6] != 'e'
		|| bundle[7] != '\0' )
	{
		ERR_PRINT_STRING( "OSC received message : bad bundle address pattern" );	//maa
		return;	//maa
		//throw MalformedMessageException( "bad bundle address pattern" );	//maa
	}
 
	_end = bundle + size;

	_time_tag = bundle + 8;

	const char *p = _time_tag + 8;
		
	while( p < _end ){
		if( p + osc::OSC_SIZEOF_INT32 > _end )
		{
			ERR_PRINT_STRING( "OSC received message : packet too short for elementSize" );	//maa
			return;	//maa
			//throw MalformedMessageException( "packet too short for elementSize" );	//maa
		}

		uint32 elementSize = ToUInt32( p );
		if( (elementSize & 0x03L) != 0 )
		{
			ERR_PRINT_STRING( "OSC received message : bundle element size must be multiple of four" );	//maa
			return;	//maa
			//throw MalformedMessageException( "bundle element size must be multiple of four" );	//maa
		}

		p += osc::OSC_SIZEOF_INT32 + elementSize;
		if( p > _end )
		{
			ERR_PRINT_STRING( "OSC received message : packet too short for bundle element" );	//maa
			return;	//maa
			//throw MalformedMessageException( "packet too short for bundle element" );	//maa
		}

		++_element_count;
	}

	if( p != _end )
	{
		ERR_PRINT_STRING( "OSC received message : bundle contents have a problem" );	//maa
		return;	//maa
		//throw MalformedMessageException( "bundle contents " );	//maa
	}
}


uint64 ReceivedBundle::TimeTag() const
{
	return ToUInt64( _time_tag );
}


} // namespace osc
