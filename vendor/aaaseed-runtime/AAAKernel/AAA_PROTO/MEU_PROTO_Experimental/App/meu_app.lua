function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "Core", "Experimental", "Utility" },
			help="An experience unfinisshed"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local iy = 1
	local DY = .8

end

function meu:get_preset_nb() 	return 0	end

function meu:__get_app_local()
	local lapp = self.__app
	if not lapp then
		local name = self:get_name()
		lapp = APP_FACTORY.create_app_inst( string.upper(name), name )
		self.__app = lapp
	end
	return lapp
end

function meu:update()
	local lapp = self:__get_app_local()
	local modules = self.__modules
	if modules then
		aaa.obj.set_focus_ui( modules )
		aaa.obj.update_then_draw( modules )
	end
end

function meu:draw_icon()
	gol.set_line_width( 2 )
	gol.push_translate_scale_2d( -.48, -.3, .13, .8 )
		aaa.draw_str( "APP" )
	gol.pop_matrix()
end

function meu:draw()
	local lapp = self:__get_app_local()
	if lapp then
		lapp:push()
		lapp:update()
		lapp:draw()
		lapp:pop()
	end
end

