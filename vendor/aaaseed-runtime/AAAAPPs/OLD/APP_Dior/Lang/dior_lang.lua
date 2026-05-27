
DIOR.__langs = {
	{ name="Traditional Chinese",	code="CH",	HL="EN"	},
	{ name="English",				code="EN" 			},
	{ name="Francais",				code="FR",			},
	{ name="Japanese",				code="JP"			},
	{ name="Korean",				code="KR",	HL="EN"	},
	{ name="Thai",					code="TH",	HL="EN"	},
}

function DIOR:init_lang()
	local la = self.__langs
	self.__lang_by_code = {}
	for i=1,#la do
		local t = la[i]
		self.__lang_by_code[t.code] = t
	end
	self:set_lang( "FR" )
end

function DIOR:set_lang( sym )
	local exp = app:get_experience_code()
	sym = self.__lang_by_code[sym][exp] or sym
	self:print( "  lang will be "..sym )
	self.__lang_code_cur = sym
	self.__lang_dir = "TXT_"..sym.."/"
end

function DIOR:get_lang_dir( sym )
	return self.__lang_dir
end


function DIOR:make_lang_button( bu )
	self.__lang_bu = bu
	local t = array.extract_subfield_as_array( self.__langs, "name" )
	bu:set_menu( t, "lang_menu" )
end

function DIOR:update_lang()
	local la = self.__lang_bu:get_value()
	local code = self.__langs[la].code
	self:print( "lang set to "..la.." -> "..code )
	self:set_lang( code )
end

function DIOR:load_lang()
	self:update_lang()
	self:load_anim_lang()
	self:load_imgs_lang()
end
