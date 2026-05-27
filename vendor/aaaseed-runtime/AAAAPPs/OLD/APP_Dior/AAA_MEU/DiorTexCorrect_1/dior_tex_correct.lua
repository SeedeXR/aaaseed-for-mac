
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()
	self:add_bu_texture_target_unit()

	self:add_size_uvf_video()
	self:add_trz()

	self:add_monitor( {9,2.2} )

	local ix = 1
	local iy = 4.8
	bu = self:add_button( {ix,	iy },				"Auto",			self,	"b_auto", 	false  )
		bu:set_text_rect_ratio( 3.3333 )
	bu = self:add_slider(	{ix+3,iy,	5,1},	"White",		self,	"white", 	1, 0, 2  )
		ui.bu_white = bu
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Black",		nil,	nil, 		0, 0, 1  )
		ui.bu_black = bu

	local SY = 1
	iy = iy + .2 + 3
	bu = self:add_slider(	{ix,iy,	8,SY}, 	"Gain_Red",		self,	"gain_r", 	1, .5, 2.  )
	bu = self:add_slider(	{ix,iy+SY,	8,SY}, 	"Gain_Green",	self,	"gain_g", 	1, .5, 2.  )
	bu = self:add_slider(	{ix,iy+SY*2,	8,SY}, 	"Gain_Blue",	self,	"gain_b", 	1, .5, 2.  )

	iy = iy + .2 + SY*3
	SY = 1
	bu = self:add_slider(	{ix,iy,	8,SY}, 	"Gamma",		self,	"gamma", 	1, .5, 2.  )
	bu = self:add_slider(	{ix,iy+SY,	8,SY}, 	"Gam_Red",		self,	"gam_r", 	1, .5, 2.  )
	bu = self:add_slider(	{ix,iy+SY*2,	8,SY}, 	"Gam_Green",	self,	"gam_g", 	1, .5, 2.  )
	bu = self:add_slider(	{ix,iy+SY*3,	8,SY}, 	"Gam_Blue",		self,	"gam_b", 	1, .5, 2.  )
end

function meu:init()
	self:add_shading()
	self.black = 0
end
--function meu:update()
--end

function meu:draw()
	self.black = self.white * self.ui.bu_black:get_value()
	local sha = self:get_shading()
	sha:set_frag_float_1_8(
			self.black, 				1. / ( self.white - self.black ),
			self.gain_r,				self.gain_g,						self.gain_b,
			self.gam_r * self.gamma,	self.gam_g * self.gamma,			self.gam_b * self.gamma
			)
	MEU.draw( self )
	--sha:set_frag_float_1( inter )
end

function meu:update_ui()
	local bu = self.ui.bu_white
	bu:set_color_back( self.b_auto and { 1, .5, .5, .5 } or nil )
	bu:set_highlight( not self.b_auto )
end

local b_verbose = false
function meu:update()
	if self.b_auto then
		local mu = app.mu_facetrak
		if mu then
			local m = mu:get_meu_used()
			if m then
				local grey_src, coverage = m:get_max_grey()
				grey_src = interpolate( self.grey_src or .5, grey_src, .5 )
				self.grey_src = grey_src
				if b_verbose then self:print( "\tgrey_src "..grey_src .." "..coverage ) end
				local ui = self.ui
				local b,w = self.black, ui.bu_white:get_value()
				--self:print( b )
				--local grey = grey_src * (w-b) + b
				--local f = math.min( aaa.time.dt, .04 )

				local d
				local limit_high	= .98
				local limit_low		= .96
				local speed_adjust	= .1
				--if grey_src == 1 or ( grey_src > limit and coverage > .5 ) then
				--if grey_src == 1.  then
				--	d = math.max( w, .01 ) * math.max( math.sqrt( coverage ), .01 )
				--	if b_verbose then self:print( "\tSaturated "..d ) end
				--else
				if grey_src > limit_high then
					d = grey_src - limit_high
					d = d * speed_adjust
					if b_verbose then self:print( "\tUnder "..d ) end
				elseif grey_src > limit_low then
					d = 0;
				else
					d = limit_low - grey_src
					d = -d * speed_adjust
					if b_verbose then self:print( "\tOver ".. d ) end
				end


				if d == 0 then
					if b_verbose then self:print( "Stable" ) end
				else
					w = w + d
					w = math.max( w, .001 )	-- protect from a flip to negative some time
					--self:print( "w + d ".. w )

					--self:print( grey_src )
					if b_verbose then self:print( "\t\t"..grey_src.." -> "..w ) end
					--grey = math.sin( aaa.time.t ) * .5 + .5

					--self:set_bu_value( "black", w * .05 )

					--ui.bu_white:set_value( grey )
					--self:print( "white -> "..self.white )
					--ui.bu_black:set_value( w * .05 )
					self:set_bu_value( "white", w )
				end

			end
		end
	end
end
