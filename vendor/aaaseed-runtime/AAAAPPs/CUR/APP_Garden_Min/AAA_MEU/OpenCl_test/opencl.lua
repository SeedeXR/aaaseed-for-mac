function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d","3d", "Draw", "texture", "Unfinished" },
				help = "Effect based on Particle and OpenCl: restore or dump",
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{nil,nil,	nil,nil}, "Main_" )

	self:add_camera()

	self:add_bu_texture( {1,3} )

	local names = { "LevLow", "LevHigh", "VisLow", "Vis", "VisHigh", "GradInf", "Speed", "Displace", "Grad Dist X", "Grad Dist Y"  }
	self:add_opencl( {nil,nil, 8,.8}, names, self:get_opencl(), 10 )	-- self:get_opencl() kind of strange (safe because nit_opencl() was called) 
end

function meu:init()
	self:init_opencl()
end

--todo optimize on change only
function meu:update()
	local bind = self:get_texture_bind_2d()
	--self:print( bind )
	self:set_opencl_bind_2d( 1, bind )
end