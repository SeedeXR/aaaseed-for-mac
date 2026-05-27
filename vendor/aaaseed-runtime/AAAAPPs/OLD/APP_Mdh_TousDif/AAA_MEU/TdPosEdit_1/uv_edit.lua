
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	bu = self:add_slider_multi( { 3, 2, 12, 12}	,"UV",	16 )
		local s = .04
		bu:set_elt_sxy( s, s )
		for i=1,bu:get_elt_nb() do
			local elt = bu:get_elt( i )
			elt:set_text( i )
			elt:set_text_xyf( -.5, .5, 1 )
			elt:set_frame_plus( true )
		end
		bu:set_back_bind( 32 )	--todo check this function
		local bus = bu:get_bus_down()
		bus:set_method(	"draw_before",	self, "draw_uv_before" )
		bu:set_uv_transfo( .8, .8, .5, 0 )
	self.ui.uv = bu

	bu = self:add_trig_method(	{1,	1},			"init", 	self.ui.uv, "place_elts"	)

	bu = self:add_trig_method(	{1,	14},			"Import 1", self,		"import_uv", 1		)
	bu = self:add_trig_method(	{1,	15},			"Import 2", self,		"import_uv", 2	)

	--	bus:set_method( "draw_before", 	self, "draw_background" )
	--	bus:set_method( "draw_after",	smartv, "draw_foreground" )
end
function meu:draw_line( a, b )
	gol.vertex2( a[1], a[2], b[1], b[2] )
end
function meu:draw_seg_one( a_id, b_id )
	local uv= self.uvs
	self:draw_line( uv[a_id], uv[b_id] )
end
function meu:draw_uv_before()
	local m = self:get_meu_by_name( "TdSkel_1" )
	self.uvs = self.ui.uv:get_values()
	gol.begin_lines()
		m:get_segs():draw_with_method( self, "draw_seg_one" )
	gol.do_end()
end

function meu:import_uv( id )
	--self:print( id )
	local m = self:get_meu_by_name( "TdSkel_1" )
	local tab = {}
	for i=1,16 do tab[i] = pack( m:get_pt_xyz( i ) ) end
	self.ui.uv:set_values_transformed( tab )
end
function meu:exports_uv()
	return self.ui.uv:get_values_tranformed()
end

-- function meu:update()
-- --	self.ui.uv:set_uv_transfo( .8, .8, .5, 0 )
-- end
function meu:get_preset_nb()	return 24	end
