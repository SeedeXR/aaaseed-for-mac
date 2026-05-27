
#ifdef AAA_GYPSY_H
#error "GYPSY_H included more than once."
#endif
#define AAA_GYPSY_H 1



// Gypsy.h interface to Gypsy.dll
// Copyright (c) 1999 ID8 Media
// Written by Edwin P. Berlin, Jr., Secret Software
// Modified by Maa for AAASeed

#ifndef Gypsy_Matrix_H
#	include "Matrix.h"		//Defines Matrix and Vector
#endif

#ifndef DLLEXPORT
//maa#define DLLEXPORT		__declspec(dllexport)
#define DLLEXPORT		__declspec(dllimport)
#endif

//Export all Gypsy functions as C functions
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tnode {
	char name[16];					//Name of this node
	Vector offset;					//Offset to the next node
//	BOOL Moving;					//The ?RotInd entries are valid channels
	INT32 Moving;					//The ?RotInd entries are valid channels
	int YRotInd, XRotInd, ZRotInd;	//Indices of rotation angles if Moving
	int NumChildren;				//Number of children
	struct tnode *Child[3];			//Maximum 3 children
	Vector CurrentPosition;			//Computed by traversing the tree
	Matrix CurrentTransform;		//Computed by traversing the tree
} Node;

#define NUM_CHANNELS	57			//Max possible number of channels

typedef struct {
	int ContactPoints;				//Which feet are on floor
	float channel[NUM_CHANNELS];
} Frame;

//Flag values for ContactPoints
#define CONTACT_LEFTFOOTHEEL	1
#define CONTACT_RIGHTFOOTHEEL	2
#define CONTACT_LEFTFOOTBALL	4
#define CONTACT_RIGHTFOOTBALL	8

//This defines each of the channels in a Frame
//The first three are position in inches
//The rest are angles in degrees
//Rotations are all CCW looking down the axis of rotation
//(in the minus direction)
//The order of the Euler angles at each node is z, then x, then y
//so the complete rotation matrix is YXZ
#define CHANNEL_POS_X			0
#define CHANNEL_POS_Y			1
#define CHANNEL_POS_Z			2
#define CHANNEL_HIPS_X			3
#define CHANNEL_HIPS_Y			4
#define CHANNEL_HIPS_Z			5
#define CHANNEL_LEFTHIP_X		6
#define CHANNEL_LEFTHIP_Y		7
#define CHANNEL_LEFTHIP_Z		8
#define CHANNEL_LEFTKNEE_X		9
#define CHANNEL_LEFTKNEE_Y		10
#define CHANNEL_LEFTKNEE_Z		11
#define CHANNEL_LEFTANKLE_X		12
#define CHANNEL_LEFTANKLE_Y		13
#define CHANNEL_LEFTANKLE_Z		14
#define CHANNEL_RIGHTHIP_X		15
#define CHANNEL_RIGHTHIP_Y		16
#define CHANNEL_RIGHTHIP_Z		17
#define CHANNEL_RIGHTKNEE_X		18
#define CHANNEL_RIGHTKNEE_Y		19
#define CHANNEL_RIGHTKNEE_Z		20
#define CHANNEL_RIGHTANKLE_X	21
#define CHANNEL_RIGHTANKLE_Y	22
#define CHANNEL_RIGHTANKLE_Z	23
#define CHANNEL_CHEST_X			24
#define CHANNEL_CHEST_Y			25
#define CHANNEL_CHEST_Z			26
#define CHANNEL_LEFTCOLLAR_X	27
#define CHANNEL_LEFTCOLLAR_Y	28
#define CHANNEL_LEFTCOLLAR_Z	29
#define CHANNEL_LEFTSHOULDER_X	30
#define CHANNEL_LEFTSHOULDER_Y	31
#define CHANNEL_LEFTSHOULDER_Z	32
#define CHANNEL_LEFTELBOW_X		33
#define CHANNEL_LEFTELBOW_Y		34
#define CHANNEL_LEFTELBOW_Z		35
#define CHANNEL_LEFTWRIST_X		36
#define CHANNEL_LEFTWRIST_Y		37
#define CHANNEL_LEFTWRIST_Z		38
#define CHANNEL_RIGHTCOLLAR_X	39
#define CHANNEL_RIGHTCOLLAR_Y	40
#define CHANNEL_RIGHTCOLLAR_Z	41
#define CHANNEL_RIGHTSHOULDER_X	42
#define CHANNEL_RIGHTSHOULDER_Y	43
#define CHANNEL_RIGHTSHOULDER_Z	44
#define CHANNEL_RIGHTELBOW_X	45
#define CHANNEL_RIGHTELBOW_Y	46
#define CHANNEL_RIGHTELBOW_Z	47
#define CHANNEL_RIGHTWRIST_X	48
#define CHANNEL_RIGHTWRIST_Y	49
#define CHANNEL_RIGHTWRIST_Z	50
#define CHANNEL_NECK_X			51
#define CHANNEL_NECK_Y			52
#define CHANNEL_NECK_Z			53
#define CHANNEL_HEAD_X			54
#define CHANNEL_HEAD_Y			55
#define CHANNEL_HEAD_Z			56


//Number of nodes in the Gypsy standard skeleton hierarchy
#define NUM_NODES	25

//These are the names of the 25 nodes
//These are arranged with the nodes that have more than one child listed first
#define HIPS			0		//The root node
#define LEFTHIP			1
#define RIGHTHIP		2
#define CHEST			3
#define LEFTCOLLAR		4
#define RIGHTCOLLAR		5
#define NECK			6
//These only have one child each
#define LEFTKNEE		7
#define LEFTANKLE		8
#define RIGHTKNEE		9
#define RIGHTANKLE		10
#define LEFTSHOULDER	11
#define LEFTELBOW		12
#define LEFTWRIST		13
#define RIGHTSHOULDER	14
#define RIGHTELBOW		15
#define RIGHTWRIST		16
#define HEAD			17
//End Sites
#define TOPOFHEAD		18
#define LEFTHAND		19
#define RIGHTHAND		20
#define LEFTFOOTHEEL	21
#define RIGHTFOOTHEEL	22
//Not in BVH file
#define LEFTFOOTBALL	23
#define RIGHTFOOTBALL	24

#define NUM_NODES_WITH_MULTIPLE_CHILDREN	7
#define FIRST_END_SITE_NODE		18

typedef struct Skeleton
{
	int NumFrames;					//Number of frames of data
	float frameTime;				//Seconds per frame
	Frame *frames;					//Malloc'd array of Frames
	Node nodes[NUM_NODES];			//Array of Nodes, first is root
} Skeleton;

typedef struct SuitInfo
{
	union {
		unsigned char b[4];			//IP address b[0].b[1].b[2].b[3]
		unsigned long l;			//Four bytes packed into a long
	} address;
	unsigned short port;
	unsigned short flags;
	char name[16];
	float FrameRate;				//Frames per second
} SuitInfo;

//Values for flags.
#define GYPSY_HAS_GYRO				1	//The suit has a gyro
#define GYPSY_HAS_POSITION_SENSOR	2	//The suit has a position sensor

//Names for the bones in ActorData
#define BONE_LEFT_PELVIS				0
#define BONE_LEFT_UPPER_LEG				1
#define BONE_LEFT_LOWER_LEG				2
#define BONE_LEFT_ANKLE_HEIGHT			3
#define BONE_LEFT_HEEL					4
#define BONE_LEFT_FOOT					5
#define BONE_RIGHT_PELVIS				6
#define BONE_RIGHT_UPPER_LEG			7
#define BONE_RIGHT_LOWER_LEG			8
#define BONE_RIGHT_ANKLE_HEIGHT			9
#define BONE_RIGHT_HEEL					10
#define BONE_RIGHT_FOOT					11
#define BONE_PELVIS_HEIGHT				12
#define BONE_PELVIS_DEPTH				13
#define BONE_LEFT_CLAVICLE				14
#define BONE_LEFT_UPPER_ARM				15
#define BONE_LEFT_FOREARM				16
#define BONE_LEFT_HAND					17
#define BONE_RIGHT_CLAVICLE				18
#define BONE_RIGHT_UPPER_ARM			19
#define BONE_RIGHT_FOREARM				20
#define BONE_RIGHT_HAND					21
#define BONE_SPINE						22
#define BONE_NECK						23
#define BONE_HEAD						24

#define NUM_BONES						25

//Names for the displacement parameters in ActorData
#define DISP_POT_TO_LEFT_WRIST_X		0
#define DISP_POT_TO_LEFT_WRIST_Y		1
#define DISP_POT_TO_LEFT_WRIST_Z		2
#define DISP_POT_TO_RIGHT_WRIST_X		3
#define DISP_POT_TO_RIGHT_WRIST_Y		4
#define DISP_POT_TO_RIGHT_WRIST_Z		5
#define DISP_POT_TO_LEFT_ANKLE_X		6
#define DISP_POT_TO_LEFT_ANKLE_Y		7
#define DISP_POT_TO_LEFT_ANKLE_Z		8
#define DISP_POT_TO_RIGHT_ANKLE_X		9
#define DISP_POT_TO_RIGHT_ANKLE_Y		10
#define DISP_POT_TO_RIGHT_ANKLE_Z		11
#define DISP_POT_TO_NECK_X				12
#define DISP_POT_TO_NECK_Y				13
#define DISP_POT_TO_NECK_Z				14

#define NUM_DISPS						15

//Names for the scale parameters in ActorData
#define SCALE_POSITION_X				0
#define SCALE_POSITION_Y				1
#define SCALE_POSITION_Z				2
#define SCALE_CHEST_X					3
#define SCALE_LEFTELBOW_X				4
#define SCALE_RIGHTELBOW_X				5
#define SCALE_LEFTKNEE_X				6
#define SCALE_RIGHTKNEE_X				7
#define SCALE_LEFTANKLE_X				8
#define SCALE_RIGHTANKLE_X				9
#define SCALE_LEFTSHOULDER_X			10
#define SCALE_LEFTSHOULDER_Y			11
#define SCALE_LEFTSHOULDER_Z			12
#define SCALE_RIGHTSHOULDER_X			13
#define SCALE_RIGHTSHOULDER_Y			14
#define SCALE_RIGHTSHOULDER_Z			15
#define SCALE_LEFTHIP_X					16
#define SCALE_LEFTHIP_Y					17
#define SCALE_LEFTHIP_Z					18
#define SCALE_RIGHTHIP_X				19
#define SCALE_RIGHTHIP_Y				20
#define SCALE_RIGHTHIP_Z				21
#define SCALE_JUMP_X					22
#define SCALE_JUMP_Y					23
#define SCALE_JUMP_Z					24
#define SCALE_JUMP_SENSITIVITY			25
#define SCALE_WEIGHT_SHIFT				26

#define NUM_SCALES						27

//All the calibration data for an actor
typedef struct {
	float Bone[NUM_BONES];
	float Disp[NUM_DISPS];
	float Tweak[NUM_CHANNELS];
	float Scale[NUM_SCALES];
} ActorData;

typedef void *HGYPSY;
typedef void (__cdecl * pGypsyCallback) (Frame *frame, SuitInfo *info);
typedef void (*pGypsyTimeout)(SuitInfo *info);

///////////////////////////////////////////////
//Returns version number of this interface .dll
//Major version in low WORD
//Minor version in high WORD
//You can use the macros LOWORD(result) and HIWORD(result)
//This function may be called at any time.
unsigned long DLLEXPORT GypsyVersion();	//maa	DLLEXPORT unsigned long GypsyVersion();

////////////////////////////////////////////////////
//Call this to begin using the Gypsy interface.
//When you are done, you should call GypsyFinish()
//It is OK to call GypsyInit() several times
//without calling GypsyFinish() as long as you call
//GypsyFinish() before exiting your program.
//Returns TRUE if successful.
INT32 DLLEXPORT	GypsyInit();	//maa	DLLEXPORT BOOL GypsyInit();

////////////////////////////////////////////////////
//Call this to finish using the Gypsy interface.
//It is OK to call GypsyFinish() several times or
//without calling GypsyInit()
void DLLEXPORT GypsyFinish();	//maa	DLLEXPORT void GypsyFinish();

/////////////////////////////////////////////////////////
//You must call this after calling GypsyInit() and before
//calling GypsyOpen() to open a handle to a suit.
//This function finds all the Gypsy suits which are
//available at the time this function is called.
//This function takes about one second.  It may be
//called repeatedly, but it must not be called with
//any open handles.  If it is, it returns -1;
//Otherwise, it returns the number of suits it found.
int DLLEXPORT GypsyFindSuits();	//maa	DLLEXPORT int GypsyFindSuits();

///////////////////////////////////////////////////////////////
//This function returns the number of suits found by the last
//successful call to GypsyFindSuits(), or 0 if GypsyFindSuits()
//has not been called.
int DLLEXPORT GypsyNumSuits();	//maa	DLLEXPORT int GypsyNumSuits();

////////////////////////////////////////////////////////////////
//This function fills a SuitInfo structure with info about
//a suit, whether or not it has been opened with GypsyOpen().
//suit ranges from 0 to Num-1, where Num is the value returned
//by GypsyFindSuits() or GypsyNumSuits()
//This information can be used to choose a suit or suits to open.
//Returns FALSE if invalid suit number
INT32 DLLEXPORT GypsyEnumerateSuitInfo(int suit, SuitInfo *info);	//maa	DLLEXPORT BOOL GypsyEnumerateSuitInfo(int suit, SuitInfo *info);

////////////////////////////////////////////////////////////////
//This function opens a Gypsy suit for access.  If info is NULL,
//the first available unopened suit is opened.  Otherwise,
//the suit matching the IP address and port specified in the
//structure pointed to by info, if it exists, is opened.
//If the open is successful, this function returns a handle
//to the opened suit for use with subsequent functions.
//If unsuccessful, the return value is NULL.
//When you are done using a suit, you should call GypsyClose()
//or GypsyFinish(), which will close all open suits.
//The callback parameter points to a function which is called
//every time data is received from the suit.  This parameter
//may be NULL, in which case you can read the most recent suit
//data received using GypsyRead().
//The timeout parameter points to a function which is called
//in the event that the suit ceases to send data for one second.
//This argument may also be NULL
HGYPSY DLLEXPORT GypsyOpen(SuitInfo *info, pGypsyCallback callback,
						   pGypsyTimeout timeout);	//maa	DLLEXPORT HGYPSY

///////////////////////////////////////////////////////////////
//Call this function to terminate all calls to the callback and
//timeout functions associated with an open suit.
void DLLEXPORT GypsyClose(HGYPSY hGypsy);	//maa	DLLEXPORT void

/////////////////////////////////////////////////////////////
//This function returns the handle of the open suit, if any,
//matching the IP address and port specified in the structure
//pointed to by info
HGYPSY DLLEXPORT GypsyHandleFromInfo(SuitInfo *info);	//maa	DLLEXPORT HGYPSY	

///////////////////////////////////////////////////////////
//This function fills the structure pointed to by info with
//info about the open suit specified by hGypsy.  If hGypsy
//is NULL, the structure is filled with zeros.
void DLLEXPORT GypsyGetSuitInfo(HGYPSY hGypsy, SuitInfo *info);	//maa	DLLEXPORT void

///////////////////////////////////////////////////////////
//This function returns the frame rate of the the open suit
//specified by hGypsy.  If hGypsy is NULL, it returns a
//default value.  The result is in frames per second.
float DLLEXPORT GypsyGetFrameRate(HGYPSY hGypsy);	//maa	DLLEXPORT float

////////////////////////////////////////////////////////////
//This function sets or clears the EnablePositionSensor flag
//associated with the open suit specified by hGypsy.
//The position sensor will not be enabled if the suit
//specified by hGypsy does not have a position sensor.
//You can tell if the suit has a position sensor by checking
//the GYPSY_HAS_POSITION_SENSOR flag in the flags member of
//the SuitInfo struct returned by GypsyGetSuitInfo().
//When the flag is set, data from the position sensor is used
//to set the position of the model.  If the flag is cleared,
//the position is determined using the foot contact points
//and an incremental calculation.  The incremental
//calculation can drift over time, and does not handle cases
//where both feet are off the floor or where the floor is not
//a horizontal plane (such as walking up steps or an incline).
//If this flag is changed, a new null point is set.  See
//GypsySetNull().
void DLLEXPORT GypsyEnablePositionSensor(HGYPSY hGypsy, INT32 enable);	//maa	DLLEXPORT void GypsyEnablePositionSensor(HGYPSY hGypsy, BOOL enable);

////////////////////////////////////////////////////////////
//This function sets the null position of the suit specified
//by hGypsy.  The actor stands erect with feet facing forward,
//arms straight down with palms facing the body and thumbs
//facing forward.  Spine, arms and legs should be as straight
//as possible.  This is the null position.  All joint angles
//and position calculations will be relative to this initial
//position.  The actor will be placed standing up along the
//positive Z axis with the lowest foot at Z=0, facing down
//the minus Y axis, with the positive X axis pointing to the
//actor's right.  This is a right handed coordinate system.
//This function also resets the count of dropped packets.
void DLLEXPORT GypsySetNull(HGYPSY hGypsy);	//maa	DLLEXPORT void

/////////////////////////////////////////////////////////////
//This function reads the actor data associated with the open
//suit specified by hGypsy into actor.  If hGypsy is NULL,
//the structure pointed to by actor is filled with default
//actor values.
void DLLEXPORT GypsyGetActorData(HGYPSY hGypsy, ActorData *actor);	//maa	DLLEXPORT void

////////////////////////////////////////////////////////////
//This function writes the actor data pointed to by actor to
//the open suit specified by hGypsy.
//If hGypsy is NULL, nothing happens.
void DLLEXPORT GypsySetActorData(HGYPSY hGypsy, ActorData *actor);	//maa	DLLEXPORT void

/////////////////////////////////////////////////////////
//This function copies the most recent Frame data for the
//open suit specified by hGypsy into the Frame structure
//pointed to by frame.  This allows you to poll the suit
//instead of or in addition to using a callback function
//passed to GypsyOpen.  While the callback is the
//preferred method, this function may be adequate for some
//applications.  See GypsyOpen().
//This function returns TRUE if successful.  It will return
//FALSE if no valid data has yet been received from the
//suit.
//If hGypsy is NULL, nothing happens and FALSE is returned.
INT32 DLLEXPORT GypsyRead(HGYPSY hGypsy, Frame *frame);	//maa	DLLEXPORT BOOL

//////////////////////////////////////////////////////////
//Returns number of frames we didn't have time to process.
//This number is cleared when GypsySetNull() or
//GypsyClearDropped() is called.
//Does not include packets which were never received.
unsigned long DLLEXPORTGypsyGetDropped(HGYPSY hGypsy);	//maa	DLLEXPORT unsigned long

///////////////////////////////////////////////
//Returns number of dropped frames.
//This number is cleared when GypsySetNull() or
//GypsyClearDropped() is called.
void DLLEXPORT GypsyClearDropped(HGYPSY hGypsy);	//maa	DLLEXPORT void

//////////////////////////////////////////////////////////
//If ZeroJoints is TRUE, it forces all raw joint data to 0
void DLLEXPORT GypsySetZeroJoints(HGYPSY hGypsy, INT32 ZeroJoints);	//maa	DLLEXPORT void GypsySetZeroJoints(HGYPSY hGypsy, BOOL ZeroJoints);

//////////////////////////////////////////
//Returns TRUE if suit is in jumping state
INT32 DLLEXPORT GypsyJumping(HGYPSY hGypsy);	//maa	DLLEXPORT BOOL

/////////////////////////////////////////////////////////
//The following functions manipulate skeletons and files:
// GypsyInitSkeleton()
// GypsyAddFrame()
// GypsyFreeSkeletonFrames()
// GypsyApplyFrame()
// GypsyCopyActorToSkeleton()
// GypsyReadBVH()
// GypsyWriteBVH()
// GypsyReadActorData()
// GypsyWriteActorData()
//These functions do not require you to call GypsyInit() first to use them.
//
//Note the following 'gotcha' when working with skeletons:
//The frames member of a skeleton will point to an array created
//with malloc, but you should only free it with
//GypsyFreeSkeletonFrames(), not with free() because the
//malloc used by Gypsy.dll may not be the same as the malloc
//linked into your application.  Also, do not pass this pointer
//to realloc() for the same reason.

////////////////////////////////////////////////////////
//Returns the height of the hips above the feet when the
//skeleton is in the null position
float DLLEXPORT GypsyPutSkeletonOnFloor(Skeleton *pSkel);	//maa	DLLEXPORT float

///////////////////////////////////////////////////////////////
//Sets your skeleton up with the default hierarchy and offsets.
//Also allocates one frame set to all 0 angles.
//Assumes pSkel points to an uninitialized skeleton structure.
//If the skeleton has been used, be sure to first call
//GypsyFreeSkeletonFrames()
void DLLEXPORT GypsyInitSkeleton(Skeleton *pSkel);	//maa	DLLEXPORT void

/////////////////////////////////////////////////////////////////
//Frees any frames that might have been allocated in the skeleton
//pointed to by pSkel and sets the frames member to NULL.
//Frames can be allocated by the following functions:
// GypsyInitSkeleton()
// GypsyAddFrame()
// GypsyReadBVH()
void DLLEXPORT GypsyFreeSkeletonFrames(Skeleton *pSkel);	//maa	DLLEXPORT void

///////////////////////////////////////////////////////////////////
//Increases pSkel->NumFrames by one and uses realloc to
//make the frames member point to memory big enough for
//pSkel->NumFrames Frames.  As with the other skeleton functions,
//you should only free these frames with GypsyFreeSkeletonFrames(),
//not with free() because the malloc used by Gypsy.dll may not be
//the same as the malloc linked into your application.  Also, do
//not pass this pointer to realloc() for the same reason.
//Returns FALSE if out of memory
INT32 DLLEXPORT GypsyAddFrame(Skeleton *pSkel);	//maa	DLLEXPORT BOOL

/////////////////////////////////////////////////////////////////
//Initialize the skeleton pointed to by pSkel with the actor data
//in actor, or default actor data if actor is NULL.
//The frame data of the skeleton remains unchanged.
void DLLEXPORT GypsyCopyActorToSkeleton(Skeleton *pSkel, ActorData *actor);	//maa	DLLEXPORT void

/////////////////////////////////////////////////////////////////
//This routine traverses the hierarchy of the skeleton pointed
//to by pSkel, applying the transformations in the frame data
//pointed to by frame.
//The result is that the CurrentPosition and CurrentTransform
//members of each node in the skeleton hierarchy are updated.
//If ZeroPosition is TRUE, it forces the root position to 0, 0, 0
//If ZeroAngles is TRUE, it forces all node angles to 0, 0, 0
void DLLEXPORT GypsyApplyFrame( Skeleton *pSkel, Frame *frame, INT32 ZeroPosition, INT32 ZeroAngles);	//maa	DLLEXPORT void GypsyApplyFrame( Skeleton *pSkel, Frame *frame, BOOL ZeroPosition, BOOL ZeroAngles);

////////////////////////////////////////////////////////
//Read a BVH file into the skeleton pointed to by pSkel.
//The filename is in file.
//Since the BVH format stops at the heel of the foot and
//we include the bottom of the foot in the hierarchy, we
//have two additional parameters which allow us to compute
//the offsets for the feet.  These are the angle from the
//ankle to the heel to the ball of the foot (in degrees)
//and this ratio: the distance from the ball of the foot to
//the heel divided by the distance from the ankle to the heel.
//The frames member of pSkel will point to an array created
//with malloc, but you should only free it with
//GypsyFreeSkeletonFrames(), not with free() because the
//malloc used by Gypsy.dll may not be the same as the malloc
//linked into your application.  Also, do not pass this pointer
//to realloc() for the same reason.
//The BVH units are always in inches
//Returns FALSE if it can't open the file or if it runs out of memory
INT32 DLLEXPORT GypsyReadBVH(char *file, Skeleton *pSkel,
							float AnkleHeelBallAngle,
							float BallToHeel_AnkleToHeel_Ratio);	//DLLEXPORT BOOL

////////////////////////////////////////////////////////
//Write a BVH file from the skeleton pointed to by pSkel.
//The filename is in file.
//The BVH units are always in inches
void DLLEXPORT GypsyWriteBVH(char *file, Skeleton *pSkel);	//maa DLLEXPORT void

///////////////////////////////////////////////////////
//Read a GYP file to the structure pointed to by actor.
//The filename is in file.
//This file contains dimensions for a specific actor
INT32 DLLEXPORT GypsyReadActorData(const char *file, ActorData *actor);	//maa	DLLEXPORT BOOL

//////////////////////////////////////////////////////////
//Write a GYP file from the structure pointed to by actor.
//Returns TRUE if successful
//The filename is in file.
//This file contains dimensions for a specific actor
//Units specifies the units written to the file
//It is one of the following:
#define INCHES			0
#define FEET			1
#define MILLIMETERS		2
#define CENTIMETERS		3
#define METERS			4
INT32 DLLEXPORT GypsyWriteActorData(const char *file, ActorData *actor,
								   int Units);	//maa	DLLEXPORT BOOL

///////////////////////////////////////////////////
//This function displays msg in a simple dialog box
//It is safe to call from any thread.
void DLLEXPORT GypsyMessageBox(char *msg);	//maa	DLLEXPORT void

///////////////////////////////////////////////
//Returns 48 WORDs which are the raw pot values
//This is used for calibration and testing
void DLLEXPORT GypsyGetRaw(HGYPSY hGypsy, unsigned short *result);	//maa	DLLEXPORT void

#ifdef __cplusplus
}
#endif


