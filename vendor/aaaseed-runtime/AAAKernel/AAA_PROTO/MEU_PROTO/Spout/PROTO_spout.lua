
if CLASS.DECLARE( "MEU_SPOUT", MEU ) then
	MEU_SPOUT:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,						
		"Spout is a protocol to share low level graphic ressource",
		"Praticly it allows AAASeed to share in texture with other software, with nearly no penalty.",
		"You can receive from or send to other spout software:",
		"TouchDesigner, Smode, Unity, Millumin... to name only a few.",
		"There is spout software to send and receive image flux from machine to machine (e.g. NDI).",
		"with the use of OSC this is the base to work in conjunction with several software." )
end

function MEU_SPOUT:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "Vj", "Core", "CoreGraphic", "input", "output", "interoperability", "texture" }
			}
end

function MEU_SPOUT:__update_expert( exp )
	local b = self.ui.bu_expert:get_value_as_bool()
	for _m,bu in ipairs(exp) do
		bu:set_visible( b )
	end 
end

function MEU_SPOUT:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy = 1,1
	local SY = 1.2
	local DY = .2

	local SX3  = 8/3
	bu = self:add_trig_fn(				{ix,iy,	SX3,SY},	"Focus",	aaa.obj.set_focus_ui, ref.spout2 )
	local exp = {}
	local rec = {}
	local sen = {}
	bu = self:add_button(				{ix,15,	SX3,1},	"Expert",	false )
		bu:set_method_on_value_change(	self, "__update_expert", exp )
		ui.bu_expert = bu
	
	--iy = iy + SY + DY

	--ui.bu_def	= self:add_text_info(	{ix,iy+1,	8,SY}, "State" )
	iy = iy + SY + DY

	--	self:add_button( {		ix, iy + 3 },		"Enum Sender",		self, "b_enum_sender",	false	)
	
	self:add_button(			{ix,	iy,		5,SY},		"Receive",			ref.receive,				nil,	false	)
	rec[1] = self:add_button(	{ix+5,	iy,		3,SY},		"receive V Flip",	ref.receiver_vertical_flip,	nil,	false	):set_text( "V Flip" )

	iy = iy + SY + DY
	local SYT = .6
	bu = self:add_text_info(	{ix,iy,	8,SYT}, "spout_nb"	)
		self.ui.bu_spout_nb = bu
		rec[2] = bu
	
	iy = iy + SYT
	bu = self:add_selector(	{ix,iy,	8,SY}, "Receive Index" )
		bu:set_target_param( ref.receiver_index )
		bu:set_nb_min_0( 8, 1 )
		bu:set_item_text_from_nb_minus_1( 1 )
		bu:set_text_visible( false )
		rec[3] = bu
		--bu:disable_mobile()
		-- bu:set_method_on_value_change(	self, "__update_camera_sel" )	
		-- bu:set_method_on_click(			self, "__set_ui_camera" )
		-- bu:set_method_on_click_double(	self, "__focus_camera" )
	--self:add_slider(	{ix,	iy,		8,SY},		"Receive Index", 	ref.receiver_index,		nil, 	0, 	0,15 	)
	--	:set_min_max_strict( true )
	iy = iy + SY
	bu = self:add_text_info(	{ix,iy,	8,SYT}, "flux used"	)
		self.ui.bu_flux_used = bu
		rec[4] = bu
	iy = iy + SYT*2 + DY
	
	bu = self:add_button(		{ix+1,	iy,		4,SY},	"Receive Image",	ref.receive_image,		nil,	false	):set_text( "Image" )
		exp[1] = bu 
		rec[5] = bu 
	bu = self:add_button(		{ix+5,	iy,		3,SY},	"Receive DX9",		ref.receiver_dx9_asked, nil,	false	):set_text( "DX9" )
		exp[2] = bu
		rec[6] = bu 
	
	iy = iy + SY + DY
	bu = self:add_button( 		{ix,	iy,		4,SY},	"Control Next", 	self,		"b_control_next", 			false	)
		rec[7] = bu 
	bu = self:add_text_info(	{ix+4,	iy,		4,SY},	"Control Info" )
		bu:set_value_load_save( false )
		ui.bu_control_info = bu
		rec[8] = bu 
	iy = iy + SY*2


	ix,iy = 9,1
	ui.bu_state = self:add_text_info(	{ix,iy,	8,SY}, "State" )
	--ui.bu_asked = self:add_text_info(	{ix,iy,		8,SY}, "Asked" )
	iy = iy + SY + DY

	local SYM = 7
	bu = self:add_bu_texture_target_unit( 	{ix,iy, 	8,SYM},	"Texture", 1, false )
		ui.bu_tex = bu
--	bu = self:add_monitor( 				{ix,iy+SYM,	8,SYM},	"Src" )
--		bu:set_render_mode_alpha( 2 )
	iy = iy + SYM + DY

	--self:add_text_info(			{ix,	iy,		8,.7}, 		"Spout Out" )
	--iy = iy + SY + DY
	self:add_button( 			{ix,	iy,		5,SY},	"Send",				ref.send,			  nil,	false	)
	sen[1] = self:add_button(	{ix+5,	iy,		3,SY},	"send V Flip",		ref.sender_vertical_flip,	nil,	false	):set_text( "V Flip" )
	iy = iy + SY + DY
	bu = self:add_text_info(	{ix,	iy,		8,SYT}, "spout_Out"	)
		self.ui.bu_spout_out = bu
		sen[2] = bu
--	self:add_button( {		ix, iy + 3 },			"Enum Sender",		self, "b_enum_sender",	false	)
--	self.ui.bu_spout_nb = self:add_text_info(	{ix,iy + 4,	16,.6}, "spout_nb" )
	iy = iy + SYT*2 + DY


	exp[3] = self:add_button( 	{ix+1,	iy,		4,SY},		"Send Image",		ref.send_image,       nil,	false	):set_text( "Image" )
		sen[3] = exp[3]
	exp[4] = self:add_button( 	{ix+5,	iy,		3,SY},		"Send DX9",			ref.sender_dx9_asked, nil,	false	):set_text( "DX9" )
		sen[4] = exp[4]

	ui.rec = rec
	ui.sen = sen
	self:__update_expert(exp)
end

local function pgr( ref, name, obj, pname )
	ref[name]	=	param.get_ref(	obj, pname and pname or name )
end

function MEU_SPOUT:init()
	local ref = self.ref
	--ref.spout2	= self:get_obj_down( "bdd_spout2" )
--	ref.bdd_cv_writer =aaa.layer.get_bdd( self:get_layer(1) )
	ref.spout2 = aaa.layer.get_bdd( self:get_layer(1) )
	local o = ref.spout2
		pgr( ref, "receive",				o	)
		pgr( ref, "receiving",				o	)
		pgr( ref, "receiving_count",		o	)
		pgr( ref, "receiver_name",			o	)
		pgr( ref, "receiver_index",			o	)
		pgr( ref, "receive_image",			o	)
		pgr( ref, "receiver_vertical_flip",	o	)

		pgr( ref, "send",					o	)
		pgr( ref, "sender_name",			o	)
		pgr( ref, "send_image",				o	)
		pgr( ref, "sending",				o	)
		pgr( ref, "sending_count",			o	)
		pgr( ref, "sender_vertical_flip",	o	)

		pgr( ref, "size_x",					o	)
		pgr( ref, "size_y",					o	)
--		pgr( ref, "memory_share",			o	)
		pgr( ref, "sender_dx9_asked",		o	)
		pgr( ref, "receiver_dx9_asked",		o	)
--		pgr( ref, "is_dx9",					o	)

		pgr( ref, "receiver_sender_count",		o	)
		pgr( ref, "receiver_sender_enum_trig",	o	)

		param.set( ref.sender_name, "AAASeed Garden "..self:get_inst_key() )
end

function MEU_SPOUT:is_send()		return param.get_bool( self.ref.send 		)	end
function MEU_SPOUT:is_sending()		return param.get_bool( self.ref.sending 	)	end
function MEU_SPOUT:is_receive()		return param.get_bool( self.ref.receive		)	end
function MEU_SPOUT:is_receiving()	return param.get_bool( self.ref.receiving	)	end

--function MEU_SPOUT:is_memory_share()	return param.get_bool( self.ref.memory_share )	end
function MEU_SPOUT:is_dx9()			return param.get_bool( self.ref.is_dx9		)	end

function MEU_SPOUT:update_ui()
	local	pg	= param.get
	local 	ref	= self.ref
	local	ui	= self.ui
	local	sx = pg( ref.size_x )
	local	sy = pg( ref.size_y )

	local bu = ui.bu_state
	local str
	local b_sending		= self:is_sending()
	local b_receiving	= self:is_receiving()
	local b_send		= self:is_send()
	local b_receive		= self:is_receive()
	local color

	local b_tex = true
	if b_sending then
		if b_receiving then	str = "Receiving Sending"
		else				str = "Sending"
		end
		color = {0,1,0,1}
	elseif b_receiving then	str = "Receiving"	color = {0,1,0,1}
	else					str = "Neither"		color = {1,0,0,1}	b_tex = false
	end
	bu:set_text_color( color )
	bu:set_text( str ) --..sx.." by "..sy )
	ui.bu_tex:interpolate_alpha_bu( b_tex )
	-- if b_send then
	-- 	if b_receive then	str = "Receive Send"
	-- 	else				str = "Send"
	-- 	end
	-- elseif b_receive then	str = "Receive"
	-- else					str = "Neither"
	-- end
--[[
	local str2
	if self:is_memory_share() then		str2 = "Memory Share"		
	elseif self:is_dx9() then			str2 = "DX9"
	else								str2 = "DX11"
	end
	str = str .. " "..str2
--]]
--	ui.bu_asked:set_text( str )

	ui.bu_spout_nb:set_text( "Receiver Sender Count : "..param.get( self.ref.receiver_sender_count) )
	local index = param.get(ref.receiver_index)
	if inside( index, 0,15 ) then
		ui.bu_flux_used:set_text( param.get( ref.spout2, "src_"..index ).."\n"..param.get(ref.receiving_count) )
	end
	ui.bu_spout_out:set_text( param.get( ref.sender_name ).."\n"..param.get(ref.sending_count) )

	local bu = ui.bu_control_info
	if self.b_control_next then
		if self.__mu_dir_kinect_process then
			--if not self.__b_frame_new then
				bu:set_text( self.__b_frame_new and "Dir Ok" or "Dir Skip" ) 
			--end
		end
	else
		bu:set_text( "No" ) 
	end

	local b = param.get_bool( ref.receive )
	self:interpolate_array_bu( ui.rec, param.get_bool( ref.receive ) )
	self:interpolate_array_bu( ui.sen, param.get_bool( ref.send ) )
end

function MEU_SPOUT:interpolate_array_bu( t, b )
	local alpha = t[1]:interpolate_alpha_bu( b )
	for i = 2,#t do
		t[i]:set_alpha_bu( alpha )
	end
end

function MEU_SPOUT:draw()
	local ref = self.ref

	self:draw_layers_begin()

		self:draw_layer(1)

		if self:is_receiving() then
			local frame_index = param.get(ref.receiving_count)
			self.__b_frame_new = self.__frame_index_last ~= frame_index
			if self.__b_frame_new then
				self.__frame_index_last = frame_index
			end
		else
			self.__b_frame_new = false
		end
		local b_skip = self.b_control_next and (not self.__b_frame_new)

		local m = self.__mu_dir_kinect_process
		if not m then
			local name = self:get_inst_key()
			m = self:get_mu_by_name_no_error( "Dir_KinectA" )
			if m then
				self.__mu_dir_kinect_process = m
				self:print( "found dir mu to control "..m  )
			else
				self:print_error( "no Dir or MuBegin to control" )
			end
		end
		if m then
			m:set_render_skip_next( b_skip )
		end

	self:draw_layers_end()	
end