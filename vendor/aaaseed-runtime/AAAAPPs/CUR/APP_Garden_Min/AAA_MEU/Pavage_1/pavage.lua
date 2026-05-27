--[[
Primary
alpha = slider 8
zoom/tra_z = slider 1
roll = rot 8
tra_x = rot 1
tra_y = rot 2

Secondary
espace plan = slider 3
nb plan = rot 3

deformation volume = slider 4
deformation freq = slider 5
deformation phase = slider 6

sym_u = rot 4
sym_v = rot 5

rot_x = rot 6
rot_y = rot 7
--]]

--https://www.shadertoy.com/view/4dS3Dc
--https://www.shadertoy.com/view/llSyDh
--https://www.shadertoy.com/view/Xd2GR3
--https://www.shadertoy.com/view/Xljczw
--http://www.f-lohmueller.de/pov_tut/tex/tex_580e.htm
--https://en.wikipedia.org/wiki/List_of_convex_uniform_tilings

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1, 1

	self:add_camera()

	self:add_bu_texture_target_unit()

	iy = 5
	bu = self:add_slider(	{ix+8,iy,	8,1}, "Nb", self, "sym_frac" )
	bu = self:add_selector(	{ix,iy,	8,4}, "Sym" )
		bu:set_nb_min_0( 8, 4 )
		bu:set_item_text_from_nb_minus_1( 1 )
		bu:set_target_lua( self, "sym" )

	iy = iy + 4
	bu = self:add_slider(	{ix,iy,	8,1}, "Angle", self, "angle", 0, -1, 1 )
	bu:add_values_def( -.5, -2/6, -1/6, 0, 1/6, 2/6, .5, .75 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1}, "offset", self, "offset", 1, 0, 2 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1}, "distance", self, "dist", 1, 0, 2 )



	local sha = self:get_shading()
	bu = self:add_button(		{9,2, 	4,1}, "Grid", sha:get_ref_frag_int(2) )
	--todo generalize shader access
	bu = self:add_trig(		{9,3,	4,1 },	"Edit" )
		bu:set_method_on_click( sha, "edit_shader_frag" )

	bu = self:add_slider_multi( {9,4, 	8,6}	,"UV",	1 )
	bu:set_mobile( false )
	bu:set_ui_top( false )
		
		--bu:set_elt_sxy( s, s )
	--	for i=1,bu:get_elt_nb() do
	--		local elt = bu:get_elt( i )
				--elt:set_text( i )
				--elt:set_text_xyf( -.5, .5, 1. )
				--elt:set_text_rect_ratio(1)
				--elt:set_back_bind( 102 )
	--	end
		bu:set_back_bind( 102 )
	self.ui.bu_uv = bu

end

function meu:init()
	self:add_shading()
end

function meu:update()
	local ref = self.ref
	local sha = self:get_shading()
	--self:print( sx.." / "..sy)
	--table.print( elt, "", 3 )
	param.set( sha:get_ref_frag_int(1), self.sym )

	local angle = self.angle * 3.14159 * 2
	for i = 1,3 do
		param.set( sha:get_ref_frag_vec4_x(i), math.sin(angle*(i-1) ) )
		param.set( sha:get_ref_frag_vec4_y(i), math.cos(angle*(i-1) ) )
		param.set( sha:get_ref_frag_vec4_w(i), self.offset )
		param.set( sha:get_ref_frag_vec4_w(i), 1./self.dist )
	end

	local bu_uv = self.ui.bu_uv
	local x,y  = bu_uv:get_elt_xy(1)
	local sx,sy  = bu_uv:get_elt_sxy(1)
	param.set( sha:get_ref_frag_vec4_x(4), sx )
	param.set( sha:get_ref_frag_vec4_y(4), sy )
	param.set( sha:get_ref_frag_vec4_z(4), x - sx*.5)
	param.set( sha:get_ref_frag_vec4_w(4), y - sy*.5)
end

