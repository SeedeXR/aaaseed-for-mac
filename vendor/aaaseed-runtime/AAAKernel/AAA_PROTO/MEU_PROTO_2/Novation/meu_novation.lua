function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "Vj", "input", "device", "unfinished" },
				help =	{	"UI for the midi controller NOVATION",
						}
			}
end

function meu:bu_change( bu, ix, iy )
	local val = bu:get_value()
	if aaa.lua.global.get( "novation" ) then
		novation:set_led( ix, iy, val )
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	local ix=1
	local iy=2
	local s = 1.2
	local nb = 8
	for x = 1,nb do
		self:add_slider(	{ix+x*s,iy,	s,s}, "slider_x_"..x ):set_text_draw(false)
	end
	local d = .2
	iy=iy+d
	for y=1,nb do
		for x = 1,nb do
			local bu = self:add_slider(	{ix+x*s,iy+y*s,	s,s}, x.."_"..y ):set_text_draw( false )
			bu:set_method_on_value_change( self, "bu_change", bu, x, y  )
		end
	end
	for y=1,nb do
		self:add_slider(	{ix+9*s+d,iy+y*s,	s,s}, "slider_y_"..y ):set_text_draw(false)
	end

end


--function meu:init()
--end

function meu:update()
	NOVATION.TEST()
end
