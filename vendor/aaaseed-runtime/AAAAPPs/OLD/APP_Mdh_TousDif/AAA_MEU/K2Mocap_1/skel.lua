function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1, 1
	self:add_camera()

	self:add_button( {ix, iy },	"Draw",		self, "b_draw",		false )
	self:add_button( {ix, iy+1 },	"Number",	self, "b_number",	false )

	iy = 4
	self:add_slider(	{ix,iy,	8,1}, 	"z",		self, "cen_z",	2, 2, 4 )
	self:add_slider(	{ix,iy+1,	8,1}, 	"x",		self, "cen_x",	1, -2, 2 )
	self:add_slider(	{ix,iy+2,	8,1}, 	"x Off",	self, "off_x",	0, -2, 2 )
	self:add_slider(	{ix,iy+3,	8,1}, 	"Dist",		self, "dist",	1, .25, 2 )

end
function meu:init()
	local ref = self.ref
	local use_bdd		= {}
	local draw_number	= {}
	for i=2,7 do
		local l = self:get_layer( i )
		use_bdd[i-1]		= param.get_ref( l, "use_bdd" )
		local bdd = self:get_layer_bdd( i )
		draw_number[i-1]	= param.get_ref( bdd, "draw_number" )
	end
	ref.use_bdd		= use_bdd
	ref.draw_number	= draw_number
end

-- todo why this
function meu:render( alpha )

	MEU.render( self, alpha )

	--todo should be move to update ?
	--		check it is ok
	--		then remove this render fn
	local ref = self.ref
	local v_use_bdd = self.b_draw and "update_and_draw" or "update"
	local v_dn		= self.b_number and "Billboard" or "No"
	local use_bdd	= ref.use_bdd
	local dn		= ref.draw_number
	for i=1,6 do
		param.set( use_bdd[i],	v_use_bdd )
		param.set( dn[i],		v_dn )
	end
	--local m = MEU.get_inst_last_by_type( "Kinect" )
	--self:print( "found "..m )
end

function meu:get_bdd_mocap( id )
	return self:get_layer_bdd( id+1 )
end

function meu:draw()
	MEU.draw( self )

	local x = self.cen_x
	local ox = self.off_x
	local z = self.cen_z
	local d_max = self.dist
	local cen = { { ox-x, 0, z }, { ox+x, 0, z } }

	gol.set_line_width( 8 )

	local m = self:get_meu_by_name( "Kinect_2" )
	local used = { 0, 0 }
	local skel_x = { 0, 0 }
	local nb_used = 0
	for is=1,6 do
		local id, p, dst = m:get_skel_id_pos_dst( is )
		if id then
			local color = { 1, 0, 0, 1 }
			for i=1,2 do
				local d = V2.dist_from_xyxy( cen[i][1], cen[i][3], p[1], p[3] )
				if d < d_max then
					used[i] = dst
					skel_x[i] = cen[i][1]
					nb_used = nb_used + 1
					color = { 0, 1, 0, 1 }
					break
				end
			end
			color[4] = .5
			gol.color( color )
			aaa.draw_disk_axe_y( p[1], p[2], p[3], .5 )
			--self:print( "w "..p[4] )
			color[4] = 1
			gol.color( color )
			aaa.draw_circle_axe_y( p[1], p[2], p[3], .5 )
			aaa.draw_line( p[1], -4, p[3], p[1], 4, p[3] )
		end
	end

	local s = d_max*2.
	for i=1,2 do
		local c = cen[i]
		gol.color_blue(.5)
		aaa.draw_disk_axe_y( c[1], c[2], c[3], s, 48 )
		gol.color_blue(1)
		aaa.draw_circle_axe_y( c[1], c[2], c[3], s, 48 )
	end

	app.skel_used = used
	app.skel_x = skel_x
end
