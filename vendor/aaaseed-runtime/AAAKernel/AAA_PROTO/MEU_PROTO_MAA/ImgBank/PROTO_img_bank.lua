function meu:define_meu_infos( )
	return	{ author = "Mâa", date = "2025 November",
				name_long = "Image Bank",
				tags = { "2D", "core", "coregraphic", "texture" },
				help = { "Save and load a binds interval [Begin,End] of the image bank.",
						"  Save trigger the generation of a lua file according to the [Begin,End] selected.",
						"  Load trigger the use of this file to load saved binds starting at Destination,",
						"   Check \"Load at init\" to trigger it at init.",
						"  The selected range can eventually be drawn as a square grid." }
			}
end

function meu:draw_icon()
	aaa.draw_grid_lbrt( 5,4, -.5,-.5, -.2, .5 )
end

function meu:define_ui()
	local ref	 = self.ref
	local ui 	 = self.ui
	local bu

	local ix,iy = 1,1
    local SY	= 1
	local DY	= .2
	local SXB = 4
--	self:add_rendering()

	--bu = self:add_trig_method(	{ix,iy,		SXB,SY},	"Load",			self,"load_bind",	false )
	--	iy = iy + SY
	iy = iy + SY
	local SYM = SY * 5
	self:add_bu_texture( 			{ix,iy,		8,SYM},		"Begin",	1    )
	iy = iy + SYM
	self:add_bu_texture(			{ix,iy,		8,SYM},		"End",		2    )
	iy = iy + SYM

	bu = self:add_trig_method(		{ix,iy,		SXB,SY},	"Save",				self,"save_bind" )

	self:add_rendering()

	ix,iy = 9,1
	self:add_camera()
	iy = iy + 1 + DY

	self:add_button(				{ix,iy,		SY,SY},		"Draw",			self,"b_draw",	false	)
	self:add_slider(				{ix+4,iy,	4,SY},		"Offset",		self,"offset",	0,	0,8	)
	iy = iy + SY
	local SXM = 8/3
	self:add_button(				{ix,iy,			SXM,SY},	"Number",	self,"b_number",	true	)
	self:add_button(				{ix+SXM,iy,		SXM,SY},	"Frame",	self,"b_frame",		true	)
	self:add_button(				{ix+SXM*2,iy,	SXM,SY},	"Center",	self,"b_center",	true	)

	iy = iy + SY
	self:add_sliders_xyz_min_max(	{ix,iy,		8,SY},		"Pos",			self, true, 	0, -16,16	)
	iy = iy + SY
	self:add_slider(				{ix,iy,		8,SY},		"Size",			self,"size",	8,	0,64	)

	iy = iy + SY + DY
	bu = self:add_trig_method(		{ix,iy,		SXB,SY},	"Load To",		self,"load_bind",true )
	iy = iy + SY
	bu = self:add_button(			{ix,iy,		SY,SY},		"Load at Init",	self,"b_load_init",	true )
		:set_text_rect_ratio(6)
	iy = iy + SY
	self:add_bu_texture(			{ix,iy,		8,SYM},		"Dst",			3   )
	iy = iy + SYM

end

local filename = "load_binds_generated.lua"
function meu:save_bind()
	local file = io.open( self:get_dir()..filename, "w" )
	if file then
		--io.output(file)
		local i_begin = self:get_texture_bind_2d( 1 )
		local i_end   = self:get_texture_bind_2d( 2 )
		file:write( "meu:print( \"Begin load img binds starting at \"..meu.load_bind_dst )\n" )
		file:write( "local i = meu.load_bind_dst\n" )
		file:write( "local fn = aaa.img.set_bind_filename\n" )
		local count = 0
		for i = i_begin,i_end do
			local fname = aaa.img.get_bind_filename(i)
			if fname == "" then
				file:write( "fn( i+", count, " )\n" )
			else
				file:write( "fn( i+", count, ", \"", fname, "\" )\n" )
			end
			count = count + 1
		end
		file:write( "meu:print( \"End load img binds.\" )\n" )

		file:close()
	end
end
function meu:load_bind( b_dst )
--	aaa.print_fn()
--	self:box_debug( "Hello ooy" )
	-- set the destination in this field so the loading script can get it
	meu.load_bind_dst = self:get_texture_bind_2d( b_dst and 3 or 1 )
 	local fname = self:get_dir()..filename
	aaa.file.do_if_exist_protected( fname, true )
end

function meu:init_after()
	if self.b_load_init then
		self:load_bind( true )
	end
end

function meu:draw()

	self:draw_layers_begin()

		if self.b_draw then
			self:draw_layer( 1 )
			local i_begin = self:get_texture_bind_2d( 1 )
			local i_end   = self:get_texture_bind_2d( 2 )
			local s = self.size

			local nb = i_end - i_begin + 1
			local nb_u = math.ceil( math.sqrt(nb) )
			local dx = s / nb_u
			local dy = dx
			local x = -s / 2 + self.x
			local y =  s / 2 + self.y - dy
			local z = self.z
			local bind_offset = math.floor( self.offset * nb )
			local iu = 0
			for i = 0,nb-1 do
				local bind = i_begin + (i + bind_offset) % nb
				local psx,psy = aaa.img.get_size( bind )
				if psx then
					local ox,oy
					if not self.b_center then
						ox,oy = 0,0
					elseif psx >= psy then
						ox = 0
						oy = (1 - psy/psx) * dx * .5
					else 
						ox = (1 - psx/psy) * dx * .5
						oy = 0
					end
					aaa.img.draw_lbz_size( bind, x+ox,y+oy,z, dx, self.b_frame, false )
				else
					if self.b_number then 
						BU:draw_text_nice_shadow( tostring(bind), x+dx*.25,y+dy*.35,z,	dx*.5,dy*.5, "center", "left" )
					end
				end
				iu = iu + 1
				if iu == nb_u then
					x = -s / 2 + self.x
					y = y - dy
					iu = 0
				else
					x = x + dx
				end
			end
		end
	
	self:draw_layers_end()
end


