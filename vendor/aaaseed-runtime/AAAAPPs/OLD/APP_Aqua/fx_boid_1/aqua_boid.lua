if not FX.cur then
	return
end
local fx = FX.cur

local function load_from_dir( dir, prefix, ext )
	MEDIA.set_dir( dir )
	local i = 0
	local bind_first = 0
	repeat
		i = i + 1
		local bind = IMGS.get_bind( prefix..i.."."..ext )
		if i == 1 then
			bind_first = bind
		end
	until not bind
	i = i - 1
	aaa.print( "load_from_dir( "..dir.." ) : loaded "..prefix.." "..i.." images" )
	return bind_first, i
end
function fx:load()
	self.fish_bind,	 self.fish_nb_tex	= load_from_dir( app.media_dir_rel.."Aqua/fish/", "fish", "tga" )
	self.zebre_bind, self.zebre_nb_tex	= load_from_dir( app.media_dir_rel.."Aqua/fish/Zebre/", "Zebre_", "tga" )
end

function fx:init_fish_color()
	local	ref = self.ref
	local	nb = param.get( ref.nb_alloc )
	local	b = self.rnd_base
	local	r = self.rnd_range
	local	ab = self.alpha_base
	local	ar = self.alpha_range

	--aaa.bdd.set_lua_cur( ref.boid )
	--local get_point = aaa.bdd.get_point_and_id
	math.randomseed( 42 )
	for i=1,nb do
		--local x,y,z,id = get_point( i )
		local elt = self.elt[i]
		elt.r = math.random()*r + b
		elt.g = math.random()*r + b
		elt.b = math.random()*r + b
		elt.a = math.random()*ar + ab
	end
end
function fx:init_fish()
	local ref = self.ref
	self.elt = {}

	local nb = param.get( ref.nb_alloc )
	aaa.bdd.set_lua_cur( ref.boid )
	local get_point = aaa.bdd.get_point
	for i=1,nb do
		local x,y,z = get_point( i )
		self.elt[i] = { x=x, y=y, z=z, s=math.random() }
	end
	self:init_fish_color()
end

function fx:init()
	fx:load()
	local ref = self.ref
	ref.boid	=	aaa.obj.get( "aqua_boid_1" )
		ref.nb_alloc		=	param.get_ref( ref.boid,	"nb_allocated"		)
		ref.nb_current		=	param.get_ref( ref.boid,	"nb_current"		)
		ref.target_x		=	param.get_ref( ref.boid,	"target_x"			)
		ref.target_y		=	param.get_ref( ref.boid,	"target_y"			)
		ref.target_z		=	param.get_ref( ref.boid,	"target_z"			)
		ref.target_active	=	param.get_ref( ref.boid,	"target_active"		)
		ref.box_size		=	param.get_ref( ref.boid,	"box_size_factor"	)

	--ref.layer_3d =	aaa.obj.get( "aqua_fish" )
	self.rnd_base		= 1.
	self.rnd_range		= 1.
	self.alpha_base		= .3
	self.alpha_range	= .7
	self.fish_size_min	= .1
	self.fish_size_max	= 1.
	self.b_rot1			= true
	self.fish_alpha		= 1.
	self:init_fish()
end

function fx:set_target( b, x, y, z )
	local ref = self.ref
	param.set( ref.target_active,	b	)
	param.set( ref.target_x,		x	)
	param.set( ref.target_y,		y	)
	param.set( ref.target_z,		z	)
end

function fx:set_living_box_size( f )
	local ref = self.ref
	param.set( ref.box_size,	f	)
end

function fx:draw()
	FX.draw( self )

	local ref = self.ref

	local nb = param.get( ref.nb_current )
	if nb <=0 then return end

	local bank = self.sel_bank:get_item_data()
	local bind = self.sel_fish:get_value()
	--aaa.obj.update_then_draw( ref.layer_a )

	local pi2 =  math.pi2
	local over_pi2 = 1. / pi2
	local ref = self.ref

--	gol.push_attrib()
		gol.set_texture_dim( 2 )
		aaa.bdd.set_lua_cur( ref.boid )
		local get_point = aaa.bdd.get_point_and_id_local
		local get_img_size = aaa.img.get_size
		for i=1,nb 	do
			local x,y,z = get_point( i )

			--self:print( i  )
			local elt = self.elt[i]
			local r,g,b,alpha = elt.r, elt.g, elt.b, elt.a
			--self:print( alpha )
			if elt then
				local tex_bind
				if bank == "fx" then
					tex_bind = self.fish_bind + bind
				elseif bank == "app" then
					tex_bind = IMGS.get_bind_free() + bind
				elseif bank == "bind" then
					tex_bind = bind
				elseif bank == "code" then
					math.randomseed( i*12345 )
					if bind == 0 then
						tex_bind = 62
					elseif bind == 1 then
						if math.random() <.1 then
							tex_bind = 63
						else
							tex_bind = 62
						end
					elseif bind == 2 then
						tex_bind = 62
						if math.random() <.5 then
							r,g,b = 0,0,0
						end
					else
						tex_bind = self.zebre_bind + math.fmod( i + aaa.time.t * self.zebre_nb_tex * 24/6, self.zebre_nb_tex-1 )
					end
				end
				if tex_bind then
					gol.bind_texture( tex_bind )
					local sx, sy = get_img_size( tex_bind )
					if sx then
						local S = .05
						local min_x = 0
						local max_x = S*sx/sy*2.
						local min_y = -S
						local max_y = S
						local lx = elt.x
						local ly = elt.y
						local lz = elt.z
						elt.x = x
						elt.y = y
						elt.z = z
						gol.push_matrix()
							--gol.set_quad_uv( u_min, v_min, u_min+u_size, v_min+v_size )
							gol.translate( x, y, z )
							local dx = x-lx
							local dy = y-ly
							local dz = z-lz
							if self.b_rot1 then
								local a = math.atan2( dx, dz )
								local w = math.sin( a )
								if w < 0 then
									--	back face
 									w = -w-.1
								else
									--	front face
									w = w-.1
								end
								--w = w * w
								--w = w * w
								w = w * 1.5 - .5
								w = math.max( 0., w )
								gol.color( r,g,b, alpha * self.fish_alpha * w )
								--if w < 0 then gol.color_white( 0 ) end

								--gol.set_texture_dim( 0 )
								--aaa.draw_line( 0,0,0, -dx*10., -dy*10., -dz*10. )
								--gol.set_texture_dim( 2 )

								a = a * over_pi2 -.25
								gol.rotate_y( a )
								--gol.rotate_x( .25 )
								local a = math.atan2( dy, dx * math.cos( a * pi2 ) ) * over_pi2
								gol.rotate_z( a )
							else
								gol.color( r,g,b, alpha * self.fish_alpha )
								local a = math.atan2( dy, dx ) * over_pi2
								gol.rotate_z( a )
							end

							gol.scale( elt.s * ( self.fish_size_max-self.fish_size_min) + self.fish_size_min )
							aaa.draw_rect( min_x, min_y, max_x, max_y )
							--aaa.obj.update_then_draw( ref.layer_3d )
						gol.pop_matrix()
					end
				end
			end
		end
		gol.set_texture_dim( 0 )
	--gol.pop_attrib()
	--gol.pop_attrib()
end

function fx:add_ui_window()
	FX.add_ui_window( self, false )

	local ref = self.ref
	local bu

	bus_cur:move_next()

	bu = bus_cur:add_selector( "bank" )
		bu:set_nb_min_0( 5, 1 )
		bu:set_item_text(	1,	"fx", "App", "Bind", "TEX", "Code" )
		bu:set_item_data(	2,	      "app", "bind", "tex", "code" )
		self.sel_bank = bu

	bu = bus_cur:add_selector( "fish",	{nil,nil, 1,4} )
		bu:set_nb_min_0( 8, 4 )
		self.sel_fish = bu

	--bus_cur:move_next()
	bu = app:add_slider( "rnd_base" )
		bu:set_target_lua( self, "rnd_base" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( fx, "init_fish_color" )

	bu = app:add_slider( "rnd_range" )
		bu:set_target_lua( self, "rnd_range" )
		bu:set_min_max( 0, 2 )
		bu:set_method_on_value_change( fx, "init_fish_color" )

	bu = app:add_slider( "alpha_base" )
		bu:set_target_lua( self, "alpha_base" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( fx, "init_fish_color" )

	bu = app:add_slider( "alpha_range" )
		bu:set_target_lua( self, "alpha_range" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( fx, "init_fish_color" )

--	bus_cur:add_but_trig_fn( "Reset Fish", nil,	fx.init_fish_color, self )

	---[[
	bu = app:add_slider( "size_min" )
		bu:set_target_lua( self, "fish_size_min" )
		bu:set_min_max( 0, 8 )

	bu = app:add_slider( "size_max" )
		bu:set_target_lua( self, "fish_size_max" )
		bu:set_min_max( 0, 8 )

	bu = app:add_slider( "Time" )
		--todo set_target_param  should accept the non ref form (NO PARAM.GET_REF() )
		bu:set_target_obj_param( ref.boid, "time_factor" )
		bu:set_min_max( 0, 2 )

	--]]
	bu = bus_cur:add_button( "rot1" )
		bu:set_value( 1 )
		bu:set_target_lua( self, "b_rot1" )

	bu = app:add_slider( "Alpha" )
		bu:set_target_lua( self, "fish_alpha" )


--[[	bus_cur:move_next()
	bu = app:add_slider( "target_x" )
		bu:set_target_obj_param( ref.boid, "target_x" )
		bu:set_min_max( -8, 8 )
		self.bu_x = bu

	bu = app:add_slider( "target_y" )
		bu:set_target_obj_param( ref.boid, "target_y" )
		bu:set_min_max( -8, 8 )
		self.bu_y = bu

	bu = app:add_slider( "target_z" )
		bu:set_target_obj_param( ref.boid, "target_z" )
		bu:set_min_max( -8, 8 )
		self.bu_z = bu

	bu = bus_cur:add_button( "target_active" )
		bu:set_target_obj_param( ref.boid, "target_active" )
		self.bu_target_active = bu
--]]
end