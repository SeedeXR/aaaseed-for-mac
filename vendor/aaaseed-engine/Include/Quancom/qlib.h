/*
Program QLIB.H  Version: 1.9.8q

About   Include-File fuer QLIBDOS/QLIB16/QLIB32

Todo: - Update ERROR_* Codes in other Header Files
*/

//@0s

#ifndef __QLIB_H
#define __QLIB_H

#if defined (__BORLANDC__)															//	Borland C++ 5.0 und hoeher

	#define QLIB_BORLAND
	#pragma message ("used compiler:   Borland C++ (Builder)")

	#if defined (__WIN32__)																//	Windows 9x / NT (32-Bit)

	#define QLIB_WIN_32

	#if defined (__DLL__)																//	DLL

		#define QLIB_DLL

	#define QAPICALLER __stdcall
	#pragma message	("destination OS:  Windows 9x / NT / 2000 / XP")
		#pragma message ("executable type: DLL")

	#else																								//	EXE

		#define QLIB_EXE

	#define QAPICALLER __stdcall
	#pragma message	("destination OS:  Windows 9x /  NT / 2000 / XP")
		#pragma message ("executable type: EXE")

	#endif

	#elif defined (_Windows)    													//	Windows 3.1			(16-Bit)

	#define QLIB_WIN_16

	#if defined (__DLL__) && !defined(FORCE_WIN31_EXE)

	  #define QLIB_DLL                                  //  DLL

		#define QAPICALLER _far _pascal _loadds _saveregs _export
	  #pragma message	("destination OS:  Windows 3.1x")
	  #pragma message ("executable type: DLL")

	#else

	  #define QLIB_EXE                                  //  EXE oder DLL Import

		#define QAPICALLER _far _pascal _loadds _saveregs
	  #pragma message	("destination OS:  Windows 3.1x")
	  #pragma message ("executable type: EXE/DLL import")

	#endif

  #elif defined (__MSDOS__) && defined (__LARGE__)			//	MS-DOS					(16-Bit)

	#define QLIB_DOS

	#define QAPICALLER
	#pragma message	("destination OS:  MS-DOS")
	#pragma message ("memory model:    LARGE")
	#pragma message ("executable type: LIB / EXE")

  #else

	#error unknown operating system

  #endif

#elif	defined (_MSC_VER)

	#define QLIB_MSC
	#pragma message ("used compiler:   Microsoft Visual C++")

	#if defined (_WIN32)																	//	Windows 9x / NT (32-Bit)

	#define QLIB_WIN_32

	#if defined (_DLL)																	//	DLL

		#define QLIB_DLL

		#define QAPICALLER _stdcall
		#pragma message	("destination OS:  Windows 9x / NT / 2000 / XP")
	  #pragma message ("executable type: DLL")

	#else																								//	EXE

		#define QLIB_EXE

		#define QAPICALLER _stdcall
		#pragma message	("destination OS:  Windows 9x / NT / 2000 / XP")
	  #pragma message ("executable type: EXE")

		#endif

  #elif defined (_WINDOWS)															//	Windows 3.1			(16-Bit)

	#define QLIB_WIN_16

		#define QAPICALLER 	_far _pascal _loadds _saveregs
	#pragma message	("destination OS:  Windows 3.1x")
	#pragma message ("executable type: DLL / EXE")

  #elif defined (_MSDOS) && defined (__LARGE__)			    //	MS-DOS					(16-Bit)

	#define QLIB_DOS

	#define QAPICALLER
	#pragma message	("destination OS:  MS-DOS")
	#pragma message ("memory model:    LARGE")
	#pragma message ("executable type: LIB / EXE")

  #else

	#error unknown operating system

  #endif

#elif defined (_CVI_)

	#define QLIB_MSC
	#define QLIB_CVI 
	
	#pragma message ("used compiler:   CVI LabWindows C")
	
	#define QLIB_WIN_32
	#define QAPICALLER _stdcall
	#define FAR
	#pragma message	("destination OS:  Windows 9x / NT / 2000 / XP")
	#pragma message ("executable type: CVI")

#else

	#error unsupported compiler detected									//	unbekannter Compiler

#endif

//@0e


/*
**	Debugging-Konstanten fuer QAPIExt...-Funktionen
*/

//@1s

#define DBG_NONE 		   0L	/* nichts debuggen		*/
#define DBG_NULL_PTR	   1L	/* NULL-Pointer pruefen		*/
#define DBG_FUNC_DEFINED   2L	/* gueltige Funktionen pruefen	*/
#define DBG_VALID_CARD_ID  4L	/* gueltige Karten-ID pruefen	*/
#define DBG_MAKE_LOG       8L	/* Log-File erzeugen            */
#define DBG_WINDOW_LOG	   16L  /* Output to Logger Window      */
#define DBG_FULL_DEBUG     15L	/* alles pruefen und aufzeichnen*/

//@1e


/*
**	allgemeine Konstanten
*/

//@2s

#define BUS_ISA		  1
#define	BUS_PCI		  2
#define BUS_VLB		  3
#define BUS_PAR		  4
#define BUS_USB		  5
#define BUS_UNKNOWN   6

#define FEATURE_AD			0x00000001L     			/*  Karte besitzt AD-Kanaele      */
#define	FEATURE_DA			0x00000002L     			/*  Karte besitzt DA-Kanaele      */
#define FEATURE_DI			0x00000004L     			/*  Karte besitzt Digitaleingaenge*/
#define FEATURE_DO			0x00000008L     			/*  Karte besitzt Digitalausgaenge*/
#define FEATURE_DX			0x00000010L     			/*  Karte besitzt beliebige DI/Os */
#define FEATURE_WD			0x00000020L     			/*  Karte besitzt Watchdog        */
#define FEATURE_CT			0x00000040L     			/*  Karte besitzt Counter/Timer   */
#define FEATURE_8253		0x00000080L            		/*  Karte besitzt 8253/54         */
#define FEATURE_8255		0x00000100L					/*  Karte besitzt 8255            */
#define FEATURE_WD_ST		0x00000200L					/*	Karte besitzt Watchdog Status Register */
#define FEATURE_READSTRING  0x00000400L					/*  Karte kann String lesen */
#define FEATURE_WRITESTRING 0x00000800L					/*  Karte kann String schreiben */

#define LIST_DEFAULT_VALUE  		0x80000000L		/*	Resourcenwert ist Defaultwert						*/
#define LIST_RESOURCE_NOT_USED		0x40000000L		/*	Resource wird nicht verwendet						*/
#define	LIST_AUTO_RESOURCE			0x20000000L		/*	Resourcenwert wird auto. vergeben (PCI)	*/
#define LIST_EOL					0x00000000L		/*	Ende der Liste													*/

#define VC_QIS		2L			/*	Hersteller-ID: QUANCOM Informationssysteme GmbH	*/
#define VC_KOLTER	3L      	/*	Hersteller-ID: Kolter Electronic				*/

//@2e

/*
**	Constants for the "Mode" parameter i.e. QAPIExtReadAD, QAPIExtConvertDWToVoltage
*/

/* Voltages */

//@2s

#define MODE_BI_20V				10L
#define MODE_BI_10V				 1L
#define MODE_BI_5V				 0L
#define	MODE_BI_3V3				 2L
#define MODE_BI_2V5              6L
#define MODE_BI_2V		        12L
#define MODE_BI_1V25             7L
#define MODE_BI_1V	            14L
#define MODE_BI_0V625           16L
#define MODE_BI_0V5             18L

#define	MODE_UNI_20V            11L
#define	MODE_UNI_10V             3L
#define	MODE_UNI_5V              4L
#define	MODE_UNI_3V3             5L
#define MODE_UNI_2V5             8L
#define MODE_UNI_2V		        13L
#define MODE_UNI_1V25            9L
#define MODE_UNI_1V             15L
#define MODE_UNI_0V625          17L
#define MODE_UNI_0V5            19L

#define MODE_SINGLE_ENDED		0x20000L
#define MODE_DIFFERENTIAL_ENDED	0x10000L

//@2e

//  Current

//@2s

#define MODE_0_TO_20MA        100L
#define MODE_4_TO_20MA        101L

//@2e

	//  Temperature

//@2s

#define MODE_0_TO_100_DEGREE  200L

//@2e

	//  Filter (darf mit zuvor genannten Werten ODER-verknuepft werden

//@2s

#define MODE_FILTER           0x80000000L

//@2e

	// Counter 

//@2s

#define MODE_DEFAULT					0x00000000L
#define MODE_RESET_COUNTER_ON_READ		0x00000001L
#define MODE_ENABLE_COUNTER_HW_GATE		0x00000002L

/*
**  Fehlermeldungen und Sprache ( mr. 17.04.2002 )
*/

//@2s

#define QAPI_MESSAGES_ON     0
#define QAPI_MESSAGES_OFF    1

//@2e


/*
**	Jobs
*/

//@3s

#define	JOB_READ_8255			      0
#define	JOB_WRITE_8255			      1

#define	JOB_ENABLE_WATCHDOG		      2
#define	JOB_DISABLE_WATCHDOG  	      3
#define JOB_RETRIGGER_WATCHDOG	      4
#define JOB_STATUS_WATCHDOG           72
#define JOB_RELAYON_WATCHDOG		  93

#define JOB_READ_8253                 5
#define JOB_WRITE_8253                6

#define	JOB_ENABLE_IRQ				  7
#define JOB_DISABLE_IRQ				  8
#define JOB_ENABLE_IRQ_A			  7
#define JOB_DISABLE_IRQ_A			  8
#define JOB_ENABLE_IRQ_B			  9
#define JOB_DISABLE_IRQ_B			  10

#define JOB_IOREAD_BYTE 			  11
#define JOB_IOREAD_WORD				  12
#define JOB_IOREAD_LONG				  13
#define JOB_IOWRITE_BYTE              14
#define JOB_IOWRITE_WORD              15
#define JOB_IOWRITE_LONG              16

#define JOB_RESET_IN_FFS              17
#define JOB_READ_IN_FFS               18
#define JOB_ENABLE_IN_FFS			  84
#define JOB_DISABLE_IN_FFS			  89

#define JOB_ENABLE_TIMEOUT            19
#define JOB_DISABLE_TIMEOUT           20
#define JOB_RESET_TIMEOUT_STATUS      21
#define JOB_READ_TIMEOUT_STATUS       22
#define JOB_SET_WATCHDOG_TIME		  24

#define JOB_READ_DIP_SWITCH			  85
#define JOB_SET_LED					  88

#define JOB_READ_BACK_RELAYS		  123	

#define JOB_WDOG3_INITIALIZE          23
#define JOB_WDOG3_SET_WATCHDOG_TIME   24
#define JOB_WDOG3_SET_RELAIS_TIME     25
#define JOB_WDOG3_SET_REPEAT_TIME     26
#define JOB_WDOG3_RELAIS_INVERSION    27

/* mr 20.10.2000 */

#define JOB_DOWNLOAD				28L

/* mr 22.10.2000 Jobs für UNITIMER */

#define JOB_UNITIMER_RELAIS1		29L
#define JOB_UNITIMER_RELAIS2		30L
#define JOB_UNITIMER_OUT0			31L
#define JOB_UNITIMER_OUT1			32L
#define JOB_UNITIMER_OUT2			33L
#define JOB_UNITIMER_OUT3			34L
#define JOB_UNITIMER_OUT4			35L
#define JOB_UNITIMER_OUT5			36L
#define JOB_UNITIMER_OUT6			37L
#define JOB_UNITIMER_OUT7			38L	
#define JOB_UNITIMER_GETLCAREG		39L
#define JOB_UNITIMER_SETLCAREG		40L
#define JOB_UNITIMER_GETCNTA		41L
#define JOB_UNITIMER_GETCNTB		42L
#define JOB_UNITIMER_GETCNTAB		43L	
#define JOB_UNITIMER_GETCTREG		44L
#define JOB_UNITIMER_SETCTREG		45L
#define JOB_UNITIMER_INITIALIZE		46L
#define JOB_UNITIMER_SETCNTMODE		47L
#define JOB_UNITIMER_UNIT7			48L

/* mr 27.11.2000 Jobs für PAR48IO */

#define JOB_PAR48IO_INPUT			0L
#define JOB_PAR48IO_OUTPUT			1L
#define JOB_PAR48IO_READ_DIRECTION	2L

#define JOB_PAR48IO_LATCH			49L
#define JOB_PAR48IO_READCNT			50L
#define JOB_PAR48IO_SETMODE			51L
#define JOB_PAR48IO_WRITE			52L
#define JOB_PAR48IO_READ			54L

#define JOB_PAR48IO_RESET_TO_0		58L		
#define JOB_PAR48IO_RESET_TO_1		59L
#define JOB_PAR48IO_IOMODE0_7		60L
#define JOB_PAR48IO_IOMODE00_07		60L
#define JOB_PAR48IO_IOMODE8_15		61L
#define JOB_PAR48IO_IOMODE08_15		61L
#define JOB_PAR48IO_IOMODE16_23		62L
#define JOB_PAR48IO_IOMODE24_31		63L
#define JOB_PAR48IO_IOMODE32_39		64L
#define JOB_PAR48IO_IOMODE40_47		65L



/* mr 29.11.2000 Jobs für OPTOLCA */
			
#define JOB_OPTOLCA_SETEXTRAMEMORY	66L
#define JOB_OPTOLCA_GETEXTRAMEMORY	67L
#define JOB_OPTOLCA_SM_START		68L
#define JOB_OPTOLCA_SM_STOP		69L
#define JOB_OPTOLCA_SM_SSR		70L

/* mr 22.04.2002 Jobs für GPIB, PCIGPIB und USBGPIB */

#define JOB_REGW			73L
#define JOB_REGR			74L
#define JOB_READSRQ			75L
#define JOB_SERIALPOLL		76L
#define JOB_GTL				77L	
#define JOB_GET				78L
#define JOB_SDC				79L
#define JOB_LLO				80L	
#define JOB_DCL				81L
#define JOB_REN				90L
#define JOB_RESET			91L
#define JOB_TIMEOUT			92L
#define JOB_READ_TIMEOUT	127L

/*  Jobs für PAR2DA Modul mr 03.06.2002 */

#define JOB_PAR2DA_ENABLE1     82L 
#define JOB_PAR2DA_ENABLE2     83L 

/*  Jobs für schnellen Memory Zugriff mr 20.10.2002  */

#define JOB_FASTMEM_INIT		86L 
#define JOB_FASTMEM_RELEASE		87L 

/* mr 25.11.2002 Jobs für PCITTL32IO */

#define JOB_PCITTL32_INPUT			0L
#define JOB_PCITTL32_OUTPUT			1L
#define JOB_PCITTL32_READ_DIRECTION	2L
		
#define JOB_PCITTL32_IOMODE0_7		60L
#define JOB_PCITTL32_IOMODE00_07	60L
#define JOB_PCITTL32_IOMODE8_15		61L
#define JOB_PCITTL32_IOMODE08_15	61L
#define JOB_PCITTL32_IOMODE16_23	62L
#define JOB_PCITTL32_IOMODE24_31	63L
#define JOB_PCITTL32_IOMODE32_39	64L
#define JOB_PCITTL32_IOMODE40_47	65L

// Jobs für PCIEXT64 mr. 28.06.2004

#define JOB_PCIEXT64_READ_TEMPERATURE	94L
#define JOB_PCIEXT64_ENABLE				95L
#define JOB_PCIEXT64_DISABLE			96L
#define JOB_PCIEXT64_CARD_DETECT_STATUS 97L
#define JOB_PCIEXT64_RESET_ACTIVE		98L
#define JOB_PCIEXT64_ACTIVE				99L
#define JOB_PCIEXT64_PCI_CONFIGSPACE    100L

// Jobs für PCIWDOG3 & 4 mr. 27.09.2004

#define JOB_LOAD_WATCHDOG				101L			
#define JOB_CLEAR_LOG					102L								
#define JOB_READ_LOG					103L											
#define JOB_TIME_SET					104L											
#define JOB_TIME_GET					105L								
#define JOB_SEND_SMS					106L								
#define JOB_GOTO_IDLE					107L  	
#define JOB_EEPROM_WRITE				108L	
#define JOB_READ_DEFAULT_TIMEOUT		109L
#define JOB_WRITE_DEFAULT_TIMEOUT		110L
#define JOB_LOCK_DEVICE					111L		
#define JOB_UNLOCK_DEVICE				112L		
#define JOB_GETSTATUS					113L		
#define JOB_GETSTATUS_STRING			114L
#define JOB_SHUTDOWN					115L
#define JOB_GETVERSION					116L
#define JOB_IRQ_GET_DATA_RESULTCODE		118L
#define JOB_IRQ_GET_DATA_EMAILADDRESS	119L
#define JOB_IRQ_GET_DATA_EMAILTEXT		120L
#define JOB_GET_EMAILTEXT				121L
#define JOB_GET_EMAILADDRESS			122L		
#define JOB_IRQ_GET_DATA_RESULTTYPE		124L
#define JOB_GET_SHUTDOWNUSERABORTTIME   125L
#define JOB_CANCEL_SHUTDOWN				126L
#define JOB_READ_FLASH_MEMORY_BYTE		128L
#define JOB_READ_FLASH_MEMORY_WORD		129L
#define JOB_GET_PHASETEXT				130L
#define JOB_READ_LOG_ENTRY				131L
#define JOB_GET_PHASE					132L
#define JOB_READ_WATCHDOG_TIMER			133L
#define JOB_READ_RELAY_TIMER			134L
#define JOB_READ_OPTOCOUPLER_INPUTS		135L
#define JOB_READ_SMS_STATUS				136L
#define JOB_READ_SMS_STATUS_STRING		137L
#define JOB_READ_TEMPERATURE_VALUES		138L
#define JOB_READ_VOLTAGE_VALUES			139L

// Jobs für PCITTL64 mr. 23.05.2005

#define JOB_PCITTL64_INPUT				0L
#define JOB_PCITTL64_OUTPUT				1L

#define JOB_PCITTL64_IOMODE0_7			140L
#define JOB_PCITTL64_IOMODE8_15			141L
#define JOB_PCITTL64_IOMODE16_23		142L		
#define JOB_PCITTL64_IOMODE24_31		143L
#define JOB_PCITTL64_IOMODE32_39		144L
#define JOB_PCITTL64_IOMODE40_47		145L
#define JOB_PCITTL64_IOMODE48_55		146L
#define JOB_PCITTL64_IOMODE56_63		147L

// Jobs für USB-FLASH			

#define JOB_USB_FLASH_DEVICE			148L

// Jobs für PCIWDOG3 mr. 25.10.2005

#define JOB_SET_LOG_LEVEL				149L
#define JOB_GET_LOG_LEVEL				150L

// Jobs for USBAD8DAC2 mr. 18.11.2005

#define JOB_USBAD8DAC2_IOMODE0_7		151L
#define JOB_USBAD8DAC2_IOMODE8_15		152L
#define JOB_USBAD8DAC2_IOMODE16_23		153L

#define JOB_USBAD8DAC2_INPUT			0L
#define JOB_USBAD8DAC2_OUTPUT			1L

// Jobs for TTL ports  mr. 17.01.2005 ( DDR = Data Direction Register )

#define	JOB_WRITE_DDR					154L
#define	JOB_READ_DDR					155L

#define JOB_IOMODE0_7					156L
#define JOB_IOMODE8_15					157L
#define JOB_IOMODE16_23					158L		
#define JOB_IOMODE24_31					159L
#define JOB_IOMODE32_39					160L
#define JOB_IOMODE40_47					161L
#define JOB_IOMODE48_55					162L
#define JOB_IOMODE56_63					163L

#define JOB_INPUT						0L
#define JOB_OUTPUT						1L

#define JOB_USB_SET_FLASH_MODE			164L

//@3e


/*  next job 165L ! */

//#define JOB_INVALID						0x4d414a41L

/*
**	neue Datentypen
*/

#define BOOL int
#define ULONG unsigned long


/*
**	diverse Defines
*/

#define NotUsed(x) (x)=(x)

#ifndef TRUE
	#define TRUE	1
#endif

#ifndef FALSE
	#define FALSE 0
#endif


/*
**	Structure which holds information regarding the card or module
*/

typedef struct CardDatas
	{
	ULONG SizeOf;						/*	The size of the structure in bytes.				*/
	ULONG CardID;						/*	Card-ID											*/
	ULONG BusType;						/*	Bustype (ISA, PCI, VLB, U)   					*/
	ULONG Features;						/*	Bit-Field Features							   	  */
	char  Name[32];						/*	Buffer that receives the null-terminated string */
										/*  that specifies the name of the hardware.			*/
	ULONG IOHandle;						/*														*/
	ULONG IrqChannel;					/*	The interrupt channel of the card					*/
	ULONG DMAChannel;					/*	The DMA channel of the card							*/
	ULONG Module;						/*	The module number of the card.						*/
										/*  This value is only of importance if the value of BUSType is BUS_PAR. */			
	ULONG NumOfADChannels;				/*	he number of analog input channels on the card   */
	ULONG NumOfDAChannels;				/*	The number of analog output channels on the card */
	ULONG NumOfDIChannels;				/*  The number of digital input channels on the card */
	ULONG NumOfDOChannels;				/*	The number of digital output channels on the card */
	ULONG NumOfDXChannels;				/*	The number of programmable digital input/output channels on the card */
	ULONG IOBaseList[8];				/*	The number of possible card addresses.			*/
	ULONG IRQList;						/*													*/
	ULONG IRQ2List;						/*													*/
	ULONG DMAList;    					/*													*/
	ULONG IOSize;						/*  The size of the IO range used by the card.		*/
	ULONG VendorCode;					/*  This value indicates the producer card manufacturer.	*/
	ULONG ulReserved[32];				/* 	Reseved memory for internal use.				*/
	} CARDDATAS, FAR  *LPCARDDATAS;


/*
**	Log-Entry for PCIWDOG3 Watchdog
*/


#ifndef _STRUCT_PCIWDOG3_LOGENTRY

#define _STRUCT_PCIWDOG3_LOGENTRY 1

typedef struct STRUCT_PCIWDOG3_LOGENTRY
{
	WORD		SizeOf;					/* Initialize this member to sizeof(PCIWDOG3_LOGENTRY) first	*/
	WORD		LogCount;				/* Counter														*/
	char    	LogLevel;				/* Log-Level ( Error, Warning, Information, Phase, Relay )		*/
	char		Date[11];				/* Date Entry Format '12.12.2004' + ´\0´						*/
	char		Time[9];				/* Time Entry Format '08:00:25' + '\0'							*/
	char		Phase;					/* Current Phase												*/
	char		BufferLength;			/* Receiving the total length of the string in 'BufferString'	*/
	char		BufferString[255];		/* Buffer receiving the text									*/
} PCIWDOG3_LOGENTRY;

#endif


/*
**	Funktions-Prototypen
*/

#ifndef QLIB_DYNAMICLINK

#ifdef __cplusplus
extern "C"
	{
#endif

//@6s

ULONG	QAPICALLER QAPIVersion(ULONG type);
ULONG	QAPICALLER QAPIInitialize(ULONG para1, ULONG para2, ULONG para3, ULONG para4);
void	QAPICALLER QAPIExtSetDebugLevel (ULONG dbgval);
void	QAPICALLER QAPIExtEnableIRQ (ULONG cdl,ULONG mode);
ULONG	QAPICALLER QAPIExtNumOfCards();
LPCARDDATAS QAPICALLER QAPIExtGetCardInfo (ULONG cardnum);
ULONG	QAPICALLER QAPIExtGetCardInfoEx (ULONG cardnum,LPCARDDATAS lpcd);
void 	QAPICALLER QAPIExtReleaseCardInfo (LPCARDDATAS lpcd);
ULONG	QAPICALLER QAPIExtOpenCard  (ULONG cardnum,ULONG devnum);
void 	QAPICALLER QAPIExtCloseCard (ULONG cdl);
ULONG	QAPICALLER QAPIExtReadAD    (ULONG cdl,ULONG channel,ULONG mode);
void 	QAPICALLER QAPIExtWriteDA   (ULONG cdl,ULONG channel,ULONG value,ULONG mode);
void 	QAPICALLER QAPIExtLatchDA   (ULONG cdl);
float	QAPICALLER QAPIExtConvertDWToVoltage (ULONG cdl,ULONG value,ULONG mode);
ULONG	QAPICALLER QAPIExtConvertVoltageToDW (ULONG cdl,float value,ULONG mode);
ULONG	QAPICALLER QAPIExtReadDI1   (ULONG cdl,ULONG channel,ULONG mode);
ULONG	QAPICALLER QAPIExtReadDI8   (ULONG cdl,ULONG channel,ULONG mode);
ULONG	QAPICALLER QAPIExtReadDI16  (ULONG cdl,ULONG channel,ULONG mode);
ULONG	QAPICALLER QAPIExtReadDI32  (ULONG cdl,ULONG channel,ULONG mode);
void 	QAPICALLER QAPIExtWriteDO1  (ULONG cdl,ULONG channel,ULONG value,ULONG mode);
void 	QAPICALLER QAPIExtWriteDO8	(ULONG cdl,ULONG channel,ULONG value,ULONG mode);
void 	QAPICALLER QAPIExtWriteDO16 (ULONG cdl,ULONG channel,ULONG value,ULONG mode);
void 	QAPICALLER QAPIExtWriteDO32 (ULONG cdl,ULONG channel,ULONG value,ULONG mode);
void	QAPICALLER QAPIExtWrite8255	(ULONG cdl,ULONG chipnum,ULONG reg,ULONG value);
ULONG	QAPICALLER QAPIExtRead8255	(ULONG cdl,ULONG chipnum,ULONG reg);
void	QAPICALLER QAPIExtWrite8253 (ULONG cdl,ULONG chipnum,ULONG reg,ULONG value);
ULONG	QAPICALLER QAPIExtRead8253  (ULONG cdl,ULONG chipnum,ULONG reg);
ULONG 	QAPICALLER QAPIExtWatchdog	(ULONG cdl,ULONG job);
ULONG	QAPICALLER QAPIExtSpecial   (ULONG cdl,ULONG jobcode,ULONG para1,ULONG para2);
ULONG   QAPICALLER QAPIExtReadString  (ULONG cdl, ULONG device, char* buffer, ULONG maxsize, ULONG mode);
ULONG   QAPICALLER QAPIExtWriteString (ULONG cdl, ULONG device, char* buffer, ULONG maxsize, ULONG mode);
ULONG	QAPICALLER QAPIExtWriteStringEx (ULONG cdl, ULONG device, char* buffer, ULONG maxsize, ULONG mode, ULONG bEnableREN);

ULONG	QAPICALLER QAPINumOfCards();
LPCARDDATAS QAPICALLER QAPIGetCardInfo (ULONG cardnum);
ULONG	QAPICALLER QAPIGetCardInfoEx (ULONG cardnum,LPCARDDATAS lpcd);
ULONG	QAPICALLER QAPIGetAD        (ULONG cardnum,ULONG channel);
float	QAPICALLER QAPIConvertDWToVoltage (ULONG cardnum,ULONG value, ULONG mode);
ULONG	QAPICALLER QAPIConvertVoltageToDW (ULONG cardnum,float value, ULONG mode);
void 	QAPICALLER QAPIPutDA        (ULONG cardnum,ULONG channel,ULONG value);
ULONG	QAPICALLER QAPIGetDI        (ULONG cardnum,ULONG channel);
void 	QAPICALLER QAPIPutDO        (ULONG cardnum,ULONG channel,ULONG value);
ULONG	QAPICALLER QAPIRead8253     (ULONG cardnum,ULONG reg);
void	QAPICALLER QAPIWrite8253    (ULONG cardnum,ULONG reg,ULONG value);
ULONG	QAPICALLER QAPIRead8255     (ULONG cardnum,ULONG reg);
void	QAPICALLER QAPIWrite8255    (ULONG cardnum,ULONG reg,ULONG value);
void	QAPICALLER QAPIWatchdogEnable();
void	QAPICALLER QAPIWatchdogDisable();
void	QAPICALLER QAPIWatchdogRetrigger();
void	QAPICALLER QAPIWatchdogLoad();
ULONG   QAPICALLER QAPIWatchdogStatus();
ULONG	QAPICALLER QAPISpecial      (ULONG cardnum,ULONG jobcode,ULONG para1,ULONG para2);
ULONG   QAPICALLER QAPIReadString  (ULONG cardnum, ULONG device, char* buffer, ULONG maxsize, ULONG mode);
ULONG   QAPICALLER QAPIWriteString (ULONG cardnum, ULONG device, char* buffer, ULONG maxsize, ULONG mode);
ULONG   QAPICALLER QAPIWriteStringEx (ULONG cardnum, ULONG device, char* buffer, ULONG maxsize, ULONG mode, ULONG bEnableREN);

ULONG   QAPICALLER QAPIConnect(char* ip, ULONG port, char* username, char* password, ULONG timeout);
ULONG   QAPICALLER QAPIDisconnect();
ULONG   QAPICALLER QAPIGetLastError();
ULONG   QAPICALLER QAPIGetLastErrorCode();
char*   QAPICALLER QAPIGetLastErrorString();
ULONG   QAPICALLER QAPIGetLastErrorStringEx(char* buffer, ULONG buffersize);
ULONG   QAPICALLER QAPIWaitIRQ(ULONG cardnum, ULONG devnum);
ULONG   QAPICALLER QAPIGetConnectionMode(ULONG nType, char* buffer, ULONG buffersize);

ULONG   QAPICALLER QAPISetupCounter(ULONG cardnum, ULONG counter, ULONG mode, void* reserved);
ULONG   QAPICALLER QAPIResetCounter(ULONG cardnum, ULONG counter, ULONG reserved1, ULONG reserved2);
ULONG   QAPICALLER QAPIReadCounter(ULONG cardnum, ULONG counter, ULONG reserved1, ULONG reserved2);
ULONG   QAPICALLER QAPIExtSetupCounter(ULONG cdl, ULONG counter, ULONG mode, void* reserved);
ULONG   QAPICALLER QAPIExtResetCounter(ULONG cdl, ULONG counter, ULONG reserved1, ULONG reserved2);
ULONG   QAPICALLER QAPIExtReadCounter(ULONG cdl, ULONG counter, ULONG reserved1, ULONG reserved2);

//@6e

#ifdef __cplusplus
	}
#endif

#endif

/*
**  List of error codes for function QAPIGetLastError()
*/

//@4s

#define ERROR_NONE				0L  // no error
#define ERROR_GETLASTERROR 			1L  // kernel returns special error ( for details see GetLastError() in MS SDK Documentation )
#define ERROR_WSAGETLASTERROR 			2L  // winsock returns special error ( for details see WSAGetLastError() in MS SDK Documentation)
#define ERROR_QLIB_INTERNAL 			3L  // not specified error
#define ERROR_QLIB_BUFFER_TO_SMALL 		4L  // buffer overflow
#define ERROR_QLIB_CONNECTION 			5L  // connection error
#define ERROR_QLIB_CONNECTION_TIMEOUT 		6L  // no response from server
#define ERROR_QLIB_CONNECTION_LOGIN_FAILED 	7L  // authentification failed ( username, password )
#define ERROR_QLIB_CONNECTION_DISCONNECTED 	8L  // connection has been shutdown 
#define ERROR_QLIB_ILLEGAL_PARAMETER 		9L  // illegal parameter passed to QAPIxxxx function
#define ERROR_EXCEPTION 			10L // exception ( check buffers and parameters )
#define ERROR_LOADING_WINSOCK 			11L // TCP/IP not installed ?
#define ERROR_QLIB_CARDID_NOT_VALID 		12L // illegal card id
#define ERROR_QLIB_FUNCTION_NOT_SUPPORTED 	13L // QAPIxxxx function not supported by this card
#define ERROR_GPIB_TIMEOUT 			14L // device is not responding
#define ERROR_GPIB_ERR 				15L // status register returns err flag
#define ERROR_QLIB_UNABLE_TO_LOAD_QMULTI 	16L // QMULTI32.DLL nicht im Suchpfad
#define ERROR_QLIB_QMULTI_HAS_WRONG_VERSION 	17L // remove installation and run setup again to install matching DLL's
#define ERROR_QLIB_QMULTI_DIRECTIO 		18L // load of dll failed -> no link / subsequent calls will fail
#define ERROR_QLIB_FASTMEM_MAP_FAILED 		19L // unable to get card memory pointer 
#define ERROR_QLIB_FASTMEM_UNMAP_FAILED 	20L // unable to release card memory pointer
#define ERROR_QLIB_DEVICE_BUSY 			21L // another thread has locked access to device ( semaphore )		
#define ERROR_QLIB_DEVICE_NOT_PRESENT		22L // device removed, isa card not present ( register check )
#define ERROR_QLIB_DEVICE_CLOSED		23L // close on handle for card waiting for an IRQ
#define ERROR_QLIB_IRQ_DISABLED			24L // IRQ disabled for card waiting for an IRQ
#define ERROR_QLIB_IRQ_ALREADY_ENABLED		25L // IRQ has been enabled before this call
#define ERROR_QLIB_IRQ_ALREADY_DISABLED		26L // IRQ has been disabled before this call
#define ERROR_QLIB_IRQ_NOT_AVAILABLE		27L // IRQ not available for this card
#define ERROR_QLIB_TIMEOUT			28L // Timeout (i.e. waiting for data from onboard cpu)
#define ERROR_QLIB_RESET_ERROR			29L // Error, onboard cpu reset failed
#define ERROR_QLIB_INVALID_DATA			30L // Invalid Data received ( i.e. onboard cpu returns wrong date length)
#define ERROR_INVALID_LOG_DATA			31L // Invalid Log Entry ( PCIWDOG3, ... )
#define ERROR_QLIB_FILE_NOT_FOUND		32L // File not found
#define ERROR_QLIB_FILE_HEX_FORMAT_REQUIRED	33L // File must be in Hex-Format
#define ERROR_QLIB_WRITE_FLASH_FAILED		34L // Writing to the Flash Memory failed
#define ERROR_QLIB_VERIFY_FLASH_FAILED		35L // Flash Memory Verify failed
#define ERROR_QLIB_UNABLE_INITIALIZE_DEVICE	36L // Unable to Initialize the Device
#define ERROR_QLIB_NOT_SUPPORTED_IN_REMOTE_MODE	37L  // Function not supported in remote mode ( over a TCP/IP connection )
#define ERROR_QLIB_COUNTER_OVERFLOW			38L  // Counter overflow
#define ERROR_QLIB_SIGNAL_OUT_OF_RANGE		39L	  // AI line signal out of range

//@4e

/*
**  List of valid card id's
*/

//@5s

#define	PAR8DA				0         
#define UNITIMER			1         
#define PAR12AD				2
#define	PDAC4				3
#define	PAD12				4
#define PAD16				5
#define PAD12DAC4			6
#define PAD16DAC4			7
#define	PUNIREL				8
#define	ADGVT12				9
#define	ADGVT16				10
#define PAR16AD				11
#define PREL8 				12
#define PREL16   	   		13
#define POPTOREL16			14
#define	POPTO16IN			15
#define PWDOG				16
#define POPTOLCA			17
#define WATCHDOG			18
#define PTTL24IO			19
#define PROTO1				20
#define PROTO2				21
#define PAR8R     			22
#define PAR8O       		23
#define PAR48IO     		24
#define PAR2DA      		25
#define DAC4				26
#define OPTORELTTL  		27
#define OPTOREL16   		28
#define OPTOMOS     		29
#define OPTOLCALC   		30
#define OPTOLCA     		31
#define OPTO16IN    		32
#define DAC4UI      		33
#define DAC16BITDUAL		34
#define ADI1				35
#define ADI2				36
#define AD12BIT				37
#define C3X32BIT			38
#define R220V				39
#define REL16				40
#define	REL8				41
#define REL8UM				42
#define TIMER9				43
#define TIMER9LCA			44
#define TTL24IO				45
#define WATCHDOG3			46
#define MFB51				47

#define TAP14PCI			48
#define TAP14ISA			49

#define USBWDOG1			50	
#define USBWDOG2			51
#define USBWDOG3			52
#define GPIB				53
#define PCIGPIB				54
#define USBGPIB				55
#define PCITTL32			56
#define PCIOPTOREL16		57
#define PCIOPTO16IO			58
#define PCIOPTO16IOLC		59
#define PCIREL16			60
#define PCIPROTO			61
#define USBOPTOREL16		62
#define USBOPTO16IO			63
#define USBREL8				64
#define USBOPTO8			65
#define PCIAD16DAC4			66
#define USBREL8LC			67
#define USBOPTO8LC			68
#define LOGICANALYZER		69
#define TASTMAUS1			70
#define PCIWDOG3			71
#define PCIWDOG4			72
#define USBOPTOREL32		73
#define USBOPTOIO32			74
#define PCIEXT64			75
#define PCITTL64			76
#define USBFLASH			77
#define USBAD8LC			78
#define USBOPTOIN64			79
#define USBOPTOOUT64		80
#define USBREL64			81
#define PCITTL64FIFO		82
#define PCIDAC416			83
#define USBAD8DAC2			84

#define LASTCARD			84

#define DEFAULTGPIB			0x8000

//@5e

// define new cards qlibpci.h too

/*
**  alle nachfolgenden Karten werden von der QLIB noch nicht unterstuetzt
*/

#define PCLOCK77      1000


/*
**	Defines für Abwärtskompatibilität
*/

#define POPTOREL		POPTOREL16
#define WATCHDOG1		WATCHDOG
#define WATCHDOG2		WATCHDOG
#define PWDOG1   		PWDOG
#define PWDOG2			PWDOG
#define PWDOG2N			PWDOG

#define WATCHDOG12		WATCHDOG
#define PWDOG12			PWDOG

#define	USBWDOG			USBWDOG1

#endif
