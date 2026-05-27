function meu:define_ui()
	self:add_camera()

	local ix,iy = 1,1
	--lf:add_rgbfa(9,2)
	self:add_button( {ix,iy },		"Back",			self, "b_back",			true )
	self:add_button( {ix,iy+1 },	"Circle",		self, "b_circle",		true )
	self:add_button( {ix,iy+2 },	"Model",		self, "b_model",		true )
	self:add_button( {ix,iy+3 },	"Body",			self, "b_body",			true )
	self:add_button( {ix+1,iy+4 },	"Flip",			self, "b_body_flip",	true )
	self:add_button( {ix,iy+5 },	"Score",		self, "b_score",		true )
	self:add_button( {ix,iy+7 },	"Photo",		self, "b_photo",		true )
	self:add_button( {ix,iy+8 },	"Video",		self, "b_video",		true )
	self:add_button( {ix,iy+9 },	"Conformity",	self, "b_conformity",		true )
end

function meu:init( )
	local ref = self.ref
	ref.circle_layer = self:get_layer(2)
		ref.circle_color	= aaa.layer.create_color( ref.circle_layer, "circle" )
	ref.arc_layer = self:get_layer(3)
		ref.arc_color		= aaa.layer.create_color( ref.arc_layer,  "arc" )
		ref.arc_bdd			= aaa.layer.get_bdd( ref.arc_layer )
		ref.arc_center		= param.get_ref( ref.arc_bdd, "u_center" )
		ref.arc_range		= param.get_ref( ref.arc_bdd, "u_range" )
end
--[[
function meu:draw_grid()
	local m = self.meu_grid
	if not m then
		m = self:get_meu_by_name( "Grid_1" )
		self.meu_grid = m
	end
	m:render( 1 )
end
--]]
-- function meu:set_alpha( alpha )
-- 	--self:print( "alpha "..alpha )
-- 	self.__alpha_meu = alpha
-- --	local col = self.ref.color
-- --	if col then
-- --		col:set_alpha(alpha)
-- --	end
-- end

function meu:draw_img_xy( img, alpha, x, y, sy )
	gol.set_texture_dim(2)
	gol.color_white( self.loc_alpha * ( alpha or 1 ) )
	if y and sy then
		--gol.color_white( .5 )
		img	:draw_xy_sy( x, y, sy )
	else
		img	:draw_sy( 8 )
	end
	gol.set_texture_dim(0)
end

function meu:draw_img( img, alpha, y, sy )
	self:draw_img_xy( img, alpha, 0, y, sy )
end
function meu:draw_photo( img, alpha )
	self:draw_img( img, alpha, -.78, 1238/1920*8. )
end

function meu:draw_circle( id, player )
	local img
	local ph = player.ph_result
	if ph < 1 then
		local str = player.b_made and "bravo" or "dom"
		img = app:get_img_id( str.."_circle_", id )
	else
		img = app:get_img_level_id( "photo_circle", id )
	end
	local rot = .25 - clamp( math.abs( ph - 1 ) * 4, 0, .25 )
	gol.rotate_y( rot )
	self:draw_img( img, 1, 2.82, 322/1920*8. )
	gol.rotate_y( -rot )
end

local colors =
{
	{	back	=	{		.729,	.898,	.894,	1. },
		ellip	=	{		.952,	.831,	.792,	1. }
	},
	{	back	=	{		1,		.776,	.568,	1. },
		ellip	=	{		.663,	.750,	.785,	1. }
	},

}

function meu:draw_rect( x, y, sx, sy )
	x = x or 0
	y = y or 0
	sx = sx or 4.5
	sy = sy or 8
	sx = sx * .5
	sy = sy * .5
	aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
end
function meu:prepare_circle()
	gol.set_blend_add()
	gol.bind_texture( 63 )
	gol.set_texture_dim( 2 )
	gol.texcoord_set_unit_0()
	gol.set_quad_uv()
end
local grey_back = 1
function meu:color_ui( b, alpha )
	if b then
		gol.color_white( .6*alpha )
	else
 		gol.color( grey_back, grey_back, grey_back, .2*alpha )
	end
end
function meu:draw_conformity( id, con, alpha )
	local id_f = (id*2-3)
	gol.set_texture_dim( 0 )
	local x = -id_f * 2
	local y = -3.25
	local sx = id_f * .4
	local sy = 2.5
	self:color_ui( false, alpha )
	aaa.draw_rect( x, y, x+sx, y+sy )
	self:color_ui( true, alpha )
	aaa.draw_rect( x, y, x+sx, y+sy*con )
end
function meu:draw_score( alpha )
	self:prepare_circle()
	local y = 3.8
	local dx = .25

	local level = self.player.level
	local made = self.player.made
	local level_cur = app:get_level()
	for i=1,7 do
		local x = (i-4)*dx
		--local s = made[i] and .2 or .1
		local s = .1
		local col = {1,1,1,1}
		if i<=level_cur then
			if made[i] then
				col = {1,1,0,1}
			else
				col = {1,0,0,1}
			end
		end
		gol.color( col )
		self:draw_rect( x,y, s,s )
	end
end
function meu:draw_back( id )
	gol.set_texture_dim(0)
	gol.color( colors[id].back )
	--self:draw_rect( 0, 0, 4.5, 8 )
	--local str = "back_ref_"..(id==1 and "l" or "r")
	--self:print( str )
	--ga:get_imgs()[str]	:draw_sy( 8 )
	self:draw_rect()
end
function meu:draw_circle_floor( id )
	gol.set_texture_dim(2)
	gol.color( colors[id].ellip )
	gol.bind_texture( 63 )
	self:draw_rect( 0, -2.84, 2.6, 1.15 )
	gol.set_texture_dim(0)
end

function meu:draw_circle_back_back( val, alpha )
	local ref = self.ref
	ref.circle_color:set_alpha( alpha * (self.id==1 and .3 or .2) )
	aaa.obj.update_then_draw( ref.circle_layer )
	ref.arc_color:set_alpha( alpha * (self.id==1 and .5 or .45) )
	param.set( ref.arc_center, .25 - val *.5 )
	param.set( ref.arc_range, val	 )
	aaa.obj.update_then_draw( ref.arc_layer )
end
function meu:draw_circle_back( val, alpha )
	local ref = self.ref
	meu:draw_circle_back_back( val, alpha )
	ref.arc_color:set_alpha( alpha )
	local S = .008
	param.set( ref.arc_center, .25 - val + S*.5 )
	param.set( ref.arc_range, S )
	aaa.obj.update_then_draw( ref.arc_layer )

end

function meu:draw_txt( txt )
	--self:print( txt )
	ga:get_imgs()["txt_"..txt]:draw_xy_sx( 0, 4*(1-400/1920)-.150, 4.5 )
end
function meu:draw_txt_full( txt )
	gol.set_texture_dim(2)
	gol.color_white()

			gol.set_blend_min()

	self:draw_txt( txt )
	gol.set_texture_dim(0)
end
function meu:draw_perso( id, perso )
	gol.set_default()
	gol.color_white(1)
	gol.set_texture_dim(2)
	if perso then
		local imgs = ga:get_imgs()
		local id = id==1 and "l" or "r"
		local str = perso..id
		--self:print( str )
		imgs[str]:draw_sy( 8 )
	end
end
function meu:draw_perso_txt( id, perso, txt )
	self:draw_perso( id, perso )
	self:draw_txt_full( txt )
end
function meu:draw_wait( seq, id )
	self:draw_circle_floor(id)
	local _, i_perso = math.modf( seq:get_time() * .08 )
	_, i_perso = math.modf( i_perso + (id-1)*.5 )
 	i_perso = clamp( math.floor( i_perso * 8 ), 0, 3) + 1
 	self:draw_perso( id, "perso_intro_"..i_perso )
 	local _, i_text = math.modf( seq:get_time() * .04 )
 	i_text = clamp( math.floor( i_text * 4. ), 0, 3) + 1
 	self:draw_txt_full( "intro_"..i_text )
end
function meu:draw_welcome( seq, id )
	self:draw_circle_back_back( 0, 1 )
	self:draw_circle_floor( id )
	self:draw_perso_txt( id, "game_1", "wel" ) --(seq:get_phase() < .5) and "wel" or "mod" )
end
function meu:draw_end( seq, id )
	--self:draw_perso( id, "perso_welcome_", "mer" )
	meu:draw_circle_floor( id )
	self:draw_perso_txt( id, nil, "mer" )
end
function meu:draw_video( id )
	gol.color_white()
	--gol.set_default()
	gol.set_texture_dim(2)
	--todo tex_named
	gol.bind_texture( 97+id )
	self:draw_rect( nil, nil, 4.52 ) -- was 4.5 but we had a color leak on the side
end
function meu:draw_one( id )
	local dz = .5
	local imgs = app:get_imgs()
	local seq = ga:get_seq_cur()
	local sn = seq:get_title()
	--self:print( sn )
	local sid = seq:get_seq_id()
	--local game, photo, photo_ref = app:get_img_game( id )
	local game, contour, photo = app:get_img_game( id )

	local player =  app:get_player( id )
	--self:print( sid.." "..sn)

	self.id		= id
	self.level	= app:get_level()
	self.player	= player
	--self.player:print( "player "..id.." -> "..player:get_id().." "..player.state )
	self.ease	= seq:get_ease_in()
		--MEU.draw( self )
		--gol.rotate_y( aaa.time.t * .1 )
	aaa.obj.update_then_draw( self:get_layer(1) )
--	gol.color_whitescire
	gol.texcoord_set_unit_0()
	gol.set_quad_uv()

	if self.b_back then self:draw_back( id ) end
	gol.translate( 0, 0, dz )

	if sn == "wait" then
		self:draw_wait( seq, id )
		return
	elseif sn == "welcome" then
		self:draw_welcome( seq, id )
	end
------------------
	if sid==3 or self.b_circle then
		local ph = seq.game_ph
		local a
		--self:print( "ph "..ph )
		if false then
			if ph > 1 then
				a = clamp_01( 1-((ph-1) * 15) )
			else
				a = self.loc_alpha*self.ease
			end
		else
			a = 1
		end
		--self:print( "ph "..ph.." a "..a )
		--self.player:print( "player "..id.." -> "..player:get_id().." "..player.state )

		if player.state=="playing" then
			--if a > 0 then
				self:draw_circle_back( clamp_01( self.player.conformity_draw ), a )--.5+.5*math.sin( aaa.time.t * 5 ) )
				gol.set_blend_add_color_one()

				--gol.set_blend_add_alpha_one()
				--		gol.set_blend_add()

				local i = clamp( math.floor( 16.- seq:get_time() ) ,0 ,50 )
				self:draw_img_xy( app:get_imgs()["count_"..i], 1, 1.75, 2.5, .6 )
				gol.set_blend_add()
			--end
			self:draw_circle_floor( id )
		else
			--self:draw_img( photo_ref )
			local ph = player.ph_result
			self:draw_img( game, 1. )
			self:draw_photo( photo, ph*2. -1. )
			--gol.set_blend_add()
			self:draw_img( contour, 2. - ph )
			--gol.set_blend_add()
			self:draw_circle( id, player )
			if ga:get_seqs():is_run() then
				player.ph_result = player.ph_result + aaa.time.dt * .5
			end
		end
	end
	gol.translate( 0, 0, dz )
------------------
	aaa.obj.update_then_draw( self:get_layer(4) )

	if sid==3 or sid==5 or self.b_model then
		if sid == 3 then
			if player.state=="playing" then
				self:draw_txt_full( "placez" )
				self:draw_img( game, 1 )
			end
		else
			self:draw_end( seq, id )
		end
	end
	if sn=="game" and self.b_photo then
		self:draw_img( photo, 1 )
	end
	gol.translate( 0, 0, dz )
------------------
	if sid==3 or sid==5 then
		if sid==3 and self.b_conformity then
			self:draw_conformity( id, self.player.conformity_draw, 1*clamp_01(self.ease*2) ) --  math.sin( aaa.time.t * 5. )*.5+.5 )
		end
		if self.b_score then
			self:draw_score( id )
		end
	end
	gol.translate( 0, 0, dz )
------------------
	if sn=="video" or self.b_video then
		self:draw_video( id )
	end
	gol.translate( 0, 0, dz )
------------------
	if inside( sid, 2, 5 ) or self.b_body then
		local m = self:get_meu_by_name( "TdSkel_"..1 )
		m:set_alpha( self.loc_alpha )
		local a
		if sid==2 or (sid==3 and player.state=="playing") then
			--self:print( sid )
			a = player.alpha + aaa.time.dt
		else
			--self:print( sid )
			a = player.alpha - aaa.time.dt
		end
		a = clamp( a, .2, 1 )
		player.alpha = a
		m:draw_body( id, false, a )
	end
	gol.translate( 0, 0, dz )
--]]
end
function meu:update()
--[[
	local seq = ga:get_seq_cur()
	if not seq then return end
	self.seq = seq
	local id = seq:get_seq_id()

	local ease = seq:get_ease_in()
--]]
	--local id = (self:get_inst_key() == "1") and 1 or 2
	local id = 1
	local gp = self.gp
	if not gp then
		gp = self:get_gp()
		self.gp = gp
	end
	--todo check if this not a compflic with the rest
	local render_index = gp:get_render_index()
	if self:get_render_index() ~= render_index then
		id = 1
		self:set_render_index( render_index )
	else
		id = 2
	end
	app:set_player( self.b_body_flip and 3-id or id )
end

function meu:draw()
--[[
	local seq = ga:get_seq_cur()
	if not seq then return end
	self.seq = seq
	local id = seq:get_seq_id()

	local ease = seq:get_ease_in()
--]]
	--local id = (self:get_inst_key() == "1") and 1 or 2
	local id = app:get_player_id()
	--self:print( player.." id "..id )

	self.loc_alpha = 1
	self:draw_layers_begin()
		self:draw_one( id )
	self:draw_layers_end()
end
