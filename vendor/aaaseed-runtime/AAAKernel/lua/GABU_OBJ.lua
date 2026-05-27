aaa.show_file_begin( "GABU_OBJ" )

--todo load oo here
--todo check we have ids
--todo isolate id : __id or __gabu_id or __gabu.id and add access fn get_gabu_obj_id()
-- infact we now have __bu_id
if not CLASS then
	CLASS = {	__classes_by_name	= {},
				__str_class_field	= "__gabu",
				doc					= {},			--todo refine
			}
	function CLASS.is_class_name( name )
		return CLASS.__classes_by_name[name] ~= nil
	end
	CLASS.STATUS =	{
						GABU         = "gabu",
						GABUZOMEU    = "gabuzomeu",
						CORE         = "core",
						APP          = "app",
						EXPERIMENTAL = "experimental",
						MATH         = "math",
						TUTORIAL     = "tutorial",
						UNUSED       = "unused"
					}
	function CLASS.MAKE_GABU( name, doc, status )
		return {
					class_name			= name,			--	name of class
					classes_by_name		= {},			--	derived classes indexed by name
					classes_by_class	= {},			--	derived classes indexed by class
					created_nb			= 0,
					class_doc			= doc or "",
					class_status		= status or "",
					anonymous_count		= 0,
				}
	end

	--
	--	GABU_OBJ
	--
	GABU_OBJ = oo.class(
		{
			__gabu		=	CLASS.MAKE_GABU( "GABU_OBJ",	--	name of class
											"This is the base class for for all GaBu Object ",
											CLASS.STATUS.GABU
							),
			verbose				= 0,
			doc					= {},			--todo refine
		}
	)
	CLASS.__classes_by_name["GABU_OBJ"] = GABU_OBJ

	CLASS.doc.DECLARE = "( class_name, super, def_table )\n"..
	"super is the class we want to be derived from, if nil GABU_OBJ will be used\n"..
	"def_table is where you define the properties and methods of the class.\n"..
	"  If you already have a table prepared that represents the structure of the class, you can pass it,\n"..
	"  otherwise, DECLARE creates a new table for you (see oo.class() from loop for detail)."	
	function CLASS.DECLARE( class_name, super, def_table )
		if aaa.lua.global.get( class_name ) == nil then
			--aaa.box_debug(	aaa.script.get_dir_name().."\n"..aaa.script.get_name().."\n"..aaa.script.get_name_full()	)
			super = super or GABU_OBJ

			-- get a list of fields for the class or not 
			def_table = def_table or {}
			-- if def_table.verbose==nil then
			-- 	aaa.print_debug( "force verbose" )
			-- 	def_table.verbose = 0
			-- end
			-- init class info
			def_table[CLASS.__str_class_field] = CLASS.MAKE_GABU( class_name )
			def_table.doc = {}
			def_table.__lua_script_fullname = aaa.script.get_name_full()


			local class = oo.class( def_table, super )
			-- if class_name == "BU" then
			-- 	table.print( class, class_name )
			-- 	aaa.box_debug( class_name.." in CLASS.DECLARE" )
			-- end
			
			CLASS.__classes_by_name[class_name] = class

			super[CLASS.__str_class_field].classes_by_name[class_name] = class
			super[CLASS.__str_class_field].classes_by_class[class] = class_name

			aaa.lua.global.set( class_name, class )

			class[CLASS.__str_class_field].class = class
			class[CLASS.__str_class_field].super = super

			return class
		end
		return nil
	end

	CLASS.doc.is_class = "CLASS.is_class( value ) test is value is a class (if value is an instance it will return false)"
	function CLASS.is_class( value )
		return rawget( value, CLASS.__str_class_field ) ~= nil
	end
	
	CLASS.doc.is_gabu_obj = { "CLASS.is_gabu_obj( obj [,class] ) test first if obj is a GABU_OBJ",
								"if this is true and the class arg is passed obj is tested to be of this class"
							}
	function CLASS.is_gabu_obj( obj, class )
		--aaa.print( "CLASS.is_gabu_obj()" )
		if type(obj) ~= "table" then
			--aaa.print( "no table" )
			return false
		end
		local gabu
		if obj == _G then
			gabu = aaa.lua.global.get(CLASS.__str_class_field)
		else
			gabu = obj[CLASS.__str_class_field]
		end
		if not gabu then
			--aaa.print( "not gabu" )
			--will trigger an infinite loop of calls
			--aaa.print( "no gabu "..CLASS.__str_class_field.." "..obj[CLASS.__str_class_field] )
			return false
		end
		if not class then
			--aaa.print( "no class so return true" )
			return true
		end
		--aaa.print( "we test class now" )
		return gabu.class == class
	end


	function GABU_OBJ:set_class_status_doc( status, ... )
		local gabu = self[CLASS.__str_class_field]
		gabu.class_status = status or ""
		local str = ""
		for i, line in IPAIRS( {...} ) do
			str = i>1 and str.."\n"..line or line
			--aaa.print( "line "..i.." str is "..str )
		end
		gabu.class_doc = str	
	end

	GABU_OBJ:set_class_status_doc(	CLASS.STATUS.CORE,
									"GABU_OBJ is the base class used for most the lua object",
									"  in the GaBu or GaBuZoMeu environements.",
									"it provides usefull methods for all subclasses in particular for printing and debugging,",
									"  and eventually store informations about an object tree (up/down)"
								)

end	-- end of if not CLASS then

local doc = GABU_OBJ.doc

doc.is_class_name = "( class_name ) return if the obj is an instance or the class having the name class_name"
function GABU_OBJ:is_class_name( name )		return self[CLASS.__str_class_field].class_name == name	end
doc.get_class_name = "() return the string which is class_name of the object class"
function GABU_OBJ:get_class_name()			return self[CLASS.__str_class_field].class_name			end

--todo perhaps cache the super
--function GABU_OBJ:get_class()			return oo.getclass( self ) 					end
--function GABU_OBJ:get_class_super()	return oo.getsuper( oo.getclass(self) ) 	end
doc.is_class = "( class ) return if the obj is an instance or a class of the class arg"
function GABU_OBJ:is_class( class )			return self[CLASS.__str_class_field].class == class		end

doc.get_class = "() return the class"
function GABU_OBJ:get_class()				return self[CLASS.__str_class_field].class				end

doc.get_class_super = "() return the class super (the base class)"
function GABU_OBJ:get_class_super()			return self[CLASS.__str_class_field].super				end

doc.is_derived_from_class = "() return if the obj is an instance or a class derived from the class arg"
function GABU_OBJ:is_derived_from_class( class )
	local super = self:get_class()
	while super do
		if class == super then
			return true
		end
		super = super:get_class_super()
		--self:print( "super is "..super ) 
	end
	return false
end

doc.get_class_name = "() return a table containing all the classes indexed by their name"
function GABU_OBJ:get_classes_by_name()		return self[CLASS.__str_class_field].classes_by_name		end
doc.get_classes_by_class = "() return a table containing all the classes indexed by their class"
function GABU_OBJ:get_classes_by_class()	return self[CLASS.__str_class_field].classes_by_class		end
--function GABU_OBJ:is_class_()				return self[CLASS.__str_class_field].classes_by_class		end

function GABU_OBJ:get_created_nb()			return self[CLASS.__str_class_field].created_nb			end
function GABU_OBJ:inc_created_nb()
	local gabu = self[CLASS.__str_class_field]
	gabu.created_nb = gabu.created_nb + 1
	--self:print_debug( "gabu.created_nb is "..gabu.created_nb	)
	--self:print_debug( "gabu.created_nb is "..gabu.created_nb.." and gabu_obj_id "..self.__gabu_obj_id  )
	local super = self:get_class_super()
	--self:print( "class is " .. self:get_class() )
	--self:print( "super is " .. super )
	--self:box_debug()
	if super then
		super:inc_created_nb()
	end
end
doc.get_gabu_obj_id = { "() return an integer number which is a unique id defined for each object.",
						" for now (2025 September) it correspond to the count of gabu_obj created but this could change." }
function GABU_OBJ:get_gabu_obj_id()
	return self.__gabu_obj_id
end

function GABU_OBJ:get_doc()				return self.doc										end
function GABU_OBJ:get_class_doc()		return self[CLASS.__str_class_field].class_doc		end
function GABU_OBJ:get_class_status()	return self[CLASS.__str_class_field].class_status	end


function GABU_OBJ:print_class_info( level )
	local name = self:get_class_name()
--	self:print( "print_classes( "..level.." "..name.." )" )
--	table.print( class[CLASS.__str_class_field], name, 2 )
--	table.print( classes_by_name, CLASS..".classes_by_name" )

    local prefix = string.rep( "   ", level )

	local doc, status = self:get_class_doc(), self:get_class_status()
	status = string.len(status)>0 and status or nil
	doc = string.len(doc)>0 and doc or nil

	aaa.print( prefix..(status and name.." -- "..status or name ) )
	if doc then
		aaa.print( prefix.."  " ..doc )
	end
end

function GABU_OBJ:parse_class( level, level_max, method, ... )
    level = level or 0 -- Assuring level exists
	level_max = level_max or level

	self[method]( self, level, ... )
	
	if level < level_max then
		local classes_by_name = self:get_classes_by_name()
		if next(classes_by_name)~=nil then
			level = level + 1			
			for name, class_down in pairs_sorted(classes_by_name) do
				class_down:parse_class( level, level_max, method, ... )
			end
		end
	end
end

--only use once (2020 April)
-- function GABU_OBJ:print_classes( class )
-- 	self:print( "print_classes( "..class:get_class_name().." )" )
-- 	local classes_by_name = class:get_classes_by_name()
-- 	table.print( classes_by_name, CLASS..".classes_by_name" )
-- 	self:box_debug( class:get_class_name() )
-- 	local str = ""
-- 	--todo better
-- 	for key, class_down in pairs_sorted(classes_by_name) do
-- 		self:print( "print_classes() down is "..key )
-- 		str = str..key
-- 		str = str.."\n"..self:print_classes(class_down)
-- 	end
-- 	return str
-- end
--todo remove or redo
if false then
	function GABU_OBJ:print_classes( tab, class, level )
		level = level or 0 -- Assuring level exists
	
		local name = class:get_class_name()
		self:print( "print_classes( "..level.." "..name.." )" )
	--	table.print( class[CLASS.__str_class_field], name, 2 )
	--	table.print( classes_by_name, CLASS..".classes_by_name" )

		local prefix = string.rep( "   ", level ) -- prefix for nest level

		local doc, status = class:get_class_doc(), class:get_class_status()
		status = string.len(status)>0 and status or nil
		doc = string.len(doc)>0 and doc or nil
	--	if status then self:print_debug( "status "..status ) end
	--	if doc then self:print_debug( "doc "..doc ) end

		table.insert( tab, prefix .. ( status and name.." -- "..status or name ) )
		if doc then
			table.insert( tab, prefix.."-- "..doc )
		end
		local classes_by_name = class:get_classes_by_name()
		if next(classes_by_name)~=nil then
			for name, class_down in pairs_sorted(classes_by_name) do
				if class_down then -- Making sure there's an object before getting its name
					local sub_tab = self:print_classes( tab, class_down, level+1 )
					if sub_tab ~= "" then  -- If there are any subclasses
						table.append( tab, sub_tab )
					end
				end
			end
		end
		table.print( tab, "tab", 2 )
		self:print( "tab is "..tab )
		local result = table.concat( str, "\n" )
		return result --~= "" and result or "" -- Trimming additional new line
	end
end

function GABU_OBJ.__accumulate_class( str, class )
	local super = oo.getsuper( class )
	if super then
		local cn = super:get_class_name()
		if cn ~= "GABU_OBJ" then
			str = GABU_OBJ.__accumulate_class( cn..":"..str, super )
		end
	end
	return str
end
function GABU_OBJ:get_class_hierarchy_str( )
	return GABU_OBJ.__accumulate_class( self:get_class_name(), self:get_class() )
end

function GABU_OBJ:edit_lua()
	local name = self.__lua_script_fullname
	if name then
		aaa.os.open_editor( name )
	else
		self:print_debug( "GABU_OBJ:edit_lua() No __lua_script_fullname" )
	end
end

function GABU_OBJ:__build_print_name()
	local str = self[CLASS.__str_class_field].class_name
	if self.__id then
		str = str.."("..self.__id..")"
	end

	local name = self:get_name()
	if name then
		str = str.."|"..name
	else
		if self:get_class() == self then
			str = str	--.."_CLASS"
		else
			str = str.."_NIL"
		end
	end

	-- 2025 Jan we don't display label anymore by default to avoid confusion
	-- local label = self:get_label()
	-- if label then
	-- 	str = str.."("..label..")"
	-- end
	return str
end

--
--	NAME
--
doc.__init_name = "( name ) used at creation when creating object instance."
function GABU_OBJ:__init_name( name )
	self.__name = name
	self.__name_lowercase = string.lower( name )	-- we store the lowercase version for speed
end
doc.change_name = "( name ) to be used to change the name of an existing object." 
function GABU_OBJ:change_name( name )
	if name == self.__name then
		self:print_debug( "change_name() try to change to the same name: "..name )
		return
	end
	self:__init_name( name )
end

doc.get_name = "() return the object name."
function GABU_OBJ:get_name()
	return rawget( self, "__name" )
end
doc.get_name_lowercase = "() return the object name as get_name() but with lower case characters."
function GABU_OBJ:get_name_lowercase()
	return rawget( self, "__name_lowercase" )
end

doc.is_name = "( name ) test the name." 
function GABU_OBJ:is_name( name )
	return string.lower( name ) == rawget( self, "__name_lowercase" )
end
doc.is_name_lowercase = "( name ) test the name assuming arg name is lower case." 
function GABU_OBJ:is_name_lowercase( name )
	return name == rawget( self, "__name_lowercase" )
end

--
--	LABEL
--
--todo explain and document the MEU direct use of it
function GABU_OBJ:set_label( label )
	self.__label = label
	-- if label == "preset proto" then
	-- 	self:box_debug( "preset proto found" )
	-- end
end
function GABU_OBJ:get_label()
	local label = rawget( self, "__label" )
	-- if label == "preset proto" then
	-- 	self:box_debug( "preset proto found" )
	-- end
	return label
end
function GABU_OBJ:get_label_lowercase()
	local label = rawget( self, "__label" )
	if label then
		return string.lower( label )
	end
end
function GABU_OBJ:is_label( label )
	return string.lower( label ) == self:get_label_lowercase()
end

--todo should we extend to title and text
--todo could extend the pattern
--todo this slow do a __label_lower field to speed up access
function GABU_OBJ:is_name_match( pat )
	local text = self:get_name_lowercase()
	local b_match = string.find( text, pat ) ~= nil
	if not b_match then
		local text = self:get_label_lowercase()
		if text then
			b_match = string.find( text, pat ) ~= nil
		end
	end
--	self:print( "match is "..b_match )
	return b_match
end


--[[
function GABU_OBJ:get_sname()
	--self:print( "GABU_OBJ:get_sname() "..rawget(self,"__sname").." "..rawget(self,"__name").." "..rawget(self,"text").." "..rawget(self,"title") )
	local name = rawget( self, "__sname" )
	if name then return name end

	name = self:get_name()
	if name and name~="no_name" then return name end

	name = rawget( self, "text" )
	if name then return name end
	
	return rawget( self, "title" )
end
--]]


--
--	PROP
--
--unused 2023 Sept
function GABU_OBJ:set_prop( key, value )
	local t = self.__props
	if not t then
		t = {}
		self.__props = t
	end
	t[key] = value
end
--unused 2023 Sept
function GABU_OBJ:get_prop( key )
	local t = self.__props
	if t then 
		return t[key]
	end
end

-- values
--
function GABU_OBJ:__serialize_to_str( tab, val, level, b_cr )
	level = level or 0
	tab = tab or {}

	local typ = type(val)

	--self:print( "serialize_to_str" )
	if typ == "table" then
		if level == 0 and b_cr then
			tab[#tab+1] = "{\n"
		else
			tab[#tab+1] = "{"
		end
		local nb = #val
		if nb == 0 then
			tab[#tab+1] = " "
			local b_comma = false
			for key, val1 in pairs_sorted( val ) do
				if b_comma then
					tab[#tab+1] = ", "
				else
					b_comma = true
				end
				tab[#tab+1] = key
				tab[#tab+1] = "="

				self:__serialize_to_str( tab, val1, level + 1, b_cr )
			end
		else
			for i = 1, nb do
				if i==1 then
					tab[#tab+1] = " "
				else
					tab[#tab+1] = b_cr and ", \n" or ", "
				end
				self:__serialize_to_str( tab, val[i], level + 1, b_cr )
			end
		end

		if level == 0 and b_cr then
			tab[#tab+1] = "\n\t}\n"
		else
			tab[#tab+1] = " }"
		end
	elseif typ == "number" then
		tab[#tab+1] = tostring( val )
	elseif typ == "string" then
		tab[#tab+1] = string.format( "%q", val )
	elseif typ == "boolean" then
		tab[#tab+1] = val and "true" or "false"
	else
		--str = str..val
	end
	return tab
end
function GABU_OBJ:serialize_to_str( str, val, level )
	local tab = {}
	if str and str ~= "" then
		tab[1] = str
	end
	self:__serialize_to_str( tab, val, level, true )
	return table.concat(tab)
end
function GABU_OBJ:serialize_to_line( str, val, level )
	local tab = {}
	if str and str ~= "" then
		tab[1] = str
	end
	self:__serialize_to_str( tab, val, level, false )
	return table.concat(tab)
end
-- function GABU_OBJ:__serialize_value( str, val )
-- 	if type(val) == "table" then
-- 		str = str.."{"
-- 		local nb = #val
-- 		for i = 1, nb do
-- 			if i==1 then
-- 				str = str.." "
-- 			else
-- 				str = str..", "
-- 			end
-- 			str = self:__serialize_value( str, val[i] )
-- 		end
-- 		str = str.." }"
-- 	else
-- 		str = str..val
-- 	end
-- 	return str
-- end

--
--	Util
--
-- same than table.get_field_table_always( dst, key, sub_key ) but durectly unrolled here to avoid a call


function GABU_OBJ:get_table_always( key, sub_key )
	--  create the table if it don't exist
	--local t = self[key] 
	local t = rawget( self, key ) -- we want it be on instance so we can't use self[key]
	if not t then
		t = {}
		self[key] = t
		--self:print( "add table "..key )
	end
	-- return it if we don't need a sub_table
	if not sub_key then
		return t
	end

	-- do the same on the field of the table
	--local sub_t = t[sub_key]
	local sub_t = rawget( t, sub_key )
	if not sub_t then
		sub_t = {}
		t[sub_key] = sub_t
	end
	return sub_t
end

function GABU_OBJ:get_table( key )
	--return self[key]
	return rawget( self, key ) 
end


function GABU_OBJ:dump_up( level )
	self:print( "DUMP "..level )
	local up = self:get_up()
	if up then
		up:dump_up( level + 1 )
	end
end

function GABU_OBJ:print_table( t, str, level_nb )
	table.print( t, self..":"..str, level_nb )
end
--
--	CREATE INIT ...
--
function GABU_OBJ:__init_obj_ref( obj_ref_or_name )
-- test done in caller for optimisation
--	if obj_ref_or_name then
		local obj
		if type(obj_ref_or_name) == "string" then
			-- get it by name
			obj = aaa.obj.get( obj_ref_or_name )
		else
			-- check it is an obj ref
			if aaa.obj.is_ref_no_error( obj_ref_or_name ) then
				obj = obj_ref_or_name
			else
				self:print( obj_ref_or_name.." should be a name or an object ref, but is not." )
			end
		end
		if obj then
			local ref = {}
			ref.obj = obj
			ref.obj_active = param.get_ref_no_error( ref.obj, "active" )
			self.ref = ref
		end
--	end
end

function GABU_OBJ:init_instance()
	--local gabu = self[CLASS.__str_class_field]
	local gabu = GABU_OBJ[CLASS.__str_class_field]
	self.__gabu_obj_id = gabu.created_nb	-- unique id for each gabu_obj
	--self:print_debug( "gabu.created_nb is "..gabu.created_nb )
	-- table.print( gabu, "gabu" )
end
function GABU_OBJ.__make_name( class, name, b_use_nb )
	if name then
		if b_use_nb then
			name = name .. " " .. (class:get_created_nb() + 1)
		end
	else
		if not class._b_noname_can then
			--aaa.box_warning( "mainly all GABU_OBJ require a name" )
			--todo class is not a GABU_OBJ and should be
			class:box_debug( "mainly all GABU_OBJ require a name" )
		end
		name = class:get_class_name().."_def_".. (class:get_created_nb() + 1)
	end
	return name
end

doc.__create_instance = "( class, name, obj_ref_or_name ) private fn called for every instanciation (called indirectly by create methods)"
function GABU_OBJ.__create_instance( class, name, obj_ref_or_name )
	name = GABU_OBJ.__make_name( class, name, false )
	local self = class{}
	self.__name_at_creation = name	-- useful for debug only
	class:inc_created_nb()
	self:__init_name( name )
	if obj_ref_or_name then
		self:__init_obj_ref( obj_ref_or_name )
	end

	--aaa.print_fn( GABU_OBJ.__create_instance )
	class.init_instance( self )
	
--	self:print_inverse( "GABU_OBJ.create_instance" )
	return self
end
doc.create_instance = "( class, name, obj_ref_or_name ) the basic function to create instance"
function GABU_OBJ.create_instance( class, name, obj_ref_or_name )
	return GABU_OBJ.__create_instance( class, name, obj_ref_or_name )
end
doc.create_instance_no_name = "( class, obj_ref_or_name ) the function to create instance. "..
	"the name will be in the form anonymous_number where number is a count by class"
function GABU_OBJ.create_instance_no_name( class, obj_ref_or_name )
	local gabu = class[CLASS.__str_class_field]
	local anonymous_count = gabu.anonymous_count + 1
	gabu.anonymous_count = anonymous_count
	local name = "anonymous_"..anonymous_count
	return GABU_OBJ.__create_instance( class, name, obj_ref_or_name )
end
doc.create_instance_with_obj = "( class, obj_ref_or_name ) the function to create instance with a check on the presence of the obj."
function GABU_OBJ.create_instance_with_obj( class, name, obj_ref_or_name )
	if obj_ref_or_name then
		return GABU_OBJ.__create_instance( class, name, obj_ref_or_name )
	else
		class:print_error( "not obj provided to create GABU_OBJ" )
	end
end
--[[
function GABU_OBJ:create( name, obj_ref_or_name )
	return GABU_OBJ.create_instance( GABU_OBJ, name, obj_ref_or_name )
end
--]]

--todo add strategy to count using a generic destructor
function GABU_OBJ:free()
	self:box_error( "called because no free implemented", aaa.debug.make_traceback_table() )
end

--
-- PRINT
--
function GABU_OBJ.repeat_print_fn( self, fn, ... )
	if type(self) == "string" then
		aaa.print_error( "Probably a print done with a '.' instead of a ':'. please correct" )
		aaa.print_error( "the string to print would be : "..self )
		aaa.print_error( "should be in "..aaa.debug.get_fn_info(4)[1] )--hack [1] is a quick hack here )

		aaa.print_error( "The calls :" )
		aaa.debug.print_traceback()
	else
		--aaa.print( self:get_class_hierarchy_str() )

		local nb = select( "#", ... )
		--aaa.print( "nb = "..nb  )
		local prefix = aaa.__build_table_str( self ).." : "
		for i = 1,nb do
			local str = select( i, ... )
			fn( prefix..aaa.__build_str( str ) )
		end
	end
end

--
-- ERROR VERBOSE DEBUG
--
function GABU_OBJ:error( ... )			self:print( ... ) error( ... )	end
function GABU_OBJ:print( ... )			GABU_OBJ.repeat_print_fn(	self,	aaa.print,			...	)	end
function GABU_OBJ:print_inverse( ... )	GABU_OBJ.repeat_print_fn(	self,	aaa.print_inverse,	... )	end
function GABU_OBJ:print_debug( ... )	GABU_OBJ.repeat_print_fn(	self,	aaa.print_debug,	... )	end
function GABU_OBJ:print_error( ... )	GABU_OBJ.repeat_print_fn(	self,	aaa.print_error,	... )	end


function GABU_OBJ:print_args( ... )
	if ... then
		local nb = select( "#", ... )
		local str = "  args : "..select( 1, ... )
		for i = 2, nb do
			str = str..", "..select( i, ... )
		end
		self:print( str )
	end
end

function GABU_OBJ:show( val, str )
	aaa.debug.show( val, str, self )
end
function GABU_OBJ:show_error( str, ... )
	aaa.debug.show_error( str, self, ... )
end
function GABU_OBJ:show_error_ui( str )
	self:show_error( " "..str, nil, 5 )
	aaa.bell()
end


function GABU_OBJ:set_verbose( s )
	-- if s == 1 then
	-- 	aaa.debug.print_traceback()
	-- 	self:print( "GABU_OBJ:set_verbose( "..s.." )" )
	-- end
	self.verbose = s
end
function GABU_OBJ:__print_mark( verbose, fn_level )
	aaa.print( "VERBOSE_"..verbose.." from "..aaa.debug.get_fn_info(fn_level)[1] ) --hack [1] is a quick hack here
end
function GABU_OBJ:verbose_0( ... )								self:__print_mark( 0, 		4			)	self:print( ... ) end
function GABU_OBJ:verbose_level( level, fn_level, ... )			self:__print_mark( level, 	fn_level	)	if self.verbose and self.verbose >= level then self:print( ... )	end		end
function GABU_OBJ:verbose_1( ... )								self:verbose_level( 1,		5, ... )		end
function GABU_OBJ:verbose_2( ... )								self:verbose_level( 2,		5, ... )		end
function GABU_OBJ:verbose_3( ... )								self:verbose_level( 3,		5, ... )		end

function GABU_OBJ:verbose_0_inverse( ... )						self:__print_mark(	0,		4			)	self:print_inverse( ... ) end
function GABU_OBJ:verbose_level_inverse( level, fn_level, ... )	self:__print_mark(	level,	fn_level	)	if self.verbose and self.verbose >= level then self:print_inverse( ... )	end		end
function GABU_OBJ:verbose_1_inverse( ... )						self:verbose_level_inverse( 1, 5, ... )	end
function GABU_OBJ:verbose_2_inverse( ... )						self:verbose_level_inverse( 2, 5, ... )	end
function GABU_OBJ:verbose_3_inverse( ... )						self:verbose_level_inverse( 3, 5, ... )	end

--
-- BOX mean dialog here
--
function GABU_OBJ:__box_helper( fn, ... )
	--testing
	--debug.debug()
	if fn ~= aaa.box_good then aaa.debug.print_traceback() end
	local method = aaa.debug.get_fn_name(4)
	GABU_OBJ.b_box_on = true
	local ret = fn( "in "..self, method.."()", ... )
	GABU_OBJ.b_box_on = false
	return ret
end

--	add a flag to silent these on location
function GABU_OBJ:box_good(		... )		return self:__box_helper( aaa.box_good,  	... )	end
function GABU_OBJ:box_debug(	... )		return self:__box_helper( aaa.box_warning,	... )	end
function GABU_OBJ:box_error(	... )		return self:__box_helper( aaa.box_error,	... )	end
function GABU_OBJ:box_look(		... )		return self:__box_helper( aaa.box_error,	"LOOK ----------------\n", ... )	end
function GABU_OBJ:box_dev(		title, ... )
	aaa.debug.print_traceback()
	local fn_name = aaa.debug.get_fn_name(3)
	GABU_OBJ.b_box_on = true
	local str = title.." in "..self..":"..fn_name.."()"
	local ret = aaa.box_dev( str, str.."\n", ... )
	GABU_OBJ.b_box_on = false
	return ret
end
--GABU_OBJ.box_dbg 		= GABU_OBJ.box_debug
GABU_OBJ.box_warning	= GABU_OBJ.box_debug
--GABU_OBJ.box_err 		= GABU_OBJ.box_error

function GABU_OBJ:dump( level )
	table.print( self, "Dump "..self, level or 3 )
end

--
--	FN
--
function GABU_OBJ.__make_table_add_arg( t, ... )
	local nb = select( "#", ... )
	--aaa.print( "GABU_OBJ.__make_table_add_arg() nb is "..nb )
	if nb > 0 then
		t.arg_nb = nb
		t.args = pack(...)	-- nil value are stored be carefule when using the table
		--table.print( t.args, "t.args" )
	end
	--table.print( self.__fn_table[key].args, key.." fn or method args", 2 )
	return t
end

doc.make_table_to_call_method = "( gabu_obj, method_name, ... ) prepare a table encapsulating gabu_obj, method, and args,\nreturn nil to uninstall"
function GABU_OBJ:make_table_to_call_method( gabu_obj, method_name, ... )
	--aaa.print_fn( "GABU_OBJ:make_table_to_call_method" )
	if gabu_obj == nil then
		return -- we want to remove the method so we return nothing
	end

	-- gabu_obj can be a table
	-- if not CLASS.is_gabu_obj(gabu_obj) then
	-- 	--table.print( gabu_obj, "gabu_obj", 2 )
	-- 	self:error( gabu_obj.." not a gabu_obj" )
	-- end

	if method_name~=nil then
		if type(method_name) ~= "string" then
			self:error( "method_name is not nil it should be a string" )
		end
		if method_name == "" then
			self:error( "method_name is an empty string" )
		end
	end
	return GABU_OBJ.__make_table_add_arg( { obj=gabu_obj, method_name=method_name }, ... )
end

doc.make_table_to_call_fn = "( fn, ... ) prepare a table encapsulating fn, and arg, return nil to uninstall"
function GABU_OBJ:make_table_to_call_fn( fn, ... )
	if fn == nil then
		return -- we want to remove the method so we return nothing
	end

	if type(fn) ~= "function" then
		self:error( "fn arg should be a function" )
	end
	return GABU_OBJ.__make_table_add_arg( { fn=fn }, ... )
end

function GABU_OBJ:do_nothing()
end

function GABU_OBJ:clear_fn_and_method()
	self.__fn_table = nil
end

function GABU_OBJ:__set_function_or_method( key, make_method, ... )
	--aaa.print_fn( "GABU_OBJ:__set_function_or_method" )
	if type(self)=="string" then
		aaa.print_error( "self is a string probably a call to GABU_OBJ:set_function with a . instead of a :" )
	elseif key == nil then
		self:error( "try to set_function() with nil key" )
	else
		local t = self[make_method]( self, ... )
		if t then
			self:get_table_always( "__fn_table" )[key] = t
		else
			if self.__fn_table then
				self.__fn_table[key] = nil
			end
		end
	end
	return self
end
function GABU_OBJ:set_function( key, fn, ... )					return self:__set_function_or_method( key, "make_table_to_call_fn",		fn, ... )						end
function GABU_OBJ:set_method( key, gabu_obj, method_name, ... )	return self:__set_function_or_method( key, "make_table_to_call_method",	gabu_obj, method_name, ... )	end

--todo add a print_table for these debugging
function GABU_OBJ:do_table( t, ... )
	if not t then return false end

	-- argument passed here are set after the one store
	--if self.verbose >= 3 then
		--todo
		--self:print( "GABU_OBJ:do_fn() "..key.." "..arga.." "..argb.." "..argc.." "..argd )
	--end
--	table.print( t.args, key.." method args", 2 )

	local args
	local arg_nb	-- we deal with nil arg in table
	if t.args then
		--table.print( t.args, "t.args" )
		local nb = t.arg_nb
		-- lua don't deal with call( unpack(something), ... )
		-- so we need ro build a table
		--todo reuse the existing
		if nb ~= 0 then
			args = {}
			for i=1,nb do
				args[i] = t.args[i]
			end
			arg_nb = nb
			local nb_to_add = select( "#", ... )
			if nb_to_add > 0 then
				for i=1,nb_to_add do
					args[nb+i] =  select( i, ... )
				end
				arg_nb = arg_nb + nb_to_add
			end
		end
		--table.print( args, "args" )
	end

	local fn_ret
	if t.fn then
		if args then	fn_ret = t.fn( unpack( args, 1,arg_nb ) )
		else			fn_ret = t.fn( ... )
		end
	else
		local obj = t.obj
		local method = obj[t.method_name]
		--aaa.print_method()
		--todo print_fn or print_method get method as fn name for the call not easy to debug
		if method then
			if args then	fn_ret = method( obj, unpack(args, 1,arg_nb ) )
			else			fn_ret = method( obj, ... )
			end
			--self:print_error( t.method_name.." called and return "..fn_ret )
		else
			--aaa.debug.print_traceback()
			self:print_error( "trying to call non existant method \""..t.method_name.."\"" )
			self:print_error( "\ton table "..obj )
			return false
		end
	end


	if fn_ret ~= nil then
		GABU_OBJ.__do_table_return_last = fn_ret
	end
	return true, fn_ret
end
function GABU_OBJ:get_do_table_return_last()
	return GABU_OBJ.__do_table_return_last
end

function GABU_OBJ:has_fn( key )
	local t = self.__fn_table
	if t then
		t = t[key]
		if t then
			return true
		end
	else
		--	this really useful for debugging
		if type(self) == "string" then
			aaa.print_error( "self is a string probably a call to GABU_OBJ:has_fn with a . instead of a :" )
		end
	end
	return false
end
function GABU_OBJ:do_fn( key, ... )
	-- if key=="click_double" then
	-- 	aaa.print_fn()
	-- end
	--aaa.print( "GABU_OBJ:do_fn() "..key )
	local t = self.__fn_table
	if t then
		t = t[key]
		if t then
			return self:do_table( t, ... )
		end
	else
		--	this really useful for debugging
		if type(self)=="string" then
			aaa.print_error( "self is a string probably a call to GABU_OBJ:do_fn with a . instead of a :" )
		end
	end
	return false
end

--
--	METHOD
--
function GABU_OBJ:apply_method( tab, method, ... )
	if not tab then return end
	local fn = self[method]
	if not fn then
		self:print( "Can't apply_method "..method.." because not in gabu_obj." )
		return
	end
	for _, val in pairs( tab ) do
		fn( self, val, ... )
	end
end
function GABU_OBJ:call_method_protected( method, b_dialog )
	--	switch for debugging
	local fn = function() self[method](self) end
	--todonow should be a ga option
	if false then
		fn()
		return true
	end
	return aaa.lua.pcall_protected( fn, self..":"..method.."()", b_dialog )
end

function GABU_OBJ:apply_method_to_all_classes(method_name, ...)
	local classes = CLASS.__classes_by_name
	
	for class_name, class in pairs(classes) do
		
		local fn = rawget( class, method_name )
		if fn then
			local typ = type(fn) 
			if typ == "function" then
				-- Safe protected call
				--local ok, err = pcall(fn, class, ...)
				-- 			if not ok then
				-- 	print("Error calling " .. method_name .. 
				-- 	      " on class " .. class_name .. 
				-- 	      ": " .. tostring(err))
				-- end
				fn( class, ... )
			else
				class:print_error( method_name.." is not a function but a "..typ.." : "..fn )
			end
		end
	end
end
--
--	Active
--
--todo	this ambiguous
--	this should not be overridden by subclass
function GABU_OBJ:is_active()					return self.__b_active	end
function GABU_OBJ:set_active_direct( b_on )		self.__b_active = b_on 	end
--	return true when there is a change
function GABU_OBJ:set_active( b_on )
	if self.__b_active ~= b_on then
		if self.verbose >= 1 then aaa.print_method() end
		--self:print( "set_active "..b_on  )
		self:set_active_direct( b_on )
	end
	return self
end
function GABU_OBJ:flip_active()		self:set_active( not self:is_active() )	return self:is_active() end

--
--	Util for FX/MEU
--	todo use IMG/IMGS from Nov 2022
--	Used in Qwartz APPs
function GABU_OBJ:load_media( key, filename, b_async )
	local bind, sx,sy = IMGS.get_bind_sxy( filename, b_async )
	if not bind then
		self:box_error( "missing file : "..filename )
		return nil
	end

	self:print( "Loading "..filename.." at bind "..bind..", sx : "..sx..", sy : "..sy )
	local media = {}
	local ba, bi = aaa.img.make_bank_bind_2d( bind )
	media.bind = bind
	media.bank_2d = ba
	media.bind_2d = bi
	media.sx = sx
	media.sy = sy
	if sy then
		media.ratio = sy / sx
	else
		self:box_error( "No sy, Pb to load : "..filename )
	end
	if key then
		local medias =  self:get_table_always( "__medias" )
		medias[key] = media
	end
	return media
end

--
--	UP
--
doc.__sep_up = { "( gabu_obj ) called by add_down, add the link up which is unique. ",
				"  we deal with a tree going down here." }
function GABU_OBJ:__set_up( up )
	-- if up and up:is_class(GP) then
	-- 	self:print( "GP case" )
	-- 	aaa.debug.print_traceback()
	-- end
	if self.__up then
		--aaa.debug.print_traceback()
		if self.__up == up then
			self:print_error( "__set_up("..up..") already have this up" )
		else
			self:print_error( "__set_up("..up..") already have a up : "..self.__up )
		end
	end
	self.__up = up
end
doc.__remove_up = { "() remove the link up." }
function GABU_OBJ:__remove_up()
	self.__up = nil
end
doc.get_up = { "() return the gabu_obj linked up." }
function GABU_OBJ:get_up()			return	self.__up		end
doc.get_up_by_class = { "( class ) recursively het the gabu_obj up until it is of class class." }
function GABU_OBJ:get_up_by_class( class )
	local up = self:get_up()
	while up do
		if oo.isinstanceof( up, class ) then
			return up
		end
		up = up:get_up()
	end
end
function GABU_OBJ:get_up_before_by_class( class )
	local before = self
	local up = self:get_up()
	while up do
		if oo.isinstanceof( up, class ) then
			return before
		end
		before = up
		up = up:get_up()
	end
end

--
--	DOWN
--
doc.add_down = "( gabu_obj ) if not existing yet, add a link down, and if sucessfull set a link up to gabu_obj."
function GABU_OBJ:add_down( gabu_obj )
	local down = self.__down
	local nb
	if not down then
		down = {}
		self.__down = down
		nb = 1
	else
		local i = self:get_down_index( gabu_obj )
		if i then
			self:print( "already have "..gabu_obj.." down" )
			self:print( "\twill not add again" )
			--todo check the up of object also
			nb = 0
		else
			nb = self.__down_nb + 1
		end
	end
	if nb > 0 then
		--self:print( "add gabu_obj at : "..nb )
		down[nb] = gabu_obj
		self.__down_nb = nb
		gabu_obj:__set_up( self )
	end
end
doc.print_down = { "( [pre] ) print the array holding the down gabu_obj.", "if pre is passed it is used as a prefix to the name." } 
function GABU_OBJ:print_down( pre )
	local name = ( pre and pre.." "..self or self ) .." dowm"
	table.print( self.__down, name, 1 )
end
doc.get_down_nb = { "() return the number of object down." } 
function GABU_OBJ:get_down_nb()
	local nb = self.__down_nb
	return nb or 0
end
doc.get_down_index = { "( gabu_obj ) return the index of gabu_obj in the down array if found,", "  return nil otherwise." } 
function GABU_OBJ:get_down_index( gabu_obj )
	return table.find_key_by_val( self.__down, gabu_obj )
end
doc.get_down_array_active = { "() build and return an array of the down gabu_obj which are active.", "  return nil if empty."  } 
function GABU_OBJ:get_down_array_active()
	local nb = self:get_down_nb()
	if nb > 0 then
		local t = table.create(nb)	--for speed we do it this way
		local i = 0
		for _, elt in ipairs(self.__down) do
			if elt:is_active() then
				i = i + 1
				t[i] = elt
			end
		end
		if i > 0 then
			return t
		end
	end
end
doc.get_down_array = "() return a direct reference to the array down."
function GABU_OBJ:get_down_array()
	return self.__down
end
doc.is_down = { "( id ) return boolean meaning existence of array element at index id.",
				"  if id is negative we start from the end: for id -1 it returns last element." }
function GABU_OBJ:is_down( id )
	local down = self.__down
	--table.print( down, "down", 1 )
	if down then
		local nb = self.__down_nb
		if id > 0 then
			if id <= nb then
				return down[ id ] ~= nil
			end
		elseif id < 0 then
			if -id <= nb  then
				return down[ nb + id + 1 ] ~= nil
			end
		else
			self:print_error( "index can't be 0 to access down array" )
			aaa.debug.print_traceback()
		end
	end
	return false
end
doc.get_down_no_error = {	"( id ) return array element at index id.",
							"  If id is negative we start from the end: for id -1 it returns last element.",
							"  id can't be 0.",
							"  If id is out of the array rangem, it returns nil" }
function GABU_OBJ:get_down_no_error( id )
	local down = self.__down
	--table.print( down, "down", 1 )
	if down then
		local nb = self.__down_nb
		if id > 0 then
			if id <= nb  then
				return down[ id ]
			end
		elseif id < 0 then
			if -id <= nb  then 
				return down[ nb + id + 1 ]
			end
		else
			self:print_error( "index can't be 0 to access down array" )
			aaa.debug.print_traceback()
		end
	end
end
doc.get_down = {	"( id ) return array element at index id.",
					"  If id is negative we start from the end: for id -1 it returns last element.",
					"  id can't be 0.",
					"  If id is out of the array range, it returns nil and triggers a print and a debug traceback." }
function GABU_OBJ:get_down( id )
	local down = self.__down
	--table.print( down, "down", 1 )
	if down then
		local nb = self.__down_nb
		if id > 0 then
			if id > nb  then
				self:print_error( "id can't be "..id.." to access down array of len "..nb )
				aaa.debug.print_traceback()
			else 
				return down[ id ]
			end
		elseif id < 0 then
			if -id > nb  then
				self:print_error( "id can't be "..id.." to access down array of len "..nb )
				aaa.debug.print_traceback()
			else 
				return down[ nb + id + 1 ]
			end
		else
			self:print_error( "index can't be 0 to access down array" )
			aaa.debug.print_traceback()
		end
	end
end
function GABU_OBJ:sort_down( fn )
	local down = self.__down
	if down then
		table.sort( down, fn )
	end
end
function GABU_OBJ:remove_all_down()
	self:apply_method_down( "__remove_up" )
	local old_down = self.__down
	self.__down = nil
	self.__down_nb = nil	--todo	stop storing __down_nb use table size
	return old_down
end
function GABU_OBJ:remove_down( gabu_obj, b_error )
	-- local name = "Menu_Field_dim"
	-- local b_debug
	-- if gabu_obj:get_name() == name then
	-- 	self:print( name )
	-- 	aaa.debug.print_traceback()
	-- 	b_debug = true
	-- 	table.print( self.__down, self.." down", 1 )
	-- end
	local down = self.__down
	if not down then
		self:box_error( "Can't remove_down "..gabu_obj.." because no down table." )
		return nil
	end
	local rm = array.remove_by_val( down, gabu_obj )
	if not rm then
		if not(b_error==false) then
			self:print_error( "Can't remove_down "..gabu_obj.." because is not in the down table." )
		end
		return nil
	end
	self.__down_nb = self.__down_nb - 1		--todo	stop storing __down_nb use table size
	gabu_obj:__remove_up()
	return gabu_obj
end


function GABU_OBJ:apply_fn_down_start_stop( start, stop, fn, ... )
	local down = self.__down
	if down then
		for i = (start or 1), (stop or self.__down_nb) do
			fn( down[i], ... )
		end
	end
end
function GABU_OBJ:apply_method_down( method, ... )
	local down = self.__down
	if down then
		for i = 1, self.__down_nb do
			local d = down[i]
			d[method]( d, ... )
		end
	end
end
function GABU_OBJ:apply_method_down_active( method, ... )
	local down = self.__down
	if down then
		for i = 1, self.__down_nb do
			local d = down[i]
			if d:is_active() then
				d[method]( d, ... )
			end
		end
	end
end
function GABU_OBJ:apply_fn_down( fn, ... )
	local down = self.__down
	if down then
		for i = 1, self.__down_nb do
			fn( down[i], ... )
		end
	end
end
function GABU_OBJ:apply_fn_down_reverse( fn, ... )
	local down = self.__down
	if down then
		for i = self.__down_nb,1,-1 do
			fn( down[i], ... )
		end
	end
end

function GABU_OBJ:apply_fn_down_active( fn, ... )
	local down = self:get_down_array_active()
	if down then
		for i = 1,#down do
			fn( down[i], ... )
		end
	end
end
function GABU_OBJ:test_down( fn, ... )
	--self:print( "------------------------ "..self.nb.." bu" )
	local down = self.__down
	if down then
		for i = 1,self.__down_nb do
			--self:print( "------------------------ "..i.." deal with "..down[i].." verbose "..down[i].verbose )
			local ret = fn( down[i], ... )
			if ret then
				--self:print( "------------------------ exit with "..ret )
				return ret
			end
		end
	end
	--self:print( "------------------------ exit " )
end
function GABU_OBJ:test_down_reverse( fn, ... )
	--self:print( "------------------------ "..self.nb.." bu" )
	local down = self.__down
	if down then
		for i = self.__down_nb,1,-1 do
			--self:print( "------------------------ "..i.." deal with "..down[i].." verbose "..down[i].verbose )
			local ret = fn( down[i], ... )
			if ret then
				--self:print( "------------------------ exit with "..ret )
				return ret
			end
		end
	end
	--self:print( "------------------------ exit " )
end
function GABU_OBJ.find_in_array_by_name_lowercase( t, nb, name )
	for i=1,nb do
		local elt = t[i]
		local key = elt:get_name_lowercase()
		--aaa.print( "name is "..key )
		if key and (key == name) then
			--aaa.print( "elt is ok "..elt )
			return elt
		end
	end
end
function GABU_OBJ:find_in_down_by_name_lowercase( name )
	return self.find_in_array_by_name_lowercase( self.__down, self.__down_nb, name )
end
function GABU_OBJ:find_in_down_by_id( id )
	return array.find_elt_by_id( self.__down, self.__down_nb, id )
end
function GABU_OBJ:find_in_down_by_field_val( field_name, val )
	return array.find_elt_by_field_val( self.__down, self.__down_nb, field_name, val )
end
function GABU_OBJ:have_in_down_val( val )
	return array.have_val( self.__down, self.__down_nb, val )
end
-- function GABU_OBJ:find_down_by_value( obj )
-- 	return array.find_elt_by_id( self.__down, self.__down_nb, id )
-- end

--
--	UI
--
function GABU_OBJ:do_action( command )
	local b_used = true
	if 		command == "dump"	then
		--aaa.debug.print_traceback( "dump call stack" )
		self:dump(2)
	elseif	command == "doc"	then
		local bu_doc = ga.bu_doc
		if bu_doc then
			bu_doc:set_window_state( "normal" )
			bu_doc:open_table( self:get_name(), self )
		end
	elseif	command == "name"	then aaa.clipboard.move_to( self:get_name() )
	elseif	command == "values"	then
		local get = self.get_values
		if get then
			local values = get( self )
			local str = self:serialize_to_line( "", values )
			aaa.clipboard.move_to( str )
		else
			b_used = false
			self:print( "this object have no get_values method" )
		end
	else
		self:print( "GABU_OBJ did not recognized action \""..command.."\"" )
		b_used = false
	end
	return b_used
end

function GABU_OBJ:do_uif_command_with_super( b_used, uif, class ) 
	aaa.print_fn( "GABU_OBJ:do_uif_command_with_super" )
	local command = BU:get_uif_name_sel( uif )

	if b_used then 
		self:print( self.." executed command "..command.." using class "..class )
	else
		local super = oo.getsuper(class)
		self:print( class.." did not recognized command \""..command.."\" try now with "..super )		
		if super == GABU_OBJ then	b_used = GABU_OBJ.do_action( self, command )
		else						b_used = super.do_uif_command( self, uif )
		end
	end
	return b_used
end

function GABU_OBJ:print_do_key( str, key )
	self:print( "--------------------- "..str..":do_key( "..key.." )" )
end
function GABU_OBJ:print_do_key_special( str, key )
	self:print( "--------------------- "..str..":do_key_special( "..key.." )" )
end

function GABU_OBJ:do_key( key )
	self:print( "--------------------- GABU_OBJ unused key ".. key )
	return false
end
function GABU_OBJ:do_key_special( key )
	self:print( "--------------------- GABU_OBJ unused special key ".. key )
	return false
end

--
--	REF util
--
function GABU_OBJ:check_ref( ref_in )
	local ref
	if type(ref_in) == "string" then
		ref = aaa.obj.get_no_error( ref_in )
		if not ref then
			self:print_error( "can't found objet "..ref_in )
			return nil
		end
	else
		if not aaa.obj.is_ref_no_error(ref_in) then
			self:print_error( "this is not a valid ref : "..ref_in )
			return nil
		end
		ref = ref_in
	end
	return ref
end

--
--	AAASeed Obj
--
function GABU_OBJ:get_obj()
	return self.ref and self.ref.obj or nil
end
function GABU_OBJ:update()
	local obj = self:get_obj()
	if obj then
		aaa.obj.update( obj )
	end
end
function GABU_OBJ:draw()
	local obj = self:get_obj()
	if obj then
		aaa.obj.draw( obj )
	end
end
function GABU_OBJ:update_then_draw()
	local obj = self:get_obj()
	if obj then
		aaa.obj.update_then_draw( obj )
	end
end
function GABU_OBJ:set_focus()
	local obj = self:get_obj()
	if obj then
		self:print( "focus on "..obj )
		aaa.obj.set_focus_ui( obj )
	else
		self:print( "no obj to focus on" )
	end
end
function GABU_OBJ:set_focus_ui()
	local obj = self:get_obj()
	if obj then
		self:print( "focus ui on "..obj )
		aaa.obj.set_focus_ui( obj )
	else
		self:print( "no obj to focus ui on" )
	end
end

function GABU_OBJ:set_obj_active( b )		param.set( self.ref.obj_active, b )			end

--function GABU_OBJ:is_param_active( b )		return param.get_bool( self.ref.active )	end
--function GABU_OBJ:set_param_active( b )		return param.set( self.ref.active, b )		end
--
--  PARAM
--
function GABU_OBJ:get_param_ref( name )
	local r = self.ref[name]
	if r then return r end

	local ref = self.ref
	r = param.get_ref( ref.obj, name )
	self.ref[name] = r
	return r
end
function GABU_OBJ:get_param( name )
	local r = self:get_param_ref( name )
	return param.get( r )
end
function GABU_OBJ:set_param( name, value )
	local r = self:get_param_ref( name )
	param.set( r, value )
end
function GABU_OBJ:set_param_and_nosave( name, value )
	local r = self:get_param_ref( name )
	param.set( r, value )
	param.set_save( r, false )
end
function GABU_OBJ:set_param_save( name, b )
	local r = self:get_param_ref( name )
	param.set_save( r, b )
end
function GABU_OBJ:do_param_action_open( name )
	local r = self:get_param_ref( name )
	param.do_action_open( r )
end

-- REGISTRY
--
function GABU_OBJ:register_obj( obj )
	if obj then
		local tab = self:get_table_always( "__gabu_registry_obj" )
		if tab[obj] then
			table.print( tab, self..".__gabu_registry_obj", 1 )
			self:box_error( "try to register_obj() an already registered object" )
		else
			tab[obj] = true
			return true
		end
	else
		self:print_error( "try to register_obj() a nil object" )
	end
end
function GABU_OBJ:get_registry_obj()
	return self:get_table( "__gabu_registry_obj" )
end
function GABU_OBJ:delete_registry_objs()
	for obj,_ in PAIRS( self:get_registry_obj() ) do
		aaa.obj.delete( obj )
	end
	self.__gabu_registry_obj = nil
end
function GABU_OBJ:is_registry_obj( obj )
	if obj then
		local tab = self:get_registry_obj()
		if tab then
			if tab[obj] then
				return true
			end
		end
	end
	return false
end
function GABU_OBJ:unregister_obj( obj )
	if obj then
		local tab = self:get_registry_obj()
		if tab then
			if tab[obj] then
				tab[obj] = nil
				return true
			else
				table.print( tab, self..".__gabu_registry_obj", 1 )
				self:box_error( "try to unregister_obj() a non registered object" )
			end
		else
			self:box_error( "try to unregister_obj() from a non existant registry (empty)" )
		end
	else
		self:print_error( "try to unregister_obj() a nil object" )
	end
end

-- UNIT TEST
--
function GABU_OBJ:__test_unit_begin( suite_name )
	self.__test_passed = 0
	self.__test_failed = 0
	self.__test_count = 0
	self.__test_suite_name = suite_name
	self.__test_start_time = os.clock()
	self:print_inverse( "| BEGIN Tests --------" )
end

function GABU_OBJ:__test_unit_end()
	local elapsed = os.clock() - self.__test_start_time
	
	if self.__test_failed == 0 then
		self:print( string.format( "    PASSED: %d/%d In %.3f sec", self.__test_passed, self.__test_count, elapsed ) )
	else
		self:print_error( string.format( "    FAILED: %d/%d In %.3f sec", self.__test_failed, self.__test_count, elapsed ) )
	end
	self:print_inverse(  "END Tests -----------|" )
	return (self.__test_failed or 0) == 0
end

function GABU_OBJ:do_test( description, condition )
	local nb = self.__test_count + 1
	self.__test_count = nb
    if condition then
        self.__test_passed = self.__test_passed + 1
		self:print( "    PASS: " ..  nb.."- "..description )
    else
        self.__test_failed = self.__test_failed + 1
       self:print_error( "  FAIL: " .. nb.."- "..description )
    end
    return condition
end

function GABU_OBJ:do_test_error( description, func )
    local success, _ = pcall( func )
    return self:do_test( description, not success )
end

function  GABU_OBJ:run_all_unit_tests()
	for class_name, class in pairs(CLASS.__classes_by_name) do
		local fn = rawget( class, "do_unit_test" )
		if fn then
			local typ = type(fn)		
			if typ == "function" then
				class:__test_unit_begin( class_name )
				fn( class )
				class:__test_unit_end()
			else
				class:print_error( "  Can't call do_unit_test this is not a function but a "..typ.." : "..fn )
			end
		end
	end
end


aaa.show_file_end( "GABU_OBJ" )