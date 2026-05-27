--
--	LANG
--
--	reference is https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
if CLASS.DECLARE( "LANG", nil, {
	ext_def = { french = "fr", english = "en", spanish = "es" },
	} ) then
	LANG:set_class_status_doc(	CLASS.STATUS.CORE,
								"deal with different language" )
end

--mdh	do it and integrate in IMG/IMGS
function LANG:create( name, ... )
	local self = LANG:create_instance( name )
	self.__langs		= {}	--first one will be the default one
	self.__ext_to_name	= {}
	self.__name_to_ext	= {}

	local tab = pack(...)
	for i=1,#tab do
		self:add( tab[i] )
	end
	self:set_def( tab[1] )
	self:set_cur( tab[1] )
	return self
end
function LANG:update()
end

function LANG:set_def( name )	self.__def = name	end
function LANG:get_def()			return self.__def	end

function LANG:set_cur( name )	self.__cur = name	end
function LANG:get_cur()			return self.__cur	end

function LANG:add( name, ext )
	if not ext then
		ext = self.ext_def[name]
	end
	self.__langs[#(self.__langs)+1] = { name = name, ext = ext }
	self.__ext_to_name[ext] = name
	self.__name_to_ext[name] = ext
end

function LANG:is_short_for_lang( la )
	la = string.lower( la )
	return self.__ext_to_name[la] ~= nil
end

--todo refine
function LANG:extract_lang( fname )
	local char = string.sub( fname, -3, -3  )
	--aaa.print( "Char at -3 : "..char )
	if char == "_" then
		local la = string.sub( fname, -2  )
		--aaa.print( fname.." have lang : "..lang )
		if self:is_short_for_lang(la) then
			return la
		end
	end
end

function LANG:filter_out_translation( t_file )
	for key, fname in PAIRS(t_file) do
		--self:print_inverse( "---------- "..key.." "..fname )
		local name_pure, ext = FNAME.split_ext( fname )
		local la = self:extract_lang( name_pure )
		if la and la ~= self.__langs[1].ext then	--id 1 is the default one
			t_file[key] = nil
		end
	end
end

function LANG:find_file_translation( filename, b_full )
	local langs = self.__langs
	--table.print( langs, "retiti", 4 )
	--self:box_debug( "try find_file_translation "..filename.." "..#langs )
	if #langs == 0 then return end

	local pre, ext = FNAME.split_ext( filename )
	-- remove lang extention if there is one
	if self:extract_lang( pre ) then
		pre = string.sub( pre, 1, -3  )
	else
		pre = pre.."_"
	end

	local is_exist = b_full and aaa.file.is_exist or MEDIA.is_exist
	local tab
	for i=1,#langs do
		local lang = langs[i]
		local la = lang.ext
		local fname_new = pre..la.."."..ext
--		self:box_debug( "try langue "..fname_new )
		if is_exist( fname_new ) then
			if not tab then tab = {} end
			tab[lang.name] = fname_new
--			self:box_debug( "langue "..lang.name.." -> "..fname_new )
		end
	end
	return tab
end

