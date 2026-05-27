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
#ifdef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#error "INCLUDED_OSCRECEIVEDELEMENTS_H included more than once."
#endif
#define AAA_INCLUDED_OSCRECEIVEDELEMENTS_H 1


#include <cstddef>

#ifndef AAA_INCLUDED_OSCTYPES_H
#	include "OscTypes.h"
#endif
#ifndef AAA_INCLUDED_OSC_EXCEPTION_H
#	include "OscException.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif


namespace osc{


class MalformedMessageException : public Exception{
public:
	MalformedMessageException( const char *w="malformed message" )
		: Exception( w ) {}
};
/*maa
class MalformedBundleException : public Exception{
public:
	MalformedBundleException( const char *w="malformed bundle" )
		: Exception( w ) {}
};

class WrongArgumentTypeException : public Exception{
public:
	WrongArgumentTypeException( const char *w="wrong argument type" )
		: Exception( w ) {}
};

class MissingArgumentException : public Exception{
public:
	MissingArgumentException( const char *w="missing argument" )
		: Exception( w ) {}
};

class ExcessArgumentException : public Exception{
public:
	ExcessArgumentException( const char *w="too many arguments" )
		: Exception( w ) {}
};
*/

class ReceivedPacket{
public:
	ReceivedPacket( const char *contents, int32 size )
		: _contents( contents )
		, _size( size ) {}

	bool is_message() const { return !is_bundle(); }
	bool is_bundle() const;

	int32 get_size() const { return _size; }
	const char *Contents() const { return _contents; }

private:
	const char *_contents;
	int32 _size;
};


class ReceivedBundleElement{
public:
	ReceivedBundleElement( const char *size )
		: _size( size ) {}

	friend class ReceivedBundleElementIterator;

	bool is_message() const { return !is_bundle(); }
	bool is_bundle() const;

	int32 get_size() const;
	const char *Contents() const { return _size + 4; }

private:
	const char *_size;
};


class ReceivedBundleElementIterator{
public:
	ReceivedBundleElementIterator( const char *sizePtr )
		: value_( sizePtr ) {}

	ReceivedBundleElementIterator operator++()
	{
		Advance();
		return *this;
	}

	ReceivedBundleElementIterator operator++(int)
	{
		ReceivedBundleElementIterator old( *this );
		Advance();
		return old;
	}

	const ReceivedBundleElement& operator*() const { return value_; }

	const ReceivedBundleElement* operator->() const { return &value_; }

	friend bool operator==(const ReceivedBundleElementIterator& lhs,
			const ReceivedBundleElementIterator& rhs );

private:
	ReceivedBundleElement value_;

	void Advance() { value_._size = value_.Contents() + value_.get_size(); }

	bool IsEqualTo( const ReceivedBundleElementIterator& rhs ) const
	{
		return value_._size == rhs.value_._size;
	}
};

inline bool operator==(const ReceivedBundleElementIterator& lhs,
		const ReceivedBundleElementIterator& rhs )
{
	return lhs.IsEqualTo( rhs );
}

inline bool operator!=(const ReceivedBundleElementIterator& lhs,
		const ReceivedBundleElementIterator& rhs )
{
	return !( lhs == rhs );
}


class ReceivedMessageArgument{
public:
	ReceivedMessageArgument( const char *type_tag, const char *argument )
		: _type_tag( type_tag )
		, _argument( argument ) {}

	friend class ReceivedMessageArgumentIterator;

	const char get_type_tag() const { return *_type_tag; }

	DOUBLE	get_as_double() const;

	// the unchecked methods below don't check whether the argument actually
	// is of the specified type. they should only be used if you've already
	// checked the type tag or the associated IsType() method.

	bool IsBool() const
		{ return *_type_tag == TRUE_TYPE_TAG || *_type_tag == FALSE_TYPE_TAG; }
	bool AsBool() const;
	bool AsBoolUnchecked() const;

	bool IsNil() const { return *_type_tag == NIL_TYPE_TAG; }
	bool IsInfinitum() const { return *_type_tag == INFINITUM_TYPE_TAG; }

	bool IsInt32() const { return *_type_tag == INT32_TYPE_TAG; }
	int32 AsInt32() const;
	int32 AsInt32Unchecked() const;

	bool IsFloat() const { return *_type_tag == FLOAT_TYPE_TAG; }
	float AsFloat() const;
	float AsFloatUnchecked() const;

	bool IsChar() const { return *_type_tag == CHAR_TYPE_TAG; }
	char AsChar() const;
	char AsCharUnchecked() const;

	bool IsRgbaColor() const { return *_type_tag == RGBA_COLOR_TYPE_TAG; }
	uint32 AsRgbaColor() const;
	uint32 AsRgbaColorUnchecked() const;

	bool IsMidiMessage() const { return *_type_tag == MIDI_MESSAGE_TYPE_TAG; }
	uint32 AsMidiMessage() const;
	uint32 AsMidiMessageUnchecked() const;

	bool IsInt64() const { return *_type_tag == INT64_TYPE_TAG; }
	int64 AsInt64() const;
	int64 AsInt64Unchecked() const;

	bool IsTimeTag() const { return *_type_tag == TIME_TAG_TYPE_TAG; }
	uint64 AsTimeTag() const;
	uint64 AsTimeTagUnchecked() const;

	bool IsDouble() const { return *_type_tag == DOUBLE_TYPE_TAG; }
	double AsDouble() const;
	double AsDoubleUnchecked() const;

	bool IsString() const { return *_type_tag == STRING_TYPE_TAG; }
	const char* AsString() const;
	const char* AsStringUnchecked() const { return _argument; }

	bool IsSymbol() const { return *_type_tag == SYMBOL_TYPE_TAG; }
	const char* AsSymbol() const;
	const char* AsSymbolUnchecked() const { return _argument; }

	bool IsBlob() const { return *_type_tag == BLOB_TYPE_TAG; }
	void AsBlob( const void*& data, osc_bundle_element_size_t& size ) const;
	void AsBlobUnchecked( const void*& data, osc_bundle_element_size_t& size ) const;

	bool IsArrayBegin() const { return *_type_tag == ARRAY_BEGIN_TYPE_TAG; }
	bool IsArrayEnd() const { return *_type_tag == ARRAY_END_TYPE_TAG; }
	std::size_t ComputeArrayItemCount() const;
	
private:
	const char *_type_tag;
	const char *_argument;
};


class ReceivedMessageArgumentIterator{
public:
	ReceivedMessageArgumentIterator( const char *typeTags, const char *arguments )
		: value_( typeTags, arguments ) {}

	ReceivedMessageArgumentIterator operator++()
	{
		Advance();
		return *this;
	}

	ReceivedMessageArgumentIterator operator++(int)
	{
		ReceivedMessageArgumentIterator old( *this );
		Advance();
		return old;
	}

	const ReceivedMessageArgument& operator*() const { return value_; }

	const ReceivedMessageArgument* operator->() const { return &value_; }

	friend bool operator==(const ReceivedMessageArgumentIterator& lhs,
			const ReceivedMessageArgumentIterator& rhs );

private:
	ReceivedMessageArgument value_;

	void Advance();

	bool IsEqualTo( const ReceivedMessageArgumentIterator& rhs ) const
	{
		return value_._type_tag == rhs.value_._type_tag;
	}
};

FINLINE bool operator==(const ReceivedMessageArgumentIterator& lhs,
		const ReceivedMessageArgumentIterator& rhs )
{
	return lhs.IsEqualTo( rhs );
}

FINLINE bool operator!=(const ReceivedMessageArgumentIterator& lhs,
		const ReceivedMessageArgumentIterator& rhs )
{
	return !( lhs == rhs );
}

extern	void	osc_err_missing_argument();
extern	void	osc_err_wrong_argument();
extern	void	osc_err_excess_argument();

class ReceivedMessageArgumentStream{
	friend class ReceivedMessage;
	ReceivedMessageArgumentStream( const ReceivedMessageArgumentIterator& begin,
			const ReceivedMessageArgumentIterator& end )
		: p_( begin )
		, end_( end ) {}

	ReceivedMessageArgumentIterator p_, end_;
	
public:

	// end of stream
	bool Eos() const { return p_ == end_; }

	ReceivedMessageArgumentStream& operator>>( bool& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsBool();
		return *this;
	}

	// not sure if it would be useful to stream Nil and Infinitum
	// for now it's not possible

	ReceivedMessageArgumentStream& operator>>( int32& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsInt32();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( float& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsFloat();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( char& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsChar();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( RgbaColor& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs.value = (*p_++).AsRgbaColor();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( MidiMessage& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs.value = (*p_++).AsMidiMessage();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( int64& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsInt64();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( TimeTag& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs.value = (*p_++).AsTimeTag();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( double& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsDouble();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( Blob& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		(*p_++).AsBlob( rhs.data, rhs.size );
		return *this;
	}
	
	ReceivedMessageArgumentStream& operator>>( const char*& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs = (*p_++).AsString();
		return *this;
	}
	
	ReceivedMessageArgumentStream& operator>>( Symbol& rhs )
	{
		if( Eos() )
		{
			osc_err_missing_argument();				//maa
			return *this;							//maa
			//	throw MissingArgumentException();	//maa
		}
		rhs.value = (*p_++).AsSymbol();
		return *this;
	}

	ReceivedMessageArgumentStream& operator>>( MessageTerminator& rhs )
	{
		if( !Eos() )
		{
			osc_err_excess_argument();							//maa
			//	throw ExcessArgumentException();
		}
	   return *this;
	}
};


class ReceivedMessage{
private:
	const char *	_address_pattern;
	const char *	_type_tags_begin;
	const char *	_type_tags_end;
	const char *	_arguments;
	bool			_b_valid;

	void Init( const char * bundle, unsigned long size );
public:
	explicit ReceivedMessage( const ReceivedPacket& packet );
	explicit ReceivedMessage( const ReceivedBundleElement& bundleElement );

	bool			is_valid()			const { return _b_valid; }
	const char *	AddressPattern()	const { return _address_pattern; }

	// Support for non-standard SuperCollider integer address patterns:
	bool AddressPatternIsUInt32() const;
	uint32 AddressPatternAsUInt32() const;

	uint32 ArgumentCount() const { return static_cast<uint32>(_type_tags_end - _type_tags_begin); }

	const char * TypeTags() const { return _type_tags_begin; }


	typedef ReceivedMessageArgumentIterator const_iterator;

	ReceivedMessageArgumentIterator ArgumentsBegin() const
	{
		return ReceivedMessageArgumentIterator( _type_tags_begin, _arguments );
	}

	ReceivedMessageArgumentIterator ArgumentsEnd() const
	{
		return ReceivedMessageArgumentIterator( _type_tags_end, 0 );
	}

	ReceivedMessageArgumentStream ArgumentStream() const
	{
		return ReceivedMessageArgumentStream( ArgumentsBegin(), ArgumentsEnd() );
	}
};


class ReceivedBundle{
	void Init( const char *message, unsigned long size );
public:
	explicit ReceivedBundle( const ReceivedPacket& packet );
	explicit ReceivedBundle( const ReceivedBundleElement& bundleElement );

	uint64 TimeTag() const;

	unsigned long ElementCount() const { return _element_count; }

	typedef ReceivedBundleElementIterator const_iterator;

	ReceivedBundleElementIterator ElementsBegin() const
	{
		return ReceivedBundleElementIterator( _time_tag + 8 );
	}

	ReceivedBundleElementIterator ElementsEnd() const
	{
		return ReceivedBundleElementIterator( _end );
	}

private:
	const char *	_time_tag;
	const char *	_end;
	unsigned long	_element_count;
};


} // namespace osc

