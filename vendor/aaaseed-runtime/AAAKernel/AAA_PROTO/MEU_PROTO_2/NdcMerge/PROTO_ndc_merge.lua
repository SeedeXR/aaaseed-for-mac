function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "core", "coregraphic", "texture", "imageprocessing", "Camera", "unfinished" },
			help = 	{
					"dedicaded Meu to merge two image flux, very fast (fragment shader on Gpu).",
					"Also it have the ui to choose which image is used from an fbo with multiple image:",
					"so a good way to make difference between different framed in same image flux,",
					"and so get an image or just where \"it move\", abs(A-B)+ specialized for this.",
					"It output in the current Fbo",
					"need to be documented better"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix,iy
	local SY = .8
	local DY = .2

--	self:add_camera()

	self:set_tab_key_def()

	ix,iy =  1,1
--	ix,iy = self:define_ui_ndc( {ix,iy, 	8,4}, true )
	ix,iy = self:define_ui_ndc( false, true )
	local iyn = iy + SY + DY

	bu = self:add_selector(		{5,1,		8,SY*3},	"Draw" )
		bu:set_nb_min_0( 4, 3 )
		bu:set_item_text(	1,	"A",		"B",		"Min",		"Max"		)
		bu:set_item_text(	5,	"A-B",		"B-A",		"abs(A-B)",	"abs(A-B)+"	)
		bu:set_item_text(	9,	"Debug",	"",			"",			""			)
		bu:set_target_param( sha:get_ref_frag_int(1) )

	bu = self:add_slider(		{ix,iy,		4,SY},		"Threshold Alpha",	sha:get_ref_frag_float(1), nil,		0,	0,1 )
	bu = self:add_slider(		{ix+12,iy,	4,SY},		"Threshold Out",	sha:get_ref_frag_float(2), nil,		0,	0,1 )
	iy = iy + SY * 2 + DY

--	ix,iy = 1,iyn
	local target = {}
	for i=1,2 do
		local ix = i==1 and 1 or 9
		local name = i==1 and "FboA" or "FboB"
		local t = {}
		bu = self:add_button(	{ix,iy,		1.5,SY},	name,				t,"b_use", 	false )
		bu = self:add_text(		{ix+1.5,iy,	3.5,SY},	name.." Src"		)
			bu:set_value_load_save(true)
			bu:set_preset_use(true)
			t.bu_target = bu
		bu = self:add_slider(	{ix+5,iy,	3,SY},		name.." NB offset", t,"offset",	0,		0,16 )
			:set_value_type_integer(true):set_min_max_strict(true)
		target[i] = t
	end
	self.__fbo_target = target
	iy = iy + SY

	local SYM = 5
	ui.bu_mon = {}
	bu = self:add_monitor( 		{1,iy, 8,SYM}, "Monitor_A", 256 )
		ui.bu_mon[1] = bu
	bu = self:add_monitor( 		{9,iy, 8,SYM}, "Monitor_B", 257 )
		ui.bu_mon[2] = bu
	iy = iy + SYM + DY

	self:add_bu_texture(	{1,iy, 8,SYM}, "In A", 1, false )
	self:add_bu_texture(	{9,iy, 8,SYM}, "In B", 2, false )

	iy = iy + SY + DY
--	bu = self:add_slider(	{ix,iy,		4,SY*.8}, 	"Y_Factor_Top", 	sha:get_ref_frag_float(4), nil, 0, -1, 1 )
--	iy = iy + SY*.8
	-- bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Gamma",		false	)
	-- 	ui.bu_gamma_out = bu

	ix,iy = 9,2.2
	local SYM = 6


end

function meu:init()
	local ref = self.ref

--	self:set_meu_fbo( "light" )

	local sha = self:add_shading()
	sha:set_save_frag_float(	false,	1,2 )
	sha:set_save_frag_int(		false,	1,1 )
	sha:set_save_frag_vec4(		false,	1,1 )
end

function meu:__update_binds()
	local binds = self:get_table_always( "__src_binds" )
	local target = self.__fbo_target
	for i=1,2 do
		local bind
		local t = target[i]
		if t.b_use then
			local name = t.bu_target:get_text()
			local meu_fbo = self:get_meu_by_name_cached( name )
			if meu_fbo then
				--self:print( "name is "..name.." -> "..meu_fbo )		
				local fbo = meu_fbo:__get_fbo_from_offset( -t.offset )
				if fbo then
					bind = fbo:get_attachment_bind( 1 )
				else
					self:print_debug( "could not get fbo with offset "..-t.offset  )
				end
			end
		end
		binds[i] = bind or self:get_texture_bind_2d( i )
	end
	return binds
end

function meu:update_ui()
	local binds = self:get_table( "__src_binds" )
	if binds then
		local mons = self.ui.bu_mon
		for i=1,2 do
			mons[i]:set_texture_bind_2d( binds[i] )
		end
	end
end

function meu:update()
	local binds = self:__update_binds()
-- unused for the moment, used to look at neighboor pixels
--[[
	local sx,sy = aaa.img.get_size( binds[1] )
	if sx then
		local sha = self:get_shading()
		sha:set_frag_vec4( 1, sx,sy, 1/sx,1/sy )
	end	
--]]
end

function meu:draw()
	local binds = self:get_table( "__src_binds" )
--	local ui = self.ui


	-- r,g,b,a = ui.bu_gamma_out:get_rgba()
	-- sha:set_frag_vec4( 4, r,g,b,a )

 	self:draw_layers_begin()
 		self:draw_layer( 1 )

		if binds then
			for i=2,1,-1 do
				gol.set_tex_unit_2d_bind( i-1, binds[i] )
			end
		end
		--gol.set_tex_unit_cur(0)
		local s = 1.
		aaa.draw_rect_uv( -s,-s,s,s )
 	self:draw_layers_end()
end



