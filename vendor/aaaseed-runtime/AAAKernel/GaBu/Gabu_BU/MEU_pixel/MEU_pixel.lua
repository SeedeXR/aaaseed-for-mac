
-- function MEU:__get_pixel_size_obj()
-- 	local obj = self.__obj_pixel_size_format
-- 	if not obj then
-- 		self:print( "MEU:__get_pixel_size_obj() no obj: try to get a fbo" )
-- 		obj = self:get_fbo()
--  	end
-- 	return obj
-- end

-- set/get pixel caracteristic
--
--todomona deal with fbo or not explicitly
--todomaa deal with bu direclty instead of param
function MEU:set_pixel_size( sx,sy )
	--aaa.print_fn()
	if type(sx) == "string" then
		sx,sy = PIXEL_SIZE:get_sxy( sx )
	end
	if not self.__ref_pixel_size_x then
    	self:__build_pixel_size_ref()
		if not self.__ref_pixel_size_x then
			self:print_error( "in MEU:set_pixel_size() no __ref_pixel_size_..." )
			return
		end
	end
	param.set( self.__ref_pixel_size_x, sx )
	param.set( self.__ref_pixel_size_y, sy )
end
function MEU:set_pixel_size_ui( sx,sy )
	local ui_bu = self.ui.__bu_pixel
	--todo aglomerate the two for undo/redo
	ui_bu.sx:set_value_ui( sx )
	ui_bu.sy:set_value_ui( sy )
end


function MEU:get_pixel_size()
	if self.__ref_pixel_size_x then
		return param.get(self.__ref_pixel_size_x), param.get(self.__ref_pixel_size_y)
    end
	self:__build_pixel_size_ref()
	if self.__ref_pixel_size_x then
		return param.get(self.__ref_pixel_size_x), param.get(self.__ref_pixel_size_y)
	end
	self:print_error( "in MEU:get_pixel_size() no __ref_pixel_size_..." )
end
function MEU:get_pixel_size_channel_type()
	if self.__ref_pixel_size_x then
		local sx		= param.get( self.__ref_pixel_size_x	)
		local sy		= param.get( self.__ref_pixel_size_y	)
		local ch_nb		= param.get( self.__ref_pixel_ch_nb		)
		local ch_type	= param.get( self.__ref_pixel_ch_type	)
		return sx,sy, ch_nb,ch_type
	end
	self:__build_pixel_size_ref()
	if self.__ref_pixel_size_x then
		local sx		= param.get( self.__ref_pixel_size_x	)
		local sy		= param.get( self.__ref_pixel_size_y	)
		local ch_nb		= param.get( self.__ref_pixel_ch_nb		)
		local ch_type	= param.get( self.__ref_pixel_ch_type	)
		return sx,sy, ch_nb,ch_type
	end			
	self:print_error( "in MEU:get_pixel_size_channel_type() no __ref_pixel_size_..." )
end

function MEU:__swap_pixel_size()
	local sx,sy = self:get_pixel_size()
	self:set_pixel_size_ui( sy,sx )
end
function MEU:__mul_pixel_size( fx,fy )
	--aaa.print_fn()
	local sx,sy = self:get_pixel_size()
	self:set_pixel_size_ui( sx*fx, sy*fy )
end


--todo unify with FBO Obj
function MEU:get_pixel_channel_type()
	local ref  = self.ref
	--table.print( ref, "ref", 2 )
	return param.get(self.__ref_pixel_ch_nb), param.get(self.__ref_pixel_ch_type)
end

local type_name_to_sel_id 	= {	int8=1,	int16=2,	fp16=3, fp32=4	}
local type_sel_id_to_name	= { "Int8",	"Int16",	"Fp16", "Fp32" }  
local function ch_type_name_to_sel( str )
	local res = type_name_to_sel_id[string.lower(str)]
	if not res then
		aaa.print_error( str.." is not a recognized pixel type, defaulting to int8" )
		res = 1
	end
	return res
end
local function ch_type_sel_to_name( id )
	local str = type_sel_id_to_name[id]
	if not str then
		aaa.print_error( id.." is not a recognized pixel type, defaulting to int8 (type 1)" )
		str = "int8"
	end
	return str
end
function MEU:set_pixel_channel_type( ch_nb, ch_type )
	local fbo = self:get_fbo()
	--fbo:set_channel_nb( ch_nb )
	--fbo:set_channel_type( ch_type )

	local ref  = self.ref
	--table.print( ref, "ref", 2 )
	param.set( self.__ref_pixel_ch_nb, ch_nb )
	if type(ch_type)=="string" then
		ch_type = ch_type_name_to_sel(ch_type)
	end
	param.set( self.__ref_pixel_ch_type, ch_type )
end
--todo do a real push/pop
function MEU:push_pixel_channel_type( ch, pixel_type )
	self.__ch_nb_last, self.__ch_type_last = self:get_pixel_channel_type()
	self:set_pixel_channel_type( ch, pixel_type )
end
function MEU:pop_pixel_channel_type()
	self:set_pixel_channel_type( self.__ch_nb_last, self.__ch_type_last )
end


function MEU:__build_pixel_size_ref( obj )
	if not obj then
		self:print( "MEU:__build_pixel_size_min_ref() no obj: try to get a fbo" )
		obj = self:get_fbo()
	end
	if obj then
		--doit the first or when the obj is different
		if not self.__obj_pixel_size or obj ~= self.__obj_pixel_size then
			local obj_ref
			if CLASS.is_gabu_obj( obj, FBO ) then 
				obj_ref = obj:get_obj()
			elseif aaa.obj.is_ref_no_error(obj) then
				obj_ref = obj
			else
				self:print_error( "in MEU:set_pixel_size() no idea what this is "..obj )
			end
			if obj_ref then
				self.__obj_pixel_size = obj
				self.__ref_pixel_size_x		= param.get_ref( obj_ref, "size_asked_x"	)
				self.__ref_pixel_size_y		= param.get_ref( obj_ref, "size_asked_y"	)
				self.__ref_pixel_ch_nb		= param.get_ref( obj_ref, "channel_nb"		)
				self.__ref_pixel_ch_type	= param.get_ref( obj_ref, "channel_type"	)
			end
		end
	else
		self:print_error( "in MEU:__build_pixel_size_min_ref() no obj" )
	end	
end

function MEU:__set_size_from_bu( bu )
--	aaa.print_fn()
	local v = bu:get_value()
	local sx,sy = PIXEL_SIZE:get_sxy( v+1 )
	--self:print( v )
	if sx then
		self:set_pixel_size( sx,sy )
	end
end

function MEU:__add_pixel_size_min( rect, name )
	local ix,iy, sx,sy = unpack(rect)
	local bu = self:add_button( {ix,iy, sx,sy }, name, nil, nil, 3 )
 		bu:set_menu( PIXEL_SIZE:get_ui_str(), "Size", nil ):set_nb_min_0( 4, 3 )
 		bu:set_method_on_value_change( self, "__set_size_from_bu", bu )
		bu:set_value_load_save( false )
		bu:set_text_xyf( 0, .1, 1 )		--hack to compensate y offset in MEU_ref
		bu:set_text_inside( true )
 		--bu:set_target_lua( par )
 	return bu
end

-- unused
-- function MEU:add_pixel_size_min( rect, name, obj_pixel_size_format )
-- 	local bu = self:__add_pixel_size_min( rect, name )
-- 	self:__build_pixel_size_ref( obj_pixel_size_format )
--  	return bu
-- end


--todo we need a name at some point
function MEU:add_pixel_size( rect, target_obj )
	local ui_bu = {}
	self.ui.__bu_pixel = ui_bu

	local function add_resizer( rect, name, text, method, arg1, arg2 )
		local bu = self:add_trig_method(	rect,	name,	self, method, arg1, arg2 )
			bu:set_preset_use( false )
			if text then bu:set_text( text ) end
			bu:set_text_inside( true )
			bu:set_text_xy( nil, .12 )
	end

	self:__build_pixel_size_ref( target_obj )

	local ix,iy, sx,sy = unpack(rect)
	sy = sy / 3
	local bu = self:add_text_info(	{ix+sx/16,iy,		sx*14/16,		sy*.9},	"pixel_size_text" )
		ui_bu.size_info = bu
	iy = iy + sy

	ui_bu.sx = self:add_slider(			{ix,iy,		sx/2,	sy}, 	"Size X",	self.__ref_pixel_size_x, nil,	nil,	4,1024*16	)	--todo deal with max
		:set_value_type_integer( true ):set_color_back( "u" )
	ui_bu.sy = self:add_slider(			{ix+sx/2,iy,sx/2,	sy}, 	"Size Y",	self.__ref_pixel_size_y, nil,	nil,	4,1024*16	)	--todo deal with max
		:set_value_type_integer( true ):set_color_back( "v" )
	iy = iy + sy


	local sxb = sx/4
	local sxs = (sx - sxb) *.5
	self:__add_pixel_size_min( 	{ix,		iy,			sxs, sy},	"Size"		)
	add_resizer(				{ix+sxs,	iy,			sxb, sy/2},	"div_xy",	"/",	"__mul_pixel_size", 1/2,1/2	)
	add_resizer(				{ix+sxs,	iy+sy/2,	sxb, sy/2},	"mul_xy",	"*",	"__mul_pixel_size",   2,  2	)
	add_resizer(				{ix+sx-sxs,	iy,			sxs, sy},	"Swap",     nil,	"__swap_pixel_size"	)

--	local DX = self:get_button_size_factor()
--	local DXH = (1-DX)*.5
--	sx = 1
--	ix = ix + DXH
--	iy = iy + sy
--	add_resizer(		{ix,		iy,	sx,sy},	"div_x",	"/",	"__mul_pixel_size", .5, 1	)
--	add_resizer(		{ix+DX,		iy,	sx,sy},	"mul_x",	"*",	"__mul_pixel_size", 2, 1	)
--	add_resizer(		{ix+2,		iy,	sx,sy},	"div_y",	"/",	"__mul_pixel_size", 1, .5	)
--	add_resizer(		{ix+2+DX,	iy,	sx,sy},	"mul_y",	"*",	"__mul_pixel_size", 1, 2	)
--	ix = ix - DXH
--	iy = iy + sy

	self:register_update_ui( "__update_ui_pixel_size_format" )
end

function MEU:add_pixel_format( rect, obj )
	local ref  = self.ref
	local bu

	rect[4] = rect[4] /3
	local ix,iy, sx,sy = unpack(rect)
	bu = self:add_text_info(	{ix+sx/16,iy,		sx*14/16,		sy*.9},	"pixel_format_text" )
		self.ui.__bu_pixel_format_info = bu
	rect[2] = rect[2] + rect[4]


	bu = self:add_button(	rect,	"channel_nb"	)	
		bu:set_menu( {	"1 Red        ",
						"2 Red Green",
						"3 RGB       ",
						"4 RGB Alpha"}, "Channel" ):set_nb_min_0( 1, 4 )
		bu:set_text_selector(true)
--[[was 	
	bu = self:add_selector(	rect,	"channel_nb"	)	--notext not save in preset
		bu:set_nb( 4 )
		bu:set_item_text_from_nb()
		bu:set_item_text( 1, "R", "RG", "RGB", "RGBA" )
		bu:set_text_draw( false )
--]]
	bu:set_target_param( self.__ref_pixel_ch_nb )
	rect[2] = rect[2] + rect[4]

	bu = self:add_button(	rect,	"channel_type"	)
		bu:set_menu( { "Integer 8 bit    ",	"Integer 16 bit   ",	"Float 16 bit(half)",	"Float 32 bit     "}, "Type" ):set_nb_min_0( 1, 4 )
		bu:set_text_selector(true)
--[[was 
	bu = self:add_selector(	rect,	"channel_type" )	--notext not save in preset
		bu:set_nb( 4 )
		bu:set_item_text( 1, "8", "16", "Fp16", "Fp32" )
		bu:set_text_draw( false )
--]]
	bu:set_target_param( self.__ref_pixel_ch_type )


		--table.print( ref, "ref", 2 )
	self:register_update_ui( "__update_ui_pixel_size_format" )
end

function MEU:__update_ui_pixel_size_format()
	local ui = self.ui
	local bu
	local sx,sy, ch_nb,ch_type = self:get_pixel_size_channel_type()
	--aaa.print_fn()
	bu = ui.__bu_pixel.size_info
	if bu then
		bu:set_text( sx.." x "..sy )
	end
	bu = ui.__bu_pixel_format_info
	if bu then
		bu:set_text( ch_nb.." x "..ch_type_sel_to_name(ch_type+1) )
	end
end
