function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "art", "draw", "procedural", "3d", "experimental", "unfinished", "tutorial", "texture", "point" },
				help =	{	"A galaxy of particle with a texture projected on it.",
							"Done with OpenCl."
						}
			}
end

function meu:define_ui()	self:define_ui_oclgl_galaxy()	end
function meu:init()			self:ocl_init()		end
function meu:update()
	local bind = self:get_texture_bind_2d( 1 )
	local ocl = self:get_opencl()
	ocl:set_tex_src( 1, bind )
	self:update_oclgl_galaxy()
end

function meu:draw()
	MEU.draw( self )

	if self:get_bu_value( "visu" )==1 then
		self:draw_oclgl_galaxy_visu()
	end
end
