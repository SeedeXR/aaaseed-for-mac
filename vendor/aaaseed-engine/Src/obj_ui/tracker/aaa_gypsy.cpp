#include "obj_ui/tracker/aaa_gypsy.h"
#include "Gypsy/wrap_gypsy.h"

//FACTORY_CREATE_V1( c_gypsy_skel, gypsy_skeleton, Gypsy Skeleton, gypsy_skeleton );

//The offset from FootBall to FootHeel are not in
//the BVH file so we compute them using these parameters
//set by the user.  We assume that the left and right
//values are the same.
CONST	float	AnkleHeelBallAngle = 85.f;				//Angle in degrees
CONST	float	BallToHeel_AnkleToHeel_Ratio = 1.5f;	//Ratio of lengths

bool	c_gypsy_skel::b_dll_loaded = false;

#if !AAA_WIN64()
AAA_ERR	c_gypsy_skel::read_bvh_from_filename( C_PCHAR filename )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _skel )
	{
		dll_gypsy.GypsyFreeSkeletonFrames( _skel );
	}
	else
		_skel = new Skeleton;

	if( _skel )
	{
		dll_gypsy.GypsyInitSkeleton( _skel );
		BOOL b = dll_gypsy.GypsyReadBVH( (char *)filename, _skel, AnkleHeelBallAngle, BallToHeel_AnkleToHeel_Ratio );
		//hack	todo centralize these hacks
		if( !b )
		{
			if( *(filename+1) == ':' )
			{
				CHAR str[AAA_PATH_MAX()];
				*str = 0;
				if( *filename == 'M' )
				{
					strcpy( str, filename );
					*str = 'C';
				}
				if( *str )
					b = dll_gypsy.GypsyReadBVH( (char *)str, _skel, AnkleHeelBallAngle, BallToHeel_AnkleToHeel_Ratio );
			}
		}
		//hack
		if( !b )
		{
			if( *(filename+1) != ':' )
			{
				o_str o( "../../Media/" );
				o.add( filename );
				b = dll_gypsy.GypsyReadBVH( (CHAR*) o.get(), _skel, AnkleHeelBallAngle, BallToHeel_AnkleToHeel_Ratio );
			}
		}

		if( !b )
		{
			ERR_PRINT_STRING( "read_bvh_from_filename() Can't open file : %s", filename );
		}
		retcode = b ? AAA_OK : ERR_ANY;
	}
	else
		ERR_PRINT_STRING("read_bvh_from_filename() No Gypsy Skeleton" );
	return	retcode;
}

AAA_ERR	c_gypsy_skel::read_actor_from_filename( C_PCHAR filename )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _skel )
	{
		_b_actor_data = dll_gypsy.GypsyReadActorData( filename, _actor );
		//hack	todo centralize these hacks
		if( !_b_actor_data )
		{
			if( *(filename+1) == ':' )
			{
				CHAR str[AAA_PATH_MAX()];
				*str = 0;
				if( *filename == 'M' )
				{
					strcpy( str, filename );
					*str = 'C';
				}
				if( *str )
					_b_actor_data = dll_gypsy.GypsyReadActorData( str, _actor );
			}
		}
//		HEAP_IS_CORRUPT();
		//hack
		if( !_b_actor_data )
		{
			if( *(filename+1) != ':' )
			{
				o_str o( "../../Media/" );
				o.add( filename );
				_b_actor_data = dll_gypsy.GypsyReadActorData( o.get(), _actor );
			}
		}
//		HEAP_IS_CORRUPT();
		if( _b_actor_data )
		{
			dll_gypsy.GypsyCopyActorToSkeleton( _skel, _actor );
			retcode = AAA_OK;
		}
		else
			ERR_PRINT_STRING( "read_actor_from_filename() Can't GypsyReadActorData" );
	}
	else
		ERR_PRINT_STRING( "read_actor_from_filename() No Gypsy Skeleton" );
	return	retcode;
}
#endif //#if !AAA_WIN64()

void	c_gypsy_skel::init()
{
	_skel = nullptr;
	_b_actor_data = false;
	_b_live = false;
}

#define		OUR_LOWORD(l)	((unsigned short) (l))
#define		OUR_HIWORD(l)	((unsigned short) (((unsigned long) (l) >> 16) & 0xFFFF))
bool		b_gypsy_init = false;
bool		b_gypsy_live_possible = false;
bool		b_gypsy_live = FALSE;
SuitInfo*	suit_info;
UINT32		gypsy_suit_nb = 0;
CONST	UINT32		GYPSY_SUIT_MAX_NB = 4;
HGYPSY		h_gypsy[ GYPSY_SUIT_MAX_NB ];

//The single frame constantly updated during live display
Frame	live_frame = {
	0,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f
};

#if AAA_WIN64()
void	gypsy_close()	{}
#else
void	gypsy_close()
{
	if( b_gypsy_init )
	{
		//INT32	i;
		
		for( UINT32 i = 0; i < gypsy_suit_nb; ++i )
		{
			dll_gypsy.GypsyClose( h_gypsy[i] );
			h_gypsy[i] = nullptr;
		}
		dll_gypsy.GypsyFinish();
		b_gypsy_init = false;
		SAFE_DELETE( suit_info );
	}
	if( c_gypsy_skel::b_dll_loaded )
		dll_gypsy.deinit();
		//		wrap_gypsy_Term();
}

void __cdecl gypsy_callback( Frame *frame, SuitInfo *info )
{
//	printf("suit %x\n", info);
/*
	static bool StopRecording = false;
	int FrameLimit;
	int i;

	if( !Record )
		StopRecording = false;

	if(!Record && !Live) return;
	if(Play) return;

	//If we have multiple suits open and we vector into the
	//same callback for all of them, we can tell which
	//suit is sending this data by checking info.

	if(IgnoreLowerBody) {
		//Set all the channels for the lower body to 0
		//These are the channels below CHANNEL_CHEST_X
		for(i = 0; i < CHANNEL_CHEST_X; ++i)
		 frame->channel[i] = 0.f;
	}

	//Store the new frame if we're recording and not paused
	if(Record && !Pause)
		{
		//Calculate record limit in frames
		if(RecordFramesOrSeconds)
			FrameLimit = RecordLimitFS;
		else
			FrameLimit = (int)(RecordLimitFS * info->FrameRate + .5f);

		//See if we need to stop because we hit record limit
		if(skeleton.NumFrames >= FrameLimit && RecordStop)
			{
			//Stop recording
			//We do it this way instead of pMainFrame->OnActionsStop();
			//because we are running in a time critical thread here
			//Stop recording until the message is processed
			StopRecording = true;
			pMainFrame->PostMessage(WM_COMMAND, ID_ACTIONS_STOP);
			//We could instead use SendMessage(), but then we would
			//drop frames while we wait for our message to be processed
			}
		else
			{
			//Add one frame to frames in skeleton
			if(!GypsyAddFrame(&skeleton))
				{
				//Stop recording
				//We do it this way instead of pMainFrame->OnActionsStop();
				//because we are running in a time critical thread here
				//Stop recording until the message is processed
				StopRecording = true;
				pMainFrame->PostMessage(WM_COMMAND, ID_ACTIONS_STOP);
				//We could instead use SendMessage(), but then we
				//drop frames while we wait for our message to be processed

				//Tell user we ran out of memory
				AfxMessageBox("Out of memory during record");
				}
			else
				{
				//Add this frame to the list
				MEMCPY(&skeleton.frames[skeleton.NumFrames - 1],
					   frame, sizeof(Frame));
				}
			}
		}

	//We're currently redrawing a frame, so don't wipe out LiveFrame
	if(InRedraw) return;
	InRedraw = TRUE;

	//We are Live or Recording, so show the current frame
	//Update the most recent samples here into LiveFrame
	MEMCPY(&LiveFrame, frame, sizeof(Frame));
	pMainFrame->Invalidate();
*/
	if( b_gypsy_live )
		MEMCPY( &live_frame, frame, sizeof(Frame) );
}

bool	c_gypsy_skel::set_live( bool in )
{
	if( _b_live != in )
	{
		if( in )
		{
			if( b_gypsy_live_possible )
			{
				if( _b_actor_data)
					dll_gypsy.GypsySetActorData( h_gypsy, _actor );
			}
			else
				in = false;
		}
		_b_live = in;
		b_gypsy_live = in;
	}
	return( _b_live );
}

void	c_gypsy_skel::set_refrence_position()
{
	if( _b_live )
		dll_gypsy.GypsySetNull( h_gypsy );
}

//This function is called by Gypsy when a suit times out
//This function is reentrant
void	gypsy_timeout( SuitInfo *info )
{
static	bool	InTimeout = false;

	//For this program, we'll handle reentrancy by ignoring
	//subsequent timeouts while we are still telling the
	//user about this one
	if( InTimeout )
		return;
	InTimeout = true;

	if( *info->name )
	{
		ERR_PRINT_STRING( "Timeout on suit at %d.%d.%d.%d:%d \"%s\"",
				info->address.b[0], info->address.b[1],
				info->address.b[2], info->address.b[3],
				info->port,
				info->name);
	}
	else
	{
		ERR_PRINT_STRING( "Timeout on suit at %d.%d.%d.%d:%d",
				info->address.b[0], info->address.b[1],
				info->address.b[2], info->address.b[3],
				info->port);
	}

//	GypsyClose(h_gypsy);
//	h_gypsy = nullptr;
/*
	if(AfxMessageBox(msg, MB_RETRYCANCEL | MB_ICONEXCLAMATION | MB_TOPMOST ) == IDCANCEL) {
		//The user wants to stop listening to this suit
		pMainFrame->OnActionsStop();
		GypsyClose(hGypsy);
		hGypsy = nullptr;
	}
*/
	InTimeout = false;
}

void	gypsy_open()
{
	if( b_gypsy_init )
		return;

	CHAR	str[256];
	unsigned long	version;
	INT32	major;
	INT32	minor;

	c_gypsy_skel::b_dll_loaded = dll_gypsy.init();
//	c_gypsy_skel::b_dll_loaded = wrap_gypsy_Init() == 0;
	if( c_gypsy_skel::b_dll_loaded )
	{
		version = dll_gypsy.GypsyVersion();
		major	= OUR_LOWORD(version);
		minor	= OUR_HIWORD(version);
		sprintf( str, "gypsy.dll version %d.%d", major, minor );
		GOOD_PRINT_STRING( str );

		if( major < 3 || (major==3 && minor<107) )
		{
			ERR_PRINT_STRING( "%s\nAAASeed need version 3.107 at least.\nUpdate gypsy.dll now or expect the worse.", str );
		}
		else if( dll_gypsy.GypsyInit() )
		{
			b_gypsy_init = true;
			gypsy_suit_nb = dll_gypsy.GypsyFindSuits();
			GOOD_PRINT_STRING( "find %d Gypsy suits", gypsy_suit_nb );
			gypsy_suit_nb = MIN( gypsy_suit_nb, GYPSY_SUIT_MAX_NB);

			suit_info = new SuitInfo[gypsy_suit_nb];

			gypsy_suit_nb =  1; //hack
			for( UINT32 i = 0; i < gypsy_suit_nb; ++i )
			{
				dll_gypsy.GypsyEnumerateSuitInfo( i, &suit_info[i] );
				GOOD_PRINT_STRING(	"IP %d.%d.%d.%d name %s",
								(INT32)suit_info[i].address.b[0],
								(INT32)suit_info[i].address.b[1],
								(INT32)suit_info[i].address.b[2],
								(INT32)suit_info[i].address.b[3],
								(INT32)suit_info[i].name);
				h_gypsy[i] = dll_gypsy.GypsyOpen( &suit_info[i], gypsy_callback, gypsy_timeout );
				if( h_gypsy[i] == 0 )
				{
					ERR_PRINT_STRING( "Can't open Gypsy suit %d", i );
				}
			}
			if( h_gypsy[0] )	//hack
				b_gypsy_live_possible = true;
		}
		else
			ERR_PRINT_STRING( "Can't init Gypsy" );
	}
}
#endif //#if AAA_WIN64()

c_gypsy_skel::c_gypsy_skel()
{
	//	we alloc double because reading the actor crash the memory
	_actor = (ActorData*) MALLOC( sizeof(ActorData)*2 );
#if !AAA_WIN64()
	gypsy_open();
#endif
	init();
}

#if !AAA_WIN64()
c_gypsy_skel::~c_gypsy_skel()
{
	gypsy_close();
	if( _skel )
	{
		dll_gypsy.GypsyFreeSkeletonFrames( _skel );
		delete _skel;
	}
	FREE( _actor );
}

REAL*	c_gypsy_skel::get_data_for_all_node( Frame* pt_frame, REAL* data, INT32 real_by_node )
{
	UINT32	node_nb	= (UINT32)get_node_nb();
	dll_gypsy.GypsyApplyFrame( _skel, pt_frame, FALSE, FALSE );

	//hack add a mecanism so the caller can be asked his data pt for frame and/or node
	REAL* dst = data -1;	//	proc like preinc
	INT32	stride = real_by_node - 12;
	for( UINT32 node=0; node<node_nb; ++node )
	{

		Vector*	vec = &_skel->nodes[node].CurrentPosition;
		*++dst	=	vec->x;
		*++dst	=	vec->z;
		*++dst	=	-vec->y;

//		skel->nodes[node].CurrentTransform.MatrixToEulerZYX(&vec->x, &vec->y, &vec->z);
		Matrix* mat = &_skel->nodes[node].CurrentTransform;
/*
		*data++ =  mat->xx;
		*data++ =  mat->xz;
		*data++ = -mat->xy;

		*data++ =  mat->zx;
		*data++ =  mat->zz;
		*data++ = -mat->zy;

		*data++ = -mat->yx;
		*data++ = -mat->yz;
		*data++ =  mat->yy;
*/
		*++dst =  mat->xx;
		*++dst =  mat->zx;
		*++dst = -mat->yx;

		*++dst =  mat->xz;
		*++dst =  mat->zz;
		*++dst = -mat->yz;

		*++dst = -mat->xy;
		*++dst = -mat->zy;
		*++dst =  mat->yy;

		dst += stride;
	}
	return ++dst;
}


AAA_ERR	c_gypsy_skel::get_data_live( REAL* data, INT32 real_by_node )
{
	if( _skel )
	{
		if( _b_live )
		{
			get_data_for_all_node( &live_frame, data, real_by_node );
//			HEAP_IS_CORRUPT();
		}
		return AAA_OK;
	}
	return ERR_ANY;
}

AAA_ERR	c_gypsy_skel::get_data( REAL* data, INT32 real_by_node )
{
	if( _skel )
	{
//		HEAP_IS_CORRUPT();
		UINT32	frame_nb = (UINT32)get_frame_nb();
		//INT32	frame;
		Frame*	frames = &_skel->frames[0];
		for( UINT32 frame = 0; frame < frame_nb; ++frame )
		{
			data = get_data_for_all_node( &frames[frame], data, real_by_node );
		}
//		HEAP_IS_CORRUPT();
		return AAA_OK;
	}
	return ERR_ANY;
}

INT32	c_gypsy_skel::get_node_nb()			{	return NUM_NODES;						}
INT32	c_gypsy_skel::get_frame_nb()		{	return _skel->NumFrames;				}
REAL	c_gypsy_skel::get_frame_by_sec()	{	return REAL( 1. / _skel->frameTime );	}

#endif //#if !AAA_WIN64()
