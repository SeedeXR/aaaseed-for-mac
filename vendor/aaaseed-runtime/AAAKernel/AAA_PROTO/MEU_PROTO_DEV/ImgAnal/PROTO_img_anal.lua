
function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "art", "Draw", "Core", "CoreGraphic", "Procedural", "texture", "unfinished" },
			help = "used to measure the coverage in a rectangle (number of pixel)\n"..
					"and detect the center of non black pixels\n"..
			 		"Encapsulate c_bdd_img_analyse.\n"..
					"Used for Musée de la romanité in Nimes: should be updated to a new generic ImgAnalyse." 
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local SXS = 4
	local SYS = SY * 2

	--todo use it to set the bdd
	self:add_bu_texture( {ix,iy,	8,6},	"Tex In",	1, 	false	)

	--todo add ui to choose rect we use
	ix,iy = 9,1
	self:add_selector(		{ix,iy,			8,SY},	"Detection"		)
		:set_item_text( 1, "Detection", "Simulation", "Direct" )
		:set_target_lua( self, "s_detection" )
		--:set_nb( 3 )
	--	:set_value( 2 )
	iy = iy + SY
	local pres = self.presence
	for i = 1,self.nb_anal do
		local s = 8/self.nb_anal
		local pre = pres[i]
		self:add_slider(	{ix+s*(i-1),iy,		s,SY},	"Cov Max"..i,	pre,	"coverage_max",	0,	0,.5 	)
		self:add_slider(	{ix+s*(i-1),iy+SY,	s,SY},	"Cov "..i,		pre,	"cov",			0,	0,1 	)
		self:add_slider(	{ix+s*(i-1),iy+SY*2,s,SY},	"Accum "..i,	pre,	"accum",		0,	0,1 	)
	--	:set_draw_min(false)	--todo works but not the right way ? solve later
	end
	iy = iy + SY*3 + DY

	--self:add_slider(	{1,12, 8,SY},	"Test",	self:get_layer_bdd(1),	"threshold",		0,	0,1 	)
	self:add_slider(	{ix,iy, 8,SY},	"Center U",	ref.center_u,	nil,		0,	0,1 	):set_color_back("u")
	iy = iy + SY
	self:add_slider(	{ix,iy, 8,SY},	"Center V",	ref.center_v,	nil,		0,	0,1 	):set_color_back("v")
	iy = iy + SY
end


function meu:init()
	local ref = self.ref
--	local bdd = self:get_layer_bdd(1)
--	ref.temp	= param.get_ref( bdd, "received_number_last" )

	self.s_init = 0

	self.presence = {}
	self.nb_anal = 1
	for i = 1,self.nb_anal do
		local pre = {}
		pre.ref_bdd = self:get_layer_bdd(i)
		pre.ref_cov     = param.get_ref( pre.ref_bdd, "out_coverage" )
		pre.ref_cov_max = param.get_ref( pre.ref_bdd, "coverage_max" )
		self.presence[i] = pre
	end

	local bdd = self.presence[1].ref_bdd
	ref.center_u = param.get_ref( bdd, "out_center_u" )
	ref.center_v = param.get_ref( bdd, "out_center_v" )
end



function meu:get_circle_cov( i )
	local pre = self.presence[i]
	return pre.cov
end
function meu:get_circle_accum( i )
	local pre = self.presence[i]
	return pre.accum
end
function meu:set_circle_accum( i, accum )
	local pre = self.presence[i]
	pre.accum = accum
end

--todo deal with coverage, center, rect this was done settle for nimes
--infact call it ImgAnalyse
function meu:update()
	local ref = self.ref

	local dt = aaa.time.dt

	local pres = self.presence
	for i=1,self.nb_anal do
	 	local pre = pres[i]
	 	if self.s_detection~=3 then
	 		if self.s_detection==1 then
	 			param.set( pre.ref_cov_max, pre.coverage_max )
	 			self:update_then_draw( pre.ref_bdd )
	 			pre.cov = param.get( pre.ref_cov )
	 		end
	 		local cov = pre.cov
			--todo expose values used in the interface
	 		pre.accum = clamp_01( pre.accum + ((cov>.05) and (cov*.5) or -1) * dt )
	 	end
	end

--	aaa.show( aaa.time.get_time_now(), "time" )
end

function meu:draw()
	local ui = self.ui
	self:draw_layers_begin()
		self:draw_layer(1)
	self:draw_layers_end()
end