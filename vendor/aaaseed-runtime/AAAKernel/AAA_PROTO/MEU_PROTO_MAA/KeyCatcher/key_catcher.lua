function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "device", "unfinished", "tutorial", "experimental", "input", "utility" },
				help =	{	"MEU to catch a key pressed, output it to a BU, so a MEY Trax can use it."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui

	local ix,iy = 1,1
	local SY = 2
	local DY = .25
	local bu

	self:add_button(	{ix,iy,		4,SY },	"Learn", 	self, "b_learn",	false 	)

	bu = self:add_text(	{ix+4,iy,	4,SY },	"Ascii"		)
	ui.bu_ascii = bu
	bu = self:add_text(	{ix+8,iy,	4,SY },	"Char"		)
	ui.bu_char = bu

	self:add_button(	{ix+12,iy,	4,SY },	"Out", 	self, "b_out",	false 	)
end

-- function meu:init()
-- 	local ref = self.ref
-- 	ref.ascii = param.get_ref( aaa.ref.pref, "keyboard_ascii_out" )
-- end

function meu:update()
	local ref = self.ref
	local ui = self.ui

	local key = aaa.keyboard.get_ascii_down()
	if key~=0 then
		if self.b_learn then
			ui.bu_ascii:set_text( key )
			ui.bu_char:set_text( string.char(key) )
			self.b_learn = false
		end
		local target = tonumber( ui.bu_ascii:get_text() )
		--self:print( "target is "..target )
		self.b_out = key == target
	else
		self.b_out = false
	end
end

-- function meu:draw()
-- end