#include "obj_ui/com/serial.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
#include "bdd_irtouch.h"


FACTORY_CREATE_PROP_V1( c_bdd_ir_touch, ir_touch, IRTouch, ir_touch, sub_menu="Hardware"; );


namespace n_ir_touch
{
	CONSTEXPR INT32 BASE_PARAM_NB		=	5 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 TRANSFER_PARAM_NB	=	9;
	CONSTEXPR INT32 GROUP_PARAM_NB		=	1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TRANSFER_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_BOOL_OFF(			open )
		PARAM_DEF_BOOL_OFF(			verbose )
		PARAM_DEF_INT32_POS_ONE(	com_port_nb )
		//PARAM_DEF_INT32_POS( com_port_nb_last, 4, 3 )
		//PARAM_DEF_BOOL_OFF( com_port_scan )
		//PARAM_DEF_INT32( com_port_scan_start, 4, 3, 1, 32 )
		//PARAM_DEF_INT32( com_port_scan_stop, 3, 4, 1, 32 )
		PARAM_DEF_INT32(			max_touch, c_bdd_ir_touch::IRT_MAX_TOUCH, 1, 1, c_bdd_ir_touch::IRT_MAX_TOUCH )
		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB )
			PARAM_DEF_REF(				transfer_bdd_target )
			PARAM_DEF_REAL_ONE(			transfer_blob_radius )
			PARAM_DEF_POINT_XYZ(		transfer_center )
			PARAM_DEF_SCALE_XYZF(		transfer_size )

	};
}

void	c_bdd_ir_touch::param_init_pt()
{
	INT32	h = 0;

	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _com_port_nb );
	//param_set_pt( h, _com_port_nb_last );
	//param_set_pt( h, _b_com_port_scan );
	//param_set_pt( h, _com_port_start );
	//param_set_pt( h, _com_port_stop );
	param_set_pt( h, _irt_max_touch );
	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo,		_bdd_target );
		param_set_pt(				h, _transfer_blob_radius	);
		param_set_pt_3(			h, _transfer_translate		);
		param_set_pt_4(			h, _transfer_scale_ui		);

	err_param_init_pt(h);

}

CONSTRUCTOR_CREATE(c_bdd_ir_touch)
{
//	_com_port_nb_last = 0;
	_bdd_target = nullptr;
	_buff_offset = 0;
	param_init_with( n_ir_touch::param, n_ir_touch::PARAM_NB_MAX );
	_serial = nullptr;
	blobs_reset();
}

c_bdd_ir_touch::~c_bdd_ir_touch()
{
	close();
}

void	c_bdd_ir_touch::blobs_reset()
{
	for( UINT32 i = 0; i < IRT_MAX_TOUCH; ++i )
		_b_blobs[ i ] = false;
}

void	c_bdd_ir_touch::analyze_serial( UINT8* read_buffer, INT32 nb )
{
	UINT8*	buf;
	bool	b_end = false;
	UINT32	offset = 0;
	UINT32	nb_all;

	blobs_reset();

	INT32	nb_copy = MIN( nb, IRT_BUF_SIZE - _buff_offset );
	if ( nb_copy < nb )
	{
		ERR_PRINT_STRING( "c_bdd_ir_touch::analyze_serial() : buffer too small, missing %d bytes", nb-nb_copy );
	}
	MEMCPY( _buffer + _buff_offset, read_buffer, nb_copy, __FUNCTION__ );
	nb_all = nb_copy + _buff_offset;
	_buff_offset = 0;
	do
	{
		buf = _buffer + offset;
		if( buf[ 0 ] == 0xAA && buf[ 1 ] == 0 && buf[ 2 ] == 0 && buf[ 3 ] == 0xFF )
		{
			blobs_reset();
			if( _b_verbose )
				GOOD_PRINT_STRING( "IRTouch : Touch End" );
			offset += 4;
		}
		else if( buf[ 0 ] == 0xAA )
		{
			UINT32	nb_x, nb_y;
			UINT32	packet_length;

			nb_x =buf[ 1 ];
			nb_y = buf[ 2 ];
			nb_x = MIN( nb_x, _irt_max_touch );
			packet_length = ( nb_x * 4) + (nb_y * 4) + 4;

			// check if we have enough data left
			if( packet_length > ( nb_all - offset ) )
			{
				// not enough data, copy to buffer
				MEMCPY( _buffer, _buffer + offset, nb_all - offset, __FUNCTION__ );
				_buff_offset = nb_all - offset;
				return;
			}
			blobs_reset();

			// check CRC ??
			for( UINT32 k = 0; k < nb_x; ++k )
			{
				REAL	x, y;
				REAL	sx, sy;
				UINT32	offset_x = 3 + k * 4;
				UINT32	offset_y = 3 + ( nb_x * 4 ) + k * 4;
				x  = ( ( buf[ offset_x ] )     + ( buf[ offset_x + 1 ] << 8 ) ) / REAL(4095);
				y  = ( ( buf[ offset_y ] )     + ( buf[ offset_y + 1 ] << 8 ) ) / REAL(4095);
				sx = ( ( buf[ offset_x + 2 ] ) + ( buf[ offset_x + 3 ] << 8 ) ) / REAL(4095);
				sy = ( ( buf[ offset_y + 2 ] ) + ( buf[ offset_y + 3 ] << 8 ) ) / REAL(4095);
				_blobs[ k ].set_quick( k, x,y, sx,sy );
				//_blobs[ k ]._center.set( x, y );
				_b_blobs[ k ] = true;
				if( _b_verbose )
				{
					GOOD_PRINT_STRING( "Finger %d at x = %.4f, y = %.4f, sx = %.4f, sy = %.4f", k, x, y, sx, sy );
				}
			}
			offset += packet_length;
		}
		else if( buf[ 0 ] == 0x55 && buf[ 3 ] == 0xAA )
		{
			blobs_reset();
			if( _b_verbose )
			{
				GOOD_PRINT_STRING( "IRTouch : Touch End" );
			}
			offset += 4;
		}
		else
		{
			b_end = true;
			if( _b_verbose )
			{
				ERR_PRINT_STRING( "IRTouch : unknown packet" );
			}
		}
		if( offset >= nb_all )
			b_end = true;
	}
	while( !b_end );

	return;
}

void	c_bdd_ir_touch::draw()
{
}

void	c_bdd_ir_touch::transfer_blobs_to( BLOBS_CONT& blobs )
{
	REAL	pos_blob[ 2 ];
	REAL	sca[ 3 ];
	REAL	pos[ 2 ];		//todonow extend to 3
	c_blob	blob;

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );

	for( UINT32 i = 0; i < _irt_max_touch; ++i )
	{
		if( _b_blobs[ i ] )
		{
			pos_blob[ 0 ] = _blobs[ i ]._center.get_x();
			pos_blob[ 1 ] = _blobs[ i ]._center.get_y();

			add_mul_v2r( pos, _transfer_translate, pos_blob, sca );

			blob.set_quick( _blobs[ i ].get_id(), pos[ 0 ], pos[ 1 ], _transfer_blob_radius );
			blobs.push_back( blob );
		}
	}
}

void	c_bdd_ir_touch::update_low()
{
	INT32	nb = _serial->read_get_nb_ready();
	if( nb != 0 )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "c_bdd_ir_touch::update_low(), read %d bytes from com port", nb );
		if( nb > IRT_BUF_SIZE )
			ERR_PRINT_STRING( "c_bdd_ir_touch::update_low(), buffer too small when reading from COM port" );
		nb = MIN( IRT_BUF_SIZE, nb );
		if ( NOERR( _serial->read( _buf_read, nb ) ) )
		{
			_buf_read[ nb ] = 0;
			analyze_serial( _buf_read, nb );
		}
	}
}

void	c_bdd_ir_touch::update()
{
	if( is_active() )
	{
		if( _b_open_ui )
		{
			if( !_serial )
				open();
			if( _serial )
				update_low();
			if( _serial )
			{
				if( !_target_name_symbo.is_empty() )
				{
					if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
						_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo );
				}
				else
					_bdd_target = nullptr;
				if( _bdd_target )
					((c_bdd_blob*)_bdd_target)->register_as_src( this );
			}
		}
		else
		{
			blobs_reset();
			if( _serial )
				close();
		}
	}
	else
		blobs_reset();

}

void	c_bdd_ir_touch::close()
{
	if( _serial )
	{
		_serial->close();
		obj_delete( _serial );
	}
}

void	c_bdd_ir_touch::open()
{
	if( !_serial )
	{
		bool	b_open = false;
		_serial = new c_serial;
		if( _serial )
		{
			_serial->set_baudrate( c_serial::BAUDRATE_38400 );
			//UINT32	start, stop;
			//start = stop = _com_port_nb;
			if( NOERR( _serial->open( _com_port_nb ) ) )
				b_open = true;

			//if ( _b_com_port_scan )
			//{
			//	start = MIN( _com_port_start, _com_port_stop );
			//	stop = MAX( _com_port_start, _com_port_stop );
			//	// try last com port used first, might speed up things...
			//	if( _com_port_nb_last > 0 && NOERR( _serial->open( _com_port_nb_last ) ) )
			//	{
			//		_b_open = true;
			//	}
			//}
			//if ( !_b_open )
			//{
			//	for( UINT32 i = start; i <= stop; ++i )
			//	{
			//		if( _com_port_nb_last != i && NOERR( _serial->open( i ) ) )
			//		{
			//			_com_port_nb_last = i;
			//			_b_open = true;
			//			break;
			//		}
			//	}
			//}
		}
		if( !b_open )
		{
			err_print( "Can't start IRTouch on COM %d", _com_port_nb );
			close();
		}
	}
}
