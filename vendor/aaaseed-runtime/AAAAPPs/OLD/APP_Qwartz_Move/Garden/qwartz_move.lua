APP.DECLARE( "QWARTZ_MOVE", APP_GP )

local L_APP = QWARTZ_MOVE

function L_APP:set_media_dir()
	MEDIA.set_dir( "Qwartz_Move/" )
end

function L_APP:set_color_fox( id1, id2 )
	self.meu_fox:set_color( id1, id2 )
end
function L_APP:set_color( id1, id2 )
	local meu_colors = self.meu_colors
	local bu1 = meu_colors:get_color_bu( id1 )
	id2 = id2 or (id1+8)
	local bu2 = meu_colors:get_color_bu( id2 )
	VOX.update_def_color_with_bu( bu1, bu2 )
end
function L_APP:get_rgba_back()
	local bu_col = self.bu_color_sel_back
	return self.meu_colors:get_color_bu( bu_col:get_value()+1 ):get_rgba()
end

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	ga:set_onsite_by_machine( { "AAA_QWARTZ_01",  "AAA_QWARTZ_02",  "AAA_QWARTZ_03" } )

	self:create_log_in_dir( app.media_dir_rel.."Qwartz/Move/", 30, true )
	self:add_log( "Start" )
	if self:is_onsite() then
		self:set_shutdown_no_save_on_time( 22, 30 )
	end


	local meu_mocaps	= self:get_meu_by_name( "GridSel_Mocaps"	)
	local meu_particles	= self:get_meu_by_name( "GridSel_Particles")
	local meu_text		= self:get_meu_by_name( "Text_1"			)
	self.meu_colors		= self:get_meu_by_name( "Colors16_Qw"		)
	self.meu_fox		= self:get_meu_by_name( "Fox_1"			)
	self.meu_snd		= self:get_meu_by_name( "SndPlaylist_1"	)
	self.meu_kinect		= self:get_meu_by_name( "Kinect_QwMove"	)
	local meu_voxel		= self:get_meu_by_name( "VoxLed_1"			)

	self:get_mu_by_name( "VoxLed_1"		):set_ui_slot( 1 )
	self:get_mu_by_name( "Fox_1"			):set_ui_slot( 2 )
	self:get_mu_by_name( "Kinect_QwMove"	):set_ui_slot( 3 )

	--now
	if self:is_onsite() then
		local gp = self:get_gp()
		for i = 1, 3 do
			gp:get_ui_slot_bu_meu( i ):__set_window_state("mini")
		end
		self.meu_snd:set_vol_max( .89 )
		self.meu_snd:set_vol_min( .789 )
	end
	meu_voxel:set_bu_value( "simul", false )
	self.meu_fox:set_bu_value( "manual", false )

	local bdd_mocap_kinect = aaa.obj.get( "qwmove_skel_1" );
	self.vox = VOX:create( "Qwartz_move" )
	self.vox:set_meu_voxel( meu_voxel )
	self.vox:set_bdd_mocap_kinect( bdd_mocap_kinect )
	self.vox:set_meu_mocap( meu_mocaps )
	self.vox:set_meu_particle( meu_particles )
	self.vox:set_meu_text( meu_text )

	self.fox = FOX:create( "Main" )
	self.fox.vox = self.vox
	self.fox:set_seq( 3 )

	aaa.flatland.set_focus_offset( nil, -40 )

	return true
end
function L_APP:render_fox( meu )
	local fox = self.fox
	fox.meu = meu
	if self:is_onsite() then
		if meu.b_black_at_night then
			if inside( aaa.time.hour, 0, 7 ) then
				return
			end
		end
	end
	fox:update()
	fox:draw()
end
function L_APP:receive_point_table( str )
	local ok
	local tab
	ok,tab = SERPENT.load( str )
	if ok then
		self.point_table = tab
		--self:print( "received" )
	end
end


if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
--[[
	aaa.lua.global.declare( "qwartz_move_receive" )
	function qwartz_move_receive( str )
		qwartz_move:receive_point_table( str )
	end
--]]
end



