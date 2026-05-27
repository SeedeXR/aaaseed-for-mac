

function DIOR:init_experiences()
	self.__experiences = {
	--	Menu name 					code		directory name
		{	menu="Dream Skin Advanced",	code="DSA",	dir="DreamskinAdvanced" },
		{	menu="Hydra Life",			code="HL",	dir="HydraLife"			},
	}
	local names = {}
	for i, t in ipairs(self.__experiences) do
		names[i] = t.menu
	end
	self.__experience_names = names
	self:set_experience( "DSA" )
end

function DIOR:set_experience( sym )
	local t = array.find_elt_by_field_val( self.__experiences, #(self.__experiences), "code", sym )
	if t then
		self.__experience = t
	else
		self:box_error( "can't find experience "..sym )
	end
end

function DIOR:get_experience()		return self.__experience 		end
function DIOR:get_experience_menu()	return self.__experience.menu	end
function DIOR:get_experience_code()	return self.__experience.code	end
function DIOR:get_experience_dir()	return self.__experience.dir	end

function DIOR:make_experience_button( bu )
	self.__experience_bu = bu
	bu:set_menu( self.__experience_names, "lang_menu" )
end

function DIOR:update_experience()
	local id = self.__experience_bu:get_value()
	local code = self.__experiences[id].code
	self:print( "experience is "..id.." -> "..code )
	self:set_experience( code )
end

--[[
function DIOR:load_lang()
	self:update_lang()
	self:load_anim_lang()
	self:load_imgs_lang()
end
--]]
