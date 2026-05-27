#include "midi_patch.h"
#include "midi_data.h"
#include "file/file_csv.h"

static	CHAR	str_midi[256];	//todo	do better

c_midi_patcher_in::c_midi_patcher_in()
{
		init();
}

void c_midi_patcher_in::init()
{
	for( INT32 ctl=0; ctl<128; ++ctl )
		for( INT32 ch=0; ch<16; ++ch )
		{
			_data[ctl][ch][CH_INDEX]	= ch  + 1; 
			_data[ctl][ch][CTL_INDEX]	= ctl; 
		}
}
void c_midi_patcher_in::patch( INT32& ch_id, INT32& ctl_index )	//	ch are 1 to 16 controller/note 1 to 128
{
	INT32* data = _data[ctl_index][ch_id-1];
	//	we output starting at 1	(midi/lua style)
	ch_id  = data[CH_INDEX];
	ctl_index = data[CTL_INDEX];
}
AAA_ERR	c_midi_patcher_in::load_data( C_PCHAR_C filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( "midi_control_patch.csv" );
		if( file_csv::read_int32_with_hole( filename.get(), &_data[0][0][0], 16*2, 128 ) != 128*16*2 )
			init();
	o_str::pop_name();
	return AAA_OK;
}

c_midi_patch::c_midi_patch( INT32 ch_in, INT32 ctl_in, INT32 ch_dst_in, INT32 ctl_dst_in )
{
	_ch			= ch_in;
	_ctl		= ctl_in;
	_ch_dst		= ch_dst_in;
	_ctl_dst	= ctl_dst_in;
}

/*
c_midi_patch& c_midi_patch::operator=( CONST c_midi_patch& obj ) 
{
//	if ( this == &obj ) return *this;

	ch		= obj.ch;
	ctl		= obj.ctl;
	ch_dst	= obj.ch_dst;
	ctl_dst = obj.ctl_dst;

	return *this;	
}
*/

CHAR*	c_midi_patch::print( CHAR* str ) CONST	{	return str += sprintf( str, "%d, %d, %d, %d", _ch, _ctl, _ch_dst, _ctl_dst );	}
bool	c_midi_patch::scan( CHAR* str )			{	return sscanf( str, "%d, %d, %d, %d", &_ch, &_ctl, &_ch_dst, &_ctl_dst ) == 4;	}

/*
c_midi_patchs& c_midi_patchs::operator=( CONST c_midi_patchs& obj ) 
{
list<c_midi_patch>::const_iterator		i;
	patches.clear();
	for( i = obj.patches.begin(); i != obj.patches.end(); ++i )
		patches.push_back( *i );

	return *this;	
}
*/

//c_midi_patchs::c_midi_patchs()
//{
//}
//todo optimize by keeping double array if pointer in vector
//todo add name to c_midi_patchs and use it in messages
FINLINE void	c_midi_patchs::add( c_midi_patch CONST & patch )
{
//	c_midi::CLASS_PRINT_STRING( "Patch %d/%d to %d/%d", patch._ch, patch._ctl, patch._ch_dst, patch._ctl_dst );
	_patches.push_back( patch );
}

void	c_midi_patchs::add( INT32 CONST ch, INT32 CONST ctl_index, INT32 CONST ch_dst, INT32 CONST ctl_index_dst )
{
	begin_find();
	if( find( ch, ctl_index, ch_dst, ctl_index_dst ) )
		DBG_PRINT_STRING( "Midi : already patched %d/%d to %d/%d : skipping", ch, ctl_index, ch_dst, ctl_index_dst );
	else
		add( c_midi_patch( ch, ctl_index, ch_dst, ctl_index_dst ) );
}


void	c_midi_patchs::begin_find()
{
	_it = _patches.begin();
}

c_midi_patch*	c_midi_patchs::find( INT32 CONST ch, INT32 CONST ctl, INT32 CONST ch_dst, INT32 CONST ctl_dst )
{
	while( _it != _patches.end() )
	{
		c_midi_patch* pt = &( *_it );
		++_it;
		if( pt->is_src( ch, ctl ) && pt->is_dst( ch_dst, ctl_dst ) )	
			return pt;
	}
	return nullptr;
}
c_midi_patch*	c_midi_patchs::find_using( INT32 CONST ch, INT32 CONST ctl )
{
	while( _it != _patches.end() )
	{
		c_midi_patch* pt = &( *_it );
		++_it;
		if( pt->is_using( ch, ctl ) )	
			return pt;
	}
	return nullptr;
}
bool	c_midi_patchs::find_and_get_other( INT32 CONST ch, INT32 CONST ctl, INT32& ch_out, INT32& ctl_out )
{
	while( _it != _patches.end() )
	{
		c_midi_patch* pt = &( *_it );
		++_it;
		if( pt->is_src( ch, ctl ) )
		{
			pt->get_dst( ch_out, ctl_out );
			return true;
		}
		else if( pt->is_dst( ch, ctl ) )
		{
			pt->get_src( ch_out, ctl_out );
			return true;
		}
	}
	return false;
}
bool	c_midi_patchs::remove( INT32 CONST ch, INT32 CONST ctl )
{
	bool	b_done = false;
	for( auto it = _patches.begin(); it != _patches.end(); )
	{
		if( it->is_using( ch, ctl ) )
		{
			it = _patches.erase( it );	// it = make sure we can go on
			b_done = true;
		}
		else
			++it;
	}
	return b_done;
}

void	c_midi_patchs::print()
{
	for( auto const & patch : _patches )
	{
		patch.print( str_midi );
//		c_midi::CLASS_PRINT_STRING( "patch( %s );", str_midi );
	}
}

void	c_midi_patchs::save( FILE* file )
{
	for( auto const & patch : _patches )
	{
		patch.print( str_midi );
		fprintf( file, "add_patch( %s );\n", str_midi );
	}
}

void	c_midi_patchs::add( CHAR* str )
{
	c_midi_patch	patch;
	if( patch.scan( str ) )
		add( patch );
}


namespace
{
	bool	b_midi_auto_patch = false; 
	bool	b_midi_auto_depatch = false;

	INT32	ch_last = 1;
	INT32	ctl_last = 1;
	INT32	ch_to_patch = 1;
	INT32	ctl_to_patch = 1;

	void	midi_patch_store()
	{
		ch_to_patch = ch_last;
		ctl_to_patch = ctl_last;
	}
}


void	midi_trig_auto_patch()
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	if( b_midi_auto_patch )
	{
		b_midi_auto_patch = false;
		c_midi::CLASS_PRINT_STRING( "AutoPatch( %d,%d ) Canceled", ch_to_patch, ctl_to_patch );
	}
	else
	{
		b_midi_auto_patch = true;
		midi_patch_store();
		c_midi::CLASS_PRINT_STRING( "AutoPatch( %d,%d ) -> Waiting for midi input", ch_to_patch, ctl_to_patch );
	}
}

void	midi_trig_auto_depatch()
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	if( b_midi_auto_depatch )
	{
		b_midi_auto_depatch = false;
		c_midi::CLASS_PRINT_STRING( "AutoDepatch Canceled" );
	}
	else
	{
		b_midi_auto_depatch = true;
		midi_patch_store();
		c_midi::CLASS_PRINT_STRING( "AutoDepatch -> Waiting for midi input" );
	}
}

void midi_update_auto_patch( INT32 channel_id, INT32 control_id )
{
	if( b_midi_auto_patch )
	{
		if( channel_id == ch_to_patch && control_id == ctl_to_patch )
		{
			ERR_PRINT_STRING( "AutoPatch( %d,%d ) to self refused", ch_to_patch, ctl_to_patch );
		}
		else
		{
			c_midi_universe::cur->_midi_data.get_patchs()->add( channel_id, control_id, ch_to_patch, ctl_to_patch );
			c_midi::CLASS_PRINT_STRING( "AutoPatch( %d,%d ) <-> ( %d,%d )  Done ", ch_to_patch, ctl_to_patch, channel_id, control_id );
		}
		b_midi_auto_patch = false;
	}
	if( b_midi_auto_depatch )
	{
		if( c_midi_universe::cur->_midi_data.get_patchs()->remove( channel_id, control_id ) )
			c_midi::CLASS_PRINT_STRING( "AutoDepatch( %d,%d ) Done ", channel_id, control_id );
		else
			ERR_PRINT_STRING( "AutoDepatch( %d,%d ) Failed no patch on these", channel_id, control_id );

		b_midi_auto_depatch = false;
	}
	ch_last = channel_id;
	ctl_last = control_id;
}