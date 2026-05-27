#include "aaavr.h"
#include "aaavr_matrix.h"
#include "OpenVR/openvr.h"
#ifndef	AAA_SENSOR_6DOF_H
#	include "obj_ui/tracker/sensor_6dof.h"
#endif
#include "gol/gol.h"
#include "draw/tex.h"
#include "infrastructure/param/param_declare.h"
#include "spy.h"

#include <lib_use.h>
AAA_LIB_USE( "OpenVR/openvr_api" )

FACTORY_CREATE_V1( c_aaavr, aaavr, openvr, aaavr );

c_aaavr*		c_aaavr::cur			= nullptr;

vr::IVRSystem*	c_aaavr::g_system		= nullptr;
bool			c_aaavr::b_start_with	= false;
bool			c_aaavr::b_verbose		= false;

namespace {
	INT32 CONST DEVICE_MAX_NB = vr::k_unMaxTrackedDeviceCount;
	INT32 index_sensor[	DEVICE_MAX_NB ];

	INT32 CONST HMD_START_INDEX	= 0;
	INT32 CONST HMD_MAX_NB		= 1;
	INT32 CONST HMD_STOP_INDEX	= HMD_START_INDEX + HMD_MAX_NB - 1;
	INT32 hmd_count;
	INT32 CONST CTL_START_INDEX	= HMD_STOP_INDEX + 1;
	INT32 CONST CTL_MAX_NB		= 2;
	INT32 CONST CTL_STOP_INDEX	= CTL_START_INDEX + CTL_MAX_NB - 1;
	INT32 ctl_count;
	INT32 CONST REF_START_INDEX	= CTL_STOP_INDEX + 1;
	INT32 CONST REF_MAX_NB		= 2;
	INT32 CONST REF_STOP_INDEX	= REF_START_INDEX + REF_MAX_NB - 1;
	INT32 ref_count;

	INT32 CONST SENSOR_MAX_NB	= REF_START_INDEX + REF_MAX_NB;
}

namespace	n_aaavr
{
	CONSTEXPR INT32 CTL_PARAM_NB	= 5;
	CONSTEXPR INT32 BASE_PARAM_NB	= 6 + CTL_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(					active				)
		PARAM_DEF_BOOL_OFF(					open				)
		PARAM_DEF_BOOL_OFF(					submit				)
		PARAM_DEF_BIND_2D_ALONE_DEF_INA(	submit_bind_left,	1, 104	)
		PARAM_DEF_BIND_2D_ALONE_DEF_INA(	submit_bind_right,	1, 105	)
		PARAM_DEF_BOOL_OFF(					submit_clear_trig	)
		PARAM_DEF_NONE(						hmd					)
		PARAM_DEF_NONE(						controller_1		)
		PARAM_DEF_NONE(						controller_2		)
		PARAM_DEF_NONE(						reference_1			)
		PARAM_DEF_NONE(						reference_2			)	
	};
}

void	c_aaavr::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_submit );
	param_set_pt( h, _submit_bind_left );
	param_set_pt( h, _submit_bind_right );
	param_set_pt( h, _b_submit_clear_trig );
	for( INT32 i=0; i<n_aaavr::CTL_PARAM_NB; i++ )
	{
		c_sensor_6dof* sen = sensors[i];
		param_attach_obj( h, (c_obj_ui*) sen );
	}

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_aaavr)
,_b_vr_init				{false}
,_b_submit				{false}
,_b_submit_clear_trig	{false}
{
	C_PCHAR_C names[ SENSOR_MAX_NB ] = { "Vive_Hmd",  "Vive_Ctl_1", "Vive_Ctl_2", "Vive_Ref_1", "Vive_Ref_2" };
	for( INT32 i=0; i<DEVICE_MAX_NB; ++i )
	{
		c_sensor_6dof* sensor = nullptr;
		obj_get( sensor );
		sensor->set_absolute( true );
		if( i < SENSOR_MAX_NB )
		{
			sensor->set_name( names[i] );
			//sensor->set_focus();
		}
		sensors.push_back( sensor );
	}

	param_init_with( n_aaavr::param, n_aaavr::PARAM_NB_MAX ); // polhemus_param, POLHEMUS_PARAM_NB);
//	clear_v3r( angle );
//	clear_v3r( a_last);
}

c_aaavr::~c_aaavr()
{
	if( cur == this )
		cur = nullptr;
	close();
	for( UINT32 i=0; i<sensors.size(); ++i )
		obj_delete( sensors[i] );
	sensors.clear();
}

using namespace ::vr;

namespace {
	INT32				pose_valid_count				=	0;
	INT32				pose_valid_count_last			=	-1;

	INT32				controller_tracked_count		=	0;
	INT32				controller_tracked_count_last	=	-1;

	std::string			string_sumup;	// what classes we saw poses for this frame

	struct st_device_data
	{
		Matrix4				mat;
		uint32_t			packet_num_last;
	//	bool				m_rbShowTrackedDevice;
		CHAR				char_device_class;
	};

	struct st_device_data	device_data	[DEVICE_MAX_NB];
	//can't be in st_device_data because function filling data need this
	TrackedDevicePose_t		poses		[DEVICE_MAX_NB];

	Matrix4 mat_hmd_pose;

	uint32_t size_x = 0;
	uint32_t size_y = 0;

	CONSTEXPR C_PCHAR_C VR_HEADER = "# VR ";

	void	VR_PRINT_STRING( C_PCHAR_C fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		HEADER_PRINT_STRING_VA( VR_HEADER, fmt, args );
		va_end( args );
	}
	void	VR_ERR_PRINT_STRING( C_PCHAR_C fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( VR_HEADER, fmt, args );
		va_end( args );
	}

	//-----------------------------------------------------------------------------
	// Purpose: Converts a SteamVR matrix to our local matrix class
	//-----------------------------------------------------------------------------
	Matrix4 ConvertSteamVRMatrixToMatrix4( const HmdMatrix34_t &matPose )
	{
		Matrix4 matrixObj(
			matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
			matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
			matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
			matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
			);
		return matrixObj;
	}

	CHAR* GetTrackedDeviceString( IVRSystem* pHmd, TrackedDeviceIndex_t device, TrackedDeviceProperty prop, TrackedPropertyError* peError = nullptr )
	{
		uint32_t required_len = pHmd->GetStringTrackedDeviceProperty( device, prop, nullptr, 0, peError );
		if( required_len == 0 )
			return (CHAR*) MALLOC( 1 );

		CHAR* buffer = (CHAR*) MALLOC( required_len );
		//required_len = 
		pHmd->GetStringTrackedDeviceProperty( device, prop, buffer, required_len, peError );
		return buffer;
	}

	void SetupRenderModelForTrackedDevice( IVRSystem* system, TrackedDeviceIndex_t index )
	{
		if( index >= DEVICE_MAX_NB )
			return;

		// try to find a model we've already set up
		CHAR* sRenderModelName = GetTrackedDeviceString( system, index, Prop_RenderModelName_String );
		VR_PRINT_STRING( "%s() RenderModelName is %s.", __FUNCTION__, sRenderModelName );

/*
		//VRRenderModels
		CGLRenderModel *pRenderModel = FindOrLoadRenderModel( sRenderModelName.c_str() );
		if( !pRenderModel )
		{
			std::string sTrackingSystemName = GetTrackedDeviceString( m_pHMD, index, Prop_TrackingSystemName_String );
			dprintf( "Unable to load render model for tracked device %d (%s.%s)", index, sTrackingSystemName.c_str(), sRenderModelName.c_str() );
		}
		else
		{
			m_rTrackedDeviceToRenderModel[ index ] = pRenderModel;
			m_rbShowTrackedDevice[ index ] = true;
		}
		*/
	}

	C_PCHAR get_error_compositor( EVRCompositorError error )
	{
		C_PCHAR str;
		switch( error )
		{
		case VRCompositorError_None							:	str = "None";							break;
		case VRCompositorError_RequestFailed				:	str = "RequestFailed";					break;
		case VRCompositorError_IncompatibleVersion			:	str = "IncompatibleVersion";			break;
		case VRCompositorError_DoNotHaveFocus				:	str = "DoNotHaveFocus";					break;
		case VRCompositorError_InvalidTexture				:	str = "InvalidTexture";					break;
		case VRCompositorError_IsNotSceneApplication		:	str = "IsNotSceneApplication";			break;
		case VRCompositorError_TextureIsOnWrongDevice		:	str = "TextureIsOnWrongDevice";			break;
		case VRCompositorError_TextureUsesUnsupportedFormat :	str = "TextureUsesUnsupportedFormat";	break;
		case VRCompositorError_SharedTexturesNotSupported	:	str = "SharedTexturesNotSupported";		break;
		case VRCompositorError_IndexOutOfRange				:	str = "IndexOutOfRange";				break;
		case VRCompositorError_AlreadySubmitted				:	str = "AlreadySubmitted";				break;
		default												:	str = "Unknown error";					break;
		}
		return str;
	}

	C_PCHAR get_type_str( const uint32_t event_type )
	{
		C_PCHAR name = nullptr;
		switch( event_type )
		{
		case vr::VREvent_TrackedDeviceActivated:				name = "TrackedDeviceActivated";				break;
		case vr::VREvent_TrackedDeviceDeactivated:				name = "TrackedDeviceDeactivated";				break;
		case vr::VREvent_TrackedDeviceUpdated:					name = "TrackedDeviceUpdated";					break;

		case vr::VREvent_TrackedDeviceUserInteractionStarted:	name = "TrackedDeviceUserInteractionStarted";	break;
		case vr::VREvent_TrackedDeviceUserInteractionEnded:		name = "TrackedDeviceUserInteractionEnded";		break;
		case vr::VREvent_IpdChanged:							name = "IpdChanged";							break;
		case vr::VREvent_EnterStandbyMode:						name = "EnterStandbyMode";						break;
		case vr::VREvent_LeaveStandbyMode:						name = "LeaveStandbyMode";						break;
		case vr::VREvent_TrackedDeviceRoleChanged:				name = "TrackedDeviceRoleChanged";				break;
		case vr::VREvent_WatchdogWakeUpRequested:				name = "WatchdogWakeUpRequested";				break;
		case vr::VREvent_LensDistortionChanged:					name = "LensDistortionChanged";					break;

		case vr::VREvent_ButtonPress:							name = "ButtonPress";							break;
		case vr::VREvent_ButtonUnpress:							name = "ButtonUnpress";							break;
		case vr::VREvent_ButtonTouch:							name = "ButtonTouch";							break;
		case vr::VREvent_ButtonUntouch:							name = "ButtonUntouch";							break;

		case vr::VREvent_MouseMove:								name = "MouseMove";								break;
		case vr::VREvent_MouseButtonDown:						name = "MouseButtonDown";						break;
		case vr::VREvent_MouseButtonUp:							name = "MouseButtonUp";							break;
		case vr::VREvent_FocusEnter:							name = "FocusEnter";							break;
		case vr::VREvent_FocusLeave:							name = "FocusLeave";							break;
//		case vr::VREvent_Scroll:								name = "Scroll";								break;
		case vr::VREvent_TouchPadMove:							name = "TouchPadMove";							break;
		case vr::VREvent_OverlayFocusChanged:					name = "OverlayFocusChanged";					break;

		case vr::VREvent_InputFocusCaptured:					name = "InputFocusCaptured";					break;	// data is process DEPRECATED
		case vr::VREvent_InputFocusReleased:					name = "InputFocusReleased";					break;	// data is process DEPRECATED
//		case vr::VREvent_SceneFocusLost:						name = "SceneFocusLost";						break;	// data is process
//		case vr::VREvent_SceneFocusGained:						name = "SceneFocusGained";						break;	// data is process
		case vr::VREvent_SceneApplicationChanged:				name = "SceneApplicationChanged";				break;	// data is process - The App actually drawing the scene changed (usually to or from the compositor)
//		case vr::VREvent_SceneFocusChanged:						name = "SceneFocusChanged";						break;	// data is process - New app got access to draw the scene
		case vr::VREvent_InputFocusChanged:						name = "InputFocusChanged";						break;	// data is process
//		case vr::VREvent_SceneApplicationSecondaryRenderingStarted:		name = "SceneApplicationSecondaryRenderingStarted";	break;	// data is process

		case vr::VREvent_HideRenderModels:						name = "HideRenderModels";						break;	// Sent to the scene application to request hiding render models temporarily
		case vr::VREvent_ShowRenderModels:						name = "ShowRenderModels";						break;	// Sent to the scene application to request restoring render model visibility
	
		case vr::VREvent_OverlayShown:							name = "OverlayShown";							break;
		case vr::VREvent_OverlayHidden:							name = "OverlayHidden";							break;
		case vr::VREvent_DashboardActivated:					name = "DashboardActivated";					break;
		case vr::VREvent_DashboardDeactivated:					name = "DashboardDeactivated";					break;
//		case vr::VREvent_DashboardThumbSelected:				name = "DashboardThumbSelected";				break;	// Sent to the overlay manager - data is overlay
//		case vr::VREvent_DashboardRequested:					name = "DashboardRequested";					break;	// Sent to the overlay manager - data is overlay
		case vr::VREvent_ResetDashboard:						name = "ResetDashboard";						break;	// Send to the overlay manager
//		case vr::VREvent_RenderToast:							name = "RenderToast";							break;	// Send to the dashboard to render a toast - data is the notification ID
		case vr::VREvent_ImageLoaded:							name = "ImageLoaded";							break;	// Sent to overlays when a SetOverlayRaw or SetOverlayFromFile call finishes loading
		case vr::VREvent_ShowKeyboard:							name = "ShowKeyboard";							break;	// Sent to keyboard renderer in the dashboard to invoke it
		case vr::VREvent_HideKeyboard:							name = "HideKeyboard";							break;	// Sent to keyboard renderer in the dashboard to hide it
		case vr::VREvent_OverlayGamepadFocusGained:				name = "OverlayGamepadFocusGained";				break;	// Sent to an overlay when IVROverlay::SetFocusOverlay is called on it
		case vr::VREvent_OverlayGamepadFocusLost:				name = "OverlayGamepadFocusLost";				break;	// Send to an overlay when it previously had focus and IVROverlay::SetFocusOverlay is called on something else
		case vr::VREvent_OverlaySharedTextureChanged:			name = "OverlaySharedTextureChanged";			break;
//		case vr::VREvent_DashboardGuideButtonDown:				name = "DashboardGuideButtonDown";				break;
//		case vr::VREvent_DashboardGuideButtonUp:				name = "DashboardGuideButtonUp";				break;
		case vr::VREvent_ScreenshotTriggered:					name = "ScreenshotTriggered";					break;	// Screenshot button combo was pressed, Dashboard should request a screenshot
		case vr::VREvent_ImageFailed:							name = "ImageFailed";							break;	// Sent to overlays when a SetOverlayRaw or SetOverlayfromFail fails to load
		case vr::VREvent_DashboardOverlayCreated:				name = "DashboardOverlayCreated";				break;

		// Screenshot API
		case vr::VREvent_RequestScreenshot:						name = "RequestScreenshot";						break;	// Sent by vrclient application to compositor to take a screenshot
		case vr::VREvent_ScreenshotTaken:						name = "ScreenshotTaken";						break;	// Sent by compositor to the application that the screenshot has been taken
		case vr::VREvent_ScreenshotFailed:						name = "ScreenshotFailed";						break;	// Sent by compositor to the application that the screenshot failed to be taken
		case vr::VREvent_SubmitScreenshotToDashboard:			name = "SubmitScreenshotToDashboard";			break;	// Sent by compositor to the dashboard that a completed screenshot was submitted
		case vr::VREvent_ScreenshotProgressToDashboard:			name = "ScreenshotProgressToDashboard";			break;	// Sent by compositor to the dashboard that a completed screenshot was submitted

		case vr::VREvent_Notification_Shown:					name = "Notification_Shown";					break;	
		case vr::VREvent_Notification_Hidden:					name = "Notification_Hidden";					break;	
		case vr::VREvent_Notification_BeginInteraction:			name = "Notification_BeginInteraction";			break;	
		case vr::VREvent_Notification_Destroyed:				name = "Notification_Destroyed";				break;	
	
		case vr::VREvent_Quit:									name = "Quit";									break;	// data is process
		case vr::VREvent_ProcessQuit:							name = "ProcessQuit";							break;	// data is process
//		case vr::VREvent_QuitAborted_UserPrompt:				name = "QuitAborted_UserPrompt";				break;	// data is process
		case vr::VREvent_QuitAcknowledged:						name = "QuitAcknowledged";						break;	// data is process
		case vr::VREvent_DriverRequestedQuit:					name = "DriverRequestedQuit";					break;	// The driver has requested that SteamVR shut down

		case vr::VREvent_ChaperoneDataHasChanged:				name = "ChaperoneDataHasChanged";				break;	
		case vr::VREvent_ChaperoneUniverseHasChanged:			name = "ChaperoneUniverseHasChanged";			break;	
		case vr::VREvent_ChaperoneTempDataHasChanged:			name = "ChaperoneTempDataHasChanged";			break;	
		case vr::VREvent_ChaperoneSettingsHaveChanged:			name = "ChaperoneSettingsHaveChanged";			break;	
		case vr::VREvent_SeatedZeroPoseReset:					name = "SeatedZeroPoseReset";					break;	

		case vr::VREvent_AudioSettingsHaveChanged:				name = "AudioSettingsHaveChanged";				break;	

		case vr::VREvent_BackgroundSettingHasChanged:			name = "BackgroundSettingHasChanged";			break;	
		case vr::VREvent_CameraSettingsHaveChanged:				name = "CameraSettingsHaveChanged";				break;	
		case vr::VREvent_ReprojectionSettingHasChanged:			name = "ReprojectionSettingHasChanged";			break;	
		case vr::VREvent_ModelSkinSettingsHaveChanged:			name = "ModelSkinSettingsHaveChanged";			break;	
		case vr::VREvent_EnvironmentSettingsHaveChanged:		name = "EnvironmentSettingsHaveChanged";		break;	
		case vr::VREvent_PowerSettingsHaveChanged:				name = "PowerSettingsHaveChanged";				break;	

		case vr::VREvent_StatusUpdate:							name = "StatusUpdate";							break;	

		case vr::VREvent_MCImageUpdated:						name = "MCImageUpdated";						break;	

		case vr::VREvent_FirmwareUpdateStarted:					name = "FirmwareUpdateStarted";					break;	
		case vr::VREvent_FirmwareUpdateFinished:				name = "FirmwareUpdateFinished";				break;	

		case vr::VREvent_KeyboardClosed:						name = "KeyboardClosed";						break;	
		case vr::VREvent_KeyboardCharInput:						name = "KeyboardCharInput";						break;	
		case vr::VREvent_KeyboardDone:							name = "KeyboardDone";							break;	// Sent when DONE button clicked on keyboard

//		case vr::VREvent_ApplicationTransitionStarted:			name = "ApplicationTransitionStarted";			break;	
//		case vr::VREvent_ApplicationTransitionAborted:			name = "ApplicationTransitionAborted";			break;	
//		case vr::VREvent_ApplicationTransitionNewAppStarted:	name = "ApplicationTransitionNewAppStarted";	break;	
		case vr::VREvent_ApplicationListUpdated:				name = "ApplicationListUpdated";				break;	
		case vr::VREvent_ApplicationMimeTypeLoad:				name = "ApplicationMimeTypeLoad";				break;	

//		case vr::VREvent_Compositor_MirrorWindowShown:			name = "Compositor_MirrorWindowShown";			break;	
//		case vr::VREvent_Compositor_MirrorWindowHidden:			name = "Compositor_MirrorWindowHidden";			break;	
		case vr::VREvent_Compositor_ChaperoneBoundsShown:		name = "Compositor_ChaperoneBoundsShown";		break;	
		case vr::VREvent_Compositor_ChaperoneBoundsHidden:		name = "Compositor_ChaperoneBoundsHidden";		break;	

		case vr::VREvent_TrackedCamera_StartVideoStream:		name = "TrackedCamera_StartVideoStream";		break;	
		case vr::VREvent_TrackedCamera_StopVideoStream:			name = "TrackedCamera_StopVideoStream";			break;	
		case vr::VREvent_TrackedCamera_PauseVideoStream:		name = "TrackedCamera_PauseVideoStream";		break;	
		case vr::VREvent_TrackedCamera_ResumeVideoStream:		name = "TrackedCamera_ResumeVideoStream";		break;	
		case vr::VREvent_TrackedCamera_EditingSurface:			name = "TrackedCamera_EditingSurface";			break;	

		case vr::VREvent_PerformanceTest_EnableCapture:			name = "PerformanceTest_EnableCapture";			break;	
		case vr::VREvent_PerformanceTest_DisableCapture:		name = "PerformanceTest_DisableCapture";		break;	
		case vr::VREvent_PerformanceTest_FidelityLevel:			name = "PerformanceTest_FidelityLevel";			break;	
		default:
			VR_ERR_PRINT_STRING( "Event type %d unknown", event_type );
			break;
		}
		return name;
	}

	// Purpose: Processes a single VR event
	void process_vr_event( IVRSystem* system, const VREvent_t& event )
	{
		C_PCHAR name = nullptr;
		uint32_t type = event.eventType;
		TrackedDeviceIndex_t device_index = event.trackedDeviceIndex;
		switch( type )
		{
		case VREvent_TrackedDeviceActivated:
			SetupRenderModelForTrackedDevice( system, device_index );
			VR_PRINT_STRING( "Device %u : Attached. Setting up render model.", device_index );
			break;
		case VREvent_TrackedDeviceDeactivated:
			VR_PRINT_STRING( "Device %u : Detached.", device_index );
			break;
		case VREvent_TrackedDeviceUpdated:
			VR_PRINT_STRING( "Device %u : Updated", device_index );
			break;
		default:
			if( c_aaavr::b_verbose )
			{
				name = get_type_str( type );
				if( name )
				{
					if( device_index == k_unTrackedDeviceIndexInvalid )
						VR_PRINT_STRING( "Got event %s", name );
					else
						VR_PRINT_STRING( "Device %d : Got event %s", device_index, name );
				}
				else if( type!=0 )
				{
					if( device_index == k_unTrackedDeviceIndexInvalid )
						VR_PRINT_STRING( "Got event %d", type );
					else
						VR_PRINT_STRING( "Device %d : Got event %d", device_index, type );
				}
			}
			break;
		}
	}
}

void c_aaavr::handle_input()
{
	// Process SteamVR events
	VREvent_t event;
	while( g_system->PollNextEvent( &event, sizeof( event ) ) )
	{
		process_vr_event( g_system, event );
	}

	// Process SteamVR controller state
	for( TrackedDeviceIndex_t dev = 0; dev < DEVICE_MAX_NB; dev++ )
	{
		VRControllerState_t state;
		if( g_system->GetControllerState( dev, &state, sizeof(state) ) )
		{
			struct st_device_data& dev_data = device_data[dev];
			INT32 index = index_sensor[ dev ];
			c_sensor_6dof* sen = ( index >= 0 ) ? sensors[index] : nullptr;	

			// If packet num matches that on your prior call, then the controller state hasn't been changed since 
			// your last call and there is no need to process it
			if( dev_data.packet_num_last != state.unPacketNum )
			{
				dev_data.packet_num_last = state.unPacketNum;

				if( c_aaavr::b_verbose )
				{
					//todo use PRIx64 for win64
					// bit flags for each of the buttons. Use ButtonMaskFromId to turn an ID into a mask
					VR_PRINT_STRING( "Button pressed 0x%I64x", state.ulButtonPressed );
					VR_PRINT_STRING( "Button touched 0x%I64x", state.ulButtonTouched );
					//if( state.ulButtonPressed != 0 )
					//	VR_PRINT_STRING( "Button pressed on ", event.trackedDeviceIndex );
					//m_rbShowTrackedDevice[ dev ] = state.ulButtonPressed == 0;

					// Axis data for the controller's analog inputs
					for( INT32 i=0; i<5; ++i )
					{
						VRControllerAxis_t& ca = state.rAxis[i];
						VR_PRINT_STRING( " axis %f %f", ca.x, ca.y );
					}
				}

				if( sen )
				{
					c_sensor_6dof* sen = sensors[index];	
						
					sen->set_button( 0, state.ulButtonPressed & ButtonMaskFromId(k_EButton_SteamVR_Touchpad)	);
					sen->set_button( 1, state.ulButtonPressed & ButtonMaskFromId(k_EButton_SteamVR_Trigger)		);
					sen->set_button( 2, state.ulButtonPressed & ButtonMaskFromId(k_EButton_ApplicationMenu)		);
					sen->set_button( 3, state.ulButtonPressed & ButtonMaskFromId(k_EButton_Grip)				);

					sen->set_touch( 0, state.ulButtonTouched & ButtonMaskFromId(k_EButton_SteamVR_Touchpad)		);
					sen->set_touch( 1, state.ulButtonTouched & ButtonMaskFromId(k_EButton_SteamVR_Trigger)		);

					sen->set_value( 0, state.rAxis[0].x );
					sen->set_value( 1, state.rAxis[0].y );
					sen->set_value( 2, state.rAxis[1].x );

					sen->set_button_change( true );
				}
			}
			else
			{
				if( sen )
				{
					c_sensor_6dof* sen = sensors[index];	
					sen->set_button_change( false );
				}

			}
			if( INSIDE_MIN_MAX( index, CTL_START_INDEX, CTL_STOP_INDEX ) )
			{
				if( sen->is_vibrate() )
				{
					g_system->TriggerHapticPulse( dev, 0, sen->get_vibrate_time() );
				}
			}
		}
	}
	//return bRet;
}


void c_aaavr::get_controller_info()
{		// Spew out the controller and pose count whenever they change.
	if( controller_tracked_count != controller_tracked_count_last || pose_valid_count != pose_valid_count_last )
	{
		pose_valid_count_last = pose_valid_count;
		controller_tracked_count_last = controller_tracked_count;
		
		VR_PRINT_STRING( "PoseCount:%d(%s) Controllers:%d", pose_valid_count, string_sumup.c_str(), controller_tracked_count );
	}

	// don't read controllers if somebody else has input focus
	// 2025 Sep don't seems to exist anymore
	// if( g_system->IsInputFocusCapturedByAnotherProcess() )
	//	return;

	controller_tracked_count = 0;
//		for( TrackedDeviceIndex_t dev = k_unTrackedDeviceIndex_Hmd + 1; dev < DEVICE_MAX_NB; ++dev )
	for( TrackedDeviceIndex_t dev = 0; dev < DEVICE_MAX_NB; ++dev )
	{
		if( !poses[dev].bPoseIsValid )
			continue;

		if( !g_system->IsTrackedDeviceConnected( dev ) )
			continue;

		if( g_system->GetTrackedDeviceClass( dev ) == TrackedDeviceClass_Controller )
			++controller_tracked_count;

		const Matrix4& mat = device_data[dev].mat;

		//VR_PRINT_STRING( "%d -> %f, %f, %f ", dev, center.x, center.y, center.z );
		INT32 index = index_sensor[dev];
		if( index >= 0 )
		{
			sensors[index]->set_matrix_in( mat.get() );
			sensors[index]->update_sensor_6dof();
		}
	}
}
void c_aaavr::get_matrices()
{
	VRCompositor()->WaitGetPoses( poses, DEVICE_MAX_NB, nullptr, 0 );

	pose_valid_count = 0;
	string_sumup = "";
	for( INT32 dev = 0; dev < DEVICE_MAX_NB; ++dev )
	{
		CHAR res;
		if( poses[dev].bPoseIsValid )
		{
			++pose_valid_count;
			struct st_device_data& dev_data = device_data[dev];
			dev_data.mat = ConvertSteamVRMatrixToMatrix4( poses[dev].mDeviceToAbsoluteTracking );
			res = dev_data.char_device_class;
			if( res==0 )
			{
				switch( g_system->GetTrackedDeviceClass(dev) )
				{
				case TrackedDeviceClass_HMD:
					res = 'H';
					if( hmd_count < HMD_MAX_NB )
					{
						index_sensor[ dev ] = HMD_START_INDEX + hmd_count;
						++hmd_count;
					}
					else
						VR_ERR_PRINT_STRING( "AAASeed deal with %d Hmd only", HMD_MAX_NB );
					break;
				case TrackedDeviceClass_Controller:	
					res = 'C';
					if( ctl_count < CTL_MAX_NB )
					{
						index_sensor[ dev ] = CTL_START_INDEX + ctl_count;
						++ctl_count;
					}
					else
						VR_ERR_PRINT_STRING( "AAASeed deal with %d controller only", CTL_MAX_NB );
					break;
				case TrackedDeviceClass_TrackingReference:	
					res = 'T';
					if( ref_count < REF_MAX_NB )
					{
						index_sensor[ dev ] = REF_START_INDEX + ref_count;
						++ref_count;
					}
					else
						VR_ERR_PRINT_STRING( "AAASeed deal with %d reference only", REF_MAX_NB );
					break;
				case TrackedDeviceClass_Invalid:			res = 'I';	break;
//				case TrackedDeviceClass_Other:				
				case TrackedDeviceClass_GenericTracker:		res = 'O';	break;
				default:									res = '?';	break;
				}
				dev_data.char_device_class = res;
			}
		}
		else
			res = ' ';
		string_sumup += res;
	}

	if( poses[k_unTrackedDeviceIndex_Hmd].bPoseIsValid )
	{
		mat_hmd_pose = device_data[k_unTrackedDeviceIndex_Hmd].mat;
		mat_hmd_pose.invert();
	}
}


void c_aaavr::c_init()
{
	if( !b_start_with )
		return;

	bool b = VR_IsRuntimeInstalled();
	VR_PRINT_STRING( "Runtime installed : %s", b ? "Yes" : "No" );

	b = VR_IsHmdPresent();
	VR_PRINT_STRING( "Hmd is present : %s", b ? "Yes" : "No" );
	if( !b )
		return;

	EVRInitError vr_error = VRInitError_None;
	g_system = VR_Init( &vr_error, VRApplication_Scene );
	if( vr_error != VRInitError_None )
	{
		g_system = nullptr;
		VR_ERR_PRINT_STRING( "Unable to init VR runtime : %s", VR_GetVRInitErrorAsEnglishDescription( vr_error ) );
		return;
	}
}

void c_aaavr::c_deinit()
{
	g_system = nullptr;
	VR_Shutdown();
}

AAA_ERR c_aaavr::open()
{
	if( !g_system )
		return ERR_ANY;

	for( INT32 i=0; i<DEVICE_MAX_NB; ++i )
	{
		index_sensor	[i] = -1;
		device_data[i].packet_num_last = -42;
	}
	hmd_count = 0;
	ctl_count = 0;
	ref_count = 0;

	VR_PRINT_STRING( "Hmd inited" );
	_b_vr_init = true;

	EVRInitError vr_error = VRInitError_None;
	IVRRenderModels* p_render_model = (IVRRenderModels *)VR_GetGenericInterface( IVRRenderModels_Version, &vr_error );
	if( !p_render_model )
	{
		VR_ERR_PRINT_STRING( "Unable to get render model interface : %s", VR_GetVRInitErrorAsEnglishDescription( vr_error ) );
	}

	//todo more to read
	CHAR* str_driver	= GetTrackedDeviceString(	g_system, k_unTrackedDeviceIndex_Hmd, Prop_TrackingSystemName_String	);
	CHAR* str_display	= GetTrackedDeviceString(	g_system, k_unTrackedDeviceIndex_Hmd, Prop_SerialNumber_String			);
	VR_PRINT_STRING( "Driver %s Display %s", str_driver, str_display );
	FREE( str_display );
	FREE( str_driver );

	_compositor = VRCompositor();
	if( !_compositor )
	{
		VR_ERR_PRINT_STRING( "Compositor initialization failed. See log file for details" );
		return ERR_ANY;
	}
	VR_PRINT_STRING( "Compositor initialized." );

	// get the size of the texture to submit to the hmd
	g_system->GetRecommendedRenderTargetSize( &size_x, &size_y );
	VR_PRINT_STRING( "recommended size is %d x %d", size_x, size_y );

	return _compositor ? AAA_OK : ERR_ANY;
}

void c_aaavr::close()
{
//	if( _b_vr_init )
	{
		_compositor = nullptr;
		_b_vr_init = false;
	}
}

void c_aaavr::update_low() 
{
// 	if( !_compositor )
// 		return;

	handle_input();
	get_controller_info();
	if( _b_submit_clear_trig )
	{
		_b_submit_clear_trig = false;
		clear_submit();
	}
}

void c_aaavr::update_post_swap()
{
//	if( !_compositor )
//		return;
	if( _b_open && is_active() )
		get_matrices();
}

AAA_ERR	c_aaavr::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		AAA_ERR ret = AAA_OK;
		for( INT32 i=0; i<SENSOR_MAX_NB; ++i )
		{
			make_name( filename, filename_in, i );
			if( ERR( sensors[i]->load_from_file_add_ext(filename) ) )
				ret = ERR_ANY;
		}
	o_str::pop_name();
	return ret;
}

AAA_ERR	c_aaavr::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		AAA_ERR ret = AAA_OK;
		for( INT32 i=0; i<SENSOR_MAX_NB; ++i )
		{
			make_name( filename, filename_in, i );
			if( ERR( sensors[i]->save_to_file_add_ext(filename) ) )
				ret = ERR_ANY;
		}
	o_str::pop_name();
	return ret;
}

void c_aaavr::submit_tex_to_hmd( bool b_right, INT32 bind )
{
	if( _b_open && is_active() && _b_submit )
	{
		if( !VRCompositor() )
			return;
		if( bind == -1 )
			bind = b_right ? _submit_bind_right : _submit_bind_left;
		
		SPY_PUSH_RANGE( "submit_tex_to_hmd", spy::COL_1 );
			INT32 tex_gl = tex2d.get_name_gl( bind );
			//	GOL::finish();
			Texture_t tex_info	= { reinterpret_cast<void*>(static_cast<uintptr_t>(tex_gl)), TextureType_OpenGL, ColorSpace_Gamma };
			EVRCompositorError error = VRCompositor()->Submit( b_right ? Eye_Right : Eye_Left, &tex_info );
		SPY_POP_RANGE();
		if( error != VRCompositorError_None )
			VR_ERR_PRINT_STRING( "VRCompositor unable to Submit() texture %d to %s eye : %s", bind, b_right ? "right" : "left", get_error_compositor( error ) );
		//	GOL::finish();
	}
}

void c_aaavr::submit_texs_to_hmd( INT32 bind_left, INT32 bind_right )
{
	if( _b_open && is_active() && _b_submit )
	{
		submit_tex_to_hmd( false, bind_left  );
		submit_tex_to_hmd( true,  bind_right );
	}
}

void c_aaavr::clear_submit()
{
	if( _b_open && is_active() )
		VRCompositor()->ClearLastSubmittedFrame();
}