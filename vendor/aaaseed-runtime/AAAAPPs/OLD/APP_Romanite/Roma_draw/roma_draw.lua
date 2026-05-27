
function ROMA:set_color( alpha )
	local col = self.colors[3][2]
	gol.color( col[1], col[2], col[3], alpha )
end

function ROMA.draw_main_bu( bu )
	--aaa.print_fn( "toto" )
	local ph = roma.ph_video or 0
	local ui = roma.ui
	ui.bu_main.draw = ROMA.draw_main_bu

	local bu_section = roma.ui.bu_section
	for i=1,3 do
		bu_section[i]:set_alpha_bu(	(1-ph*2) )
	end

--	bus:inc_page()
	gol.set_texture_dim( 0 )
	gol.set_blend_add()
	roma:set_color( .95 + ph *.05 )
	local rect = bu.rect
	--aaa.draw_rect(	-rect.sx*.5,	0-rect.sy*.5,	-rect.sx*.5+rect.sx, rect.sy 	)
	local x_left = -.5
	aaa.draw_rect(	x_left,	-.5,		.5, .5 	)
	roma:set_color( .7+ ph*.25	)
	aaa.draw_rect(	x_left,	-.7-ph*8,	.5, -.5 )
	gol.color_white( 1. )

	local arrow = roma.imgs.arrow
	gol.color_white( (1-ph*2.) )
	local xp = { -.018, .135, .255}
	local x = xp[roma.section_target]
	local y = -.07
	local sy = .2
	local sx = sy * bu:get_ratio_y()
	aaa.draw_bind_rect( arrow:get_bind(),	x,y, x+sx, y+sy  )
	--maatex gol.set_texture_dim(0)
	--bu:draw_border_line( 1. )
end

function ROMA:render()
	--self:print( "ROMA:render() "..ga:is_ui_group_active() )
	local bu = self.ui.bu_main
	if bu then
		local ph
		if false then
			ph = math.fmod( aaa.time.t * 1., 3. ) / 3.
			ph = ( math.sin( ph * 6.2832 ) * 1.2 + 1. ) * .5
		else
			ph = self.ph_video
			if self.s_video == "up" then
				ph = ph + aaa.time.dt
				if ph > 1 then
					ph = 1
					self.s_video = "on"
				end
			elseif self.s_video == "down" then
				ph = ph - aaa.time.dt
				if ph < 0  then
					ph = 0
					self.s_video = "off"
				end
			end
		end
		ph = clamp_01( ph )
		self.ph_video = ph

		bu:set_y( -1.9 + ph * 3.9 )
		if ph <= 0 then
			self:stop_video()
		end
		self:set_video_volume( math.pow( ph, .3 ) )
		local bu = self.ui.bu_video
		bu:set_y( -5 + ph * 4.75 )

		self.__mu_out:set_value( ga:is_ui_group_active() and 0 or 1 )
		self.bus_ui:set_active( not ga:is_ui_group_active() )

		self.ui.bu_bandeau:set_alpha( (ph-.5) * 2. )

		local inter = self.section_inter
		if inter then
			inter = inter + aaa.time.dt * .1
			if inter > 1 then
				inter = nil
				self.section_cur = self.section_target
			end
			self.section_inter = inter
			--self:print( self.section_cur.." -> "..self.section_target.." "..inter )
		end

		local sections = self.sections
		for i=1,#sections do
			local section = sections[i]
			local d = section.section_id - self.section_target
			local d2 = 1 - clamp_01( math.abs( d ) ) * 2
			section.alpha = clamp_01( section.alpha + d2 * aaa.time.dt * 1.5 )

			local color
			if d == 0 then
				color = { r=1, g=1, b=1, a=1. }
			else
				color = { r=163/255, g=197/255, b=222/255, a=1. }
			end
			self.ui.bu_section[i].color = color
		end
	end

	oo.getsuper(ROMA).render(self)
end
