
#ifdef AAA_IMG_MASTER_H
#error "IMG_MASTER_H included more than once."
#endif
#define AAA_IMG_MASTER_H 1


#ifndef AAA_IMG_BASE_H
#	include "img_base.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_img_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_img_master, c_obj_ui );
	friend	class	c_bdd_img;
public:
	enum class LIBRARY : INT32
	{
		GFLSDK = 0,
		FREEIMAGE,
		MAX_NB,
	};

protected:
	
private:
	bool	_b_bdd_img_force_compute		;
	bool	_b_tga_read_custom				;
	bool	_b_read_rgb_add_alpha			;
	bool	_b_read_bgr_keep				;
		
	bool	_b_compress_read				;	
	bool	_b_compress_write				;	//try to load compressed texture, if does not exist, compress it and save compressed texture

	bool	_b_scale_on_load				;	// rescale image on load, param is not saved
	bool	_b_scale_on_load_save			;	// save rescaled image
	INT32	_scale_on_load_size_min			;	// min size before rescaling, no need to rescale small images
	REAL	_scale_on_load_factor			;	// rescale factor, range is 0.0-1.0
	o_str	_scale_on_load_path				;	// folder to add to path to save the rescaled image

	bool	_b_memory_check_after_reading	;
	bool	_b_unimplemented_verbose		;
	bool	_b_unimplemented_break			;

	bool	_b_net_in_active				;
	bool	_b_net_in_update_continuous		;
	bool	_b_net_out_active				;
	INT32	_net_out_blk_size_max			;


public:
	FINLINE	bool	is_free_when_on_board()			CONST	{	return c_img_base::gb_free_when_on_board_ui;	}

	FINLINE	bool	is_bdd_img_force_compute()		CONST	{	return _b_bdd_img_force_compute;	}
	FINLINE	bool	is_tga_read_custom()			CONST	{	return _b_tga_read_custom;			}
	FINLINE bool	is_read_rgb_add_alpha()			CONST	{	return _b_read_rgb_add_alpha;		}
	FINLINE bool	is_read_bgr_keep()				CONST	{	return _b_read_bgr_keep;			}

	FINLINE	bool	is_compress_read()				CONST	{	return _b_compress_read;			}
	FINLINE	bool	is_compress_write()				CONST	{	return _b_compress_write;			}

	FINLINE bool	is_scale_on_load()				CONST	{	return _b_scale_on_load;			}
	FINLINE bool	is_scale_on_load_save()			CONST	{	return _b_scale_on_load_save;		}
	FINLINE INT32	scale_on_load_size_min()		CONST	{	return _scale_on_load_size_min;		}
	FINLINE REAL	scale_on_load_factor()			CONST	{	return _scale_on_load_factor;		}
	FINLINE o_str CONST*	get_scale_on_load_path() CONST	{	return &_scale_on_load_path;		}

	FINLINE	bool	is_memory_check_after_reading()	CONST	{	return _b_memory_check_after_reading;	}
	FINLINE	bool	is_unimplemented_verbose()		CONST	{	return _b_unimplemented_verbose;	}
	FINLINE	bool	is_unimplemented_break()		CONST	{	return _b_unimplemented_break;		}

	FINLINE	bool	is_net_in_active()				CONST	{	return _b_net_in_active;			}
	FINLINE	bool	is_net_in_update_continuous()	CONST	{	return _b_net_in_update_continuous;	}
	FINLINE	bool	is_net_out_active()				CONST	{	return _b_net_out_active;			}

	FINLINE	INT32	get_net_out_blk_size_max()		CONST	{	return _net_out_blk_size_max;		}

	virtual	void	param_init_pt();
			void	update();
};

extern	c_img_master*	g_img_master;

