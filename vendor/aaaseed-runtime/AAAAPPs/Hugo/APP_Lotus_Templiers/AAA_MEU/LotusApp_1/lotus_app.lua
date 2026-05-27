

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,1
	bu = self:add_button(	{ix, iy }, "Take Control", self, "b_control", false )
		--todo bug bu:set_value_load_save( false )
		ui.bu_control = bu
end

function meu:get_petal_out( str )
	local tab = {}
	for i=1,12 do
		local m = self:get_meu_by_name_no_error( "LotusOut_"..str..i )
		if not m then break end
		tab[i] = m
	end
	return #tab>0 and tab or nil
end
function meu:sync_petal_out( tmeu, ph_off )
	if not tmeu then return end

	local sha = tmeu[1]:get_shading()
	local v1, v2, v3 = sha:get_frag_float_1_3()
	local ph = sha:get_frag_float_4()
	for i=2,#tmeu do
		sha = tmeu[i]:get_shading()
		sha:set_frag_float_1_3( v1,v2,v3 )
		sha:set_frag_float_4( ph + (i-1)/6 )
	end
end

function meu:init_spe()
	local tmeu = {}
	self.tmeu_control = tmeu

	tmeu.petal			= self:get_meu_by_name( "LotusPetal_1" )
	tmeu.petal_fbo		= self:get_meu_by_name( "Ref_Fbo_F4" )

	tmeu.lotus			= self:get_meu_by_name( "Lotus_1" )
--	tmeu.lotus_attrib	= self:get_meu_by_name( "Attrib_1" )
	tmeu.lotus_fbo		= self:get_meu_by_name( "Ref_Fbo_F1" )
	tmeu.distfield		= self:get_meu_by_name( "DistField_1" )

	tmeu.lotus2			= self:get_meu_by_name( "Lotus_2" )
	tmeu.lotus2_fbo		= self:get_meu_by_name( "Ref_Fbo_F5" )

	self.ui.bu_control:set_value( false )
	self.b_control = false

	self.tmeu_back	= self:get_petal_out( "B" )
	self.tmeu_front	= self:get_petal_out( "F" )

	self.meu_kinect		= self:get_meu_by_name( "Kinect_2" )
	self.meu_fp			= self:get_meu_by_name( "LotusFP4_1" )
end

function meu:set_full( val )
	if val and self.full ~= val then
		local tmeu = self.tmeu_control
		--self:print( "set_full "..val )
		if tmeu then
			for _,m in pairs(tmeu) do
				--m:print( "iyi" )
				m:set_mu_value(	val )
			end
		end
		self.full = val
	end
end

function meu:update()
	local cnt = self.count_spe or 1
	aaa.debug.show( cnt, "cnt" )
	local full
	local kin = self.meu_kinect
	if cnt == 1 then
		self:init_spe()
		app:lock_onsite()
		if app:is_onsite() then
		else
		end
	elseif cnt < 20 then
		full = 1
		kin:set_accum( true )
	else
		if cnt < 40 then
			kin:set_accum( false )
		end
		full = self.b_control and 1 or 0
		local g,a = kin:get_grey_and_agitation()
		--if a > 30 then
		--	self.meu_fp:restart()
		--end
	end

	local t = self.lotus_time or 0
	t = t + aaa.time.dt
	if t > 180. then
		self.meu_fp:restart()
		t = 0
	end
	self.lotus_time = t

	self:set_full( full )

	self.count_spe = cnt + 1

	self:sync_petal_out( self.tmeu_back,	0 )

	self:sync_petal_out( self.tmeu_front,	0 )


	local ui = self.ui
end