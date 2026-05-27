#include "strsymbo.h"


CONSTEXPR C_PCHAR_C	gstr::xyz[3] =
{
	"X",
	"Y",
	"Z"
};

CONSTEXPR C_PCHAR_C	gstr::rot_order[6] =
{
	"XYZ",
	"YZX",
	"ZXY",
	"XZY",
	"YXZ",
	"ZYX"
};

CONSTEXPR C_PCHAR_C	gstr::uva[3] =
{
	"U",
	"V",
	"AXE"
};

CONSTEXPR C_PCHAR_C	gstr::dim[4] =
{
	"0D",
	"1D",
	"2D",
	"3D"
};

CONSTEXPR C_PCHAR_C	gstr::dim_prim[3] =
	{
	"Point",
	"Line",
	"Plane"
	};

CONSTEXPR C_PCHAR_C	gstr::current[2] =
{
	"CURRENT",
	"Owner"
};


CONSTEXPR C_PCHAR_C	gstr::borrow[3] =
{
	"CURRENT",
	"none",
	"Owner"
};

CONSTEXPR C_PCHAR_C gstr::borrow_once[4] =
{
	"CURRENT",
	"none",
	"Owner",
	"Once",
};
//CONSTEXPR C_PCHAR_C	gstr::borrow_main[3] =
//{
//	"CURRENT",
//	"main",
//	"Owner"
//};

CONSTEXPR C_PCHAR_C	gstr::p2[17] =
{
	"1",
	"2",
	"4",
	"8",

	"16",
	"32",
	"64",
	"128",

	"256",
	"512",
	"1024",
	"2048",

	"4096",
	"8192",
	"16384",
	"32768",
	
	"65536"
};

CONSTEXPR C_PCHAR_C	gstr::hint[3] =
{
	"GL_DONT_CARE",
	"GL_FASTEST",
	"GL_NICEST"
};

CONSTEXPR C_PCHAR_C	gstr::test_func[8] =
{
	"NEVER",
	"ALWAYS",
	"GREATER",
	"GREATER_OR_EQUAL",
	"EQUAL",
	"LESS_OR_EQUAL",
	"LESS",
	"NOT_EQUAL"
};

CONSTEXPR C_PCHAR_C	gstr::draw_number[8] =
{
	"No",
	"X",
	"Y",
	"Z",
	"Billboard",
	"X Rotate",
	"Y Rotate",
	"Z Rotate"
};

CONSTEXPR C_PCHAR_C	gstr::no[1] =
{
	"No"
};


CONSTEXPR C_PCHAR_C	gstr::front_and_back[3] =
{
	"BACK",
	"FRONT_AND_BACK",
	"FRONT",
};

//CONSTEXPR C_PCHAR_C	gstr::default[1] =
//{
//	"Default"
//};
