/*--------------------------------------------------------------------------*/
// fglove.h
//
// 5DT Data Glove driver SDK
// Version 1.04a
//
// Copyright (C) 2000-2001, 5DT <Fifth Dimension Technologies>
// http://www.5dt.com/
/*--------------------------------------------------------------------------*/
#ifndef _FGLOVE_H_
#define _FGLOVE_H_
/*--------------------------------------------------------------------------*/
enum EfdGloveHand
{
	FD_HAND_LEFT,   // left-handed glove
	FD_HAND_RIGHT	// right-handed glove
};

enum EfdGloveTypes
{
	FD_GLOVENONE,   // no glove
	FD_GLOVE7,      // 7-sensor
	FD_GLOVE7W,     // 7-sensor, wireless
	FD_GLOVE16,     // 16-sensor
	FD_GLOVE16W     // 16-sensor, wireless
};

enum EfdSensors
{
	FD_THUMBNEAR=0,
	FD_THUMBFAR,
	FD_THUMBINDEX,
	FD_INDEXNEAR,
	FD_INDEXFAR,
	FD_INDEXMIDDLE,
	FD_MIDDLENEAR,
	FD_MIDDLEFAR,
	FD_MIDDLERING,
	FD_RINGNEAR,
	FD_RINGFAR,
	FD_RINGLITTLE,
	FD_LITTLENEAR,
	FD_LITTLEFAR,
	FD_THUMBPALM,
	FD_WRISTBEND,
	FD_PITCH,
	FD_ROLL
};
/*--------------------------------------------------------------------------*/
typedef struct
{
	// The contents of this struct are platform-dependent and subject to
	// change. You should not manipulate the contents of this struct directly.
	void           *m_pStuff;
} fdGlove;
/*--------------------------------------------------------------------------*/

//#if	AAA_DEBUG()
//#define MaaDllExport
//	extern	"C"
////maa#define DLLEXPORT	__declspec(dllexport)
//#define DLLEXPORT	__declspec(dllimport)
//#else
#define MaaDllExport
#define DLLEXPORT	__cdecl
//#endif


#if	AAA_DEBUG()
MaaDllExport	fdGlove* fdOpen(char *pPort);
#else
MaaDllExport	fdGlove* DLLEXPORT	fdOpen(char *pPort);
#endif
MaaDllExport	int		DLLEXPORT	fdClose(fdGlove *pFG);
MaaDllExport	int		DLLEXPORT	fdGetGloveHand(fdGlove *pFG);
MaaDllExport	int		DLLEXPORT	fdGetGloveType(fdGlove *pFG);
MaaDllExport	int		DLLEXPORT	fdGetNumSensors(fdGlove *pFG);
MaaDllExport	void	DLLEXPORT	fdGetSensorRawAll(fdGlove *pFG, unsigned short *pData);
MaaDllExport	unsigned short DLLEXPORT	fdGetSensorRaw(fdGlove *pFG, int nSensor);
MaaDllExport	void  DLLEXPORT	fdSetSensorRawAll(fdGlove *pFG, unsigned short *pData);
MaaDllExport	void  DLLEXPORT	fdSetSensorRaw(fdGlove *pFG, int nSensor, unsigned short nRaw);
MaaDllExport	void  DLLEXPORT	fdGetSensorScaledAll(fdGlove *pFG, float *pData);
MaaDllExport	int   DLLEXPORT	fdGetNumGestures(fdGlove *pFG);
MaaDllExport	float DLLEXPORT	fdGetSensorScaled(fdGlove *pFG, int nSensor);
MaaDllExport	int   DLLEXPORT	fdGetGesture(fdGlove *pFG);
MaaDllExport	void  DLLEXPORT	fdGetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower);
MaaDllExport	void  DLLEXPORT	fdGetCalibration(fdGlove *pFG, int nSensor, unsigned short *pUpper, unsigned short *pLower);
MaaDllExport	void  DLLEXPORT	fdSetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower);
MaaDllExport	void  DLLEXPORT	fdSetCalibration(fdGlove *pFG, int nSensor, unsigned short nUpper, unsigned short nLower);
MaaDllExport	void  DLLEXPORT	fdResetCalibration(fdGlove *pFG);
MaaDllExport	void  DLLEXPORT	fdGetSensorMaxAll(fdGlove *pFG, float *pMax);
MaaDllExport	float DLLEXPORT	fdGetSensorMax(fdGlove *pFG, int nSensor);
MaaDllExport	void  DLLEXPORT	fdSetSensorMaxAll(fdGlove *pFG, float *pMax);
MaaDllExport	void  DLLEXPORT	fdSetSensorMax(fdGlove *pFG, int nSensor, float fMax);
MaaDllExport	void  DLLEXPORT	fdGetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower);
MaaDllExport	void  DLLEXPORT	fdGetThreshold(fdGlove *pFG, int nSensor, float *pUpper, float *pLower);
MaaDllExport	void  DLLEXPORT	fdSetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower);
MaaDllExport	void  DLLEXPORT	fdSetThreshold(fdGlove *pFG, int nSensor, float fUpper, float fLower);
MaaDllExport	void  DLLEXPORT	fdGetGloveInfo(fdGlove *pFG, unsigned char *pData);
MaaDllExport	void  DLLEXPORT	fdGetDriverInfo(fdGlove *pFG, unsigned char *pData);


/*--------------------------------------------------------------------------*/
#endif // #ifndef _FGLOVE_H_
/*--------------------------------------------------------------------------*/


