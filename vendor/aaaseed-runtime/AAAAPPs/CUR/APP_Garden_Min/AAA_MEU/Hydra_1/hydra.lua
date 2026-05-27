local function pgr( ref, name, obj, pname )
	ref[name]	=	param.get_ref(	obj, pname and pname or name )
end

function meu:init()
	local ref = self.ref
	local hydra = aaa.obj.get_from_top_by_class_no_error( "hydra" )
	if not hydra then return end

	ref.hydra	= hydra
	local o = ref.hydra
		pgr( ref, "active",					o )
		pgr( ref, "open",					o	)
		pgr( ref, "rot_x",					o, 	"base_rot_offset_x"	)
		pgr( ref, "rot_y",					o, 	"base_rot_offset_y"	)
		pgr( ref, "rot_z",					o, 	"base_rot_offset_z"	)

		ref.ctl = {}
		for i=1,2 do
			local ctl = {}
			ref.ctl[i] = ctl
			local pre =  "tra_"..i.."_"
			pgr( ctl, "zero",					o, 	"tra_"..i.."_zero_trig"	)
			pgr( ctl, "center",					o, 	"tra_"..i.."_center_trig"	)
			pgr( ctl, "tra_x",					o, 	"tra_"..i.."_x"	)
			pgr( ctl, "tra_y",					o, 	"tra_"..i.."_y"	)
			pgr( ctl, "tra_z",					o, 	"tra_"..i.."_z"	)
			pgr( ctl, "sca_x",					o, 	"sca_"..i.."_x"	)
			pgr( ctl, "sca_y",					o, 	"sca_"..i.."_y"	)
			pgr( ctl, "sca_z",					o, 	"sca_"..i.."_z"	)
			pgr( ctl, "pos_x",					o, 	"ctl_"..i.."_pos_x"	)
			pgr( ctl, "pos_y",					o, 	"ctl_"..i.."_pos_y"	)
			pgr( ctl, "pos_z",					o, 	"ctl_"..i.."_pos_z"	)
--[[
		ST_PARAM_GROUP_CLOSED( Rot, ROT_PARAM_NB ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_01 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_02 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_03 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_04 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_05 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_06 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_07 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_08 ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_rot_09 ) \
			ST_PARAM_XYZW_LOCKED( ctl_##nb##_rot_quat ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_joy_x ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_joy_y ) \
			ST_PARAM_REAL_LOCKED( ctl_##nb##_trigger ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_1 ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_2 ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_3 ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_4 ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_start ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_bumper ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_button_joystick ) \
			ST_PARAM_INT32_LOCKED( ctl_##nb##_seq_nb ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_enabled ) \
			ST_PARAM_INT32_LOCKED( ctl_##nb##_control_index ) \
			ST_PARAM_BOOL_LOCKED( ctl_##nb##_docked ) \
			ST_PARAM_INT32_LOCKED( ctl_##nb##_which_hand )

	static	ST_PARAM	param[PARAM_NB] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( verbose )

//		ST_PARAM_INT32_POS( base_index, 0, 0 )
		ST_PARAM_BOOL_OFF( open )
		ST_PARAM_BOOL_LOCKED( opened )
		ST_PARAM_STR_LOCKED( setup_txt )

		ST_PARAM_GROUP_CLOSED( Info, INFO_PARAM_NB )
			ST_PARAM_INT32_LOCKED( firmware_revision )
			ST_PARAM_INT32_LOCKED( hardware_revision )
			ST_PARAM_INT32_LOCKED( hemi_tracking )
			ST_PARAM_INT32_LOCKED( controller_nb )

		ST_PARAM_POINT_XYZ( base_rot_offset )
		ST_PARAM_BOOL_ON( coor_one_space )
		ST_PARAM_BOOL_ON( translation_out_in_meter )

		ST_PARAM_GROUP_CLOSED( Filter, FILTER_PARAM_NB )
			ST_PARAM_BOOL_OFF( filter_active )
			ST_PARAM_FLOAT_ZERO( filter_near_range )
			ST_PARAM_FLOAT_ONE_ZERO( filter_near_val )
			ST_PARAM_FLOAT_ZERO( filter_far_range )
			ST_PARAM_FLOAT_ONE_ZERO( filter_far_val )
--]]
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	bu = self:add_trig(	{1, 2 }, "Focus" )
	if ref.hydra then
		bu:set_function_on_click( aaa.obj.set_focus_ui, ref.hydra )
	end

	local ix = 9
	local iy = 2

end

function meu:update()
	if self.verbose >= 2 then aaa.print_method() end

	local ref = self.ref

end
