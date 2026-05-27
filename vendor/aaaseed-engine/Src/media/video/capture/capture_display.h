

#ifdef AAA_CAPTURE_DISPLAY_H
#error "CAPTURE_DISPLAY_H included more than once."
#endif
#define AAA_CAPTURE_DISPLAY_H 1

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif


class	c_thread_grabber;

class	c_cap_display_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_cap_display_ui, c_obj_ui );
protected:
	//UINT32	_size_x;
	//UINT32	_size_y;

	UINT32	_left_ui;
	UINT32	_top_ui;
	UINT32	_right_ui;
	UINT32	_bottom_ui;
	bool	_b_crop_ui;
	bool	_b_use_thread_ui;

public:
	virtual	void	param_init_pt();
			bool	is_threaded()				CONST	{	return _b_use_thread_ui;			}
			bool	is_crop()					CONST	{	return _b_crop_ui;					}
			void	get_crop( RECT* region )	CONST	{	if( region )
															{
																region->bottom	= _bottom_ui;
																region->top		= _top_ui;
																region->left	= _left_ui;
																region->right	= _right_ui;
															}
														}
};


struct ID3D11Device;
struct IDXGIFactory2;
struct ID3D11DeviceContext;
struct IDXGIOutputDuplication;
struct ID3D11Texture2D;
//struct IDXGIOutput1;
struct IDXGIAdapter;
struct IDXGIDevice;
//struct ID3D11Debug;


class	c_capture_display final : public c_capture
{
public:
	static	bool			b_can;
protected:
	c_thread_grabber*		_thread_grabber;
private:
	bool					_b_use_thread;
	bool					_b_thread_closing;
	bool					_b_init;

	INT32					_cap_size_x;
	INT32					_cap_size_y;
	INT32					_cap_framerate;
	c_cap_display_ui*		_display_cap_ui;
//	UINT8*					_buffer_data;
;
	// D3D interfaces
	ID3D11Device*			_d3d11_device;
	IDXGIFactory2*			_dxgi_factory2;
	ID3D11DeviceContext*	_device_context;
	IDXGIOutputDuplication*	_out_duplicate;
//	ID3D11Texture2D*		_desktop_image;
//	IDXGIOutput1*			_dxgi_output1;
	IDXGIAdapter*			_dxgi_adapter;
	IDXGIDevice*			_dxgi_device;
//	ID3D11Debug*			_dxgi_debug;
	ID3D11Texture2D*		_desktop_tex;	//used for interface
	ID3D11Texture2D*		_d3d11_tex2d;
	INT32					_d3d11_sx;
	INT32					_d3d11_sy;

	bool					_b_device_opened;

	RECT					_crop_region;
	bool					_b_crop;

	void					list_display();

	void					close_low();
	AAA_ERR					open_low( UINT32 CONST index );
	AAA_ERR					open_device();
	AAA_ERR					close_device();
	void					release_d3d11_tex();
	void 					update_threading();	

public:
	static	void			c_init();
	static	void			c_deinit();
	static	INT32			do_enum( bool const b_verbose );

	static	INT32				get_device_enumed();		//todo remove similar member fns ?
	static	o_str CONST * CONST get_device_name( INT32 CONST index );

	virtual	UINT32 				get_device_count()			{ return get_device_enumed(); }
	c_capture_display();
	virtual	~c_capture_display();

	//static	void		do_enum_format();
	virtual	AAA_ERR			open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void			close_specific();

	virtual	bool			run( bool CONST b_stream_in );
	virtual	void			stop();

	virtual	void			update();
	virtual	void			ask_frame();

	c_cap_display_ui*		get_capture_ui()							{ return _display_cap_ui; }
			void			set_capture_ui( c_cap_display_ui* p_in )	{ _display_cap_ui = p_in; }

	virtual	void			update_async();
			void			init_thread();
			void			close_thread();
};
