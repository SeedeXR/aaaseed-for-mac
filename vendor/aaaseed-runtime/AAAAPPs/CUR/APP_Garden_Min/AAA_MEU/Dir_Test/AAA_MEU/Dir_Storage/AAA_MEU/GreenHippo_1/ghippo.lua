function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1
	bu = self:add_button(	{ix, iy }, "Take Control" )

	self.ui.bu_cam_show = bu
		bu:set_method_on_value_change( self, "__update_camera_sel" )
		bu:set_preset_use( false )

	iy = iy +2
	self.fx_nb		= 16
	self.fx_id 		= 1
	self.fx_id_last = self.fx_nb
	bu = self:add_selector(	{ix+1,iy,	3,6},	"FX" )	--notext not save in preset
		bu:set_nb_min_0( 2, 8 )
		ui.fx = bu
		bu:set_text_draw( false )

		--	now we can put the hook
		bu:set_hook_on_value_change( function( bu, balue ) self:set_fx( balue:get_value()+1 ) end )
end

function meu:init_spe()
	local tmeu = {}
	self.tmeu = tmeu

	tmeu.kinect		= self:get_meu_by_name( "Kinect_2" )

	tmeu.displace	= self:get_meu_by_name( "Displace_1" )
	tmeu.opencv		= self:get_meu_by_name( "OpenCV_1" )
	tmeu.fp1		= self:get_meu_by_name( "FP4_1" )
	tmeu.fp2		= self:get_meu_by_name( "FP4_2" )
end

function meu:update_ui()
--[[
	local ui = self.ui
	local bu = ui.fx

	--aaa.debug.show( bu:get_value(), "fx" )
	--aaa.debug.show( ui.cam:get_value(), "cam" )
	local v = bu:get_value() + 1
	if self.__fx_id_last ~= v then
		self.__fx_id_last = v
		self:set_fx( v )
	end
--]]
end

function meu:set_fx_no()
	local tmeu = self.tmeu
	if tmeu then
		tmeu.displace	:set_mu_value( 0 )
		tmeu.opencv		:set_mu_value( 0 )
		tmeu.fp1		:set_mu_value( 0 )
		tmeu.fp2		:set_mu_value( 0 )
	end
end
function meu:set_fx( fx )
	if self.fx_id == fx then return end
	self.fx_id = fx
--	self:print( "sex_fx( "..fx.." )" )

	self:set_fx_no()

	local tmeu = self.tmeu
	fx = fx -1
	local sub = math.floor( fx / 4 )
	local rest = fx % 4

	if tmeu then
		if rest==0 or rest==1 then
			local off = rest + sub*2
			tmeu.displace:set_mu_value( 1 )
			tmeu.displace:change_preset( off+1 )
		elseif rest==2 then
			tmeu.opencv	:set_mu_value( 1 )
			tmeu.fp1	:set_mu_value( 1 )
		elseif rest==3 then
			tmeu.opencv	:set_mu_value( 1 )
			tmeu.fp2	:set_mu_value( 1 )
		end
	end
end

function meu:inc_fx( fx, inc )
	fx = fx + inc
	if 		fx < 1			then fx = self.fx_nb
	elseif	fx > self.fx_nb	then fx = 1
	end
	return fx
end
function meu:update()
	local cnt = self.count_spe or 1
	if cnt < 20 then
		cnt = cnt + 1
		if cnt == 15 then
			self:init_spe()
		end
		self.count_spe = cnt
		return
	end

	local ui = self.ui
	local bu = ui.fx
	--local tmeu = self.tmeu
	--aaa.debug.show( tmeu[1], 1  )
	--aaa.debug.show( tmeu[2], 2  )

	--aaa.debug.show( bu:get_value(), "fx" )
	--aaa.debug.show( ui.cam:get_value(), "cam" )

	local fx = self.fx_id
	local mk = self.tmeu.kinect
	--aaa.debug.show( mk, "mk" )
	if mk and mk.hand_le then
		if mk.hand_le.b_trig then fx = self:inc_fx( fx, -1 )	end
		if mk.hand_ri.b_trig then fx = self:inc_fx( fx, 1 )		end
	end
	self:set_fx( fx )
end

--aaa.net.osc_send( 2, "/AAASeed/test", 3, 2, "zob", 1, 0 )

--aaa.net.osc_test( "/AAASeed/" )

