#include "arcom88.h"
#include "ArcBoard.h"

FACTORY_CREATE_V1( c_arcom_88, arcom_PCRELAY8/IN8, Layer, arcom88 );

namespace	n_arcom88
{
	static	CONST	INT32	BASE_PARAM_NB		= 19;
	static	CONST	INT32	GROUP_PARAM_NB		= 0;
	static	CONST	INT32	PARAM_NB_MAX =	BASE_PARAM_NB
										+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		{	NULL,	PARAM_BIT32,	"out",		0, 255,		0, 255,		NULL, NULL },
		{	NULL,	PARAM_BIT32,	"in",		0, 255,		0, 255,		NULL, NULL },

		ST_PARAM_BOOL_OFF( out0 )
		ST_PARAM_BOOL_OFF( out1 )
		ST_PARAM_BOOL_OFF( out2 )
		ST_PARAM_BOOL_OFF( out3 )
		ST_PARAM_BOOL_OFF( out4 )
		ST_PARAM_BOOL_OFF( out5 )
		ST_PARAM_BOOL_OFF( out6 )
		ST_PARAM_BOOL_OFF( out7 )

		ST_PARAM_BOOL_OFF( in0 )
		ST_PARAM_BOOL_OFF( in1 )
		ST_PARAM_BOOL_OFF( in2 )
		ST_PARAM_BOOL_OFF( in3 )
		ST_PARAM_BOOL_OFF( in4 )
		ST_PARAM_BOOL_OFF( in5 )
		ST_PARAM_BOOL_OFF( in6 )
		ST_PARAM_BOOL_OFF( in7 )
	};
}

void	c_arcom_88::param_init_pt()
{
INT32	h=0;

	param_set_pt( h, b_active );
	param_set_pt( h, out_ui );
	param_set_pt( h, in );

	param_set_pt_v( h, b_out, 8 );
	param_set_pt_v( h, b_in, 8 );

	err_param_init_pt(h);

}

void c_arcom_88::init()
{
	param_init_with( n_arcom88::param, n_arcom88::PARAM_NB_MAX ); // arcom88_param, ARCOM88_PARAM_NB_MAX);
}

void c_arcom_88::param_init()
{
	out = 0;
}

CONSTRUCTOR_CREATE(c_arcom_88)
{
INT32	ret;
INT32 addr = 0x180
	base_addr = addr;
	init();
	ret = ARCBOARD_Init();
	//todo should be done once and not by object
	if(ret != AN_SUCCESS)
		{
		ERR_PRINT_STRING( "%ld in ARCBOARD_Init()", ret);
		}
	enable();
}

c_arcom_88::~c_arcom_88()
{
}

void c_arcom_88::enable()
{
INT32	ret;
	ret = PCRELAY8_RelaysEnable (base_addr, AN_ENABLE);
	if(ret != AN_SUCCESS)
	{
		ERR_PRINT_STRING( "%ld in PCRELAY8_RelaysEnable()", ret);
	}
	PCRELAY8_RelayGroupWrite (base_addr, 0);

}

void c_arcom_88::disable()
{
}

void	c_arcom_88::set_out_one( INT32 index)
{
	if( index )
		out_ui = 1<<(index -1);
	else
		out_ui = 0;
}

INT32	c_arcom_88::get_in_just_one()
{
	if( count!=1)
		return 0;
	else
		return num;
}

void	c_arcom_88::update()
{
	if( b_active)
		{
		INT32	ret;
		UCHAR	charin;
		//INT32	i;
		BOOL	b;

		ret = PCRELAY8_OptoInputGroupRead (base_addr, &charin );
		if (ret != AN_SUCCESS)
			{
			ERR_PRINT_STRING( "%ld in PCRELAY8_OptoInputGroupRead()", ret);
			}
		charin = 0xff^charin;
		in = charin;
		count = 0;
		for( INT32 i=0; i<8; ++i )
			{
			b = charin & 1;
			b_in[i] = b;
			if( b )
				{
				count++;
				num = i+1;
				}
			charin >>= 1;
			}
/*		out_ui = 0;
		for ( i=0; i<8; ++i )
			{
			if( b_out[i] )
				out_ui |= 1<<i;
			}
*/
		if( out != out_ui )
			{
			ret = PCRELAY8_RelayGroupWrite (base_addr, (UCHAR)out_ui);
			if(ret != AN_SUCCESS)
				{
				ERR_PRINT_STRING( "%ld in PCRELAY8_RelayGroupWrite()", ret);
				}
			else
				out = out_ui;
			}

		}
}


