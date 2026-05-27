
function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help = "Use boid with words attached to each point ",
				tags = { "2d", "3D",  "Art", "Draw", "generator", "Point", "Procedural", "unfinished" }
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix = 1
	local iy = 1


	self:add_camera()

	iy = 1
	self:add_trig_restart(	{ix,iy}		)
	self:add_button(	{	ix,	iy+1 },			"boid", 		self, "b_boid", true	)
	self:add_button(	{	ix+4,	iy+1 },		"debug", 		self, "b_debug", true	)
	iy = iy + 2.2
	local nb = #(self.data)
	bu = self:add_selector(	{ix,iy,	8,2},		"List" )
		bu:set_nb( 4, 2 )
		bu:set_target_lua( self, "data_id" )
		self.data_id = 1
		for i=1,nb do
			bu:set_item_text( i, self.data[i].abv )
		end
		bu:disable_mobile()
	iy = iy + 2.2
		self:add_button(	{	ix,	iy },		"line", 		self, "b_line", false	)
		bu = self:add_selector(	{ix+3,iy,4,1},	"Axe" )
			bu:set_nb( 3 )
			bu:set_target_lua( self, "line_axe" )
			bu:set_item_text( 1, "X", "Y", "Z" )
		self:add_slider(	{ix,iy+1,	8,1},	"Interval",		self, "line_interval",		0,	0, 3	)
		self:add_slider(	{ix,iy+3,	8,1},	"SkipBegin",	self, "skip_begin",		2,	0, 8)
		self:add_slider(	{ix,iy+4,	8,1},	"Adn", 			self, "adn_freq",			0,	0, 1	)
	iy = iy + 5.2
		self:add_button(	{ix,	iy },		"Run",			self, "b_line_run", 		false )
		self:add_button(	{ix+4,	iy },		"Inv",			self, "b_speed_inverse", 	false )
		self:add_slider(	{ix,iy+1,	8,1},	"Speed", 		self, "line_speed",			0, 0, 1	)
		bu = self:add_slider({ix,iy+2,	8,1},	"Phase", 		self, "line_phase",			0, 0, 1	)
		self.ui.bu_phase = bu

--	self:add_button(	{	ix,	iy+5 },			"young Leaders", 	self, "b_yl", true	)

	ix, iy = 9, 2
--	self:add_trig_method(	{ix+1,	iy},		"Lock", 			self, "lock_cam"	)
	iy = iy + 1
	self:add_button(	{ix, iy },				"billboard",	self, "b_bill", 		false )
	self:add_button(	{ix+4, iy },			"local",		self, "b_bill_local", 	false )
	iy = iy + 1
	self:add_slider(	{ix,iy,		4,1},		"AlpNear", 		self, "a_near",			.1, 0, 4. )
	self:add_slider(	{ix+4,iy,	4,1},		"AlpFar", 		self, "a_far",			8, 0, 16. )
	self:add_slider(	{ix,iy+1,	4,1},		"AlpNearDist", 	self, "a_near_dist",	.1, 0, 4. )
	self:add_slider(	{ix+4,iy+1,	4,1},		"AlpFarDist", 	self, "a_far_dist",		8, 0, 16. )
	iy = iy + 2.2
	self:add_slider_two(	{ix,iy,	8,1},		"z_size", 		self, "z_size_near", "z_size_far", 2., 8., 0, 12. )
	iy = iy + 1
	self:add_slider_two(	{ix,iy,	8,1},		"size", 		self, "z_size_min", "z_size_max", 0., 15., 0, .25 )
	iy = iy + 1
	local DY = .6
	self:add_button(	{ix,iy},				"shadow", 		self, "b_shadow", true	)
	iy = iy + 1
	self:add_slider(	{ix,iy,			8,DY},	"sha_x", 		self, "sha_x",  .05, 0, .5 )
	self:add_slider(	{ix,iy+DY,		8,DY},	"sha_y", 		self, "sha_y",	.05, 0, .5 )
	self:add_slider(	{ix,iy+DY*2,	8,DY},	"sha_z", 		self, "sha_z",	.05, 0, 5. )


	ref.boid = aaa.obj.get_branch_by_class( self:get_layer( 1 ), "bdd_boid" )
	aaa.debug.show( ref.boid )
	ref.nb_alloc = param.get_ref( ref.boid, "nb_allocated")
end

function meu:init()
	local ref = self.ref
	ref.layer1 = self:get_layer(1)
	ref.layer2 = self:get_layer(2)
	ref.layer3 = self:get_layer(3)
	ref.bdd_text = self:get_layer_bdd(3)
	ref.font		= param.get_ref( ref.bdd_text, "font" )
	ref.alignment	= param.get_ref( ref.bdd_text, "alignment" )
	ref.text		= param.get_ref( ref.bdd_text, "text" )

	local pre = {
					{ "",				"Key",		40, 	0	},
					{ "History",		"Hist",		40,		1	},
					{ "Donors",	 		"$$$",		98, 	0	},
					{ "CA",	 			"Vip", 		50,		0	},
					{ "Young Leaders",	 "YoungL",	491,	1	},
					{ "Members",		"Memb", 	176,	0	},
					{ "",				"Sub",		0,		0	}
				}
	self.data = {}
	for i=1,7 do
		local p = pre[i]
		local d = { name=p[1], abv=p[2], nb=p[3], off=p[4] }
		self:print_error( "on lv we hacked this : reestablish" )
		--d.obj = aaa.obj.get(  aaa.dir.get_dir_pref().."/default.0"..(i-1)..".bdd_datagrid" )
		--self.data[i] = d
	end
end

function meu:restart()
	param.set( self.ref.boid, "restart_trig", 1  )
end

local words =
{
	"City of London",		"Investment philosophy",	"Expertise",	"People",	"Performance",	"Asset manager",	"International",
	"Real estate and cash",	"Clients",	"For over 80 years",	"Long-term",	"Conviction-led",	"Stockmarket", 	"Career",
	"Challenges",	"Active management",	"Best returns"
}

--[[
	local d = aaa.obj.get(  aaa.dir.get_dir_pref().."/default.07.bdd_datagrid" )
	if not d then return end
	local bx = 0
	local by = 0
	local nb = 0
	for l=2,509 do
		--aaa.print( l )
		local name = aaa.obj_get_str( d, 2, l )
		local x = aaa.obj_get_real( d, 10, l ) / 100000
		local y = aaa.obj_get_real( d, 11, l ) / 1000000
--]]

local BLACK = 20
local BLACKCOND = 21
local BOLD = 22
local BOLDCOND = 23
local SEMIEXT = 24
local REGULAR = 25
local SEMICN = 26
local SEMICNIT = 27

function meu:update_datagrid()
	self:print_error( "on lv we hacked this : reestablish" )
	if true then return end

	local data_id = clamp( self.data_id, 1, #(self.data) )
	if data_id == self.data_id then
		self:print_error( "problem data_id member is "..self.data_id )
	end
	self.data_cur = self.data[data_id]
end

function meu:draw_text( txt )
	local ref = self.ref
	param.set( ref.text, txt )
	aaa.obj.update_then_draw( ref.layer3 )
end

function meu:draw_texture( bind )
--	gol.push_attrib()

	gol.bind_texture( bind )
	local sx,sy = aaa.img.get_size( bind )
	if sx then
		local S = 8
		gol.color_white()
		local SY = S	local SX = S*sx/sy
		local x,y, sx,sy = -SX*.5, -SY*.5, SX, SY
		gol.set_texture_dim( 2 )
		aaa.draw_rect_uv( x,	y, x+sx		, y+sy			)
		gol.set_texture_dim( 0 )
	else
		self:print( bind )
	end
end
function meu:draw_one( elt, grey )
	self:print_error( "on lv we hacked this : reestablish" )
	if true then return end

	local ref = self.ref
	--local words_nb = #words
	--local word = words[ id % words_nb + 1]
	local data = self.data_cur
	local d = data.obj

	local alpha = elt.a
	local id = elt.id
	if self.b_debug then
		param.set( ref.alignment, "left" )
		gol.color( grey, alpha )
		self:draw_text( id.." "..elt.y )
	elseif self.data_id == 3 or self.data_id == 4 then
		gol.color( grey, alpha )


		--id = 33
		local cat = aaa.obj_get_str( d, 4, id )
		local b_cor = aaa.obj_get_str( d, 3, id ) == "Corporate"

		local word = aaa.obj_get_str( d, 1, id )
		local w2 = aaa.obj_get_str( d, 2, id )
		if w2~= "" then word = word..(self.data_id == 4 and "\n" or " ")..w2 end

		if b_cor then
			cat = "Corporate\n"..cat
		else
			cat = " \n"..cat
		end

		param.set( ref.font, REGULAR )
		param.set( ref.alignment, "left" )
		self:draw_text( word )

		local tx = -.2
		local ty = 1.2
		local s = .5
		gol.translate( tx, ty, 0 )
		gol.scale( s )
			param.set( ref.font, BOLDCOND )
			param.set( ref.alignment, "right" )
			self:draw_text( cat )
		gol.scale( 1/s )
		gol.translate( -tx, -ty, 0 )

	elseif inside( self.data_id, 5, 5) then	--	Yl, members
		gol.color( grey, alpha )
		--fn_debug( word, id )

		--id = 33
		local date
		local word
		if self.data_id==5 then
			date = aaa.obj_get_real( d, 1, id )
			word = aaa.obj_get_str( d, 2, id ).."\n"..aaa.obj_get_str( d, 3, id )
		else
			word = aaa.obj_get_str( d, 1, id ).."\n"..aaa.obj_get_str( d, 2, id )
		end
		--self:print( id.." "..word )

		param.set( ref.font, REGULAR )
		param.set( ref.alignment, "left" )
		self:draw_text( word )

		if date then
			local tx = -.6
			local ty = -0.55
			local s = .8
			gol.scale( s )
			gol.translate( tx, ty, 0 )
				param.set( ref.font, BOLDCOND )
				param.set( ref.alignment, "right" )
				self:draw_text( date )
			gol.translate( -tx, -ty, 0 )
			gol.scale( 1/s )
		end
	elseif inside( self.data_id, 6, 6 ) then	--	Yl, members
		gol.color( grey, alpha )
		--fn_debug( word, id )

		--id = 33
		local date
		local word
		if self.data_id==5 then
			date = aaa.obj_get_real( d, 1, id )
			word = aaa.obj_get_str( d, 2, id ).."\n"..aaa.obj_get_str( d, 3, id )
		else
			word = aaa.obj_get_str( d, 1, id ).."\n"..aaa.obj_get_str( d, 2, id )
		end
		--self:print( id.." "..word )

		param.set( ref.font, REGULAR )
		param.set( ref.alignment, "center" )
		self:draw_text( word )

		if date then
			local tx = -.6
			local ty = -0.55
			local s = .8
			gol.scale( s )
			gol.translate( tx, ty, 0 )
				param.set( ref.font, BOLDCOND )
				param.set( ref.alignment, "right" )
				self:draw_text( date )
			gol.translate( -tx, -ty, 0 )
			gol.scale( 1/s )
		end
	elseif self.data_id==1 then 	--key
		--fn_debug( word, id )

		local word1 = aaa.obj_get_str( d, 1, id )
		local word2 = aaa.obj_get_str( d, 2, id )
		--self:print( "word2 is "..word2.." "..(word2 == "") )
		--self:print( id.." "..word )

		param.set( ref.font, REGULAR )
		param.set( ref.alignment, "center" )
		if word2 == "" then
			gol.color( grey, alpha )
			self:draw_text( word1 )
		else
			local ty = .6
			gol.translate( 0, -ty )
			local ph = math.sin( id + aaa.time.t ) * 2
			if outside( ph, -1, 1 ) then
				gol.color( grey, alpha )
				--self:print( word1.." "..word2 )
				self:draw_text( (ph<1) and word1 or word2 )
			else
				ph = ph - .5 + .5
				gol.color( grey, alpha*(1-ph) )
				self:draw_text( word1 )

				gol.color( grey, alpha*ph )
				self:draw_text( word2 )
			end
			gol.translate( 0, ty )
		end
	elseif self.data_id==2 then
		local date = aaa.obj_get_real( d, 1, id )
		local sub = aaa.obj_get_str( d, 2, id )
		local txt = aaa.obj_get_str( d, 5, id )
		--self:print( "word2 is "..word2.." "..(word2 == "") )
		--self:print( id.." "..word )

		if false then
			local index = aaa.obj_get_real( d, 6, id )
			--self:print( type(index).."("..index..")" )
			if index > 0 then
				self:print( "index "..index )
				local tz = -2
				local tx = -20
				gol.translate( tx, 0, tz )
					self:draw_texture( 320+index-1 )
				gol.translate( -tx, 0, -tz )
			end
		end

		gol.color( grey, alpha )
		param.set( ref.font, BOLD )
		param.set( ref.alignment, "center" )
		local compo
		local dx = 0
		local dy = 2
		if sub == "" then
			compo = date
		else
			compo =  sub.." "..date
		end
		gol.translate( dx, dy )
			--self:print( word1.." "..word2 )
			self:draw_text( compo )
		gol.translate( -dx, -dy )

		param.set( ref.font, REGULAR )
		param.set( ref.alignment, "center" )
		self:draw_text( txt )
	end
end

function meu:update(  )
	self:print_error( "on lv we hacked this : reestablish" )
	if true then return end

	local m	= self:get_meu_by_name( "FafTitle_1" )
	local data = self.data_cur
	if m and data then
		m:set_text( data.name )
	end
end
function meu:draw()
	self:print_error( "for LV lv we hacked this : reestablish" )
	if true then return end

	if not self.__text_obj then
		self.__text_obj = TEXT:create( "BU_TEXT_OBJ" )
		self.__text_obj:set_font_texture( 0 )
	end

	local ref = self.ref

	self:draw_layers_begin()

	--if true then return end
	if self.b_boid then
		aaa.obj.update_then_draw( ref.layer1 )
	end

	self.bdd_point = aaa.bdd.get_cur()
	aaa.obj.update_then_draw( ref.layer2 )

	--local boid = ref.boid


	local function get_cam_angle()
		local xc,yc,zc = aaa.camera.world_to_cam( 0,0,0 )
		local x,y,z = aaa.camera.world_to_cam( 1,0,0 )
		x, z = x-xc, z-zc
		return math.atan2( x, z )
	end

	local gangle = self.b_bill and (.25 - get_cam_angle() / (3.14159*2.)) or 0.
	local angle = gangle

--	aaa.show( gangle, "gangle" )

	local b_bill = self.b_bill and self.b_bill_local

	self:update_datagrid()
	local data = self.data_cur

	local boid = self.bdd_point
	local nb
	if self.b_line then
		nb = data.nb
	else
		nb = aaa.bdd.get_point_nb( boid )
	end

	if not self.elt then
		self.elt = {}
	end
	local elt = self.elt
	local elt_nb = 0
	local skip_begin = self.skip_begin
	local skip_end = 4
	local delta = self.line_interval
	local size_line = delta * (nb-1) + skip_begin+skip_end-1
	local id,x,y,z
	local xc,yc,zc
	local axe = self.line_axe

	local ph = self.ui.bu_phase:get_value()
	if self.b_line_run then
		local ph_speed = self.line_speed / size_line
		if self.b_speed_inverse then
			ph_speed = -ph_speed
		end
		ph = ph + aaa.time.dt * ph_speed
		ph = math.fmod( ph, 1. )
	end
	self.ui.bu_phase:set_value( ph )
	local d_alpha = { self.a_near, self.a_near+self.a_near_dist, self.a_far-self.a_far_dist, self.a_far }
	local	get_point = aaa.bdd.get_point_and_id_local
	for i=1,nb do
		if self.b_line then
			id = i
			local o = 0
			local v = - skip_begin - delta*(i-1) + ph * size_line
			if axe == 1 then 		x,y,z = -v,o,0
			elseif axe == 2 then	x,y,z = 0,v,o
			else 					x,y,z = o,0,v
			end
		else
			x,y,z,id = get_point( boid, i )
		end
		id = clamp( (id -1) % data.nb + data.off + 1, 1, 512 )
		xc,yc,zc = aaa.camera.world_to_cam( x,y,z )

		local dist = math.sqrt( xc*xc + yc*yc + zc*zc )
		local s = math.step_linear( dist, self.z_size_far, self.z_size_near ) * (self.z_size_max-self.z_size_min) + self.z_size_min
		if s > 0 then
			local a =  math.step_interval_linear_table( dist, d_alpha )
			if a > 0 then
				-- ok we have calid size and visibility, now we can add it
				if elt_nb <512 then
					elt_nb = elt_nb +1
				end
				local e = { x=x, y=y, z=z, zc=zc, s=s, a=a }
				if b_bill then
					e.angle = gangle + .5 + math.atan2( xc, zc ) / (3.14159*2.)
				else
					e.angle = gangle
				end
				e.id = id
				elt[elt_nb] = e
			end
		end
	end

	table.sort( elt, function (a,b) return a.zc < b.zc end )

	local sdx, sdy, sdz = self.sha_x, self.sha_y, self.sha_z
	gol.set_texture_dim( 0 )
	local ox = 0.
	local oy = 0
	for i=1,elt_nb do
		local e = elt[i]
		gol.push_matrix()
			gol.translate( e.x, e.y, e.z )
			local angle = e.angle - e.y*self.adn_freq
			gol.rotate_y( angle )
			gol.scale( e.s )
			gol.translate( ox, oy )

			if self.b_shadow then
				gol.translate( sdx, -sdy, -sdz )
				self:draw_one( e, 0 )
				gol.translate( -sdx, sdy, sdz )
			end
			self:draw_one( e, 1 )
		gol.pop_matrix()
	end

	self:draw_layers_end()
end

function meu:get_preset_nb()	return 16	end
