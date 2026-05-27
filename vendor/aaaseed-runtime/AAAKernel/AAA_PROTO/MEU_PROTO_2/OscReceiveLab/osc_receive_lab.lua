function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "core", "input", "interoperability", "unfinished" },
			help = { "Read OSC blk received and process then",
						"Deal with /AAASeed/data_nd (see document)",
						"          /AAASeed/value name value",
						"for now (2025 June)"
					}
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

	local osc = self.osc

	self:add_camera()
	self:add_rendering()
	self:add_transfo()

-- 	self:add_button(     	{ix,iy,			2,SY},	"Send", 	osc,	"b_send",		false	)

 	self:add_button(     	{ix,iy,			4,SY},	"Verbose", 		self,	"b_verbose",		false	)
 	iy = iy + SY
 	self:add_button(     	{ix,iy,			4,SY},	"Tag Verbose", 	self,	"b_verbose_tag",	false	)
	iy = iy + SY 
--	self:add_trig_method(	{ix,iy,			4,SY},	"OSC init", 	app,	"init_osc"				):set_color_back( "load" )
-- 	ui.bu_osc_info =
-- 	self:add_text_info(		{ix+4,iy,		4,SY},	"OSC Info"		):set_text_align_x( "right" )
-- 	iy = iy + SY

-- 	self:add_trig_method(	{ix,iy,			4,SY},	"OSC Test", 	osc,	"send_mess_test", "Osc Lab"	):set_color_back( "reset" )
-- 	iy = iy + SY
	
	iy = iy + SY + SY/2

end

function meu:init()
	self.osc = OSC:create( "OSC_"..self:get_name() )
end

function meu:update()
	local osc = self.osc
--	osc:erase_ndim()

--	table.print( self.osc, "OSC", 2)

	local signature = "Receive Lab"
	local function process_one( m )
		if self.b_verbose_tag then
			m:dump( signature )
		end

		m:pop_tag()

		local tag = m:get_tag()
		if self.b_verbose_tag then
			self:print( "tag is "..tag )
		end

		if		tag == "data_nd" or tag == "ndim"	then	osc:receive_ndim( m )
		elseif	tag == "midi"						then	osc:receive_midi( m )
		elseif	tag == "value"						then	osc:receive_value( m )
		end

	end
	
	OSC_MESS:process_messages( "/AAASeed/", process_one )

-- 	local count = self.s_init
-- 	--self:print( "s_init "..count )
-- 	if count then
-- 		count = count + 1
-- 		if count > 8 then
-- 			self:launch_pd( true )
-- 			self:send_osc_info( "osc_begin" )
-- 			self:send_osc_info( "begin"		)
-- --			self:configure()
-- 			count = nil
-- 		end
-- 		self.s_init = count
-- 	end

end

function meu:get_rgb( id )
	local t = BU_COLOR.COLORS[id + 6]
	return t.r, t.g, t.b
end


function meu:draw_data( ndim_frame )
	local col_id = 0
	local function set_color( id )
		id = col_id + 1
		col_id = id
		local r,g,b = self:get_rgb( id % 3 )
		gol.color4( r,g,b, self:get_alpha() )
	end

	local ts_gr = .2
	local ts_pt = ts_gr * .8

	--table.print( frame, "frame", 6 )
	for gr_name, gr in PAIRS(ndim_frame) do
		set_color()
		--table.print( gr, gr_name, 6 )gol.draw_points_2d( elt[1], elt[2] )
		for gr_id, tab in IPAIRS(gr) do
			--table.print( tab, gr_id, 6 )
			local BIG_VALUE = 1000000.
			local r = { l=BIG_VALUE, r=-BIG_VALUE, b=BIG_VALUE, t=-BIG_VALUE }
			local mz = 0
			local z_count = 0
			for id, elt in IPAIRS(tab) do
				--table.print( elt, id, 6 )
				local x,y,z = elt[1], elt[2], elt[3]
				if not x or not y then
					self:print_error( "Invalid data for id "..id )
					break
				end
				if z then
					aaa.lbrt.include_point( r, x,y )
					z_count = z_count + 1
					mz = mz + z
					gol.draw_points_3d( x,y,z )
				else
					aaa.lbrt.include_point( r, x,y )
					gol.draw_points_2d( x,y )
					aaa.draw_str_xy( tostring(id), x,y, ts_pt,ts_pt )
				end
			end
			if z_count > 0  then 
				mz = mz / z_count
			end
			if mz ~= 0 then
			 	gol.translate_z( mz )
			end
				aaa.draw_rect_line( r.l,r.b, r.r,r.t )
				aaa.draw_str_xy( gr_name.." "..gr_id, r.l+ts_gr*.1,r.t-ts_gr*1.1, ts_gr,ts_gr )
			if mz ~= 0 then
				gol.translate_z( -mz )
			end
		end
	end	

end

function meu:draw()
	local ndim_frame = self.osc:get_ndim_done()
	
	self:draw_layers_begin()
		if ndim_frame then
			self:draw_layer( 1 )	--set attr
			self:draw_data( ndim_frame )
		end
	self:draw_layers_end()
end
