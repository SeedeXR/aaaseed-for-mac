#include "aaa_os.h"
#include "err.h"


//bool dmxcard_ok = false;
HINSTANCE hLibrary;

typedef int (FAR WINAPI *FARPROC2)( INT32*, INT32*);
typedef int (FAR WINAPI *FARPROC3)( INT32*, INT32*, INT32*);
typedef int (FAR WINAPI *FARPROC6)( INT32*, INT32*, INT32*, INT32*, INT32*, INT32*);
FARPROC2 fn2;
FARPROC3 pfn3;
FARPROC3 pdmxinit;
FARPROC3 pdmxsense;
FARPROC6 fn6;

INT32 add;
INT32 dmxsize;
INT32 dmxmst;
INT32 dmxstart;
INT32 dmxstatus;
INT32 dmxtype;	//maa added

INT32 dmxhard;
INT32 dmxrev;
INT32 dmxsubrev;
INT32 serialno;

bool dmx_init_card()
{
//	add = 0xe18; // address obtained using driver base address modulo 256
//	add = 0x7b0; // address obtained using driver base address modulo 256
//	add = 0x101b0; // address obtained using driver base address modulo 256
//	add = 0xfff1b0; // address obtained using driver base address modulo 256
	add = 0xf1b0; // address obtained using driver base address modulo 256

//	hLibrary = LoadLibraryW( L"avicap32.dll" );
//	hLibrary = LoadLibraryW( L"dmxplus.dll" );
//	hLibrary = LoadLibraryW( L"DasHard.dll" );
	hLibrary = LoadLibraryW( L"dmxcrd32.dll" );

	if( !hLibrary )
		return false;
	dmxsize = 512;
	dmxmst = 1; // set to default initial state //
	pfn3 = (FARPROC3) GetProcAddress(hLibrary, "DMXINIT");
	(*pfn3) ( &add, &dmxsize, &dmxmst); // setup DLL variables //
	//DMXINIT (&add,&dmxsize,&dmxmst); // setup DLL variables //

	dmxstatus = 0;
	dmxtype = 0;
	pdmxsense = (FARPROC3) GetProcAddress(hLibrary, "DMXSENSE");
	(*pdmxsense) ( &add, &dmxstatus, &dmxtype); // setup DLL variables //
	//DMXSENSE (&add,&dmxstatus,&dmxtype); // setup DLL variables //
	if(dmxstatus != 0)
		BOX_ERR( "dmx sense failed" );
		//return false;

	dmxstart = 0;
	pfn3 = (FARPROC3) GetProcAddress(hLibrary, "DMXDRIVE");
	(*pfn3) ( &add, &dmxstart, &dmxstatus); // setup DLL variables //
	//DMXDRIVE(&lpt,&dmxstart,&dmxstatus); // enable DMX line driver //
	if(dmxstatus != 0)
		return false;

/*	dmxstart = 1;
	pfn3 = (FARPROC3) GetProcAddress(hLibrary, "DMXDRIVE2");
	(*pfn3) ( &add, &dmxstart, &dmxstatus); // setup DLL variables //
	//DMXDRIVE(&lpt,&dmxstart,&dmxstatus); // enable DMX line driver //
	if(dmxstatus != 0)
		return false;
*/
	fn6 = (FARPROC6) GetProcAddress(hLibrary, "DMXIDENT");
	(*fn6) (&add, &dmxstatus, &dmxhard, &dmxrev, &dmxsubrev, &serialno);
	//DMXIDENT (&add,&dmxstatus,&dmxhard,&dmxrev,&dmxsubrev,&serialno) //

	if(dmxstatus != 0)
		return false;


/*
	fn3 = (FARPROC3) GetProcAddress(hLibrary, "DMXDRIVE");
	(*fn3) (&add,&dmxstart,&dmxstatus); // setup DLL variables //
	//DMXDRIVE(&add,&dmxstart,&dmxstatus); // enable DMX line driver //

	if(dmxstatus != 0)
		return false;
*/
	return true;

}

//void dmx_card( unsigned char *bloc, int channels)
void dmx_card()
{
	int dmxchan[512];
//	INT32	i;
/*
	for( i=0; i<512; ++i )
		if(i<channels)
			dmxchan[i] = (int) bloc[i];
		else
			dmxchan[i] = (int) 0;
*/
	fn2 = (FARPROC2) GetProcAddress(hLibrary, "DMXSEND");
	(*fn2) ( &add, &dmxchan[0]); // setup DLL variables //
	//DMXSEND(&add,&dmxchan[0]); // send new channels on DMX line //

// last packet sent is automatically refreshed //
// on DMX line by PCMCIA Card hardware //
}

void	dmx_init()
{
	dmx_init_card();
	dmx_card();
}