function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1,1
	local DY = .2
	local SY = 1.
	self:add_shading_ui( {ix,iy,	8} )
	iy = iy + SY

	iy = iy + SY * .5
	local iys = iy
	self.b_on = {}
	self.size = {}
	local function add_one( name, id, layer_id )
		local SY = SY * .8
		self:add_button(	{ix,iy,	SY,SY},			name,			self.b_on, layer_id,	true )
		iy = iy + SY
		self:add_bu_texture_target_layer(	{ix,iy, nil,SY*2.5},	"TEX_"..name, id, false,	self:get_layer(layer_id)	)
		iy = iy + SY*2.5
		self:add_slider(	{ix,iy,	4,SY},			"Size "..name,	self.size, layer_id,	1,	0,4 )
		iy = iy + SY + DY
	end
	add_one( "Grid",		1, 2 )
	add_one( "Sphere",		2, 3 )
	add_one( "Torus",		3, 4 )

	ix,iy = 9,iys
	add_one( "Sphere 2" ,	4, 5 )
	add_one( "Teapot",		5, 6 )

	self:add_rendering()
end

function meu:init()
	self:add_shading()
end

function meu:draw()
	local sha = self:get_shading()
	self:draw_layers_begin()
		local tb = self.b_on
		for i=2,6 do
			if tb[i] then
				local s = self.size[i]
				sha:set_frag_vec4( 1, nil, nil, s, s )
				self:draw_layer( 1 )
				self:draw_layer( i )
			end
		end
 	self:draw_layers_end()
end