#include "stroke.h"
#include "primitive.h"
#include "draw/render.h"
#include "file/file_csv.h"
#include "file/file_io.h"
#include "spy.h"

c_stroke::c_stroke()
{
	clear();
}

c_stroke::~c_stroke()
{
}

void c_stroke::clear()
{
	_data.clear();
	_b_data = false;
	_pressure_max = 0;
	_msec_origin = 0;
	_msec_duration = 0;
}

void c_stroke::add( st_stroke_elt CONST & elt)
{
	_data.push_back( elt );
	_pressure_max = MAX( _pressure_max, elt._att[0] );
	_b_data = true;
}

void c_stroke::add( st_stroke_elt CONST & elt, FP32* offset )
{
	add( elt );
	sub_v3( _data[_data.size()-1]._pos, offset );
}

void c_stroke::add_raw( c_stroke CONST & stroke_in )
{
	auto CONST & data_new = stroke_in._data;
	_data.insert( _data.end(), data_new.begin(), data_new.end() );
}

void c_stroke::draw( INT32 prim_to_draw )
{
	INT32 nb = (INT32)_data.size();
	if( nb <= 0 )
		return;
	//todo make it systematic or call directly what is under
	c_prim3::set_vertex( &(_data[0]._pos[0]), sizeof(st_stroke_elt) );
	INT32 CONST index_loc = 1;
	GOL::enable_vertex_attrib_array( index_loc );
	GOL::set_vertex_attrib_pointer( index_loc, 3, GL_FLOAT, &(_data[0]._att[0]), sizeof(st_stroke_elt) );
	GOL::draw_arrays( prim_to_draw, nb );
	GOL::disable_vertex_attrib_array( index_loc );
}

void	c_stroke::save_to_file( o_str CONST & filename )
{
	FILE* file = file_csv::open( filename, true );
		if( !file )
			ERR_PRINT_STRING( "%s : error opening file %s", __FUNCTION__, filename.get() );
		else
		{
			for( auto const elt : _data )
			{
				file_csv::write_int32(		file,	elt._time	);
				file_csv::write_fp32(		file,	elt._pos,	3	);
				file_csv::write_fp32(		file,	elt._att,	3	);
				file_csv::write_int32(		file,	elt._flags	);
				file_csv::write_linefeed(	file	);
			}
		}
	file_csv::close( file );
}

namespace 
{
//	thread_local CHAR line[1024];
	c_file_io*	reader		= nullptr;
}

AAA_ERR	c_stroke_tablet::load_from_file( o_str CONST & filename )
{
	if( !reader )
		reader = new c_file_io;

	AAA_ERR retcode = ERR_FILE_BASE;
	if( NOERR(reader->read_file(filename)) )
	{
		SPY_PUSH_RANGE( "c_stroke::load_from_file", spy::FILE_LOW );
			st_stroke_elt elt;
			reader->skip_space_or_less();
			while( reader->scan_csv_int32( elt._time ) > 0 )
			{
				reader->scan_csv_fp32(	elt._pos,	3	);
				reader->scan_csv_fp32(	elt._att,	3	);
				reader->scan_csv_int32(	elt._flags	);
				add( elt );
			}
			_b_contact = _pressure_max > 0.;
			retcode = AAA_OK;
		SPY_POP_RANGE();
	}
	return retcode;
}


bool c_stroke::is_pressure()
{
	for( auto const elt : get_data() )
	{
		if( elt._att[0] > .0 )
			return true;
	}
	return false;
}

INT32 c_stroke::process_time( INT32 msec_begin )
{
	_msec_origin	= msec_begin;	
	if( _data.empty() )
	{
		_msec_duration	= 0;
		return msec_begin;
	}

	INT32	msec_first = _data[0]._time;
	INT32	msec;
	for( auto & elt : _data )
	{
		msec = elt._time - msec_first;
		if( msec < 0 )
		{
			ERR_PRINT_STRING( "neg time" );
			msec = 0;
		} 
		elt._time = msec;
	}

	_msec_duration	= msec;
	return msec_begin + msec;
}

void c_stroke::offset( REAL CONST * CONST _offset )
{
	for( auto & elt : _data )
		add_v3( elt._pos, _offset );
}

c_stroke_tablet::c_stroke_tablet()
	:_b_contact(false)
{}
c_stroke_tablet::~c_stroke_tablet()
{}

c_strokes_tablet::c_strokes_tablet()
	:_point_nb(0)
	,_msec_duration(0)
{}
c_strokes_tablet::~c_strokes_tablet()
{}

void	c_strokes_tablet::erase( INT32 index_begin,	INT32 index_end )
{
	index_begin = MAX(	index_begin,	0							);
	index_end	= MIN(	index_end,		(INT32)_strokes.size() - 1	);
	INT32 nb = index_end - index_begin + 1;
	if( nb > 0 )
	{
		auto itb = _strokes.begin();
		advance( itb, index_begin );
		auto it = itb;
		do
		{
			auto ps = *it;
			_point_nb -= ps->size();
			delete ps;
			++it;
		}	
		while( --nb );
		_strokes.erase( itb, it );
	}
}

void	c_strokes_tablet::erase_all()
{
	for( auto const p : _strokes )
		delete p;
	_strokes.clear();
	_strokes.push_back( new c_stroke_tablet );
}

void	c_strokes_tablet::erase_last()
{
	if( _strokes.size() > 1 )
	{
		auto ps = _strokes.back();
		_strokes.pop_back();
		_point_nb -= ps->size();
		delete ps;
	}
	else
		erase_all();
}

void	c_strokes_tablet::erase_drawn_last()
{
	while( _strokes.size() )
	{
		auto ps = _strokes.back();
		bool b = ps->_b_contact;
		_point_nb -= ps->size();
		delete ps;
		_strokes.pop_back();
		if( b )
			return;
	}
	erase_all();
}

void	c_strokes_tablet::erase_move_all()
{
	INT32 i = 0;
	for( auto it = _strokes.begin(); it != _strokes.end(); )
	{
		if( !(*it)->_b_contact )
		{
			auto ps = *it;
			_point_nb -= ps->size();
			delete ps;
			it = _strokes.erase(it);
		}
		else
			++it;
	}
}

void	c_strokes_tablet::offset( REAL CONST * CONST _offset )
{
	for( auto p : _strokes )
		p->offset( _offset );
}

void	c_strokes_tablet::process_time()
{
	INT32 msec = 0;
	for( auto const p : _strokes )
	{
		if( p->_b_contact )
			msec = p->process_time( msec );
	}
	_msec_duration = msec;
}