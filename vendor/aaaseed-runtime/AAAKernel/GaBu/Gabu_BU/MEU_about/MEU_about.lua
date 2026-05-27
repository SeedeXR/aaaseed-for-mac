-- WINDOW
--
-- function MEU:define_meu_infos( )
-- 	return {	version="1",
-- 				author="Mâa BERRIET",
-- 				date="2024 or before",
-- 			}
-- end
function MEU:define_meu_infos( )
	return  {	version	= "1",
				author	= "Mâa BERRIET",
				date	= "2024 or before",
				help	= "please define method define_meu_infos() for this meu type",
				b_default = true
			}
end

MEU.__proto_tag_list_src =
{
	"Generator",
	"Device",
	"Camera",
	"Texture",
	"VJ",
	"Sound",
	"Core", -- / infrastructure / AAASeed
	"CoreGraphic",
	"Tutorial",
	"3D",
	"2d",
	"Text", --/ Font / Typo
	"Experimental",
	"Procedural",
	"Geometry",
	"Point",
	"Surface",
	"Output",
	"Input",
	"Draw", --/ Graphic",
	"Art",
	"ImageProcessing",
	"Depreciated",
	"Interoperability",
	"RenderPass",
	"Utility",
	"Proprietary",
	"Unfinished"
}

function MEU:__compute_tag_tables( src )
--	table.sort( src )
	
	local index_to_lower = {}
	local lower_to_name = {}
	for i,name in ipairs(src) do
		local lower = string.lower(name)
		index_to_lower[i] = lower
		lower_to_name[lower] = name
	end
	table.sort( index_to_lower )

	local lower_to_index = {}
	local index_to_name = {}
	for i,lower in ipairs(index_to_lower) do
		lower_to_index[lower] = i
		index_to_name[i] = lower_to_name[lower]
	end

	local tabs = {	index_to_lower = index_to_lower,
					index_to_name = index_to_name,
					lower_to_index = lower_to_index,
					lower_to_name = lower_to_name
				}
	--table.print( tabs, "MEU.__compute_tag_tables", 2 )

	--table.print( src, "after", 2 )
	--aaa.box_debug( "La" )
	return tabs
end

MEU.__tags_tabs = MEU:__compute_tag_tables( MEU.__proto_tag_list_src )

MEU.doc.get_meu_infos_cleaned = "() return meu infos but checked, processed, ordered in particular for the tags"
function MEU:get_meu_infos_cleaned()
	--aaa.print_fn()
	local info = self:define_meu_infos()
	--table.print( info, "info1", 1 )

	--todo we should do reverse later probably
	--if the help is a table transform it to a string
	info = table.copy_simple( info ) -- should copy all but tags will be pointing on table
	--table.print( info, "info2" )

	if not info.help then
		info.help = self:get_class_doc()
	end
	--table.print( info, "info3" )

	if type(info.help) == "table" then
		local str
		for _,line in ipairs(info.help) do
			if str then
				str = str.."\n"..line
			else
				str = line
			end
		end
		info.help = str
	end
	--table.print( info, "info4" )

	local tags_in = info.tags
	-- if tags we clean them
	if tags_in and tags_in[1] then
		-- check tags
		local tags = {}
		local t_for_duplicate = {}
		local tabs = MEU.__tags_tabs
		for i,tag in ipairs(tags_in) do
			local lower = string.lower(tag)
			if t_for_duplicate[lower] then
				self:print_debug( "tag "..tag.." is used twice skipping second instance" )
			else
				t_for_duplicate[lower] = true
				if tabs.lower_to_index[lower] then
					table.insert( tags, lower )
				else
					--aaa.debug.print_traceback()
					self:print_debug( "tag "..tags_in[i].." do not exist in MEU.__proto_tag_list_src skipping it" )
				end
			end
		end
		-- order it using lowe name
		table.sort(tags)
		-- set back original name from __proto_tag_list_src
		for i,tag in ipairs(tags) do
			tags[i] = tabs.lower_to_name[tag]
		end
		info.tags = tags
		--table.print( info, "info", 2 )
	end
	--table.print( info, "info5" )

	return info
end

function MEU:check_tag()
	local method = self.define_meu_infos
	if method then
		local info = method(self)
		local tags = info.tags
		if tags then
			for _,tag in ipairs(tags) do
				self:print( tag )
			end
			--self:box_debug( "Toto" )
		end
	end
end

function MEU:do_about( x,y )
	local t = self:get_meu_infos_cleaned()
	local def = MEU.define_meu_infos(self)

	--table.print( t, "infos", 2 )
	local str = self:get_meu_type()

	local name_long = t.name_long or def.name_long
	if name_long then
		str = str.."    "..name_long
	end

	local version = t.version or def.version
	if version then
		str = str.."        v"..version
	end

	local author = t.author or def.author
	if author then
		str = str.."\n"..author
		local date = t.date or def.date
		if date then
			str = str.." In "..date
		end
	end
	local help = t.help or def.help
	if help then
		str = str.."\n"..help
	else
	end

	self:print( "About is "..str )
	local _, count = string.gsub( str, "\n", "" )
	ga:add_dialog_message( str, {x,y,3,count*.25} )
end
