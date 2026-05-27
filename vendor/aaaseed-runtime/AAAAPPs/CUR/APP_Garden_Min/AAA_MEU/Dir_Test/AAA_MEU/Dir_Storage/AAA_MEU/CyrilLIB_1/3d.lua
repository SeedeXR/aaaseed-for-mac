local function v (x, y, z)
	local _v = {x, y, z}
	return _v
end

local function quad (v0, v1, v2, v3, color)
	local q = {v0, v1, v2, v3}
	q.color = color
	return q
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbfa(	{9,2} )

	self:add_camera()
	self:add_mapping_by_side()
	self:add_bu_texture_target_unit()
	self:add_rendering()

	bu = self:add_slider(	{1,9,	8,1},	"Scale" , self, "scale", 1, 0, 1 )
		bu:set_meter( false )
	local ix = 9
	local iy = 5
	bu = self:add_slider(	{ix,iy,	8,1},	"step" , self, "step", 1, 1, 256 )
		bu:set_value_type_integer( true )
	bu = self:add_slider(	{ix,iy+1,	8,1},	"size_u" , self, "size_u", 1, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1},	"size_v" , self, "size_v", 1, 0, 1 )
	bu = self:add_slider(	{ix,iy+3,	8,1},	"size_axe" , self, "size_axe", 1, 0, 8 )
	bu = self:add_slider(	{ix,iy+4,	8,1},	"freq" , self, "freq", 1, 0, 1 )
	bu = self:add_slider(	{ix,iy+5,	8,1},	"bias" , self, "bias", 1, 0, 1 )
	bu = self:add_slider(	{ix,iy+6,	8,1},	"spiral" , self, "spiral", 1, 0, 16 )
		bu:set_value_type_integer( true )
	bu = self:add_slider(	{ix,iy+7,	8,1},	"seed" , self, "seed", 1, 0, 256 )
		bu:set_value_type_integer( true )
end

function meu:init()
	self.t = 0
end

function meu:update()
end

function meu:clear ()
	self.quads={}
end

function meu:add_quad (q)
	table.insert( self.quads, q )
end

function meu:final_draw ()
	gol.push_matrix()
	gol.scale (self.scale)			-- scale x, y and z
	gol.begin_quads()
		for k,q in ipairs(self.quads) do
			gol.color( unpack (q.color or {1,0,1}) )
			for i=1,4 do
				gol.vertex( unpack (q[i]) )
			end
		end
	gol.do_end ()
	gol.pop_matrix()
end

-- Split a quad by an approximate size
local function split_quad_by_size( face, size )
	local length = (face[3] - face[edge2]):length ()
	local count = math.max (1, math.floor (0.5+length/size))
	local d0 = (face[4] - face[1]) / count
	local d1 = (face[3] - face[2]) / count
	local quads = {}
	for i=1,count do
		insert (quads,
			v (	face[1] + d0*i,
				face[2] + d1*i,
				face[3] + d1*(i+1),
				face[4] + d0*(i+1))
		)
	end
end

function meu:my_draw()

	self:clear ()
	local q = quad (v(0, 0, 0), v(20, 0, 0), v(20, 20, 0), v(0, 20, 0), v(1,0,0))
	--self:add_building (q)
	self:final_draw ()
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layers()
		self:my_draw()
	self:draw_layers_end()
end
