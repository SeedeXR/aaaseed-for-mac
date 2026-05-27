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

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1, 1

	self:add_camera()
	self:add_bu_texture_target_unit()

	local sha = self:get_shading()

	iy = 5
	bu = self:add_slider(	{ix+8,iy,	8,1},	"Nb", self, "sym_frac" )
	bu = self:add_selector(	{ix,iy,		8,4},	"Sym" )
		bu:set_nb_min_0( 8, 4 )
		bu:set_item_text_from_nb_minus_1( 1 )
		bu:set_target_lua( self, "sym" )

	iy = iy + 4
	bu = self:add_slider(	{ix,iy,		8,1},	"Angle", sha:get_ref_frag_float(2), nil,	0, -1,1	)
		bu:add_values_def( -.5, -2/6, -1/6, 1/6, 2/6, .5 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,		8,1},	"fade",	sha:get_ref_frag_float(3), nil,		1, 0,2	)

	bu = self:add_slider_multi( {9,7,	8,6},	"UV",	1 )
		bu:set_mobile( false )
		bu:set_ui_top( false )

		--bu:set_elt_sxy( s, s )
		for i=1,bu:get_elt_nb() do
			local elt = bu:get_elt( i )
				--elt:set_text( i )
				--elt:set_text_xyf( -.5, .5, 1. )
				--elt:set_text_rect_ratio(1)
				elt:set_back_bind( 71 )
		end
	self.ui.bu_uv = bu
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local ref = self.ref
	local bu_uv = self.ui.bu_uv
	local x,y  = bu_uv:get_elt_xy(1)
	local sx,sy  = bu_uv:get_elt_sxy(1)
	--self:print( sx.." / "..sy)
	--table.print( elt, "", 3 )
	local sha = self:get_shading()

	param.set( sha:get_ref_frag_float(1), self.sym + self.sym_frac )
	param.set( sha:get_ref_frag_vec4_x(1), x - sx * .5 )
	param.set( sha:get_ref_frag_vec4_y(1), y - sy * .5 )
	param.set( sha:get_ref_frag_vec4_z(1), 1/sx )
	param.set( sha:get_ref_frag_vec4_w(1), 1/sy )
end
