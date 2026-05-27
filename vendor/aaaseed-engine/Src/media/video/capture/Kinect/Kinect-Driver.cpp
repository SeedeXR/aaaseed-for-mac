/*
* This file was ported by Stijn Kuipers / Zephod from a part of the OpenKinect 
* Project. http://www.openkinect.org
*
* Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
* for details.
*
* This code is licensed to you under the terms of the Apache License, version
* 2.0, or, at your option, the terms of the GNU General Public License,
* version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
* or the following URLs:
* http://www.apache.org/licenses/LICENSE-2.0
* http://www.gnu.org/licenses/gpl-2.0.txt
*
* If you redistribute this file in source form, modified or unmodified, you
* may:
*   1) Leave this header intact and distribute it under the same terms,
*      accompanying it with the APACHE20 and GPL20 files, or
*   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
*   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
* In all cases you must keep the copyright notice intact and include a copy
* of the CONTRIB file.
*
* Binary distributions must follow the binary distribution requirements of
* either License.
*/

#include <conio.h>

#include "Kinect-win32.h"
#include "Kinect-win32-internal.h"

#include "wrap_libusb.h"
#include "spy.h"
#include "aaa_mem.h"


#define DORGB	1
#define DODEPTH	1

namespace Kinect
{
#include "init.h"

	DWORD WINAPI DepthThread( LPVOID lpParam ) 
	{ 
		KinectInternalData* KID  = (KinectInternalData*) lpParam;
		if( DODEPTH )
		{
		}
		else
		{
			return 0;
		}

		KID->LockDepthThread();
		KID->mDepthInput = new KinectFrameInput(KID, KID->mDeviceHandle, 0x82, 1760, DEPTH_PKTS_PER_XFER, DEPTH_NUM_XFERS, 422400);
		if (KID->mDepthInput)
		{

			while (KID->DepthRunning)
			{
				int loopcount = 0;
				while (	KID->mDepthInput->Reap() && loopcount++ < 20);
				//if(loopcount == 20) printf("too much depth");
				spy::sleep( 0, "Kinect::DepthThread()" );
			};
			delete KID->mDepthInput;
			KID->mDepthInput = nullptr;
		};

		KID->UnlockDepthThread();

		return 0;
	};


	DWORD WINAPI RGBThread( LPVOID lpParam ) 
	{ 
		KinectInternalData *KID  = (KinectInternalData*) lpParam;
		if (DORGB)
		{
		}
		else
		{
			return 0;
		};
		KID->LockRGBThread();
		KID->mRGBInput = new KinectFrameInput(KID, KID->mDeviceHandle, 0x81, 1920, RGB_PKTS_PER_XFER, RGB_NUM_XFERS, 307200 );
		if (KID->mRGBInput )
		{

			while (KID->RGBRunning)
			{
				int loopcount = 0;
				while (	KID->mRGBInput->Reap() && loopcount++ < 20);
				//if(loopcount == 20) printf("too much depth");
				spy::sleep( 1, "sleep capture_kinect::RGBThread()" );
			};
			delete KID->mRGBInput;
			KID->mRGBInput = nullptr;
		};

		KID->UnlockRGBThread();
		return 0;
	};


	struct cam_hdr {
		uint8_t magic[2];
		uint16_t len;
		uint16_t cmd;
		uint16_t tag;
	};

	int KinectInternalData::WriteRegister(unsigned short registeridx, unsigned short value)
	{
		unsigned short reply[2];
		unsigned short command[2];
		int res;

		command[0] = registeridx;
		command[1] = value;

		TraceMessage("Writing value %04x to register %04x", value, registeridx);
		res = SendCommand(0x03, (unsigned char*)command, 4, (unsigned char*)reply, 4);
		if (res < 0) return res;
		if (res != 2)
		{
			WarningMessage("WriteRegister: SendCommand returned %d [%04x %04x], 0000 expected!", res, reply[0], reply[1]);
		}
		return 0;
	};

	int KinectInternalData::SendCommand( unsigned short command, unsigned char *commandbuffer, unsigned int commandbufferlength, unsigned char *replybuffer, unsigned int replybufferlength )
	{
		int ret = 0;
		uint8_t obuf[0x2000];
		uint8_t ibuf[0x2000];
		ZeroMemory( obuf, 0x2000 );
		ZeroMemory( ibuf, 0x2000 );

		cam_hdr *chdr = (cam_hdr *)obuf;
		cam_hdr *rhdr = (cam_hdr *)ibuf;		

		chdr->cmd = command;
		chdr->tag = mCommandTag;
		chdr->len = commandbufferlength / 2;
		chdr->magic[0] = 0x47;
		chdr->magic[1] = 0x4d;


		MEMCPY( obuf + sizeof (*chdr), commandbuffer, commandbufferlength, __FUNCTION__ );


		ret = dll_libusb.usb_control_msg( mDeviceHandle, 0x40, 0, 0, 0, (char*)obuf, commandbufferlength + sizeof (cam_hdr), 1600 );
		//ret = usb_control_msg(mDeviceHandle, 0x40, 0, 0, 0, (char*)ibuf, 0x200, 1600);
		if( ret < 0 )
		{
			ErrorMessage( "SendCommand: Failed to send control message! (%d)\n", ret );

			return ret;
		}

		do
		{
			ret = dll_libusb.usb_control_msg( mDeviceHandle, 0xc0, 0, 0, 0, (char*)ibuf, 0x200, 1600 );
			spy::sleep( 10, "KinectInternalData::SendCommand()" );
		}
		while( ret == 0 );

		TraceMessage( "Control reply: %d\n", ret );
		if( ret < sizeof(cam_hdr) )
		{
			ErrorMessage ("SendCommand: Input control transfer failed (%d)\n", ret );
			return ret;
		}
		ret -= sizeof(cam_hdr);

		if( rhdr->magic[0] != 0x52 || rhdr->magic[1] != 0x42 )
		{
			ErrorMessage ("SendCommand: Bad magic %02x %02x\n", rhdr->magic[0], rhdr->magic[1] );
			return -1;
		}
		if( rhdr->cmd != chdr->cmd )
		{
			ErrorMessage ("SendCommand: Bad cmd %02x != %02x\n", rhdr->cmd, chdr->cmd );
			return -1;
		}
		if( rhdr->tag != chdr->tag )
		{
			ErrorMessage ("SendCommand: Bad tag %04x != %04x\n", rhdr->tag, chdr->tag );
			return -1;
		}
		if(	rhdr->len != (ret / 2) )
		{
			ErrorMessage ("SendCommand: Bad len %04x != %04x\n", rhdr->len, (int) (ret / 2) );
			return -1;
		}

		if( ((unsigned int)ret) > replybufferlength)
		{
			WarningMessage ("SendCommand: Data buffer is %d bytes long, but got %d bytes\n", replybufferlength, ret );
			MEMCPY( replybuffer, ibuf + sizeof (*rhdr), replybufferlength, __FUNCTION__ );
		}
		else
		{
			MEMCPY( replybuffer, ibuf + sizeof (*rhdr), ret, __FUNCTION__ );
		}
	

		++mCommandTag;

		return ret;
	};

	void KinectInternalData::WriteCameraRegisters()
	{


		uint8_t obuf[0x2000];
		uint8_t ibuf[0x2000];
		ZeroMemory(obuf, 0x2000);
		ZeroMemory(ibuf, 0x2000);

		int ret = 0;	

		dll_libusb.usb_control_msg(mDeviceHandle, 0x80, 0x06, 0x3ee, 0, (char*)ibuf, 0x12, 500);
		
		SetRGBMode(RGB_mode_bayer);

		WriteRegister(0x12, 0x03);
		WriteRegister(0x13, 0x01);
		WriteRegister(0x14, 0x1e);
		WriteRegister(0x06, 0x02);     // start depth stream

		return;
	}

	KinectInternalData::~KinectInternalData()
	{
		CloseDevice();
		delete depth_sourcebuf2;
		delete rgb_buf2;
	};



	void KinectInternalData::RunThread_rgb()
	{
		if (DORGB)
		{
			if( RGBRunning )
			{
				ERR_PRINT_STRING( "%s() Thread rgb already running", __FUNCTION__ );
				return;
			}

			RGBRunning = true;

			DWORD rid ;
			HANDLE rgbthread = CreateThread( nullptr, 0, RGBThread, this, 0, &rid );   
			SetThreadPriority(rgbthread, THREAD_PRIORITY_TIME_CRITICAL);
		}
	};

	void KinectInternalData::RunThread_depth()
	{
		if (DODEPTH)
		{
			if( DepthRunning )
			{
				ERR_PRINT_STRING( "%s() Thread Depth already running", __FUNCTION__ );
				return;
			}

			DepthRunning = true;

			DWORD did;
			HANDLE depththread = CreateThread( nullptr, 0, DepthThread, this, 0, &did );   
			SetThreadPriority(depththread, THREAD_PRIORITY_TIME_CRITICAL);
		}
	};

	void KinectInternalData::StopThread_rgb()
	{
		RGBRunning = false;
		LockRGBThread();
		UnlockRGBThread();

		//Running = false;
		//ThreadDone = false;
	}
	void KinectInternalData::StopThread_depth()
	{
		DepthRunning = false;
		LockDepthThread();
		UnlockDepthThread();

		//Running = false;
		//ThreadDone = false;
	}

	void KinectInternalData::CloseDevice()
	{
		if (mDeviceHandle)
		{
			StopThread_rgb();
			StopThread_depth();
			if (mDeviceHandle_Motor)
			{
				dll_libusb.usb_close(mDeviceHandle_Motor);
				mDeviceHandle_Motor = nullptr;
			}
			//maa to check later
			/*
			if (mDeviceHandle_Audio)
			{
			usb_close(mDeviceHandle_Audio);
			mDeviceHandle_Audio = nullptr;
			}
			*/
			dll_libusb.usb_reset(mDeviceHandle);
			mParent->KinectDisconnected();
			dll_libusb.usb_close(mDeviceHandle);
		}
	};

	void KinectInternalData::OpenDevice(struct usb_device *dev, struct usb_device *motordev)
	{
		mDeviceHandle = dll_libusb.usb_open(dev);
		if (!mDeviceHandle) 
		{				
			return;
		}

		mDeviceHandle_Motor = dll_libusb.usb_open(motordev); // dont check for null... just dont move when asked and the pointer is null

		int ret;
		ret = dll_libusb.usb_set_configuration(mDeviceHandle, 1);
		if (ret<0)
		{
			ERR_PRINT_STRING( "usb_set_configuration error: %s", dll_libusb.usb_strerror() );
			//return;
		}

		ret = dll_libusb.usb_claim_interface(mDeviceHandle, 0);
		ret = dll_libusb.usb_set_configuration(mDeviceHandle, 1);

		if (ret<0)
		{
			ERR_PRINT_STRING( "usb_claim_interface error: %s", dll_libusb.usb_strerror() );
			return;
		}

		dll_libusb.usb_clear_halt(mDeviceHandle, 0x81);dll_libusb.usb_clear_halt(mDeviceHandle, 0x82);
		WriteCameraRegisters();
	};

	void KinectInternalData::SetMotorPosition(double newpos)
	{
		if (mDeviceHandle_Motor)
		{
			if (newpos>1) newpos = 1;if(newpos<0) newpos = 0;
			unsigned char tiltValue = (unsigned char)(newpos*255);
			unsigned short value = (unsigned short)(0xffd0 + tiltValue / 5);

			dll_libusb.usb_control_msg( mDeviceHandle_Motor, 0x40, 0x31, value, 0, nullptr, 0, 160 );		
		};
	};

	void KinectInternalData::SetLedMode(unsigned short NewMode)
	{
		if (mDeviceHandle_Motor)
		{			
			dll_libusb.usb_control_msg( mDeviceHandle_Motor, 0x40, 0x06, NewMode, 0, nullptr, 0, 160 );
		};
	};

	bool KinectInternalData::GetAcceleroData(float *x, float *y, float *z)
	{
		if (mDeviceHandle_Motor)
		{
			unsigned char outbuf[10];
			if (dll_libusb.usb_control_msg(mDeviceHandle_Motor, 0xC0, 0x32, 0, 0, (char*)outbuf, 10, 1000)>0)
			{
				unsigned short sx = outbuf[3] + (outbuf[2]<<8);
				unsigned short sy = outbuf[5] + (outbuf[3]<<8);
				unsigned short sz = outbuf[7] + (outbuf[6]<<8);
				short ix = *(short*)(&sx);
				short iy = *(short*)(&sy);
				short iz = *(short*)(&sz);
				*x = ix/512.0f;			
				*y = iy/512.0f;
				*z = iz/512.0f;
				return true;
			};
		};
		return false;
	};

	KinectInternalData::KinectInternalData(Kinect *inParent)
	{
		mParent = inParent;
		mCommandTag = 0;
		mDeviceHandle		= nullptr;
		mDeviceHandle_Audio = nullptr;
		mDeviceHandle_Motor = nullptr;

		mErrorCount = 0;

		mDepthInput			= nullptr;
		mRGBInput			= nullptr;

		//ThreadDone = false;
		//Running = false;
		RGBRunning = false;
		DepthRunning = false;

		depth_sourcebuf2 = new uint8_t[1000*1000*3];
		rgb_buf2= new uint8_t[640*480*3];

		InitializeCriticalSection(&depth_lock);
		InitializeCriticalSection(&rgb_lock);
		InitializeCriticalSection(&depththread_lock);
		InitializeCriticalSection(&rgbthread_lock);
	}

	void KinectInternalData::BufferComplete(KinectFrameInput *source)
	{
		if (source == mDepthInput)
		{
			LockDepth();
			MEMCPY(depth_sourcebuf2, source->mOutputBuffer, 422400, __FUNCTION__ );
			UnlockDepth();		
			mParent->DepthReceived();
			return;
		};

		if (source == mRGBInput)
		{
			LockRGB();
			MEMCPY(rgb_buf2,source->mOutputBuffer, 307200, __FUNCTION__ );
			UnlockRGB();

			mParent->ColorReceived();			
			return;
		};
	};

	void KinectInternalData::SetRGBMode(int newmode)
	{

		switch(newmode)
		{
		default:
		case RGB_mode_bayer:
			mRGBMode = RGB_mode_bayer;
			WriteRegister(0x05, 0x00);     // reset rgb stream
			WriteRegister(0x0c, 0x00);
			WriteRegister(0x0d, 0x01);
			WriteRegister(0x0e, 0x1e);     // 30Hz bayer
			WriteRegister(0x05, 0x01);     // start rgb stream
			WriteRegister(0x47, 0x00);     // disable Hflip
				break;
		case RGB_mode_IR:
			mRGBMode = RGB_mode_IR;			
			WriteRegister(0x05, 0x00);     // reset rgb stream
			WriteRegister(0x0c, 0x00);
			WriteRegister(0x0d, 0x01);
			WriteRegister(0x0e, 0x1e);     // 30Hz bayer
			WriteRegister(0x05, 0x03);     // start ir stream
			WriteRegister(0x47, 0x00);     // disable Hflip
				break;
		};
	};
};