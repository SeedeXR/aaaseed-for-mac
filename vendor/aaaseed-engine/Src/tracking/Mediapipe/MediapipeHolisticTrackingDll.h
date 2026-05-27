#ifndef MEDIAPIPE_HOLISTIC_TRACKING_DLL_H
#define MEDIAPIPE_HOLISTIC_TRACKING_DLL_H

#include "DynamicModuleLoader.h"
using namespace DynamicModuleLoaderSpace;

#define QUAL
//#define QUAL __declspec(dllimport)

typedef QUAL int (*FuncMediapipeHolisticTrackingInit)(const char* model_path, bool is_need_video_outputstream, bool is_need_pose_outputstream, bool is_need_hand_outputstream, bool is_need_face_outputstream);
typedef QUAL int (*FuncMediapipeHolisticTrackingDetectFrameDirect)(int image_width, int image_height, void* image_data, int* detect_result, bool show_result_image);
typedef QUAL int (*FuncMediapipeHolisticTrackingDetectCamera)(bool show_image);
typedef QUAL int (*FuncMediapipeHolisticTrackingRelease)();

class MediapipeHolisticTrackingDll
{
public:
	MediapipeHolisticTrackingDll();
	virtual~ MediapipeHolisticTrackingDll();

public:
	bool LoadMediapipeHolisticTrackingDll( C_PCHAR_C dll_path );
	bool UnLoadMediapipeHolisticTrackingDll();
	bool GetAllFunctions();

public:
	FuncMediapipeHolisticTrackingInit				HolisticTrackingInit;
	FuncMediapipeHolisticTrackingDetectFrameDirect	HolisticTrackingDetectFrameDirect;
	FuncMediapipeHolisticTrackingDetectCamera		HolisticTrackingDetectCamera;
	FuncMediapipeHolisticTrackingRelease			HolisticTrackingRelease;

private:
	DynamicModuleLoader m_DynamicModuleLoader;
};

#endif // !MEDIAPIPE_HOLISTIC_TRACKING_DLL_H
