

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,1
--	bu = self:add_button(	{ix, iy }, "Take Control", self, "b_control", false )
		--todo bug bu:set_value_load_save( false )
--		ui.bu_control = bu
end

function meu:init()
	MEDIA.set_dir_media( "Lotus" )
	self.mask = IMGS.get_img( "Lotus_petit_mask_white_blur_4096.png" )
	--self:box_debug( "mask "..self.mask:get_bind() )
end
function meu:init_spe()
	local tmeu = {}
	self.tmeu = tmeu

	tmeu.kinect			= self:get_meu_by_name( "Kinect_2" )
	tmeu.lotus_int		= self:get_meu_by_name( "LotusFP4_1" )
	tmeu.lotus_ext		= self:get_meu_by_name( "LotusExt_1" )
	--tmeu.petal			= self:get_meu_by_name( "LotusPetal_1" )
	--tmeu.petal_fbo		= self:get_meu_by_name( "Ref_Fbo_F4" )

	--tmeu.lotus			= self:get_meu_by_name( "Lotus_1" )
--	tmeu.lotus_attrib	= self:get_meu_by_name( "Attrib_1" )
	--tmeu.lotus_fbo		= self:get_meu_by_name( "Ref_Fbo_F1" )
	--tmeu.distfield		= self:get_meu_by_name( "DistField_1" )

	--self.ui.bu_control:set_value( false )
	--self.b_control = false

	--self.tmeu_back	= self:get_petal_out( "B" )
	--self.tmeu_front	= self:get_petal_out( "F" )
end


function meu:update()
	local cnt = self.count_spe or 1
	aaa.debug.show( cnt, "cnt" )
	if cnt == 1 then
		self:init_spe()
		app:lock_onsite()
		self.count_spe = cnt + 1
	elseif cnt < 20 then
		--self:set_full(1)
		self.count_spe = cnt + 1
	--else
		--self:set_full( self.b_control and 1 or 0 )
	end
	self.count_spe = cnt + 1
	local ui = self.ui

	local delay = self.delay or 0
	delay = math.max( delay - aaa.time.dt, 0 )
	self.delay = delay
	--if r > 180 then
	--	r = 0
	--	self:restart()
	--end
	--self.r = r
	if delay == 0. then
		local tmeu = self.tmeu
		local grey, agit = tmeu.kinect:get_grey_and_agitation()
		--self:print( grey.." "..agit )
		if grey > 10 then
			tmeu.lotus_int:restart()
			tmeu.lotus_ext:restart()
			self.delay = .5
		end
	end
end