function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2D", "3D", "Draw", "Procedural", "Texture", "unfinished", "experimental" },
			help = 	"display a photo with in particular BUs to control uv mapping\n"..
					" and an experimental interpolation of presets\n"..
					"Was probably used toto a dynamic photo slidesahow with continuous zoom on eyes"
			 }
end

function meu:get_preset_nb() 	return 16	end

function meu:define_ui()
	local ref = self.ref
	local bu
	local SY = 1
	local DY = .2
	local ix,iy = 1,1
	self:add_bu_texture_target_unit( {ix,iy, 8,6} )
	iy = iy + 6 + DY
	self:add_rgbf( {ix,iy, 8,SY} )
	iy = iy + SY + DY
	self:add_size_f( {ix,iy, 8,SY} )
	iy = iy + SY + DY
	self:add_mapping_and_blending( {ix,iy, 8,SY*3})
	
	ix,iy = 9,3
	self:add_camera()

	ref.grid = self:get_obj_down( "grid" )
	self.bu_x	= self:add_param_obj_name(	{ix,iy,			8,1}, 	"x",	ref.grid, "center_x",	-4, 4 )
	self.bu_y	= self:add_param_obj_name(	{ix,iy+1,		8,1}, 	"y",	ref.grid, "center_y",	-4, 4 )
	self.bu_z	= self:add_param_obj_name(	{ix,iy+2,		8,1}, 	"z",	ref.grid, "center_z",	 0, 8 )

	self.lerp = self:add_lerp()
end

function meu:do_lerp()
	--self:print( "meu:do_lerp()" )
	local i,f = self:update_lerp()
	if not i then return end

	self:lerp_presets( i, f, { "x", "y", "z", "sfactor" } )
end

function meu:update()
	local ref = self.ref
	if ref.model then
		--size from texture
		local ry = self:get_texture_ratio_y()
		if ry then
			param.set( ref.size_u, 1 )
			param.set( ref.size_v, 1*ry )
		end
	end
end

